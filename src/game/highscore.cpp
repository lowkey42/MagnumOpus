#include "highscore.hpp"

#include <core/asset/asset_manager.hpp>

#include <core/utils/md5.hpp>

#include <iomanip>
#include <sf2/sf2.hpp>
#include <sf2/FileParser.hpp>

#ifdef EMSCRIPTEN
#include <emscripten.h>
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
		constexpr const char* base_url = "http://second-system.de/leaderboard.php";
#endif
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

#ifdef EMSCRIPTEN
		std::stringstream cs_str;
		cs_str<<"magnumOpus"<<score.name<<score.score<<score.level<<score.seed<<"42#23";

		std::string cs = util::md5(cs_str.str());

		std::stringstream url;
		url<<base_url;
		url<<"?game=magnumOpus&op=add&name="<<score.name<<"&score="<<score.score<<"&level="<<score.level<<"&seed="<<score.seed<<"&cs="<<cs;
		emscripten_async_wget_data(url.str().c_str(), nullptr, +[](void* arg, void* data, int len){
		}, +[](void*){});

#else
		auto stored_scores = assets.load_maybe<Score_list>("cfg:highscore"_aid);

		auto score_list = stored_scores.process(std::vector<Score>{}, [](auto& ss){return ss->scores;});

		score_list.push_back(score);

		std::stable_sort(std::begin(score_list), std::end(score_list), [](const Score& a, const Score& b){
			return a.score > b.score;
		});

		score_list.resize(std::min(score_list.size(), std::size_t(15)));

		assets.save("cfg:highscore"_aid, Score_list{score_list});
#endif
	}

	auto list_scores(asset::Asset_manager& assets) -> std::vector<Score> {
#ifdef EMSCRIPTEN
		Score_list scores;
		scores.scores.reserve(15);

		std::stringstream url;
		url<<base_url<<"?game=magnumOpus&op=list";
		emscripten_async_wget_data(url.str().c_str(), &scores, +[](void* arg, void* data, int len){
			auto& target = *(Score_list*)(arg);

			std::string resp((char*)data, len);

			sf2::io::StringCharSource source(resp);
			sf2::ParserDefChooser<Score_list>::get().parse(source, target);

		}, +[](void*){});

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
			  <<std::setw(4)<<s.score<<" +"<<std::setw(4)<<(s.level+1)*100<<" "
			  <<std::setw(10)<<s.name.substr(0,10)<<std::endl;

			if(i>15)
				break;
		}

		return ss.str();
	}

}
