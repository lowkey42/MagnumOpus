#include "asset_manager.hpp"

#include "../utils/template_utils.hpp"
#include "../utils/log.hpp"

#include <physfs/physfs.h>

#include <cstring>
#include <cstdio>

#ifdef WIN
	#include <windows.h>
	#include <direct.h>
#else
	#include <sys/stat.h>
	#include <unistd.h>
#endif

namespace {
	std::string pwd() {
		char cCurrentPath[FILENAME_MAX];

		#ifdef WINDOWS
			_getcwd(cCurrentPath, sizeof(cCurrentPath));
		#else
			getcwd(cCurrentPath, sizeof(cCurrentPath));
		#endif

		return cCurrentPath;
	}

	std::string append_file(const std::string& folder, const std::string file) {
		return folder+PHYSFS_getDirSeparator()+file;
	}
	void create_dir(const std::string& dir) {
#ifdef WIN
		CreateDirectory(dir.c_str(), NULL);
#else
		mkdir(dir.c_str(), 0777);
#endif
	}

	std::vector<std::string> list_files(
			const std::string& dir, const std::string& prefix, const std::string& suffix)noexcept {
		std::vector<std::string> res;

		char **rc = PHYSFS_enumerateFiles(dir.c_str());

		for (char **i = rc; *i != nullptr; i++) {
			std::string str(*i);
			if(    (prefix.length()==0 || str.find(prefix)==0) &&
				   (suffix.length()==0 || str.find(suffix)==str.length()-suffix.length()) )
				res.emplace_back(std::move(str));
		}

		PHYSFS_freeList(rc);

		return res;
	}

	bool exists(const std::string path) {
		return PHYSFS_exists(path.c_str())!=0;
	}

	void print_dir_recursiv(const std::string& dir, uint8_t depth) {
		std::string p;
		for(uint8_t i=0; i<depth; i++)
			p+="  ";

		std::cerr<<p<<dir<<std::endl;
		depth++;
		for(auto&& f : list_files(dir, "", "")) {
			if(depth>=5)
				std::cerr<<p<<"  "<<f<<std::endl;
			else
				print_dir_recursiv(f, depth);
		}
	}
}

namespace mo {
namespace asset {

	Asset_manager::Asset_manager(const std::string& exe_name, const std::string& app_name) {
		if(!PHYSFS_init(exe_name.c_str()))
			FAIL("PhysFS-Init failed: "<< PHYSFS_getLastError());

		std::string write_dir_parent = append_file(PHYSFS_getUserDir(),
#ifdef WIN
			"AppData"
#else
			".config"
#endif
		);
		create_dir(write_dir_parent);

		std::string write_dir = write_dir_parent+PHYSFS_getDirSeparator()+app_name;
		create_dir(write_dir);


		if(!PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1) ||
				!PHYSFS_addToSearchPath(append_file(PHYSFS_getBaseDir(), "..").c_str(), 1)  ||
				!PHYSFS_addToSearchPath(pwd().c_str(), 1) ||
				!PHYSFS_addToSearchPath(write_dir.c_str(), 0))
			FAIL("Unable to construct search path: "<< PHYSFS_getLastError());
		
		// add optional search path
		PHYSFS_addToSearchPath(append_file(append_file(append_file(PHYSFS_getBaseDir(), ".."), "magnum_opus"), "assets").c_str(), 1);

		if(!PHYSFS_setWriteDir(write_dir.c_str()))
			FAIL("Unable to set write-dir to \""<<write_dir<<"\": "<< PHYSFS_getLastError());

		auto archive_file = _open("archives.lst");
		if(!archive_file) {
			std::cerr<<"WARN: No archives.lst found."<<std::endl;
			print_dir_recursiv("/", 0);
		}

		// load other archives
		archive_file.process([](istream& in) {
			for(auto&& l : in.lines()) {
				if(!PHYSFS_addToSearchPath(l.c_str(), 1))
					WARN("Error adding custom archive \""<<l
							 <<"\": "<<PHYSFS_getLastError());
			}
		});

		for(auto&& df : list_files("", "assets", ".map"))
			_open(df).process([this](istream& in) {
				for(auto&& l : in.lines()) {
					auto kvp =	util::split(l, "=");
					std::string path = util::trim_copy(kvp.second);
					if(!path.empty()) {
						_dispatcher.emplace(AID{kvp.first}, std::move(path));
					}
				}
			});
	}

	Asset_manager::~Asset_manager() {
		PHYSFS_deinit();
	}

	util::maybe<std::string> Asset_manager::_base_dir(Asset_type type)const {
		auto dir = _dispatcher.find(AID{type, ""}); // search for prefix-entry

		if(dir==_dispatcher.end())
			return util::nothing();

		std::string bdir = dir->second;
		return bdir;
	}

	std::vector<AID> Asset_manager::list(Asset_type type) {
		std::vector<AID> res;

		_base_dir(type).process([&](const std::string& dir){
			for(auto&& f : list_files(dir, "", ""))
				res.emplace_back(type, f);
		});

		return res;
	}

	util::maybe<istream> Asset_manager::_open(const std::string& path) {
		return _open(path, AID{Asset_type::gen, path});
	}
	util::maybe<istream> Asset_manager::_open(const std::string& path, const AID& aid) {
		return exists(path) ? util::just(istream{aid, *this, path}) : util::nothing();
	}

	Asset_manager::Asset::Asset(std::shared_ptr<void> data, Reloader reloader, int64_t last_modified)
		: data(data), reloader(reloader), last_modified(last_modified) {}

	void Asset_manager::_add_asset(const AID& id, const std::string& path,
	                               Reloader reloader, std::shared_ptr<void> asset) {
		_assets.emplace(id, Asset{asset, reloader, PHYSFS_getLastModTime(path.c_str())});
	}

	util::maybe<std::string> Asset_manager::_locate(const AID& id)const {
		auto res = _dispatcher.find(id);

		if(res!=_dispatcher.end()) {
			if(exists(res->second))
				return res->second;
			else
				WARN("Asset not found in configured place: "<<res->second);
		}

		if(exists(id.name()))
			return id.name();

		auto baseDir = _base_dir(id.type());

		return baseDir.is_some() ? util::just(append_file(baseDir.get_or_throw(), id.name())) : util::nothing();
	}

	ostream Asset_manager::_create(const AID& id) throw(Loading_failed) {
		std::string path;

		auto path_res = _dispatcher.find(id);
		if(path_res!=_dispatcher.end())
			path = path_res->second;

		else {
			auto res = _dispatcher.find(AID{id.type(), ""}); // search for prefix-entry

			path = (res!=_dispatcher.end()) ?
				   (res->second + "/" + id.name()) :
					id.name();
		}

		PHYSFS_mkdir(util::split_on_last(path, "/").first.c_str());

		return {id, *this, path};
	}

	void Asset_manager::reload() {
		for(auto& a : _assets) {
			auto location = _locate(a.first);
			location.process([&](const std::string& path){
				auto last_mod = PHYSFS_getLastModTime(path.c_str());
				if(last_mod!=-1 && last_mod>a.second.last_modified) {
					_open(path, a.first).process([&](istream& in){
						DEBUG("Reload: "<<a.first.str());
						try {
							a.second.reloader(a.second.data.get(), std::move(in));

						} catch(Loading_failed& e) {}

						a.second.last_modified = last_mod;
					});
				}
			});
		}
	}

	void Asset_manager::shrink_to_fit()noexcept {
		util::erase_if(_assets, [](const auto& v){return v.second.data.use_count()==1;});
	}

} /* namespace asset */
}
