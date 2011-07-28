#include <android/log.h>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <algorithm>
#include <string>
#include <iostream>

#include <cassert>

namespace io = boost::iostreams;
struct android_sink {
	typedef char char_type;
	typedef io::sink_tag category;
	std::string str;
	bool info;

	android_sink(bool i = true) : info(i) { }

	std::streamsize write(const char* s, std::streamsize n) {
		if (!n) return 0;
		int p;
		for (p = 0; p < n; p++) if (s[p] == '\n') break;
		std::copy(s, s + p, back_inserter(str));
		if (p != n) {
			__android_log_write(info ? ANDROID_LOG_INFO : ANDROID_LOG_WARN, "Wesnoth", str.c_str());
			str.clear();
			write(s + p + 1, n - p - 1);
		}
		return n;
	}
};

typedef io::stream_buffer<android_sink> android_buffer;

struct android_redirect_stdio {
	android_redirect_stdio() {
		static android_buffer outbuf(android_sink(0));
		static android_buffer errbuf(android_sink(1));
		std::cout.rdbuf(&outbuf);
		std::cerr.rdbuf(&errbuf);
	}
};

static android_redirect_stdio init;
