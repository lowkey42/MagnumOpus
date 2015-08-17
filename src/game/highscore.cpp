#include "highscore.hpp"

#include <core/asset/asset_manager.hpp>

#include <iomanip>
#include <sf2/sf2.hpp>
#include <sf2/FileParser.hpp>


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
		auto stored_scores = assets.load_maybe<Score_list>("cfg:highscore"_aid);

		auto score_list = stored_scores.process(std::vector<Score>{}, [](auto& ss){return ss->scores;});

		score_list.push_back(score);

		std::stable_sort(std::begin(score_list), std::end(score_list), [](const Score& a, const Score& b){
			return a.score > b.score;
		});

		score_list.resize(std::min(score_list.size(), std::size_t(15)));

		assets.save("cfg:highscore"_aid, Score_list{score_list});
	}

	auto list_scores(asset::Asset_manager& assets) -> std::vector<Score> {
		auto stored_scores = assets.load_maybe<Score_list>("cfg:highscore"_aid);

		return stored_scores.process(std::vector<Score>{}, [](auto& ss){return ss->scores;});
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
