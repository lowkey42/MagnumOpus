#include "log.hpp"

#include "stacktrace.hpp"

#include <ctime>
#include <fstream>
#include <sstream>
#include <cstring>

namespace mo {
namespace util {

	namespace {
		// format: YYYYMMDD_HHmmss
		// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
		std::string get_now_short() {
			time_t     now = time(0);
			struct tm  tstruct;
			char       buf[80];
			tstruct = *localtime(&now);
			strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct);

			return buf;
		}
		std::string get_now() {
			time_t     now = time(0);
			struct tm  tstruct;
			char       buf[80];
			tstruct = *localtime(&now);
			strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);

			return buf;
		}

		std::ostream& get_logfile() {
			static std::ofstream file("mo_"+get_now_short()+".log", std::ofstream::app);

			return file;
		}
	}

	struct stacktrace_log_target : log_target {
		stacktrace_log_target(std::initializer_list<std::ostream*> s) : log_target(s) {}

		void on_end()override {
			if(is_stacktrace_available())
				*this<<"From "+gen_stacktrace(2)<<"\n";
		}
	};
	struct fail_log_target : stacktrace_log_target {
		std::stringstream stream;

		fail_log_target(std::initializer_list<std::ostream*> s) : stacktrace_log_target(s) {
			_streams.push_back(&stream);
		}

		void on_end()override {
			stacktrace_log_target::on_end();
			auto str = stream.str();
			stream.str(std::string{});
			throw Error(str);
		}
	};

	log_target& debug(const char* func, const char* file, int32_t line)noexcept {
		static log_target l{&std::cout, &get_logfile()};
		l.on_start("DEBUG", func, file, line);
		return l;
	}
	log_target& info (const char* func, const char* file, int32_t line)noexcept {
		static log_target l{&std::cout, &get_logfile()};
		l.on_start("INFO", func, file, line);
		return l;
	}
	log_target& warn (const char* func, const char* file, int32_t line)noexcept {
		static stacktrace_log_target l{&std::cerr, &get_logfile()};
		l.on_start("WARN", func, file, line);
		return l;
	}
	log_target& error(const char* func, const char* file, int32_t line)noexcept {
		static stacktrace_log_target l{&std::cerr, &get_logfile()};
		l.on_start("ERROR", func, file, line);
		return l;
	}
	log_target& fail (const char* func, const char* file, int32_t line)noexcept {
		static fail_log_target l{&std::cerr, &get_logfile()};
		l.on_start("FAIL", func, file, line);
		return l;
	}
	log_target& crash_report()noexcept {
		static stacktrace_log_target l{&std::cerr, &get_logfile()};
		l.on_start("CRASH", "", "", 0);
		return l;
	}


	log_target::log_target(std::initializer_list<std::ostream*> s) : _streams{s} {
	}
	void log_target::on_start(const char* level, const char* func, const char* file, int32_t line) {
		*this<<get_now()<<" "<<level<<" ";

		if(func[0] && file[0]) {
			*this<<"["<<func<<":"<<line<<"@"<<(strrchr(file, '/') ? strrchr(file, '/') + 1 : file)<<"]";
		}
	}
	void log_target::on_end() {
	}

}
}
