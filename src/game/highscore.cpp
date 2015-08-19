#include "highscore.hpp"

#include <core/asset/asset_manager.hpp>

#include <core/utils/md5.hpp>

#include <iomanip>
#include <sf2/sf2.hpp>
#include <sf2/FileParser.hpp>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#else
#include <happyhttp/happyhttp.h>
#endif

namespace mo {

	sf2_structDef(Score,
		sf2_member(name),
		sf2_member(score),
		sf2_member(level),
		sf2_member(seed)
	)

	namespace {
		struct Score_list {
			std::vector<Score> scores;
		};

		sf2_structDef(Score_list, sf2_member(scores))

#ifdef EMSCRIPTEN
		Score_list global_scores;
#else
		struct My_stackframe {
			asset::Asset_manager* assets;
			std::stringstream buffer;
		};

#endif

		constexpr const char* base_host = "second-system.de";
		constexpr const char* base_url = "/leaderboard.php";
	}

	namespace asset {
		template<>
		struct Loader<Score_list> {
			using RT = std::shared_ptr<Score_list>;

			static RT load(istream in) throw(Loading_failed){
				auto r = std::make_shared<Score_list>();

				sf2::parseStream(in, *r);

				return r;
			}

			static void store(ostream out, const Score_list& asset) throw(Loading_failed) {
				sf2::writeStream(out,asset);
			}
		};
	}


	void add_score(asset::Asset_manager& assets, Score score) {
		std::stringstream cs_str;
		cs_str<<"magnumOpus"<<score.name<<score.score<<score.level<<score.seed<<"42#23";

		std::string cs = util::md5(cs_str.str());

		std::stringstream path;
		path<<base_url;
		path<<"?game=magnumOpus&op=add&name="<<score.name<<"&score="<<score.score<<"&level="<<score.level<<"&seed="<<score.seed<<"&cs="<<cs;

#ifdef EMSCRIPTEN
		std::stringstream url;
		url<<"http://"<<base_host<<path;

		emscripten_async_wget_data(url.str().c_str(), nullptr, +[](void* arg, void* data, int len){
		}, +[](void*){});

		auto score_list = global_scores.scores;

#else
		auto stored_scores = assets.load_maybe<Score_list>("cfg:highscore"_aid);

		auto score_list = stored_scores.process(std::vector<Score>{}, [](auto& ss){return ss->scores;});
#endif

		score_list.push_back(score);

		std::stable_sort(std::begin(score_list), std::end(score_list), [](const Score& a, const Score& b){
			return a.score > b.score;
		});

		score_list.resize(std::min(score_list.size(), std::size_t(15)));

#ifndef EMSCRIPTEN
		assets.save("cfg:highscore"_aid, Score_list{score_list});

		try {
			happyhttp::Connection conn(base_host, 80 );
			conn.setcallbacks(
					+[](const happyhttp::Response* r, void* userdata){},
					+[](const happyhttp::Response* r, void* userdata, const unsigned char* data, int n){},
					+[](const happyhttp::Response* r, void* userdata){},
					nullptr );

			conn.request("GET", path.str().c_str());

			while( conn.outstanding() )
				conn.pump();

		} catch(happyhttp::Wobbly e) {
		}

#endif
	}

	void prepare_list_scores(asset::Asset_manager& assets) {
#ifdef EMSCRIPTEN
		std::stringstream url;
		url<<"http://"<<base_host<<base_url<<"?game=magnumOpus&op=list";

		emscripten_async_wget_data(url.str().c_str(), nullptr, +[](void* arg, void* data, int len) {

			std::string resp((char*)data, len);

			sf2::io::StringCharSource source(resp);
			Score_list scores;
			sf2::ParserDefChooser<Score_list>::get().parse(source, scores);
			global_scores.scores = scores.scores;

		}, +[](void* arg) {
		});

#else
		My_stackframe frame;
		frame.assets = &assets;

		try {
			happyhttp::Connection conn(base_host, 80 );
			conn.setcallbacks(
					+[](const happyhttp::Response* r, void* userdata){},
					+[](const happyhttp::Response* r, void* userdata, const unsigned char* data, int n){
						auto& frame = *((My_stackframe*)userdata);

						frame.buffer<<std::string((const char*)data, n);
					},
					+[](const happyhttp::Response* r, void* userdata){
						if(r->getstatus()==200) {
							auto& frame = *((My_stackframe*)userdata);
							auto str = frame.buffer.str();

							sf2::io::StringCharSource source(str);
							Score_list scores;
							sf2::ParserDefChooser<Score_list>::get().parse(source, scores);
							frame.assets->save("cfg:highscore"_aid, scores);
						}
					},
					&frame );

			conn.request("GET", (std::string(base_url)+"?game=magnumOpus&op=list").c_str());

			while( conn.outstanding() )
				conn.pump();

		} catch(happyhttp::Wobbly e) {
			INFO("list_score request '"<<(std::string(base_url)+"?game=magnumOpus&op=list")<<"' failed: "<<e.what());
		}
#endif
	}

	auto list_scores(asset::Asset_manager& assets) -> std::vector<Score> {
#ifdef EMSCRIPTEN
		return global_scores.scores;

#else
		auto stored_scores = assets.load_maybe<Score_list>("cfg:highscore"_aid);

		return stored_scores.process(std::vector<Score>{}, [](auto& ss){return ss->scores;});
#endif
	}

	auto print_scores(std::vector<Score> scores) -> std::string {
		if(scores.empty())
			return "";

		std::stringstream ss;
		ss<<std::setw((2+2+4+2+4+1+10)*0.75f)<<"HIGH SCORES"<<std::endl<<std::endl;

		int i = 1;
		for(auto& s : scores) {
			ss<<std::setw(2)<<(i++)<<". "
			  <<std::setw(4)<<s.score<<" ("<<std::setw(2)<<(s.level+1)<<") "
			  <<std::setw(10)<<s.name.substr(0,10)<<std::endl;

			if(i>15)
				break;
		}

		return ss.str();
	}

}
