
#include <iostream>
#include <string>


#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>


namespace qi = boost::spirit::qi;

// #element = [ *LWS element *( *LWS "," *LWS element ) ]
#define list_0(expr) -( ( *LWS >> (expr) ) % ( *LWS >> ',' ) )

// 1#element = *LWS element *( *LWS "," *LWS element )
#define list_1(expr) ( *LWS >> (expr) ) % ( *LWS >> ',' )

template<typename Iterator>
struct rfc2396 /* : qi::grammar<Iterator> */
{
	typedef qi::rule<Iterator> rule_t;

	rule_t
		escaped,
		mark,
		unreserved,
		pchar,
		param,
		segment,
		path_segments,
		userinfo,
		domainlabel,
		toplabel,
		hostname,
		IPv4address,
		host,
		port,
		hostport,
		server,
		reg_name,
		authority,
		abs_path,
		net_path,
		reserved,
		uric,
		query,
		hier_part,
		scheme,
		uric_no_slash,
		opaque_part,
		absoluteURI,
		rel_segment,
		rel_path,
		relativeURI;

	rfc2396() /* : rfc2396::base_type(HTTP_message)*/
	{

		escaped = '%' >> qi::xdigit >> qi::xdigit;
		mark = qi::char_('-') | '_' | '.' | '!' | '~' | '*' | '\'' | '(' | ')';
		unreserved = qi::alnum | mark;
		pchar = unreserved | escaped | ':' | '@' | '&' | '=' | '+' | '$' | ',';
		param = *pchar;
		segment = *pchar >> *( ';' >> param );
		path_segments = segment >> *( '/' >> segment );
		userinfo = *( unreserved | escaped | ';' | ':' | '&' | '=' | '+' | '$' | ',' );
		domainlabel = ( qi::alnum >> *( qi::alnum | '-' ) >> qi::alnum ) | qi::alnum;
		toplabel = ( qi::alpha >> *( qi::alnum | '-' ) >> qi::alnum ) | qi::alpha;

//		hostname = *( domainlabel >> '.' ) >> toplabel >> -( qi::char_('.') );
		hostname = *( qi::alnum | '-' | '.' );
		IPv4address = +qi::digit >> '.' >> +qi::digit >> '.' >> +qi::digit >> '.' >> +qi::digit;
		host = hostname | IPv4address;
		port = *qi::digit;
		hostport = host >> -( ':' >> port );
		server = -( -( userinfo >> '@' ) >> hostport );
		reg_name      = +( unreserved | escaped | '$' | ',' | ';' | ':' | '@' | '&' | '=' | '+' );
		authority = reg_name | server;
		abs_path = '/' >> path_segments;
		net_path = "//" >> authority >> -( abs_path );
		reserved = qi::char_(';') | '/' | '?' | ':' | '@' | '&' | '=' | '+' | '$' | ',';
		uric = reserved | unreserved | escaped;
		query = *uric;
		hier_part = ( net_path | abs_path ) >> -( '?' >> query );

		scheme = qi::alpha >> *( qi::alpha | qi::digit | '+' | '-' | '.' );
		uric_no_slash = unreserved | escaped | ';' | '?' | ':' | '@' | '&' | '=' | '+' | '$' | ',';
		opaque_part = uric_no_slash >> *uric;
		absoluteURI = scheme >> ':' >> ( opaque_part | hier_part );
		rel_segment = +( unreserved | escaped | ';' | '@' | '&' | '=' | '+' | '$' | ',' );
		rel_path = rel_segment >> -( abs_path );
		relativeURI = ( net_path | abs_path | rel_path ) >> -( '?' >> query );
	}
};

#if 0
 "absoluteURI", "relativeURI", "port", "host", "abs_path", "rel_path", "authority"
#endif

template<typename Iterator>
struct rfc2616 : qi::grammar<Iterator>
{
	typedef qi::rule<Iterator> rule_t;

	rule_t
		SP,
		HT,
		CR,
		LF,
		CRLF,
		LWS,
		TEXT,
		qdtext,
		quoted_pair,
		quoted_string,
		token,
		unreserved,
		pct_encoded,
		sub_delims,
		pchar,
		segment,
		segment_nz,
		segment_nz_nc,
		dec_octet,
		attribute,
		value,
		parameter,
		product_version,
		product,
		ctext,
		comment,
		pseudonym,
		wkday,
		weekday,
		month,
		date1,
		date2,
		date3,
		time,
		rfc1123_date,
		rfc850_date,
		asctime_date,
		HTTP_date,
		qvalue,
		charset,
		content_coding,
		field_name,
		field_content,
		field_value,
		message_header,
		delta_seconds,
		weak,
		opaque_tag,
		entity_tag,
		type,
		subtype,
		media_type,
		accept_extension,
		accept_params,
		media_range,
		auth_scheme,
		auth_param,
		credentials,
		bytes_unit,
		primary_tag,
		subtag,
		language_tag,
		query,
		path_abempty,
		path_absolute,
		path_noscheme,
		path_rootless,
		reg_name,
		IPv4address,
		h16,
		ls32,
		IPv6address,
		IPvFuture,
		IP_literal,
		host,
		userinfo,
		port,
		authority,
		hier_part,
		scheme,
		absolute_URI,
		relative_part,
		relativeURI,
		absoluteURI,
		abs_path,
		extension_method,
		Method,
		Request_URI,
		HTTP_Version,
		Request_Line,
		warn_date,
		warn_text,
		warn_agent,
		warn_code,
		warning_value,
		Warning,
		received_by,
		protocol_name,
		protocol_version,
		received_protocol,
		Via,
		Upgrade,
		transfer_extension,
		transfer_coding,
		Transfer_Encoding,
		Trailer,
		extension_pragma,
		pragma_directive,
		Pragma,
		Date,
		connection_token,
		Connection,
		cache_extension,
		cache_response_directive,
		cache_request_directive,
		cache_directive,
		Cache_Control,
		general_header,
		atom,
		word,
		dtext,
		phrase,
		domain_ref,
		domain_literal,
		sub_domain,
		domain,
		local_part,
		route,
		addr_spec,
		route_addr,
		mailbox,
		User_Agent,
		t_codings,
		TE,
		Referer,
		first_byte_pos,
		last_byte_pos,
		byte_range_spec,
		suffix_length,
		suffix_byte_range_spec,
		byte_range_set,
		byte_ranges_specifier,
		ranges_specifier,
		Range,
		Proxy_Authorization,
		Max_Forwards,
		If_Unmodified_Since,
		If_Range,
		If_None_Match,
		If_Modified_Since,
		If_Match,
		Host,
		From,
		expect_params,
		expectation_extension,
		expectation,
		Expect,
		Authorization,
		language_range,
		Accept_Language,
		codings,
		Accept_Encoding,
		Accept_Charset,
		Accept,
		request_header,
		Last_Modified,
		Expires,
		Content_Type,
		instance_length,
		byte_range_resp_spec,
		byte_content_range_spec,
		content_range_spec,
		Content_Range,
		md5_digest,
		Content_MD5,
		Content_Location,
		Content_Length,
		Content_Language,
		Content_Encoding,
		Allow,
		extension_header,
		entity_header,
		Request,
		HTTP_message
	;

	rfc2396<Iterator> rfc2396rules;

	rfc2616() : rfc2616::base_type(HTTP_message)
	{
		SP = ' ';
		HT = '\t';
		CR = '\r';
		LF = '\n';

		CRLF = CR >> LF;
		LWS = -CRLF >> +(SP | HT);

		TEXT = LWS | qi::char_('\x20', '\x7e') | qi::char_('\x80', '\xff');

		qdtext = LWS | qi::char_('\x20', '\x21') | qi::char_('\x23', '\x7e') | qi::char_('\x80', '\xff'); // <any TEXT except <"(0x22)>>
		quoted_pair = '\\' >>  qi::char_('\x00', '\x7f');
		quoted_string = '\"' >> *( quoted_pair | qdtext ) >> '\"';

		token = +( qi::char_ - ( qi::char_('\x00', '\x1f') | '\x7f' | qi::char_('\x80', '\xff')
		                       | '(' | ')' | '<' | '>' | '@'
		                       | ',' | ';' | ':' | '\\' | '\"'
		                       | '/' | '[' | ']' | '?' | '='
		                       | '{' | '}' | ' '/*SP*/ | '\t'/*HT*/ ) );

		unreserved  = qi::alpha | qi::digit | '-' | '.' | '_' | '~';
		pct_encoded = '%' >> qi::xdigit >> qi::xdigit;
		sub_delims = qi::char_('!') | '$' | '&' | '\'' | '(' | ')' | '*' | '+' | ',' | ';' | '=';
		pchar = unreserved | pct_encoded | sub_delims | ':' | '@';
		segment = *pchar;
		segment_nz = +pchar;
		segment_nz_nc = +( unreserved | pct_encoded | sub_delims | '@' );

		dec_octet = "25" >> qi::char_('0', '5')			// 250-255
				  | '2' >> qi::char_('0', '4') >> qi::digit	// 200-249
				  | '1' >> qi::repeat(2)[qi::digit]			// 100-199
				  | qi::char_('1', '9') >> qi::digit			// 10-99
				  | qi::digit							// 0-9
		;

		attribute = token;
		value = token | quoted_string;
		parameter = attribute >> '=' >> value;

		product_version = token;
		product = token >> -('/' >> product_version);

		ctext = LWS | qi::char_('\x20', '\x27') | qi::char_('\x2a', '\x7e') | qi::char_('\x80', '\xff'); // <any TEXT excluding "("(0x28) and ")"(0x29)>

		// TODO: enable nesting comment
		comment = '(' >> *( quoted_pair | ctext /*| comment*/ ) >> ')';

		pseudonym = token;

		wkday  = qi::lit("Mon") | "Tue" | "Wed"
		              | "Thu" | "Fri" | "Sat" | "Sun";
		weekday = qi::lit("Monday") | "Tuesday" | "Wednesday"
		               | "Thursday" | "Friday" | "Saturday" | "Sunday";
		month = qi::lit("Jan") | "Feb" | "Mar" | "Apr"
		             | "May" | "Jun" | "Jul" | "Aug"
		             | "Sep" | "Oct" | "Nov" | "Dec";

		date1 = qi::repeat(2)[qi::digit] >> SP >> month >> SP >> qi::repeat(4)[qi::digit];					// day month year (e.g., 02 Jun 1982)
		date2 = qi::repeat(2)[qi::digit] >> '-' >> month >> '-' >> qi::repeat(2)[qi::digit];				// day-month-year (e.g., 02-Jun-82)
		date3 = month >> SP >> ( qi::repeat(2)[qi::digit] | ( SP >> qi::repeat(1)[qi::digit] ) );			// month day (e.g., Jun  2)
		time = qi::repeat(2)[qi::digit] >> ':' >> qi::repeat(2)[qi::digit] >> ':' >> qi::repeat(2)[qi::digit];	// 00:00:00 - 23:59:59
		rfc1123_date = wkday >> ',' >> SP >> date1 >> SP >> time >> SP >> "GMT";					// Sun, 06 Nov 1994 08:49:37 GMT
		rfc850_date  = weekday >> ',' >> SP >> date2 >> SP >> time >> SP >> "GMT";				// Sunday, 06-Nov-94 08:49:37 GMT
		asctime_date = wkday >> SP >> date3 >> SP >> time >> SP >> qi::repeat(4)[qi::digit];			// Sun Nov  6 08:49:37 1994
		HTTP_date = rfc1123_date | rfc850_date | asctime_date;

		qvalue = ( '0' >> -( '.' >> qi::repeat(0, 3)[qi::digit]) )
		                | ( '1' >> -( '.' >> qi::repeat(0, 3)['0'] ) );

		charset = token;

		content_coding = token;

		field_name = token;
		field_content = *TEXT;
		field_value = field_content;

		message_header = field_name >> ':' >> -( field_value );

		delta_seconds = +qi::digit;

		weak = "W/";
		opaque_tag = quoted_string;
		entity_tag = -( weak ) >> opaque_tag;

		type = token;
		subtype = token;
		media_type = type >> '/' >> subtype >> *( ';' >> parameter );

		accept_extension = ';' >> token >> -( '=' >> ( token | quoted_string ) );
		accept_params = qi::char_(';') >> 'q' >> '=' >> qvalue >> *( accept_extension );
		media_range = ( type >> '/' >> subtype ) >> *( ( ';' >> parameter ) - accept_params );

		// RFC 2617
		auth_scheme = token;
		auth_param = token >> '=' >> ( token | quoted_string );
		credentials = auth_scheme >> list_0(auth_param);

		bytes_unit = "bytes";

		primary_tag = qi::repeat(1, 8)[qi::alpha];
		subtag = qi::repeat(1, 8)[qi::alpha];
		language_tag = primary_tag >> *( '-' >> subtag );

		// RFC 3986
		query = *( pchar | '/' | '?' );

		path_abempty = *( '/' >> segment );
		path_absolute = '/' >> -( segment_nz >> *( '/' >> segment ) );
		path_noscheme = segment_nz_nc >> *( '/' >> segment );
		path_rootless = segment_nz >> *( '/' | pchar );
		//path_empty = qi::repeat(0)[pchar];

		reg_name = *( unreserved | pct_encoded | sub_delims );
		IPv4address = dec_octet >> '.' >> dec_octet >> '.' >> dec_octet >> '.' >> dec_octet;
		h16 = qi::repeat(1, 4)[qi::xdigit];
		ls32 = ( h16 >> ':' >> h16 ) | IPv4address;

		IPv6address   =	qi::repeat(6)[ h16 >> ':' ] >> ( ':' | IPv4address | h16 >> ':' >> h16 )
		              |                                        "::" >> qi::repeat(0, 5)[ h16 >> ':' ] >> -( IPv4address | h16 >> -( ':' >> h16 ) )
		              |                                h16  >> "::" >> qi::repeat(0, 4)[ h16 >> ':' ] >> -( IPv4address | h16 >> -( ':' >> h16 ) )
		              | qi::repeat(1)[ h16 >> ':' ] >> h16  >> "::" >> qi::repeat(0, 3)[ h16 >> ':' ] >> -( IPv4address | h16 >> -( ':' >> h16 ) )
		              | qi::repeat(2)[ h16 >> ':' ] >> h16  >> "::" >> qi::repeat(0, 2)[ h16 >> ':' ] >> -( IPv4address | h16 >> -( ':' >> h16 ) )
		              | qi::repeat(3)[ h16 >> ':' ] >> h16  >> "::" >> qi::repeat(0, 1)[ h16 >> ':' ] >> -( IPv4address | h16 >> -( ':' >> h16 ) )
		              | qi::repeat(4)[ h16 >> ':' ] >> h16  >> "::" >>                                   -( IPv4address | h16 >> -( ':' >> h16 ) )
		              | qi::repeat(5)[ h16 >> ':' ] >> h16  >> "::" >>                                   -(               h16                    )
		              ;

		IPvFuture = 'v' >> +qi::xdigit >> '.' >> +( unreserved | sub_delims | ':' );
		IP_literal = '[' >> ( IPv6address | IPvFuture  ) >> ']';
//		host = IP_literal | IPv4address | reg_name;
		host = rfc2396rules.host;
		userinfo = *( unreserved | pct_encoded | sub_delims | ':' );

//		port = *qi::digit;
//		authority = -( userinfo >> '@' ) >> host >> -( ':' >> port );
		port = rfc2396rules.port;
		authority = rfc2396rules.authority;
		hier_part = -(	// path_empty
						"//" >> authority >> path_abempty
		              | path_absolute
		              | path_rootless
		              );

		scheme = qi::alpha >> *( qi::alpha | qi::digit | '+' | '-' | '.' );
		absolute_URI  = scheme >> ':' >> hier_part >> -( '?' >> query );

		relative_part = -(
							qi::lit("//") >> authority >> path_abempty | path_absolute | path_noscheme
						);	// path-empty

//		relativeURI = relative_part >> -( '?' >> query );
//		absoluteURI = absolute_URI;
		relativeURI = rfc2396rules.relativeURI;
		absoluteURI = rfc2396rules.absoluteURI;
//		abs_path = path_absolute;
		abs_path = rfc2396rules.abs_path;

		extension_method = token;
		Method  = qi::lit("OPTIONS")
		               | "GET"
		               | "HEAD"
		               | "POST"
		               | "PUT"
		               | "DELETE"
		               | "TRACE"
		               | "CONNECT"
		               | extension_method;
		Request_URI = absoluteURI | abs_path | authority /* | '*' */;	// authority includes'*'
		HTTP_Version = qi::lit("HTTP") >> '/' >> +qi::digit >> '.' >> +qi::digit;

		Request_Line = Method >> SP >> Request_URI >> SP >> HTTP_Version >> CRLF;

		// generic-header

		warn_date = '\"' >> HTTP_date >> '\"';
		warn_text = quoted_string;
		warn_agent = ( host >> -( ':' >> port ) ) | pseudonym;
		warn_code = qi::repeat(3)[qi::digit];
		warning_value = warn_code >> SP >> warn_agent >> SP >> warn_text >> -( SP >> warn_date);
		Warning = qi::lit("Warning") >> ':' >> list_1(warning_value);

		received_by = ( ( IP_literal | IPv4address | *( unreserved | pct_encoded | sub_delims - ',' ) ) >> -( ':' >> port ) ) | pseudonym;
//		received_by = ( host >> -( ':' >> port ) ) | pseudonym;
		protocol_name = token;
		protocol_version = token;
		received_protocol = -( protocol_name >> '/' ) >> protocol_version;
		// TODO: remove *LWS
		Via = qi::lit("Via") >> ':' >> list_1( received_protocol >> *LWS >> ( received_by ) >> *LWS >> -( comment ) );

		Upgrade = qi::lit("Upgrade") >> ':' >> list_1(product);

		transfer_extension = token >> *( ';' >> parameter );
		transfer_coding = "chunked" | transfer_extension;
		Transfer_Encoding = qi::lit("Transfer-Encoding") >> ':' >> list_1(transfer_coding);

		Trailer  = qi::lit("Trailer") >> ':' >> list_1(field_name);

		extension_pragma = token >> -( '=' >> ( token | quoted_string ) );
		pragma_directive = "no-cache" | extension_pragma;
		Pragma = qi::lit("Pragma") >> ':' >> list_1(pragma_directive);

		Date = qi::lit("Date") >> ':' >> HTTP_date;

		connection_token = token;
		Connection = qi::lit("Connection") >> ':' >> list_1(connection_token);

		cache_extension = token >> -( '=' >> ( token | quoted_string ) );
		cache_response_directive =
				qi::lit("public")
		      | "private" >> -( qi::char_('=') >> '\"' >> list_1(field_name) >> '\"' )
		      | "no-cache" >> -( qi::char_('=') >> '\"' >> list_1(field_name) >> '\"' )
		      | "no-store"
		      | "no-transform"
		      | "must-revalidate"
		      | "proxy-revalidate"
		      | qi::lit("max-age") >> '=' >> delta_seconds
		      | qi::lit("s-maxage") >> '=' >> delta_seconds
		      | cache_extension;
		cache_request_directive =
//				qi::lit("no-cache")
		        qi::lit("no-store")
		      | qi::lit("max-age") >> '=' >> delta_seconds
		      | qi::lit("max-stale") >> -( '=' >> delta_seconds )
		      | qi::lit("min-fresh") >> '=' >> delta_seconds
		      | "no-transform"
		      | "only-if-cached"
		      | cache_extension;
		cache_directive = cache_request_directive | cache_response_directive;
		Cache_Control = qi::lit("Cache-Control") >> ':' >> list_1(cache_directive);

		general_header = Cache_Control
								| Connection
								| Date
								| Pragma
								| Trailer
								| Transfer_Encoding
								| Upgrade
								| Via
								| Warning;

		atom =  +( qi::char_ - ( qi::char_('(') | ')' | '<' | '>' | '@'
								|  ',' | ';' | ':' | '\\' | '"'
								|  '.' | '[' | ']' | ' '/*SPACE*/
								| qi::char_('\x00', '\x1f') | qi::char_('\x7f', '\xff') ) );
		word = atom | quoted_string;
		dtext = qi::char_ - ( qi::char_('[') | ']' | '\\' );
		/* <any CHAR excluding "["(0x5b), "]"(0x5d), "\"(0x5c)> */
		phrase = +word;

		domain_ref = atom;
		domain_literal = '[' >> *( dtext | quoted_pair ) >> ']';
		sub_domain = domain_ref | domain_literal;
		domain = sub_domain >> *( '.' >> sub_domain);

		local_part = word >> *( '.' >> word );

		route = list_1('@' >> domain) >> ':';
		addr_spec = local_part >> '@' >> domain;

		route_addr = '<' >> -(route) >> addr_spec >> '>';

		// RFC 1123
		mailbox = addr_spec | -( phrase ) >> route_addr;

#if 0
		// RFC 2822
		namespace rfc2822 {
			atext = qi::alpha | qi::digit
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

			DQUOTE = qi::char_('"');
			WSP = set[ qi::char_(' ') | '\t' ];
			obs_FWS = +WSP >> *( CRLF >> +WSP );
			FWS = ( -( *WSP >> CRLF) >> +WSP ) | obs_FWS;
			NO_WS_CTL = qi::char_('\x01', '\x08')
			| qi::char_('\x0b', '\x0c')
			| qi::char_('\x0e', '\x1f')
			| '\x7f';
			ctext = NO_WS_CTL
			| qi::char_('\x21', '\x27')
			| qi::char_('\x2a', '\x5b')
			| qi::char_('\x5d', '\x7e');
			dtext = NO_WS_CTL
			| qi::char_('\x21', '\x5a')
			| qi::char_('\x5e', '\x7e');

			obs_char = qi::char_('\x00', '\x09') | qi::char_('\x0b', '\x0c') | qi::char_('\x0e', '\x7f');
			obs_text = *LF >> *CR >> *( obs_char >> *LF >> *CR );
			text = qi::char_('\x01', '\x09') | qi::char_('\x0b', '\x0c') | qi::char_('\x0e', '\x7f') | obs_text;
			obs_qp = '\\' >> qi::char_('\x00', '\x7f');
			quoted_pair = ( '\\' >> text ) | obs_qp;

			extern rfc2822_comment;
			ccontent = ctext | quoted_pair | rfc2822_comment;
			rfc2822_comment = '(' >> *( -(FWS) >> by_ref(ccontent) ) >> -(FWS) >> ')';
			CFWS = *( -( FWS ) >> rfc2822_comment ) >> ( ( -( FWS ) >> rfc2822_comment ) | FWS );

			atom = -( CFWS ) >> +atext >> -( CFWS );
			qtext = NO_WS_CTL | '\x21' | qi::char_('\x23', '\x5b') | qi::char_('\x5d', '\x7e');

			qcontent = qtext | quoted_pair;
			quoted_string = -( CFWS ) >> DQUOTE >> *( -( FWS ) >> qcontent ) >> -( FWS ) >> DQUOTE >> -( CFWS );

			word = atom | quoted_string;

			obs_phrase = word >> *( word | '.' | CFWS );
			phrase = +word | obs_phrase;

			obs_local_part = word >> *( '.' >> word );

			dot_atom_text = +atext >> *( '.' >> +atext );
			dot_atom = -( CFWS ) >> dot_atom_text >> -( CFWS );
			local_part = dot_atom | quoted_string | obs_local_part;

			dcontent = dtext | quoted_pair;
			obs_domain = atom >> *( '.' >> atom );
			domain_literal = -( CFWS ) >> '[' >> *( -( FWS ) >> dcontent ) >> -( FWS ) >> ']' >> -( CFWS );
			domain = dot_atom | domain_literal | obs_domain;

			addr_spec = local_part >> '@' >> domain;

			display_name = phrase;
			obs_domain_list = '@' >> domain >> *( *( CFWS | ',' ) >> -( CFWS ) >> '@' >> domain );
			obs_route = -( CFWS ) >> obs_domain_list >> ':' >> -( CFWS );
			obs_angle_addr = -( CFWS ) >> '<' >> -( obs_route ) >> addr_spec >> '>' >> -( CFWS );
			angle_addr = -( CFWS ) >> '<' >> addr_spec >> '>' >> -( CFWS ) | obs_angle_addr;

			name_addr = -( display_name ) >> angle_addr;
			mailbox = name_addr | addr_spec;

		}
#endif

		// request-header

		// TODO: remove *LWS
		User_Agent = qi::lit("User-Agent") >> ':' >> +( *LWS >> ( product | comment ) );

//		t_codings = "trailers" | ( transfer_extension >> -( accept_params ) );
		t_codings = "trailers" | ( ( token >> *( ( ';' >> parameter ) - ( qi::char_(';') >> 'q' >> '=' >> qvalue >> *( accept_extension ) ) ) ) >> -( accept_params ) );
		TE = qi::lit("TE") >> ':' >> list_0( t_codings );

		Referer = qi::lit("Referer") >> ':' >> ( absoluteURI | relativeURI );

		first_byte_pos = +qi::digit;
		last_byte_pos = +qi::digit;
		byte_range_spec = first_byte_pos >> '-' >> -( last_byte_pos );
		suffix_length = +qi::digit;
		suffix_byte_range_spec = '-' >> suffix_length;
		byte_range_set  = list_1( byte_range_spec | suffix_byte_range_spec );
		byte_ranges_specifier = bytes_unit >> '=' >> byte_range_set;
		ranges_specifier = byte_ranges_specifier;
		Range = qi::lit("Range") >> ':' >> ranges_specifier;

		Proxy_Authorization = qi::lit("Proxy-Authorization") >> ':' >> credentials;

		Max_Forwards = qi::lit("Max-Forwards") >> ':' >> +qi::digit;

		If_Unmodified_Since = qi::lit("If-Unmodified-Since") >> ':' >> HTTP_date;

		If_Range = qi::lit("If-Range") >> ':' >> ( entity_tag | HTTP_date );

		If_None_Match = qi::lit("If-None-Match") >> ':' >> ( '*' | list_1(entity_tag) );

		If_Modified_Since = qi::lit("If-Modified-Since") >> ':' >> HTTP_date;

		If_Match = qi::lit("If-Match") >> ':' >> ( '*' | list_1(entity_tag) );

		Host = qi::lit("Host") >> ':' >> host >> -( ':' >> port );

//		mailbox = rfc822::mailbox;
		From = qi::lit("From") >> ':' >> mailbox;

		expect_params =  ';' >> token >> -( '=' >> ( token | quoted_string ) );
		expectation_extension = token >> -( '=' >> ( token | quoted_string ) >> *expect_params );
		expectation = qi::lit("100-continue") | expectation_extension;
		Expect = qi::lit("Expect") >> ':' >> list_1(expectation);

		Authorization = qi::lit("Authorization") >> ':' >> credentials;

		language_range  = ( ( qi::repeat(1, 8)[qi::alpha] >> *( '-' >> qi::repeat(1, 8)[qi::alpha] ) ) | '*' );
		Accept_Language = qi::lit("Accept-Language") >> ':' >> list_1( language_range >> -( qi::char_(';') >> 'q' >> '=' >> qvalue ) );

		codings = ( content_coding | '*' );
		Accept_Encoding = qi::lit("Accept-Encoding") >> ':' >> list_1( codings >> -( qi::char_(';') >> 'q' >> '=' >> qvalue ) );

		Accept_Charset = qi::lit("Accept-Charset") >> ':' >> list_1( ( charset | '*' ) >> -( qi::char_(';') >> 'q' >> '=' >> qvalue ) );

		Accept = qi::lit("Accept") >> ':' >> list_0( media_range >> -( accept_params ) );

		request_header = Accept
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

		Last_Modified = qi::lit("Last-Modified") >> ':' >> HTTP_date;

		Expires = qi::lit("Expires") >> ':' >> HTTP_date;

		Content_Type = qi::lit("Content-Type") >> ":" >> media_type;

		instance_length = +qi::digit;
		byte_range_resp_spec = ( first_byte_pos >> '-' >> last_byte_pos ) | '*';
		byte_content_range_spec = bytes_unit >> SP >> byte_range_resp_spec >> '/' >> ( instance_length | '*' );
		content_range_spec = byte_content_range_spec;
		Content_Range = qi::lit("Content-Range") >> ':' >> content_range_spec;

		md5_digest = qi::repeat(22)[( qi::alnum | '+' | '/' )] >> "==";
		Content_MD5 = qi::lit("Content-MD5") >> ':' >> md5_digest;

		Content_Location = qi::lit("Content-Location") >> ':' >> ( absoluteURI | relativeURI );

		Content_Length = qi::lit("Content-Length") >> ':' >> +qi::digit;

		Content_Language = qi::lit("Content-Language") >> ':' >> list_1(language_tag);

		Content_Encoding = qi::lit("Content-Encoding") >> ':' >> list_1(content_coding);

		Allow = qi::lit("Allow") >> ':' >> list_0(Method);

		extension_header = message_header;
		entity_header = Allow
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

		Request = Request_Line
					>>	*(( general_header
		                  | request_header
		                  | entity_header ) >> CRLF)
		            >> CRLF
		            /*>> -(message_body)*/;
		HTTP_message = Request /* | Response */;

	}

};

namespace spirit {

typedef rfc2616<std::string::const_iterator> parser_t;
static parser_t p;

bool parse(const std::string& header)
{
	std::string::const_iterator it = header.begin();
	return qi::parse(it, header.end(), p) && (it==header.end());
}

}

#ifdef RUN_TEST

struct test_base {
	typedef rfc2616<std::string::iterator> parser_t;
	typedef parser_t::rule_t rule_t;
	static parser_t p;
};
test_base::parser_t test_base::p;

bool match(const test_base::rule_t& rule, std::string& str)
{
	std::string::iterator it = str.begin();
	return qi::parse(it, str.end(), rule) && (it==str.end());
}

bool test_impl(const test_base::rule_t& rule, std::string str, bool result, const char* rule_name)
{
	if(match(rule, str) != result) {
		std::cout << "************" << std::endl;
		std::cout << rule_name << " incorrectly " << (result ? "refuses (doesn't ACCEPT)" : "accepts (doesn't REFUSE)") << " the str:" << std::endl;
		std::cout << "[" << str << "](" << str.size() << ")" << std::endl;
		std::cout << "************" << std::endl;
		return false;
	}
	return true;
}

#define DEFINE_TEST(r) \
	struct spirit_test_##r##_t : test_base { \
		rule_t& rule; \
		const char* rule_name; \
		spirit_test_##r##_t() : test_base(), rule(p.r), rule_name(#r) { \
			std::cout << "spirit testing " #r << std::endl; \
			this->test(); \
			std::cout << "spirit finish testing " #r << std::endl; \
		} \
		void test(); \
	} spirit_test_##r; \
	void spirit_test_##r##_t::test()

#define ACCEPT(str) \
		do { if(!test_impl(rule, str, true, rule_name)) return;} while(0)

#define REFUSE(str) \
		do { if(!test_impl(rule, str, false, rule_name)) return;} while(0)


#include "test_code.cpp"

#endif

