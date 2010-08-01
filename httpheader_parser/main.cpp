

#include <iostream>
#include <string>
#include <functional>

#include <boost/timer.hpp>

namespace spirit {
bool parse(const std::string& header);
}
namespace xpressive {
bool parse(const std::string& header);
}

std::string str("GET / HTTP/1.1\r\n"
		"Host:www.google.co.jp\r\n"
		"User-Agent:Mozilla/5.0 (X11; U; Linux i686; ja; rv:1.9.2.8) Gecko/20100722 Firefox/3.6.3\r\n"
		"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
		"Accept-Language:ja,en-us;q=0.7,en;q=0.3\r\n"
		"Accept-Encoding:gzip,deflate\r\n"
		"Accept-Charset:Shift_JIS,utf-8;q=0.7,*;q=0.7\r\n"
		"Keep-Alive:115\r\n"
		"Connection:keep-alive\r\n"
		"Cookie:PREF=ID=2bdd9aa98503:TM=114720:LM=614720:S=dLlh6FV3n-Z9; NID=37=m9UB21ePGXjT0IalYJC1tiCxPCbi-DyromZQhesAx7HJaL9XjffosMP1wSNDNayh28seAwJfGi4PduwQbyxG9yDYoblORavoS4IWIfyGdP9ii\r\n"
		"\r\n");

template <typename FUNC>
void run(FUNC f, const char* type, const std::string& header) {
	boost::timer t;

	if(!f(header)) {
		std::cout << "failure" << std::endl;
	}

	std::cout << type << ": " << t.elapsed() << " sec" << std::endl;
}

int main()
{
//	test();
#ifdef RUN_SP
	run(std::ptr_fun(spirit::parse), "xpressive", str);
#endif

	return 0;
}

