

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

std::string str2("GET /aaa/bbb/cccc HTTP/1.1\r\n"
		"Allow: GET, HEAD, PUT\r\n"
		"Content-Encoding: gzip,deflate\r\n"
		"Content-Language: a,ab,abc,abcd,abcde,abcdef,abcdefg,abcdefgh\r\n"
		"Content-Length:3495\r\n"
		"Content-Location:http://www.w3.org/hypertext/DataSources/Overview.html\r\n"
		"Content-MD5:22CharsConsitOfAlnum+/==\r\n"
		"Content-Range:bytes 123-456/12345\r\n"
		"Content-Type:text/html;charset=ISO-8859-4\r\n"
		"Expires:Sat, 29 Oct 1994 19:43:31 GMT\r\n"
		"Last-Modified:Sat, 29 Oct 1994 19:43:31 GMT\r\n"
		"Accept: */*, type/type;attr=value;attr=\"value\";q=0.000;token;token=token;token=\"token\"\r\n"
		"Accept-Charset: *, **, iso-8859-5, unicode-1-1;q=0.8\r\n"
		"Accept-Encoding: gzip;q=1.0, identity;q=0.5, *;q=0\r\n"
		"Accept-Language: a,ab,abc,abcd,abcde,abcdef,abcdefg,abcdefgh\r\n"
		"Authorization:aaa bbb=\"ccc\r\n ,\r\n ddd=eee\"\r\n"
		"Expect:100-continue,token,token=token,token=\"token\",token=\"token\";token,token=\"token\";token=token,token=\"token\";token=\"token\"\r\n"
		"From:name<@doman,@domain:some.addr@subdomain.subdomain>\r\n"
		"Host:www.w3.org\r\n"
		"If-Match:\"tag\", W/\"tag\"\r\n"
		"If-Modified-Since:Tuesday, 15-Nov-94 08:12:31 GMT\r\n"
		"If-None-Match:\"tag\", W/\"tag\"\r\n"
		"If-Range:Tuesday, 15-Nov-94 08:12:31 GMT\r\n"
		"If-Unmodified-Since:Tuesday, 15-Nov-94 08:12:31 GMT\r\n"
		"Max-Forwards:123\r\n"
		"Proxy-Authorization:aaa bbb=\"ccc\r\n ,\r\n ddd=eee\"\r\n"
		"Range:bytes=123-, 123-456, -456\r\n"
		"Referer:scheme::/first/segment/can/include/colons?query\r\n"
		"TE: deflate;param=value, deflate;param=value;param=\"value\", deflate;param=value;q=0.123;token;token=value;token=\"value\"\r\n"
		"User-Agent: CERN-LineMode/2.15 libwww/2.17b3\r\n"
		"Cache-Control: public, private=\"field,field\", no-cache=\"field, field\", no-store, no-transform, must-revalidate, proxy-revalidate, s-max-age=12345\r\n"
		"Connection: close, token\r\n"
		"Date:Tuesday, 15-Nov-94 08:12:31 GMT\r\n"
		"Pragma: no-cache, extension-pragma, extension-pragma=token, extension-pragma=\"value\"\r\n"
		"Trailer: field-name, field-name\r\n"
		"Transfer-Encoding: chunked, transfer-extension, transfer-extension;attr=value, transfer-extension;attr=\"value\";attr=\"value\"\r\n"
		"Upgrade: HTTP/2.0, SHTTP/1.3, IRC/6.9, RTA/x11\r\n"
		"Via: protocol-version host, protocol-name/protocol-version host:1234 (somecomment)\r\n"
		"Warning: 123 some-host \"warn-text\", 000 some-host:1234 \"warn-text: warn\" \"Sun, 06 Nov 1994 08:49:37 GMT\"\r\n"
		"\r\n");

std::string str3(1024, 'a');

template <typename FUNC>
void run_impl(FUNC f, const char* type, const std::string& header) {
	boost::timer t;
	volatile size_t failure_count = 0;

	for(int i=0; i<10000; ++i) {
		if(!f(header)) {
			++failure_count;
		}
	}

	std::cout << type << ": " << t.elapsed() << " sec" << std::endl;
}

void run(const std::string& header) {

#ifdef RUN_SP
	run_impl(std::ptr_fun(spirit::parse), "spirit", header);
#endif
#ifdef RUN_XP
	run_impl(std::ptr_fun(xpressive::parse), "xpressive", header);
#endif

	std::cout << std::endl;
}

int main()
{
//	test();
	run(str);
	run(str2);
	run(str3);

	return 0;
}

