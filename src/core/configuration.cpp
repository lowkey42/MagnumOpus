#include "configuration.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>


namespace {
	// trim from start
	inline std::string &ltrim(std::string &s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			return s;
	}

	// trim from end
	inline std::string &rtrim(std::string &s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
			return s;
	}

	// trim from both ends
	inline std::string trim(std::string s) {
			return ltrim(rtrim(s));
	}
	inline std::pair<std::string, std::string> split_cfg_pair(const std::string& line) {
		auto delIter = line.find("=");

		return std::make_pair(trim(line.substr(0, delIter)), trim(line.substr(delIter+1)));
	}
}

namespace mo {

Configuration::Configuration(int argc, char** argv, char** env) {
	// read file
	std::ifstream fs;

	for( auto& f : {"settings.cfg"} ) {
		fs.open(f);
		if(fs.good())
			break;
	}

	if(fs) {
		std::string line;
		while (std::getline(fs, line)) {
			auto commentIter = line.find(";");
			if(commentIter!=line.npos)
				line = line.substr(0, commentIter);

			line=trim(line);

			if(!line.empty())
				_properties.insert(split_cfg_pair(line));
		}
	}

	for(;*env!=0; env++)
		_properties.insert(split_cfg_pair(*env));

	for(int i=1; i<argc; ++i)
		_properties.insert(split_cfg_pair(argv[i]));

	_properties.emplace("exe", argv[0]);
}

} /* namespace core */
