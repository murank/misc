

#include <boost/xpressive/xpressive.hpp>

using namespace boost::xpressive;

namespace rfc2616 {

// #element = [ *LWS element *( *LWS "," *LWS element ) ]
#define list_0(expr) !( *LWS >> (expr) >> *( *LWS >> ',' >> *LWS >> (expr) ) )

// 1#element = *LWS element *( *LWS "," *LWS element )
#define list_1(expr) *LWS >> (expr) >> *( *LWS >> ',' >> *LWS >> (expr) )


sregex SP = as_xpr(' ');
sregex HT = as_xpr('\t');
sregex CR = as_xpr('\r');
sregex LF = as_xpr('\n');

sregex CRLF = CR >> LF;
sregex LWS = !( CRLF ) >> +( SP | HT );

sregex TEXT = LWS | range('\x20', '\x7e') | range('\x80', '\xff');

sregex qdtext = LWS | range('\x20', '\x21') | range('\x23', '\x7e') | range('\x80', '\xff'); // <any TEXT except <"(0x22)>>
sregex quoted_pair = '\\' >> range('\x00', '\x7f');
sregex quoted_string = '\"' >> *( quoted_pair | qdtext ) >> '\"';

sregex token = +( ~set[ range('\x00', '\x1f') | '\x7f' | range('\x80', '\xff')
                       | '(' | ')' | '<' | '>' | '@'
                       | ',' | ';' | ':' | '\\' | '\"'
                       | '/' | '[' | ']' | '?' | '='
                       | '{' | '}' | ' '/*SP*/ | '\t'/*HT*/ ] );

sregex unreserved  = alpha | digit | '-' | '.' | '_' | '~';
sregex pct_encoded = '%' >> xdigit >> xdigit;
sregex sub_delims = as_xpr('!') | '$' | '&' | '\'' | '(' | ')' | '*' | '+' | ',' | ';' | '=';
sregex pchar = unreserved | pct_encoded | sub_delims | ':' | '@';
sregex segment = *pchar;
sregex segment_nz = +pchar;
sregex segment_nz_nc = +( unreserved | pct_encoded | sub_delims | '@' );

sregex dec_octet = "25" >> range('0', '5')			// 250-255
				| '2' >> range('0', '4') >> digit	// 200-249
				| '1' >> repeat<2>(digit)			// 100-199
				| range('1', '9') >> digit			// 10-99
				| digit;							// 0-9

sregex attribute = token;
sregex value = token | quoted_string;
sregex parameter = attribute >> '=' >> value;

sregex product_version = token;
sregex product = token >> !('/' >> product_version);

sregex ctext = LWS | range('\x20', '\x27') | range('\x2a', '\x7e') | range('\x80', '\xff'); // <any TEXT excluding "("(0x28) and ")"(0x29)>

// TODO: enable nesting comment
sregex comment = '(' >> *( ctext | quoted_pair /*| by_ref(comment)*/ ) >> ')';

sregex pseudonym = token;

sregex wkday  = as_xpr("Mon") | "Tue" | "Wed"
              | "Thu" | "Fri" | "Sat" | "Sun";
sregex weekday = as_xpr("Monday") | "Tuesday" | "Wednesday"
               | "Thursday" | "Friday" | "Saturday" | "Sunday";
sregex month = as_xpr("Jan") | "Feb" | "Mar" | "Apr"
             | "May" | "Jun" | "Jul" | "Aug"
             | "Sep" | "Oct" | "Nov" | "Dec";
sregex date1 = repeat<2>(digit) >> SP >> month >> SP >> repeat<4>(digit);				// day month year (e.g., 02 Jun 1982)
sregex date2 = repeat<2>(digit) >> '-' >> month >> '-' >> repeat<2>(digit);				// day-month-year (e.g., 02-Jun-82)
sregex date3 = month >> SP >> ( repeat<2>(digit) | ( SP >> repeat<1>(digit) ) );		// month day (e.g., Jun  2)
sregex time = repeat<2>(digit) >> ':' >> repeat<2>(digit) >> ':' >> repeat<2>(digit);	// 00:00:00 - 23:59:59
sregex rfc1123_date = wkday >> ',' >> SP >> date1 >> SP >> time >> SP >> "GMT";					// Sun, 06 Nov 1994 08:49:37 GMT
sregex rfc850_date  = weekday >> ',' >> SP >> date2 >> SP >> time >> SP >> "GMT";				// Sunday, 06-Nov-94 08:49:37 GMT
sregex asctime_date = wkday >> SP >> date3 >> SP >> time >> SP >> repeat<4>(digit);				// Sun Nov  6 08:49:37 1994
sregex HTTP_date = rfc1123_date | rfc850_date | asctime_date;

sregex qvalue = ( '0' >> !( '.' >> repeat<0, 3>(digit)) )
                | ( '1' >> !( '.' >> repeat<0, 3>('0') ) );

sregex charset = token;

sregex content_coding = token;

sregex field_name = token;
sregex field_content = *TEXT;
sregex field_value = field_content;

sregex message_header = field_name >> ':' >> !( field_value );

sregex delta_seconds = +digit;

sregex weak = as_xpr("W/");
sregex opaque_tag = quoted_string;
sregex entity_tag = !( weak ) >> opaque_tag;

sregex type = token;
sregex subtype = token;
sregex media_type = type >> '/' >> subtype >> *( ';' >> parameter );

sregex accept_extension =as_xpr( ';') >> token >> !( '=' >> ( token | quoted_string ) );
sregex accept_params = as_xpr(';') >> 'q' >> '=' >> qvalue >> *( accept_extension );
sregex media_range = ( type >> '/' >> subtype ) >> *( ';' >> parameter );


// RFC 2617
sregex auth_scheme = token;
sregex auth_param = token >> '=' >> ( token | quoted_string );
sregex credentials = auth_scheme >> list_0(auth_param);

sregex bytes_unit = as_xpr("bytes");

sregex primary_tag = repeat<1, 8>(alpha);
sregex subtag = repeat<1, 8>(alpha);
sregex language_tag = primary_tag >> *( '-' >> subtag );


// RFC 3986
sregex query = *( pchar | '/' | '?' );

sregex path_abempty = *( '/' >> segment );
sregex path_absolute = '/' >> !( segment_nz >> *( '/' >> segment ) );
sregex path_noscheme = segment_nz_nc >> *( '/' >> segment );
sregex path_rootless = segment_nz >> *( '/' | pchar );
//sregex path_empty = repeat<0>(pchar);

sregex reg_name = *( unreserved | pct_encoded | sub_delims );
sregex IPv4address = dec_octet >> '.' >> dec_octet >> '.' >> dec_octet >> '.' >> dec_octet;
sregex h16 = repeat<1, 4>(xdigit);
sregex ls32 = ( h16 >> ':' >> h16 ) | IPv4address;

sregex IPv6address =	repeat<6>( h16 >> ':' ) >> ( ':' | IPv4address | h16 >> ':' >> h16 )
                      |                                    "::" >> repeat<0, 5>( h16 >> ':' ) >> !( IPv4address | h16 >> !( ':' >> h16 ) )
                      |                            h16  >> "::" >> repeat<0, 4>( h16 >> ':' ) >> !( IPv4address | h16 >> !( ':' >> h16 ) )
                      | repeat<1>( h16 >> ':' ) >> h16  >> "::" >> repeat<0, 3>( h16 >> ':' ) >> !( IPv4address | h16 >> !( ':' >> h16 ) )
                      | repeat<2>( h16 >> ':' ) >> h16  >> "::" >> repeat<0, 2>( h16 >> ':' ) >> !( IPv4address | h16 >> !( ':' >> h16 ) )
                      | repeat<3>( h16 >> ':' ) >> h16  >> "::" >> repeat<0, 1>( h16 >> ':' ) >> !( IPv4address | h16 >> !( ':' >> h16 ) )
                      | repeat<4>( h16 >> ':' ) >> h16  >> "::" >>                               !( IPv4address | h16 >> !( ':' >> h16 ) )
                      | repeat<5>( h16 >> ':' ) >> h16  >> "::" >>                               !(               h16                    )
              ;

sregex IPvFuture = 'v' >> +xdigit >> '.' >> +( unreserved | sub_delims | ':' );
sregex IP_literal = '[' >> ( IPv6address | IPvFuture  ) >> ']';
//sregex host = IP_literal | IPv4address | reg_name;
sregex userinfo = *( unreserved | pct_encoded | sub_delims | ':' );

// RFC 2396
namespace rfc2396 {
sregex escaped       = '%' >> xdigit >> xdigit;
sregex mark          = as_xpr('-') | '_' | '.' | '!' | '~' | '*' | '\'' | '(' | ')';
sregex unreserved    = alnum | mark;
sregex pchar         = unreserved | escaped | ':' | '@' | '&' | '=' | '+' | '$' | ',';
sregex param         = *pchar;
sregex segment       = *pchar >> *( ';' >> param );
sregex path_segments = segment >> *( '/' >> segment );
sregex userinfo      = *( unreserved | escaped | ';' | ':' | '&' | '=' | '+' | '$' | ',' );
sregex domainlabel   = alnum | ( alnum >> *( alnum | '-' ) >> alnum );
sregex toplabel      = alpha | ( alpha >> *( alnum | '-' ) >> alnum );

//sregex hostname      = *( domainlabel >> '.' ) >> toplabel >> !( as_xpr('.') );
sregex hostname = *( alnum | '-' | '.' );

sregex IPv4address   = +digit >> '.' >> +digit >> '.' >> +digit >> '.' >> +digit;
sregex host          = hostname | IPv4address;
sregex port          = *digit;
sregex hostport      = host >> !( ':' >> port );
sregex server        = !( !( userinfo >> '@' ) >> hostport );
sregex reg_name      = +( unreserved | escaped | '$' | ',' | ';' | ':' | '@' | '&' | '=' | '+' );
sregex authority = reg_name | server;
sregex abs_path      = '/' >> path_segments;
sregex net_path      = "//" >> authority >> !( abs_path );
sregex reserved      = as_xpr(';') | '/' | '?' | ':' | '@' | '&' | '=' | '+' | '$' | ',';
sregex uric          = reserved | unreserved | escaped;
sregex query         = *uric;
sregex hier_part     = ( net_path | abs_path ) >> !( '?' >> query );

sregex scheme        = alpha >> *( alpha | digit | '+' | '-' | '.' );
sregex uric_no_slash = unreserved | escaped | ';' | '?' | ':' | '@' | '&' | '=' | '+' | '$' | ',';
sregex opaque_part   = uric_no_slash >> *uric;
sregex absoluteURI = scheme >> ':' >> ( opaque_part | hier_part );
sregex rel_segment   = +( unreserved | escaped | ';' | '@' | '&' | '=' | '+' | '$' | ',' );
sregex rel_path      = rel_segment >> !( abs_path );
sregex relativeURI   = ( net_path | abs_path | rel_path ) >> !( '?' >> query );
}

sregex absoluteURI = rfc2396::absoluteURI;
sregex relativeURI = rfc2396::relativeURI;
sregex port = rfc2396::port;
sregex host = rfc2396::host;
sregex abs_path = rfc2396::abs_path;
sregex authority = rfc2396::authority;

#if 0
 "absoluteURI", "relativeURI", "port", "host", "abs_path", "rel_path", "authority"
#endif

//sregex port = *digit;
//sregex authority = !( userinfo >> '@' ) >> host >> !( ':' >> port );
//sregex hier_part = !(	// path-empty
//				"//" >> authority >> path_abempty
//              | path_absolute
//              | path_rootless
//              );

//sregex scheme = alpha >> *( alpha | digit | '+' | '-' | '.' );
//sregex absolute_URI  = scheme >> ':' >> hier_part >> !( '?' >> query );

sregex relative_part = !(
						as_xpr("//") >> authority >> path_abempty | path_absolute | path_noscheme
					);	// path-empty

//sregex relativeURI = relative_part >> !( '?' >> query );
//sregex absoluteURI = absolute_URI;
//sregex abs_path = path_absolute;


sregex extension_method = token;
sregex Method  = as_xpr("OPTIONS")
               | "GET"
               | "HEAD"
               | "POST"
               | "PUT"
               | "DELETE"
               | "TRACE"
               | "CONNECT"
               | extension_method;
sregex Request_URI = '*' | absoluteURI | abs_path | authority;
sregex HTTP_Version = as_xpr("HTTP") >> '/' >> +digit >> '.' >> +digit;

sregex Request_Line = Method >> SP >> Request_URI >> SP >> HTTP_Version >> CRLF;

// generic-header

sregex warn_date = '\"' >> HTTP_date >> '\"';
sregex warn_text = quoted_string;
sregex warn_agent = ( host >> !( ':' >> port ) ) | pseudonym;
sregex warn_code = repeat<3>(digit);
sregex warning_value = warn_code >> SP >> warn_agent >> SP >> warn_text >> !( SP >> warn_date);
sregex Warning = as_xpr("Warning") >> ':' >> list_1(warning_value);

sregex received_by = ( host >> !( ':' >> port ) ) | pseudonym;
sregex protocol_name = token;
sregex protocol_version = token;
sregex received_protocol = !( protocol_name >> '/' ) >> protocol_version;
// TODO: remove *LWS
sregex Via = as_xpr("Via") >> ':' >> list_1( received_protocol >> *LWS >> received_by >> *LWS >> !( comment ) );

sregex Upgrade = as_xpr("Upgrade") >> ':' >> list_1(product);

sregex transfer_extension = token >> *( ';' >> parameter );
sregex transfer_coding = "chunked" | transfer_extension;
sregex Transfer_Encoding = as_xpr("Transfer-Encoding") >> ':' >> list_1(transfer_coding);

sregex Trailer  = as_xpr("Trailer") >> ':' >> list_1(field_name);

sregex extension_pragma = token >> !( '=' >> ( token | quoted_string ) );
sregex pragma_directive = "no-cache" | extension_pragma;
sregex Pragma = as_xpr("Pragma") >> ':' >> list_1(pragma_directive);

sregex Date = as_xpr("Date") >> ':' >> HTTP_date;

sregex connection_token = token;
sregex Connection = as_xpr("Connection") >> ':' >> list_1(connection_token);

sregex cache_extension = token >> !( '=' >> ( token | quoted_string ) );
sregex cache_response_directive =
		as_xpr("public")
      | "private" >> !( as_xpr('=') >> '\"' >> list_1(field_name) >> '\"' )
      | "no-cache" >> !( as_xpr('=') >> '\"' >> list_1(field_name) >> '\"' )
      | "no-store"
      | "no-transform"
      | "must-revalidate"
      | "proxy-revalidate"
      | as_xpr("max-age") >> '=' >> delta_seconds
      | as_xpr("s-maxage") >> '=' >> delta_seconds
      | cache_extension;
sregex cache_request_directive =
//		as_xpr("no-cache")
		as_xpr("no-store")
      | as_xpr("max-age") >> '=' >> delta_seconds
      | as_xpr("max-stale") >> !( '=' >> delta_seconds )
      | as_xpr("min-fresh") >> '=' >> delta_seconds
      | "no-transform"
      | "only-if-cached"
      | cache_extension;
sregex cache_directive = cache_request_directive | cache_response_directive;
sregex Cache_Control = as_xpr("Cache-Control") >> ':' >> list_1(cache_directive);

sregex general_header = Cache_Control
						| Connection
						| Date
						| Pragma
						| Trailer
						| Transfer_Encoding
						| Upgrade
						| Via
						| Warning;

// RFC 822

sregex atom =  +( ~( set[ as_xpr('(') | ')' | '<' | '>' | '@'
						|  ',' | ';' | ':' | '\\' | '"'
						|  '.' | '[' | ']' | ' '/*SPACE*/
						| range('\x00', '\x1f') | range('\x7f', '\xff') ] ) );
sregex word = atom | quoted_string;
sregex dtext = ~( set[ as_xpr('[') | ']' | '\\' ] );
sregex phrase = +word;

sregex domain_ref = atom;
sregex domain_literal = '[' >> *( dtext | quoted_pair ) >> ']';
sregex sub_domain = domain_ref | domain_literal;
sregex domain = sub_domain >> *( '.' >> sub_domain);

sregex local_part = word >> *( '.' >> word );

sregex route = list_1('@' >> domain) >> ':';
sregex addr_spec = local_part >> '@' >> domain;

sregex route_addr = '<' >> !(route) >> addr_spec >> '>';

// RFC 1123
sregex mailbox = addr_spec | !( phrase ) >> route_addr;


#if 0
// RFC 2822
namespace rfc2822 {
sregex atext = alpha | digit
             | '!' | '#'
             | '$' | '%'
             | '&' | '\''
             | '*' | '+'
             | '-' | '|'
             | '=' | '?'
             | '^' | '_'
             | '`' | '{'
             | '|' | '}'
             | '~';

sregex DQUOTE = as_xpr('"');
sregex WSP = set[ as_xpr(' ') | '\t' ];
sregex obs_FWS = +WSP >> *( CRLF >> +WSP );
sregex FWS = ( !( *WSP >> CRLF) >> +WSP ) | obs_FWS;
sregex NO_WS_CTL = range('\x01', '\x08')
				| range('\x0b', '\x0c')
		        | range('\x0e', '\x1f')
		        | '\x7f';
sregex ctext = NO_WS_CTL
             | range('\x21', '\x27')
             | range('\x2a', '\x5b')
             | range('\x5d', '\x7e');
sregex dtext = NO_WS_CTL
             | range('\x21', '\x5a')
             | range('\x5e', '\x7e');

sregex obs_char = range('\x00', '\x09') | range('\x0b', '\x0c') | range('\x0e', '\x7f');
sregex obs_text = *LF >> *CR >> *( obs_char >> *LF >> *CR );
sregex text = range('\x01', '\x09') | range('\x0b', '\x0c') | range('\x0e', '\x7f') | obs_text;
sregex obs_qp = '\\' >> range('\x00', '\x7f');
sregex quoted_pair = ( '\\' >> text ) | obs_qp;

extern sregex rfc2822_comment;
sregex ccontent = ctext | quoted_pair | rfc2822_comment;
sregex rfc2822_comment = '(' >> *( !(FWS) >> by_ref(ccontent) ) >> !(FWS) >> ')';
sregex CFWS = *( !( FWS ) >> rfc2822_comment ) >> ( ( !( FWS ) >> rfc2822_comment ) | FWS );

sregex atom = !( CFWS ) >> +atext >> !( CFWS );
sregex qtext = NO_WS_CTL | '\x21' | range('\x23', '\x5b') | range('\x5d', '\x7e');

sregex qcontent = qtext | quoted_pair;
sregex quoted_string = !( CFWS ) >> DQUOTE >> *( !( FWS ) >> qcontent ) >> !( FWS ) >> DQUOTE >> !( CFWS );

sregex word = atom | quoted_string;

sregex obs_phrase = word >> *( word | '.' | CFWS );
sregex phrase = +word | obs_phrase;

sregex obs_local_part = word >> *( '.' >> word );

sregex dot_atom_text = +atext >> *( '.' >> +atext );
sregex dot_atom = !( CFWS ) >> dot_atom_text >> !( CFWS );
sregex local_part = dot_atom | quoted_string | obs_local_part;

sregex dcontent = dtext | quoted_pair;
sregex obs_domain = atom >> *( '.' >> atom );
sregex domain_literal = !( CFWS ) >> '[' >> *( !( FWS ) >> dcontent ) >> !( FWS ) >> ']' >> !( CFWS );
sregex domain = dot_atom | domain_literal | obs_domain;

sregex addr_spec = local_part >> '@' >> domain;

sregex display_name = phrase;
sregex obs_domain_list = '@' >> domain >> *( *( CFWS | ',' ) >> !( CFWS ) >> '@' >> domain );
sregex obs_route = !( CFWS ) >> obs_domain_list >> ':' >> !( CFWS );
sregex obs_angle_addr = !( CFWS ) >> '<' >> !( obs_route ) >> addr_spec >> '>' >> !( CFWS );
sregex angle_addr = !( CFWS ) >> '<' >> addr_spec >> '>' >> !( CFWS ) | obs_angle_addr;

sregex name_addr = !( display_name ) >> angle_addr;
sregex mailbox = name_addr | addr_spec;

}
#endif


// request-header

sregex User_Agent = as_xpr("User-Agent") >> ':' >> +( *LWS >> ( product | comment ) );

sregex t_codings = "trailers" | ( transfer_extension >> !( accept_params ) );
sregex TE = as_xpr("TE") >> ':' >> list_0( t_codings );

sregex Referer = as_xpr("Referer") >> ':' >> ( absoluteURI | relativeURI );

sregex first_byte_pos = +digit;
sregex last_byte_pos = +digit;
sregex byte_range_spec = first_byte_pos >> '-' >> !( last_byte_pos );
sregex suffix_length = +digit;
sregex suffix_byte_range_spec = '-' >> suffix_length;
sregex byte_range_set  = list_1( byte_range_spec | suffix_byte_range_spec );
sregex byte_ranges_specifier = bytes_unit >> '=' >> byte_range_set;
sregex ranges_specifier = byte_ranges_specifier;
sregex Range = as_xpr("Range") >> ':' >> ranges_specifier;

sregex Proxy_Authorization = as_xpr("Proxy-Authorization") >> ':' >> credentials;

sregex Max_Forwards = as_xpr("Max-Forwards") >> ':' >> +digit;

sregex If_Unmodified_Since = as_xpr("If-Unmodified-Since") >> ':' >> HTTP_date;

sregex If_Range = as_xpr("If-Range") >> ':' >> ( entity_tag | HTTP_date );

sregex If_None_Match = as_xpr("If-None-Match") >> ':' >> ( '*' | list_1(entity_tag) );

sregex If_Modified_Since = as_xpr("If-Modified-Since") >> ':' >> HTTP_date;

sregex If_Match = as_xpr("If-Match") >> ':' >> ( '*' | list_1(entity_tag) );

sregex Host = as_xpr("Host") >> ':' >> host >> !( ':' >> port );

sregex From = as_xpr("From") >> ':' >> mailbox;

sregex expect_params =  ';' >> token >> !( '=' >> ( token | quoted_string ) );
sregex expectation_extension = token >> !( '=' >> ( token | quoted_string ) >> *expect_params );
sregex expectation = as_xpr("100-continue") | expectation_extension;
sregex Expect = as_xpr("Expect") >> ':' >> list_1(expectation);

sregex Authorization = as_xpr("Authorization") >> ':' >> credentials;

sregex language_range  = ( ( repeat<1, 8>(alpha) >> *( '-' >> repeat<1, 8>(alpha) ) ) | '*' );
sregex Accept_Language = as_xpr("Accept-Language") >> ':' >> list_1( language_range >> !( as_xpr(';') >> 'q' >> '=' >> qvalue ) );

sregex codings = ( content_coding | '*' );
sregex Accept_Encoding = as_xpr("Accept-Encoding") >> ':' >> list_1( codings >> !( as_xpr(';') >> 'q' >> '=' >> qvalue ) );

sregex Accept_Charset = as_xpr("Accept-Charset") >> ':' >> list_1( ( charset | '*' ) >> !( as_xpr(';') >> 'q' >> '=' >> qvalue ) );

sregex Accept = as_xpr("Accept") >> ':' >> list_0( media_range >> !( accept_params ) );

sregex request_header = Accept
                | Accept_Charset
                | Accept_Encoding
                | Accept_Language
                | Authorization
                | Expect
                | From
                | Host
                | If_Match
                | If_Modified_Since
                | If_None_Match
                | If_Range
                | If_Unmodified_Since
                | Max_Forwards
                | Proxy_Authorization
                | Range
                | Referer
                | TE
                | User_Agent;

// entity-header

sregex Last_Modified = as_xpr("Last-Modified") >> ':' >> HTTP_date;

sregex Expires = as_xpr("Expires") >> ':' >> HTTP_date;

sregex Content_Type = as_xpr("Content-Type") >> ":" >> media_type;

sregex instance_length = +digit;
sregex byte_range_resp_spec = ( first_byte_pos >> '-' >> last_byte_pos ) | '*';
sregex byte_content_range_spec = bytes_unit >> SP >> byte_range_resp_spec >> '/' >> ( instance_length | '*' );
sregex content_range_spec = byte_content_range_spec;
sregex Content_Range = as_xpr("Content-Range") >> ':' >> content_range_spec;

sregex md5_digest = repeat<22>( set[alnum | '+' | '/'] ) >> "==";
sregex Content_MD5 = as_xpr("Content-MD5") >> ':' >> md5_digest;

sregex Content_Location = as_xpr("Content-Location") >> ':' >> ( absoluteURI | relativeURI );

sregex Content_Length = as_xpr("Content-Length") >> ':' >> +digit;

sregex Content_Language = as_xpr("Content-Language") >> ':' >> list_1(language_tag);

sregex Content_Encoding = as_xpr("Content-Encoding") >> ':' >> list_1(content_coding);

sregex Allow = as_xpr("Allow") >> ':' >> list_0(Method);

sregex extension_header = message_header;
sregex entity_header = Allow
                | Content_Encoding
                | Content_Language
                | Content_Length
                | Content_Location
                | Content_MD5
                | Content_Range
                | Content_Type
                | Expires
                | Last_Modified
                | extension_header;

sregex Request = Request_Line
			>>	*(( general_header
                  | request_header
                  | entity_header ) >> CRLF)
            >> CRLF
            /*>> !(message_body)*/;
sregex HTTP_message = Request /* | Response */;


}

namespace xpressive {
bool parse(const std::string& header)
{
	return regex_match(header, rfc2616::HTTP_message);
}

} // namespace xpressive

#ifdef RUN_TEST

#include <iostream>

namespace rfc2616 {

bool test_impl(const sregex& rule, std::string str, bool result, const char* rule_name)
{
	if(regex_match(str, rule) != result) {
		std::cout << "************" << std::endl;
		std::cout << rule_name << " incorrectly " << (result ? "refuses (doesn't ACCEPT)" : "accepts (doesn't REFUSE)") << " the str:" << std::endl;
		std::cout << "[" << str << "](" << str.size() << ")" << std::endl;
		std::cout << "************" << std::endl;
		return false;
	}
	return true;
}

#define DEFINE_TEST(r) \
	struct xpressive_test_##r##_t { \
		sregex& rule; \
		const char* rule_name; \
		xpressive_test_##r##_t() : rule(r), rule_name(#r) { \
			std::cout << "xpressive testing " #r << std::endl; \
			this->test(); \
			std::cout << "xpressive finish testing " #r << std::endl; \
		} \
		void test(); \
	} xpressive_test_##r; \
	void xpressive_test_##r##_t::test()

#define ACCEPT(str) \
		do { if(!test_impl(rule, str, true, rule_name)) return; } while(0)

#define REFUSE(str) \
		do { if(!test_impl(rule, str, false, rule_name)) return; } while(0)

#include "test_code.cpp"
}

#endif

