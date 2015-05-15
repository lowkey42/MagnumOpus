#include "stream.hpp"

#include <physfs/physfs.h>
#include <streambuf>
#include <cstring>
#include <cstdio>

#include "../utils/log.hpp"
#include "../utils/string_utils.hpp"

namespace mo {
namespace asset {


	class stream::fbuf : public std::streambuf {
		fbuf(const fbuf & other) = delete;
		fbuf& operator=(const fbuf& other) = delete;

		int_type underflow() {
			if (PHYSFS_eof(file)) {
				return traits_type::eof();
			}
			size_t bytesRead = PHYSFS_read(file, buffer, 1, bufferSize);
			if (bytesRead < 1) {
				return traits_type::eof();
			}
			setg(buffer, buffer, buffer + bytesRead);
			return (unsigned char) *gptr();
		}

		pos_type seekoff(std::streamoff pos, std::ios_base::seekdir dir, std::ios_base::openmode mode) {
			switch (dir) {
				case std::ios_base::beg:
					PHYSFS_seek(file, pos);
					break;
				case std::ios_base::cur:
					// subtract characters currently in buffer from seek position
					PHYSFS_seek(file, (PHYSFS_tell(file) + pos) - (egptr() - gptr()));
					break;
					//case std::_S_ios_seekdir_end:
				case std::ios_base::end:
				default:
					PHYSFS_seek(file, PHYSFS_fileLength(file) + pos);
					break;
			}
			if (mode & std::ios_base::in) {
				setg(egptr(), egptr(), egptr());
			}
			if (mode & std::ios_base::out) {
				setp(buffer, buffer);
			}
			return PHYSFS_tell(file);
		}

		pos_type seekpos(pos_type pos, std::ios_base::openmode mode) {
			PHYSFS_seek(file, pos);
			if (mode & std::ios_base::in) {
				setg(egptr(), egptr(), egptr());
			}
			if (mode & std::ios_base::out) {
				setp(buffer, buffer);
			}
			return PHYSFS_tell(file);
		}

		int_type overflow( int_type c = traits_type::eof() ) {
			if (pptr() == pbase() && c == traits_type::eof()) {
				return 0; // no-op
			}
			if (PHYSFS_write(file, pbase(), pptr() - pbase(), 1) < 1) {
				return traits_type::eof();
			}
			if (c != traits_type::eof()) {
				if (PHYSFS_write(file, &c, 1, 1) < 1) {
					return traits_type::eof();
				}
			}

			return 0;
		}

		int sync() {
			return overflow();
		}

		char * buffer;
		size_t const bufferSize;
	protected:
		PHYSFS_File * const file;
	public:
		fbuf(File_handle* file, std::size_t bufferSize = 2048)
				: bufferSize(bufferSize), file((PHYSFS_File*)file) {
			buffer = new char[bufferSize];
			char * end = buffer + bufferSize;
			setg(end, end, end);
			setp(buffer, end);
		}

		~fbuf() {
			sync();
			delete [] buffer;
		}
	};

	struct File_handle{};

	stream::stream(AID aid, Asset_manager& manager, File_handle* file, const std::string& path) : _file(file), _aid(aid), _manager(manager) {
		INVARIANT(file, "Error opening file \""<<path<<"\": "<< PHYSFS_getLastError());

		_fbuf.reset(new fbuf(file));
	}

	stream::stream(stream&& o) : _file(o._file), _aid(std::move(o._aid)), _manager(o._manager), _fbuf(std::move(o._fbuf)) {
		o._file = nullptr;
	}

	stream::~stream()noexcept {
		if(_file)
			PHYSFS_close((PHYSFS_File*)_file);
	}

	bool stream::eof()const noexcept {
		return PHYSFS_eof((PHYSFS_File*)_file);
	}
	size_t stream::length()const noexcept {
		return PHYSFS_fileLength((PHYSFS_File*)_file);
	}

	istream::istream(AID aid, Asset_manager& manager, const std::string& path)
	  : stream(aid, manager, (File_handle*)PHYSFS_openRead(path.c_str()), path), std::istream(_fbuf.get()) {
	}
	istream::istream(istream&& o)
	  : stream(std::move(o)), std::istream(_fbuf.get()) {
	}

	std::vector<std::string> istream::lines() {
		std::vector<std::string> lines;

		std::string str;
		while (std::getline(*this, str)) {
			util::replace_inplace(str, "\r", "");
			lines.emplace_back(std::move(str));
		}

		return lines;
	}
	std::string istream::content() {
		std::string content(std::istreambuf_iterator<char>{*this}, std::istreambuf_iterator<char>{});

		util::replace_inplace(content, "\r", "");

		return content;
	}
	std::vector<uint8_t> istream::bytes() {
		std::vector<uint8_t> res(length(), 0);
		read((char*)res.data(), res.size());

		return res;
	}


	ostream::ostream(AID aid, Asset_manager& manager, const std::string& path)
	  : stream(aid, manager, (File_handle*)PHYSFS_openWrite(path.c_str()), path), std::ostream(_fbuf.get()) {
	}
	ostream::ostream(ostream&& o)
	  : stream(std::move(o)), std::ostream(_fbuf.get()) {
	}

}
}
