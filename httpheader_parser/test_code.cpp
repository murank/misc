
DEFINE_TEST(LWS)
{
//	LWS            = [CRLF] 1*( SP | HT )

	ACCEPT("          ");
	ACCEPT("\t\t\t\t\t\t\t\t");
	ACCEPT("\t \t  \t   \t    \t     \t\t     \t");

	ACCEPT("\r\n          ");
	ACCEPT("\r\n\t\t\t\t\t\t\t\t");
	ACCEPT("\r\n\t \t  \t   \t    \t     \t\t     \t");

	REFUSE("\r\n");
	REFUSE(" \r\n ");
	REFUSE("\t\r\n ");
	REFUSE("\t\r\n\t");
	REFUSE("\t\r\n\t");
}

DEFINE_TEST(TEXT)
{
//	TEXT           = <any OCTET Expect CTLs, but including LWS>
//	 CTL            = <any US-ASCII control character
//    					(octets 0 - 31) and DEL (127)>

	for(int c=0; c<256; ++c) {
		std::string str;
		str += static_cast<char>(c);

		if(c<0x09 || (0x09<c && c<0x20) || c==0x7f) {
			// TEXT accepts HT(0x09), since LWS includes it;
			REFUSE(str);
		} else {
			ACCEPT(str);
		}
	}

	ACCEPT("          ");
	ACCEPT("\t\t\t\t\t\t\t\t");
	ACCEPT("\t \t  \t   \t    \t     \t\t     \t");

	ACCEPT("\r\n          ");
	ACCEPT("\r\n\t\t\t\t\t\t\t\t");
	ACCEPT("\r\n\t \t  \t   \t    \t     \t\t     \t");

	REFUSE("\r\n");
	REFUSE(" \r\n ");
	REFUSE("\t\r\n ");
	REFUSE("\t\r\n\t");
	REFUSE("\t\r\n\t");
}

DEFINE_TEST(qdtext)
{
//	qdtext         = <any TEXT Expect <">>

	for(int c=0; c<256; ++c) {
		std::string str;
		str += static_cast<char>(c);

		if(c<0x09 || (0x09<c && c<0x20) || c==0x22 || c==0x7f) {
			// TEXT accepts HT(0x09), since LWS includes it;
			REFUSE(str);
		} else {
			ACCEPT(str);
		}
	}

	ACCEPT("          ");
	ACCEPT("\t\t\t\t\t\t\t\t");
	ACCEPT("\t \t  \t   \t    \t     \t\t     \t");

	ACCEPT("\r\n          ");
	ACCEPT("\r\n\t\t\t\t\t\t\t\t");
	ACCEPT("\r\n\t \t  \t   \t    \t     \t\t     \t");

	REFUSE("\r\n");
	REFUSE(" \r\n ");
	REFUSE("\t\r\n ");
	REFUSE("\t\r\n\t");
	REFUSE("\t\r\n\t");
}


DEFINE_TEST(quoted_pair)
{
//	CHAR           = <any US-ASCII character (octets 0 - 127)>
//	quoted-pair    = "\" CHAR

	for(int c=0; c<256; ++c) {
		std::string str("\\");
		str += static_cast<char>(c);

		if(c<=0x7f) {
			ACCEPT(str);
		} else {
			REFUSE(str);
		}
	}
}

DEFINE_TEST(quoted_string)
{
//	quoted-string  = ( <"> *(qdtext | quoted-pair ) <"> )

	ACCEPT("\"\"");
	ACCEPT("\"abcd1234 \\\\ \t\r\n \"");
	REFUSE("\"abcd1234 \\\\ \t\r\n ");
	REFUSE("abcd1234 \\\\ \t\r\n \"");

	// " qdtext_c \ quoted_pair_c "

	int qdtexts[] = {0x00, 0x08, 0x09, 0x1a, 0x20, 0x21, 0x22, 0x23, 0x7e, 0x7f, 0x80, 0xff};
	int quoted_pairs[] = {0x00, 0x7f, 0x80, 0xff};

	std::string str;
	for(int qdtext_i=0; qdtext_i<sizeof(qdtexts)/sizeof(int); ++qdtext_i) {
		int qdtext_c = qdtexts[qdtext_i];

		for(int quoted_pair_i=0; quoted_pair_i<sizeof(quoted_pairs)/sizeof(int); ++quoted_pair_i) {
			int quoted_pair_c = quoted_pairs[quoted_pair_i];

//			std::cout << std::hex << qdtext_c << "," << quoted_pair_c << std::endl;

			// " qdtext_c \ quoted_pair_c "
			str.assign("\"").append(1, static_cast<char>(qdtext_c)).append("\\").append(1, static_cast<char>(quoted_pair_c)).append("\"");

			if(qdtext_c<0x09 || (0x09<qdtext_c && qdtext_c<0x20) || qdtext_c==0x22 || qdtext_c==0x7f) {
				// TEXT accepts HT(0x09), since LWS includes it
				REFUSE(str);
			} else {
				if(quoted_pair_c<=0x7f) {
					ACCEPT(str);
				} else {
					// \c (c>=0x80) is interpreted as qdtext (not quoted-string)
					ACCEPT(str);
//					REFUSE(str);
				}
			}
		}
	}

	for(int qdtext_i=0; qdtext_i<sizeof(qdtexts)/sizeof(int); ++qdtext_i) {
		int qdtext_c = qdtexts[qdtext_i];

		for(int quoted_pair_i=0; quoted_pair_i<sizeof(quoted_pairs)/sizeof(int); ++quoted_pair_i) {
			int quoted_pair_c = quoted_pairs[quoted_pair_i];

//			std::cout << std::hex << qdtext_c << "," << quoted_pair_c << std::endl;

			// " \ quoted_pair_c qdtext_c "
			str.assign("\"").append("\\").append(1, static_cast<char>(quoted_pair_c)).append(1, static_cast<char>(qdtext_c)).append("\"");

			if(qdtext_c<0x09 || (0x09<qdtext_c && qdtext_c<0x20) || qdtext_c==0x22 || qdtext_c==0x7f) {
				// TEXT accepts HT(0x09), since LWS includes it
				if(quoted_pair_c==0x5c) {
					// "\\c" is interpreted as " \ \c " (not " \\ c ")
					ACCEPT(str);
				} else {
					REFUSE(str);
				}
			} else {
				if(quoted_pair_c<=0x7f) {
					ACCEPT(str);
				} else {
					// \c (c>=0x80) is accepted by qdtext (not quoted-string)
					ACCEPT(str);
//					REFUSE(str);
				}
			}
		}
	}

	for(int qdtext_i1=0; qdtext_i1<sizeof(qdtexts)/sizeof(int); ++qdtext_i1) {
		int qdtext_c1 = qdtexts[qdtext_i1];

		for(int qdtext_i2=0; qdtext_i2<sizeof(qdtexts)/sizeof(int); ++qdtext_i2) {
			int qdtext_c2 = qdtexts[qdtext_i2];

//			std::cout << std::hex << qdtext_c1 << "," << qdtext_c2 << std::endl;

			// " qdtext_c1 qdtext_c2 "
			str.assign("\"").append(1, static_cast<char>(qdtext_c1)).append(1, static_cast<char>(qdtext_c2)).append("\"");

			if(qdtext_c1<0x09 || (0x09<qdtext_c1 && qdtext_c1<0x20) || qdtext_c1==0x22 || qdtext_c1==0x7f) {
				// TEXT accepts HT(0x09), since LWS includes it
				REFUSE(str);
			} else if(qdtext_c2<0x09 || (0x09<qdtext_c2 && qdtext_c2<0x20) || qdtext_c2==0x22 || qdtext_c2==0x7f) {
				if(qdtext_c1==0x5c) {
					// str is "\c" (and it's quoted-string)
					ACCEPT(str);
				} else {
					REFUSE(str);
				}
			} else {
				ACCEPT(str);
			}
		}
	}

	for(int quoted_pair_i1=0; quoted_pair_i1<sizeof(quoted_pairs)/sizeof(int); ++quoted_pair_i1) {
		int quoted_pair_c1 = quoted_pairs[quoted_pair_i1];

		for(int quoted_pair_i2=0; quoted_pair_i2<sizeof(quoted_pairs)/sizeof(int); ++quoted_pair_i2) {
			int quoted_pair_c2 = quoted_pairs[quoted_pair_i2];

//			std::cout << std::hex << quoted_pair_c1 << "," << quoted_pair_c2 << std::endl;

			// " \ quoted_pair_c \ quoted_pair_c "
			str.assign("\"").append("\\").append(1, static_cast<char>(quoted_pair_c1)).append("\\").append(1, static_cast<char>(quoted_pair_c2)).append("\"");

			if(quoted_pair_c1<=0x7f) {
				ACCEPT(str);
			} else {
				// \c (c>=0x80) is accepted by qdtext (not quoted-string)
				ACCEPT(str);
//				REFUSE(str);
			}
		}
	}
}

DEFINE_TEST(token)
{
/*	token          = 1*<any CHAR Expect CTLs or separators>
	separators     = "(" | ")" | "<" | ">" | "@"
	               | "," | ";" | ":" | "\" | <">
	               | "/" | "[" | "]" | "?" | "="
	               | "{" | "}" | SP | HT
*/

	REFUSE("");

	std::string str;
	for(int c=0; c<256; ++c) {
//		std::cout << std::hex << c << std::endl;

		str.assign(1, static_cast<char>(c));
		bool acceptable = false;

		if(c<=0x1f || c==0x7f) {
			// CTL
			acceptable = false;
		} else if(c>=0x80) {
			// not CHAR
			acceptable = false;
		} else {
			switch (c) {
				case '(':	case ')':
				case '<':	case '>':
				case '@':	case ',':
				case ';':	case ':':
				case '\\':	case '"':
				case '/':
				case '[':	case ']':
				case '?':	case '=':
				case '{':	case '}':
				case ' ':	case '\t':
					acceptable = false;
					break;
				default:
					acceptable = true;
					break;
			}
		}
		if(acceptable) {
			ACCEPT(str);
		} else {
			REFUSE(str);
		}
	}


	for(int c=0; c<=0x80; ++c) {
		for(int c2=0; c2<=0x80; ++c2) {
//			std::cout << std::hex << c << "," << c2 << std::endl;

			str.assign(1, static_cast<char>(c)).append(1, static_cast<char>(c2));
			bool acceptable = false;

			if(c<=0x1f || c==0x7f) {
				// CTL
				acceptable = false;
			} else if(c>=0x80) {
				// not CHAR
				acceptable = false;
			} else {
				switch (c) {
					case '(':	case ')':
					case '<':	case '>':
					case '@':	case ',':
					case ';':	case ':':
					case '\\':	case '"':
					case '/':
					case '[':	case ']':
					case '?':	case '=':
					case '{':	case '}':
					case ' ':	case '\t':
						acceptable = false;
						break;
					default:
						acceptable = true;
						break;
				}
			}

			if(c2<=0x1f || c2==0x7f) {
				// CTL
				acceptable &= false;
			} else if(c2>=0x80) {
				// not CHAR
				acceptable &= false;
			} else {
				switch (c2) {
					case '(':	case ')':
					case '<':	case '>':
					case '@':	case ',':
					case ';':	case ':':
					case '\\':	case '"':
					case '/':
					case '[':	case ']':
					case '?':	case '=':
					case '{':	case '}':
					case ' ':	case '\t':
						acceptable &= false;
						break;
					default:
						acceptable &= true;
						break;
				}
			}

		if(acceptable) {
			ACCEPT(str);
		} else {
			REFUSE(str);
		}
		}
	}
}

DEFINE_TEST(pct_encoded)
{
	REFUSE("%");

	REFUSE("%4");
	REFUSE("%A");

	REFUSE("%k");

	ACCEPT("%00");
	ACCEPT("%3f");
	ACCEPT("%F9");
	ACCEPT("%3a");
	ACCEPT("%e2");
	REFUSE("%9g");

	REFUSE("%9e3");
	REFUSE("%392");
}

DEFINE_TEST(dec_octet)
{
/*
	 dec-octet   = DIGIT                 ; 0-9
                  / %x31-39 DIGIT         ; 10-99
                  / "1" 2DIGIT            ; 100-199
                  / "2" %x30-34 DIGIT     ; 200-249
                  / "25" %x30-35          ; 250-255
*/

	ACCEPT("0");
	ACCEPT("9");
	REFUSE("A");
	REFUSE("a");

	REFUSE("01");
	ACCEPT("10");
	ACCEPT("99");
	REFUSE("A9");

	REFUSE("001");
	REFUSE("010");
	ACCEPT("100");
	ACCEPT("199");
	ACCEPT("200");
	ACCEPT("255");
	REFUSE("256");
	REFUSE("260");

	REFUSE("300");
	REFUSE("999");

}

DEFINE_TEST(parameter)
{
/*
		parameter               = attribute "=" value
		attribute               = token
		value                   = token | quoted-string

		token          = 1*<any CHAR Expect CTLs or separators>
		separators     = "(" | ")" | "<" | ">" | "@"
                      | "," | ";" | ":" | "\" | <">
                      | "/" | "[" | "]" | "?" | "="
                      | "{" | "}" | SP | HT

		quoted-string  = ( <"> *(qdtext | quoted-pair ) <"> )
*/

	ACCEPT("abc=def");
	ACCEPT("abc=\"def\"");
	ACCEPT("abc=\"de!#$%&'()~=|'@[`{:]*},./\\ <>?_f\"");

	REFUSE("=def");
	REFUSE("a(bc=def");
	REFUSE("a)bc=def");
	REFUSE("a<bc=def");
	REFUSE("a>bc=def");
	REFUSE("a@bc=def");
	REFUSE("a,bc=def");
	REFUSE("a;bc=def");
	REFUSE("a:bc=def");
	REFUSE("a\\bc=def");
	REFUSE("a\"bc=def");
	REFUSE("a/bc=def");
	REFUSE("a[bc=def");
	REFUSE("a]bc=def");
	REFUSE("a?bc=def");
	REFUSE("a{bc=def");
	REFUSE("a}bc=def");
	REFUSE("a(bc=def");
	REFUSE("a bc=def");
	REFUSE("a\tbc=def");

	REFUSE("abc=d\"e\"f");
	REFUSE("abc=de=f");

}

DEFINE_TEST(product)
{
/*
		product         = token ["/" product-version]
		product-version = token
 */
	ACCEPT("abc");
	ACCEPT("abc/def");
	REFUSE("abc/\"def\"");
	REFUSE("abc/\"de!#$%&'()~=|'@[`{:]*},./\\ <>?_f\"");

	REFUSE("/def");
	REFUSE("a(bc/def");
	REFUSE("a)bc/def");
	REFUSE("a<bc/def");
	REFUSE("a>bc/def");
	REFUSE("a@bc/def");
	REFUSE("a,bc/def");
	REFUSE("a;bc/def");
	REFUSE("a:bc/def");
	REFUSE("a\\bc/def");
	REFUSE("a\"bc/def");
	REFUSE("a/bc/def");
	REFUSE("a[bc/def");
	REFUSE("a]bc/def");
	REFUSE("a?bc/def");
	REFUSE("a{bc/def");
	REFUSE("a}bc/def");
	REFUSE("a(bc/def");
	REFUSE("a bc/def");
	REFUSE("a\tbc/def");

	REFUSE("def/");
	REFUSE("def/a(bc");
	REFUSE("def/a)bc");
	REFUSE("def/a<bc");
	REFUSE("def/a>bc");
	REFUSE("def/a@bc");
	REFUSE("def/a,bc");
	REFUSE("def/a;bc");
	REFUSE("def/a:bc");
	REFUSE("def/a\\bc");
	REFUSE("def/a\"bc");
	REFUSE("def/a/bc");
	REFUSE("def/a[bc");
	REFUSE("def/a]bc");
	REFUSE("def/a?bc");
	REFUSE("def/a{bc");
	REFUSE("def/a}bc");
	REFUSE("def/a(bc");
	REFUSE("def/a bc");
	REFUSE("def/a\tbc");

}

DEFINE_TEST(ctext)
{
/*
		ctext          = <any TEXT excluding "(" and ")">
		TEXT           = <any OCTET Expect CTLs,
                        but including LWS>
*/

	for(int c=0; c<256; ++c) {
		std::string str;
		str += static_cast<char>(c);

		if(c<0x09 || (0x09<c && c<0x20) || c==0x28/*'('*/ || c==0x29/*')'*/ || c==0x7f) {
			// TEXT accepts HT(0x09), since LWS includes it;
			REFUSE(str);
		} else {
			ACCEPT(str);
		}
	}

	ACCEPT("          ");
	ACCEPT("\t\t\t\t\t\t\t\t");
	ACCEPT("\t \t  \t   \t    \t     \t\t     \t");

	ACCEPT("\r\n          ");
	ACCEPT("\r\n\t\t\t\t\t\t\t\t");
	ACCEPT("\r\n\t \t  \t   \t    \t     \t\t     \t");

	REFUSE("\r\n");
	REFUSE(" \r\n ");
	REFUSE("\t\r\n ");
	REFUSE("\t\r\n\t");
	REFUSE("\t\r\n\t");
}

// TODO: do "comment" Test
#if 0
DEFINE_TEST(comment)
{
/*
	comment        = "(" *( ctext | quoted-pair | comment ) ")"
*/

	ACCEPT("()");

	ACCEPT("(\\))");
	ACCEPT("( \\))");
	ACCEPT("(\\) )");
	ACCEPT("( \\) )");

	ACCEPT("(\\)");
	ACCEPT("( \\)");
	ACCEPT("(\\ )");

	ACCEPT("(())");
	ACCEPT("(\\())");
	ACCEPT("((\\))");
	ACCEPT("(()\\)");

	ACCEPT("(()())");
	ACCEPT("(\\()())");
	ACCEPT("((\\)())");
	ACCEPT("(()\\())");
	ACCEPT("(()(\\))");
	ACCEPT("(()()\\)");

	ACCEPT("( ()())");
	ACCEPT("(( )())");
	ACCEPT("(() ())");
	ACCEPT("(()( ))");
	ACCEPT("(()() )");

	ACCEPT("( \\()())");
	ACCEPT("(\\( )())");
	ACCEPT("(\\() ())");

	ACCEPT("( (\\)())");
	ACCEPT("(( \\)())");
	ACCEPT("((\\) ())");

	ACCEPT("(( )\\())");
	ACCEPT("(() \\())");
	ACCEPT("(()\\( ))");

	ACCEPT("(()(\\))");
	ACCEPT("(() (\\))");
	ACCEPT("(()( \\))");
	ACCEPT("(()(\\) )");

	ACCEPT("(()( )\\)");
	ACCEPT("(()() \\)");
	ACCEPT("(()()\\)");

	ACCEPT("((())())");
	ACCEPT("(abc \t \r\n \t\r\n \t\"\\)\\()");
	ACCEPT("(\\)\\()");
	ACCEPT("(abc \t \r\n \t\r\n \t\"\\)\\(()((((abc \t \"\\(\\)()())))))");

	REFUSE("()(())(");
	REFUSE("()(()()");

	REFUSE("(\r\n)");
	REFUSE("(\n \t)");

}
#endif

DEFINE_TEST(date1)
{
/*
		date1        = 2DIGIT SP month SP 4DIGIT
	                      ; day month year (e.g., 02 Jun 1982)
*/

	ACCEPT("02 Jun 1982");
	ACCEPT("11 Aug 2000");
	ACCEPT("01 Jan 1970");
	ACCEPT("25 Dec 0000");

	// illegal but acceptable date
	ACCEPT("00 Jan 0000");
	ACCEPT("99 Dec 9999");
	ACCEPT("30 Feb 1999");

	REFUSE("1982 Jun 02");

	REFUSE("1 Jan 2000");
	REFUSE("111 Jan 2000");
	REFUSE("10 AAA 2000");
	REFUSE("01 Jn 2000");
	REFUSE("01 jan 2000");
	REFUSE("01 Jan 200");
	REFUSE("01 Jan 20000");

	REFUSE("02Jun 1982");
	REFUSE("02  Jun 1982");

	REFUSE("02 Jun  1982");
	REFUSE("02 Jun1982");

	REFUSE("02-Jun-1982");
	REFUSE("02/Jun/1982");
}

DEFINE_TEST(date2)
{
/*
	date2        = 2DIGIT "-" month "-" 2DIGIT
	                      ; day-month-year (e.g., 02-Jun-82)
*/
	ACCEPT("02-Jun-82");

	ACCEPT("11-Aug-00");
	ACCEPT("01-Jan-70");
	ACCEPT("25-Dec-00");

	// illegal but acceptable date
	ACCEPT("00-Jan-00");
	ACCEPT("99-Dec-99");
	ACCEPT("30-Feb-99");

	REFUSE("1-Jan-00");
	REFUSE("111-Jan-00");
	REFUSE("10-AAA-00");
	REFUSE("01-Jn-00");
	REFUSE("01-jan-00");
	REFUSE("01-Jan-0");
	REFUSE("01-Jan-999");

	REFUSE("02Jun-82");
	REFUSE("02--Jun-82");

	REFUSE("02-Jun--82");
	REFUSE("02-Jun82");

	REFUSE("02 Jun 82");
	REFUSE("02/Jun/82");
}

DEFINE_TEST(date3)
{
/*
	date3        = month SP ( 2DIGIT | ( SP 1DIGIT ))
                   ; month day (e.g., Jun  2)
*/

	ACCEPT("Jun  2");
	ACCEPT("Jun 02");

	ACCEPT("Aug 11");
	ACCEPT("Jan  1");
	ACCEPT("Dec 25");

	// illegal but acceptable date
	ACCEPT("Jan  0");
	ACCEPT("Jan 00");
	ACCEPT("Dec 99");
	ACCEPT("Feb 30");

	REFUSE("Jan 1");
	REFUSE("Jan 111");
	REFUSE("AAA 01");
	REFUSE("Jn 10");
	REFUSE("jan 10");

	REFUSE("Jun 1");
	REFUSE("Jun  01");

	REFUSE("Jun-02");
	REFUSE("Jun/02");
}


DEFINE_TEST(time)
{
/*
		time         = 2DIGIT ":" 2DIGIT ":" 2DIGIT
                      ; 00:00:00 - 23:59:59
*/
	ACCEPT("00:00:00");
	ACCEPT("23:59:59");

	// illegal but acceptable time
	ACCEPT("99:99:99");

	REFUSE("00:00");

	REFUSE("0:00:00");
	REFUSE("000:00:00");
	REFUSE("00:0:00");
	REFUSE("00:000:00");
	REFUSE("00:00:0");
	REFUSE("00:00:000");

	REFUSE("00:00:00:");
	REFUSE(":00:00:00");

	REFUSE("00-00-00");
	REFUSE("23 59 59");
}

DEFINE_TEST(rfc1123_date)
{
/*
		rfc1123-date = wkday "," SP date1 SP time SP "GMT"
		date1        = 2DIGIT SP month SP 4DIGIT
	                      ; day month year (e.g., 02 Jun 1982)
*/

	ACCEPT("Sun, 06 Nov 1994 08:49:37 GMT");
	ACCEPT("Mon, 01 Jan 0000 00:00:00 GMT");
	ACCEPT("Sat, 31 Dec 9999 23:59:59 GMT");

	// illegal but acceptable time
	ACCEPT("Tue, 00 Jan 0000 00:00:00 GMT");
	ACCEPT("Wed, 99 Dec 9999 99:99:99 GMT");

	REFUSE("Sun 06 Nov 1994 08:49:37 GMT");
	REFUSE("Sun, 06 Nov 1994 08:49:37");
	REFUSE("Sun, 06 Nov 1994 08:49 GMT");
	REFUSE("Sun, 6 Nov 1994 08:49:37 GMT");
	REFUSE("Sun, 06 Nov 94 08:49:37 GMT");
	REFUSE("Sun, 06 Nov 1994 8:49:37 GMT");
	REFUSE("Sun, 06 Nov 1994 08:9:37 GMT");
	REFUSE("Sun, 06 Nov 1994 08:49:7 GMT");
	REFUSE("Sun, 06 Nov 1994 08:49:37 EST");
	REFUSE("Sun, 02-Jun-82 08:49:37 GMT");
}

DEFINE_TEST(rfc850_date)
{
/*
		rfc850-date  = weekday "," SP date2 SP time SP "GMT"
		date2        = 2DIGIT "-" month "-" 2DIGIT
                      ; day-month-year (e.g., 02-Jun-82)
*/

	ACCEPT("Sunday, 06-Nov-94 08:49:37 GMT");
	ACCEPT("Monday, 01-Jan-00 00:00:00 GMT");
	ACCEPT("Saturday, 31-Dec-99 23:59:59 GMT");

	// illegal but acceptable time
	ACCEPT("Tuesday, 00-Jan-00 00:00:00 GMT");
	ACCEPT("Wednesday, 99-Dec-99 99:99:99 GMT");

	REFUSE("Sanday, 06-Nov-94 08:49:37 GMT");
	REFUSE("Sunday 06-Nov-94 08:49:37 GMT");
	REFUSE("Sunday, 06-Nov-94 08:49:37");
	REFUSE("Sunday, 6-Nov-94 08:49:37 GMT");
	REFUSE("Sunday, 06-Nov-1994 08:49:37 GMT");
	REFUSE("Sunday, 06-Nov-94 8:49:37 GMT");
	REFUSE("Sunday, 06-Nov-94 08:9:37 GMT");
	REFUSE("Sunday, 06-Nov-94 08:49:7 GMT");
}

DEFINE_TEST(asctime_date)
{
/*
		asctime-date = wkday SP date3 SP time SP 4DIGIT
		date3        = month SP ( 2DIGIT | ( SP 1DIGIT ))
                      ; month day (e.g., Jun  2)
*/

	ACCEPT("Sun Nov  6 08:49:37 1994");
	ACCEPT("Mon Jan 01 00:00:00 0000");
	ACCEPT("Sat Dec 31 23:59:59 9999");

	// illegal but acceptable time
	ACCEPT("Tue Jan 00 00:00:00 0000");
	ACCEPT("Wed Dec 99 99:99:99 9999");

	REFUSE("Sun, Nov 06 08:49:37 1994");
	REFUSE("Sun Nov 6 08:49:37 1994");
	REFUSE("Sun Nov 26 8:49:37 1994");
	REFUSE("Sun Nov 26 08:9:37 1994");
	REFUSE("Sun Nov 26 8:49:7 1994");
	REFUSE("Sun Nov 26 8:49:37 94");
	REFUSE("Sun Nov 26 8:49:37 1994 GMT");
	REFUSE("Sun Nov 26 8:49:37 1994 EST");
	REFUSE("San Nov 26 08:49:37 1994");

}

DEFINE_TEST(HTTP_date)
{
/*
		HTTP-date    = rfc1123-date | rfc850-date | asctime-date
*/

	// rfc1123-date
	ACCEPT("Sun, 06 Nov 1994 08:49:37 GMT");
	ACCEPT("Mon, 01 Jan 0000 00:00:00 GMT");
	ACCEPT("Sat, 31 Dec 9999 23:59:59 GMT");

	// illegal but acceptable time
	ACCEPT("Tue, 00 Jan 0000 00:00:00 GMT");
	ACCEPT("Wed, 99 Dec 9999 99:99:99 GMT");

	REFUSE("Sun 06 Nov 1994 08:49:37 GMT");
	REFUSE("Sun, 06 Nov 1994 08:49:37");
	REFUSE("Sun, 06 Nov 1994 08:49 GMT");
	REFUSE("Sun, 6 Nov 1994 08:49:37 GMT");
	REFUSE("Sun, 06 Nov 94 08:49:37 GMT");
	REFUSE("Sun, 06 Nov 1994 8:49:37 GMT");
	REFUSE("Sun, 06 Nov 1994 08:9:37 GMT");
	REFUSE("Sun, 06 Nov 1994 08:49:7 GMT");
	REFUSE("Sun, 06 Nov 1994 08:49:37 EST");
	REFUSE("Sun, 02-Jun-82 08:49:37 GMT");

	// rfc850-date
	ACCEPT("Sunday, 06-Nov-94 08:49:37 GMT");
	ACCEPT("Monday, 01-Jan-00 00:00:00 GMT");
	ACCEPT("Saturday, 31-Dec-99 23:59:59 GMT");

	// illegal but acceptable time
	ACCEPT("Tuesday, 00-Jan-00 00:00:00 GMT");
	ACCEPT("Wednesday, 99-Dec-99 99:99:99 GMT");

	REFUSE("Sanday, 06-Nov-94 08:49:37 GMT");
	REFUSE("Sunday 06-Nov-94 08:49:37 GMT");
	REFUSE("Sunday, 06-Nov-94 08:49:37");
	REFUSE("Sunday, 6-Nov-94 08:49:37 GMT");
	REFUSE("Sunday, 06-Nov-1994 08:49:37 GMT");
	REFUSE("Sunday, 06-Nov-94 8:49:37 GMT");
	REFUSE("Sunday, 06-Nov-94 08:9:37 GMT");
	REFUSE("Sunday, 06-Nov-94 08:49:7 GMT");

	// asctime-date
	ACCEPT("Sun Nov  6 08:49:37 1994");
	ACCEPT("Mon Jan 01 00:00:00 0000");
	ACCEPT("Sat Dec 31 23:59:59 9999");

	// illegal but acceptable time
	ACCEPT("Tue Jan 00 00:00:00 0000");
	ACCEPT("Wed Dec 99 99:99:99 9999");

	REFUSE("Sun, Nov 06 08:49:37 1994");
	REFUSE("Sun Nov 6 08:49:37 1994");
	REFUSE("Sun Nov 26 8:49:37 1994");
	REFUSE("Sun Nov 26 08:9:37 1994");
	REFUSE("Sun Nov 26 8:49:7 1994");
	REFUSE("Sun Nov 26 8:49:37 94");
	REFUSE("Sun Nov 26 8:49:37 1994 GMT");
	REFUSE("Sun Nov 26 8:49:37 1994 EST");
	REFUSE("San Nov 26 08:49:37 1994");

}

DEFINE_TEST(qvalue)
{
/*
		 qvalue         = ( "0" [ "." 0*3DIGIT ] )
	                      | ( "1" [ "." 0*3("0") ] )
*/

	ACCEPT("0");
	ACCEPT("0.");
	ACCEPT("0.0");
	ACCEPT("0.00");
	ACCEPT("0.000");

	ACCEPT("0.1");
	ACCEPT("0.12");
	ACCEPT("0.321");
	ACCEPT("0.90");

	ACCEPT("1");
	ACCEPT("1.");
	ACCEPT("1.0");
	ACCEPT("1.00");
	ACCEPT("1.000");

	REFUSE("0.1234");
	REFUSE("1.1");
	REFUSE("2");
}

DEFINE_TEST(field_content)
{
/*
		field-content  = <the OCTETs making up the field-value
	                        and consisting of either *TEXT or combinations
	                        of token, separators, and quoted-string>

		token          = 1*<any CHAR Expect CTLs or separators>
		separators     = "(" | ")" | "<" | ">" | "@"
	               | "," | ";" | ":" | "\" | <">
	               | "/" | "[" | "]" | "?" | "="
	               | "{" | "}" | SP | HT
		TEXT           = <any OCTET Expect CTLs, but including LWS>
		CTL            = <any US-ASCII control character
    					(octets 0 - 31) and DEL (127)>
		quoted-string  = ( <"> *(qdtext | quoted-pair ) <"> )
*/

	ACCEPT("");
	ACCEPT("abc");

	ACCEPT("contain token and \r\n\t LWS");
	ACCEPT("contain separators ()<>@,;:\\\"/[]?={} \t");
	ACCEPT("contain \"quoted\\\" string\"");

	for(int c=0; c<256; ++c) {
		std::string str;
		str += static_cast<char>(c);

		if(c<0x09 || (0x09<c && c<0x20) || c==0x7f) {
			// TEXT accepts HT(0x09), since LWS includes it;
			REFUSE(str);
		} else {
			ACCEPT(str);
		}
	}

	ACCEPT("          ");
	ACCEPT("\t\t\t\t\t\t\t\t");
	ACCEPT("\t \t  \t   \t    \t     \t\t     \t");

	ACCEPT("\r\n          ");
	ACCEPT("\r\n\t\t\t\t\t\t\t\t");
	ACCEPT("\r\n\t \t  \t   \t    \t     \t\t     \t");

	REFUSE("\r\n");
	ACCEPT(" \r\n ");
	ACCEPT("\t\r\n ");
	ACCEPT("\t\r\n\t");
	ACCEPT("\t\r\n\t");

}

DEFINE_TEST(message_header)
{
/*
		message-header = field-name ":" [ field-value ]
*/
	ACCEPT("header:");
	ACCEPT("header:value");
	ACCEPT("header: value");
	ACCEPT("header:\r\n value");

	ACCEPT("header: : : ");
}

DEFINE_TEST(media_range)
{
// media-range    = ( "*/*"
//                  | ( type "/" "*" )
//                  | ( type "/" subtype )
//                  ) *( ";" parameter )

	ACCEPT("*/*");
	ACCEPT("*/*;aaa=bbb;ccc=ddd");

	ACCEPT("type/*");
	ACCEPT("type/*;aaa=bbb;ccc=ddd");

	ACCEPT("type/subtype");
	ACCEPT("type/subtype;aaa=bbb;ccc=ddd");

	ACCEPT("**/*");
	ACCEPT("**/*;aaa=bbb;ccc=ddd");

	ACCEPT("*/**");
	ACCEPT("*/**;aaa=bbb;ccc=ddd");

	ACCEPT("**/**");
	ACCEPT("**/**;aaa=bbb;ccc=ddd");
}

DEFINE_TEST(credentials)
{
/*
		credentials = auth-scheme #auth-param
 		auth-scheme    = token
		auth-param     = token "=" ( token | quoted-string )
*/

	ACCEPT("aaa");
	ACCEPT("aaa bbb=ccc");
	ACCEPT("aaa bbb=ccc  , ddd=eee");
	ACCEPT("aaa bbb=ccc\r\n ,\r\n ddd=eee");
	ACCEPT("aaa bbb=\"ccc\r\n ,\r\n ddd=eee\"");

	REFUSE("aaa bbb=ccc ddd=eee");
	REFUSE("aaa bbb=\"ccc\r\n ,\r\n ddd\"=eee");
	REFUSE("aaa bbb=\"ccc\r\n ,\r\n ddd=\"eee");
}

DEFINE_TEST(language_tag)
{
/*
		language-tag  = primary-tag *( "-" subtag )
        primary-tag   = 1*8ALPHA
        subtag        = 1*8ALPHA
*/

	REFUSE("");
	ACCEPT("a");
	ACCEPT("A");
	ACCEPT("aB");
	ACCEPT("aBc");
	ACCEPT("aBcd");
	ACCEPT("aBcde");
	ACCEPT("aBcdef");
	ACCEPT("aBcdefg");
	ACCEPT("aBcdefgh");
	REFUSE("aBcdefghi");

	REFUSE("aBcdefgh-");
	ACCEPT("aBcdefgh-a");
	ACCEPT("aBcdefgh-A");
	ACCEPT("aBcdefgh-aB");
	ACCEPT("aBcdefgh-aBc");
	ACCEPT("aBcdefgh-aBcd");
	ACCEPT("aBcdefgh-aBcde");
	ACCEPT("aBcdefgh-aBcdef");
	ACCEPT("aBcdefgh-aBcdefg");
	ACCEPT("aBcdefgh-aBcdefgh");
	REFUSE("aBcdefgh-aBcdefghi");

	REFUSE("aBcdefgh-aBcdefgh-");
	ACCEPT("aBcdefgh-aBcdefgh-a");
	ACCEPT("aBcdefgh-aBcdefgh-A");
	ACCEPT("aBcdefgh-aBcdefgh-aB");
	ACCEPT("aBcdefgh-aBcdefgh-aBc");
	ACCEPT("aBcdefgh-aBcdefgh-aBcd");
	ACCEPT("aBcdefgh-aBcdefgh-aBcde");
	ACCEPT("aBcdefgh-aBcdefgh-aBcdef");
	ACCEPT("aBcdefgh-aBcdefgh-aBcdefg");
	ACCEPT("aBcdefgh-aBcdefgh-aBcdefgh");
	REFUSE("aBcdefgh-aBcdefgh-aBcdefghi");

	REFUSE("123");
}

DEFINE_TEST(query)
{
/*
		query       = *( pchar / "/" / "?" )
		pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
		pct-encoded   = "%" HEXDIG HEXDIG
		unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
		sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
                 	  / "*" / "+" / "," / ";" / "="
*/
	ACCEPT("");
	ACCEPT("?abc=def&efg=%ab%cd");
	ACCEPT("?abc=&=%ab%cd");

	ACCEPT("abcABC123-._~%ab!$&'()*+,;=:@/?");

	REFUSE("abc%ag");
	REFUSE("ac%a!");

	REFUSE("?abc=def&efg=%b%cd");
	REFUSE("?abc=def&efg=%Hb%cd");
}

DEFINE_TEST(path_abempty)
{
/*
		path-abempty  = *( "/" segment )
		segment       = *pchar
		pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
*/

	ACCEPT("");
	ACCEPT("/");
	ACCEPT("/path/to/some-file");
	ACCEPT("/path/to/some-dir/");
	ACCEPT("//./../");

	ACCEPT("/path:to/some@dir");
	ACCEPT("/ab%aa%bbcc/~_.-/!$&'()*+,;=");

	REFUSE("relative-path");
	REFUSE("relative/path");

	REFUSE("/%aG/aa");
	REFUSE("/%a/aa");
}

DEFINE_TEST(path_absolute)
{
/*
		path-absolute = "/" [ segment-nz *( "/" segment ) ]
		segment-nz    = 1*pchar
		segment       = *pchar
		pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
*/
	ACCEPT("/");
	ACCEPT("/path:to/some@dir");
	ACCEPT("/./../");
	ACCEPT("/:/first/segment/can/include/colons");
	ACCEPT("/ab%aa%bbcc/~_.-!$&'()*+,;=");

	REFUSE("");
	REFUSE("//");
	REFUSE("relative/path");

	REFUSE("/%aG/aa");
	REFUSE("/%a/aa");
}

DEFINE_TEST(path_noscheme)
{
/*
		path-noscheme = segment-nz-nc *( "/" segment )
		segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
	 	 	 	 	 	 ; non-zero-length segment without any colon ":"
		segment       = *pchar
		pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
*/

	REFUSE("");
	REFUSE("/");
	REFUSE("//");
	REFUSE("/a//");
	REFUSE("/./../");

	ACCEPT("relative-path");

	ACCEPT("a@b//");
	ACCEPT("./../");
	REFUSE(":/first/segment/can/not/include/colons");

	ACCEPT("path/:to/some@dir");
	ACCEPT("ab%aa%bbcc/~_.-/!$&'()*+,;=");

	REFUSE("%aG/aa");
	REFUSE("%a/aa");
}

DEFINE_TEST(path_rootless)
{
/*
		path-rootless = segment-nz *( "/" segment )
		segment-nz    = 1*pchar
		segment       = *pchar
		pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
*/

	REFUSE("");
	REFUSE("/");
	REFUSE("//");
	REFUSE("/a//");
	REFUSE("/./../");

	ACCEPT("relative-path");
	ACCEPT(":/first/segment/can/include/colons");

	ACCEPT("a@b//");
	ACCEPT("./../");
	ACCEPT("path:to/../some@dir");
	ACCEPT("path/:to/some@dir");
	ACCEPT("ab%aa%bbcc/~_.-/!$&'()*+,;=");

	REFUSE("%aG/aa");
	REFUSE("%a/aa");
}

DEFINE_TEST(IPv6address)
{
/*
      IPv6address =                            6( h16 ":" ) ls32
                  /                       "::" 5( h16 ":" ) ls32
                  / [               h16 ] "::" 4( h16 ":" ) ls32
                  / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
                  / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
                  / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
                  / [ *4( h16 ":" ) h16 ] "::"              ls32
                  / [ *5( h16 ":" ) h16 ] "::"              h16
                  / [ *6( h16 ":" ) h16 ] "::"

      ls32        = ( h16 ":" h16 ) / IPv4address
                  ; least-significant 32 bits of address

      h16         = 1*4HEXDIG
                  ; 16 bits of address represented in hexadecimal
*/

	REFUSE("");

	ACCEPT("0:0a:0Ab:0Abc:9999:0000:F:aaaa");
	ACCEPT("0:0a:0Ab:0Abc:9999:0000:0.10.100.255");
	REFUSE("a:0:0a:0Ab:0Abc:9999:0000:F:aaaa");
	REFUSE("a:0:0a:0Ab:0Abc:9999:0000:0.10.100.255");
	REFUSE("0:0a:0Ab:0Abc:99999:0000:F:aaaa");
	REFUSE("0:0a:0Ab:0Abc:99999:0000:0.10.100.255");

	ACCEPT("::0a:0Ab:0Abc:9999:0000:F:aaaa");
	ACCEPT("::0a:0Ab:0Abc:9999:0000:0.10.100.255");
	REFUSE("a::0a:0Ab:0Abc:9999:0000:F:aaaa");
	REFUSE("a::0a:0Ab:0Abc:9999:0000:0.10.100.255");
	REFUSE("::a:0a:0Ab:0Abc:9999:0000:F:aaaa");
	REFUSE("::a:0a:0Ab:0Abc:9999:0000:0.10.100.255");

	ACCEPT("::0a:0Abc:9999:0000:F:aaaa");
	ACCEPT("::0a:0Abc:9999:0000:0.10.100.255");
	ACCEPT("ab::0a:0Abc:9999:0000:F:aaaa");
	ACCEPT("ab::0a:0Abc:9999:0000:0.10.100.255");
	REFUSE("0000:ab::0a:0Abc:9999:0000:F:aaaa");
	REFUSE("0000:ab::0a:0Abc:9999:0000:0.10.100.255");

	ACCEPT("::0Abc:9999:0000:F:aaaa");
	ACCEPT("::0Abc:9999:0000:0.10.100.255");
	ACCEPT("ab::0Abc:9999:0000:F:aaaa");
	ACCEPT("ab::0Abc:9999:0000:0.10.100.255");
	ACCEPT("ab:00::0Abc:9999:0000:F:aaaa");
	ACCEPT("ab:00::0Abc:9999:0000:0.10.100.255");
	REFUSE("0000:ab:00::0Abc:9999:0000:F:aaaa");
	REFUSE("0000:ab:00::0Abc:9999:0000:0.10.100.255");

	ACCEPT("::0Abc:9999:F:aaaa");
	ACCEPT("::0Abc:9999:0.10.100.255");
	ACCEPT("ab::0Abc:9999:F:aaaa");
	ACCEPT("ab::0Abc:9999:0.10.100.255");
	ACCEPT("ab:00::0Abc:9999:F:aaaa");
	ACCEPT("ab:00::0Abc:9999:0.10.100.255");
	ACCEPT("ab:00:abcd::0Abc:9999:F:aaaa");
	ACCEPT("ab:00:abcd::0Abc:9999:0.10.100.255");
	REFUSE("0000:ab:00:abcd::0Abc:9999:F:aaaa");
	REFUSE("0000:ab:00:abcd::0Abc:9999:0.10.100.255");

	ACCEPT("::0Abc:F:aaaa");
	ACCEPT("::0Abc:0.10.100.255");
	ACCEPT("ab::0Abc:F:aaaa");
	ACCEPT("ab::0Abc:0.10.100.255");
	ACCEPT("ab:00::0Abc:F:aaaa");
	ACCEPT("ab:00::0Abc:0.10.100.255");
	ACCEPT("ab:00:abcd::0Abc:F:aaaa");
	ACCEPT("ab:00:abcd::0Abc:0.10.100.255");
	ACCEPT("ab:00:abcd:b::0Abc:F:aaaa");
	ACCEPT("ab:00:abcd:b::0Abc:0.10.100.255");
	REFUSE("0000:ab:00:abcd:b::0Abc:F:aaaa");
	REFUSE("0000:ab:00:abcd:b::0Abc:0.10.100.255");

	ACCEPT("::F:aaaa");
	ACCEPT("::0.10.100.255");
	ACCEPT("ab::F:aaaa");
	ACCEPT("ab::0.10.100.255");
	ACCEPT("ab:00::F:aaaa");
	ACCEPT("ab:00::0.10.100.255");
	ACCEPT("ab:00:abcd::F:aaaa");
	ACCEPT("ab:00:abcd::0.10.100.255");
	ACCEPT("ab:00:abcd:b::F:aaaa");
	ACCEPT("ab:00:abcd:b::0.10.100.255");
	ACCEPT("ab:00:abcd:b:13a::F:aaaa");
	ACCEPT("ab:00:abcd:b:13a::0.10.100.255");
	REFUSE("0000:ab:00:abcd:b:13a::F:aaaa");
	REFUSE("0000:ab:00:abcd:b:13a::0.10.100.255");

}

DEFINE_TEST(authority)
{
/*
		authority     = server | reg_name
		reg_name      = 1*( unreserved | escaped | "$" | "," |
                          ";" | ":" | "@" | "&" | "=" | "+" )
		server        = [ [ userinfo "@" ] hostport ]
		userinfo      = *( unreserved | escaped |
                         ";" | ":" | "&" | "=" | "+" | "$" | "," )
		hostport      = host [ ":" port ]
		host          = hostname | IPv4address
		hostname      = *( domainlabel "." ) toplabel [ "." ]
		domainlabel   = alphanum | alphanum *( alphanum | "-" ) alphanum
		toplabel      = alpha | alpha *( alphanum | "-" ) alphanum
		reserved      = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" |
                      "$" | ","
		unreserved    = alphanum | mark
		mark          = "-" | "_" | "." | "!" | "~" | "*" | "'" |
                      "(" | ")"
		escaped       = "%" hex hex
*/
	ACCEPT("");
	ACCEPT("some-host");
	ACCEPT("user@some-host");
	ACCEPT("user:pass@some-host");
	ACCEPT("some-host:1234");
	ACCEPT("user@some-host:1234");
	ACCEPT("user:pass@some-host:1234");
	ACCEPT("@some-host:");
	ACCEPT("user:pass@some-host:");
	ACCEPT("@some-host");

	ACCEPT("@:");

	ACCEPT("some-host:12AB");
}


DEFINE_TEST(absoluteURI)
{
/*
		absoluteURI   = scheme ":" ( hier_part | opaque_part )
		hier_part     = ( net_path | abs_path ) [ "?" query ]
		net_path      = "//" authority [ abs_path ]
		abs_path      = "/"  path_segments
		path_segments = segment *( "/" segment )
		segment       = *pchar *( ";" param )
		opaque_part   = uric_no_slash *uric
		uric_no_slash = unreserved | escaped | ";" | "?" | ":" | "@" |
                      "&" | "=" | "+" | "$" | ","

*/
	ACCEPT("scheme://some-host");
	ACCEPT("scheme://user:path@some-host/");
	ACCEPT("scheme://user:path@some-host/path/to/file");
	ACCEPT("scheme://user:path@some-host/path/to/dir/");
	ACCEPT("scheme://user:path@some-host/path/to/dir/?query");

	ACCEPT("scheme:/path/to/absolute/file");
	ACCEPT("scheme:/path/to/absolute/dir/");
	ACCEPT("scheme:/path/to/absolute/dir/?query");

	ACCEPT("scheme:relative/path");
	ACCEPT("scheme::/first/segment/can/include/colons");
	ACCEPT("scheme::/first/segment/can/include/colons?query");

	REFUSE("scheme:");
	ACCEPT("scheme:?query");
}

DEFINE_TEST(relativeURI)
{
/*
		relativeURI   = ( net_path | abs_path | rel_path ) [ "?" query ]
		net_path      = "//" authority [ abs_path ]
		abs_path      = "/"  path_segments
		rel_path      = rel_segment [ abs_path ]
     	rel_segment   = 1*( unreserved | escaped |
                          ";" | "@" | "&" | "=" | "+" | "$" | "," )

*/

	ACCEPT("//some-host");
	ACCEPT("//user:path@some-host/");
	ACCEPT("//user:path@some-host/path/to/file");
	ACCEPT("//user:path@some-host/path/to/dir/");
	ACCEPT("//user:path@some-host/path/to/dir/?query");

	ACCEPT("/path/to/absolute/file");
	ACCEPT("/path/to/absolute/dir/");
	ACCEPT("/path/to/absolute/dir/?query");

	ACCEPT("relative/path");
	REFUSE(":/first/segment/can/not/include/colons");
	REFUSE(":/first/segment/can/not/include/colons?query");

	REFUSE("");
	REFUSE("?query");
}

DEFINE_TEST(Request_URI)
{
/*
		Request-URI    = "*" | absoluteURI | abs_path | authority
		abs_path = path_absolute
*/

	ACCEPT("*");
	ACCEPT("**");	// subdelims inclues '*', so this is regarded as host part of authority

	ACCEPT("/csi");
	REFUSE("/csi?v=3");	// Request-URI doesn't accept abs_path with query

	// aosoluteURI
	ACCEPT("scheme://some-host");
	ACCEPT("scheme://user:path@some-host/");
	ACCEPT("scheme://user:path@some-host/path/to/file");
	ACCEPT("scheme://user:path@some-host/path/to/dir/");
	ACCEPT("scheme://user:path@some-host/path/to/dir/?query");

	ACCEPT("scheme:/path/to/absolute/file");
	ACCEPT("scheme:/path/to/absolute/dir/");
	ACCEPT("scheme:/path/to/absolute/dir/?query");

	ACCEPT("scheme:relative/path");
	ACCEPT("scheme::/first/segment/can/include/colons");
	ACCEPT("scheme::/first/segment/can/include/colons?query");

	ACCEPT("scheme:");
	ACCEPT("scheme:?query");

	ACCEPT("some-host:12AB");

	// abs_path
	ACCEPT("/");
	ACCEPT("/path:to/some@dir");
	ACCEPT("/./../");
	ACCEPT("/:/first/segment/can/include/colons");
	ACCEPT("/ab%aa%bbcc/~_.-!$&'()*+,;=");

	ACCEPT("");
	ACCEPT("//");
	REFUSE("relative/path");

	// authority
	ACCEPT("");
	ACCEPT("some-host");
	ACCEPT("user@some-host");
	ACCEPT("user:pass@some-host");
	ACCEPT("some-host:1234");
	ACCEPT("user@some-host:1234");
	ACCEPT("user:pass@some-host:1234");
	ACCEPT("@some-host:");
	ACCEPT("user:pass@some-host:");
	ACCEPT("@some-host");

	ACCEPT("@:");
}

DEFINE_TEST(Warning)
{
/*
		Warning    = "Warning" ":" 1#warning-value

		warning-value = warn-code SP warn-agent SP warn-text
                                             [SP warn-date]

		warn-code  = 3DIGIT
		warn-agent = ( host [ ":" port ] ) | pseudonym
                       ; the name or pseudonym of the server adding
                       ; the Warning header, for use in debugging
		warn-text  = quoted-string
		warn-date  = <"> HTTP-date <">
*/

	ACCEPT("Warning: 123 some.host \"warn-text: \\\"text\\\"\"");
	ACCEPT("Warning: 123 some-host \"warn-text: \\\"text\\\"\"");
	ACCEPT("Warning: 123 some-host \"warn-text\", 000 some-host:1234 \"warn-text: warn\" \"Sun, 06 Nov 1994 08:49:37 GMT\"");
}

DEFINE_TEST(Via)
{
/*
		Via =  "Via" ":" 1#( received-protocol received-by [ comment ] )
		received-protocol = [ protocol-name "/" ] protocol-version
		protocol-name     = token
		protocol-version  = token
		received-by       = ( host [ ":" port ] ) | pseudonym
		pseudonym         = token
*/
	ACCEPT("Via: 1.0 fred");
	ACCEPT("Via: 1.0 fred , 1.0 fred");
	ACCEPT("Via: 1.0 fred, 1.1 nowhere.com");
	ACCEPT("Via: 1.0 fred, 1.1 nowhere.com (Apache/1.1)");

	ACCEPT("Via: protocol-version host");
	ACCEPT("Via: protocol-version host, protocol-name/protocol-version host:1234 (somecomment)");

}

DEFINE_TEST(Upgrade)
{
/*
		Upgrade        = "Upgrade" ":" 1#product
		product         = token ["/" product-version]
		product-version = token
*/

	ACCEPT("Upgrade: HTTP/2.0, SHTTP/1.3, IRC/6.9, RTA/x11");

	ACCEPT("Upgrade: token");
	ACCEPT("Upgrade: token, token/product-version");
}

DEFINE_TEST(Transfer_Encoding)
{
/*
		Transfer-Encoding       = "Transfer-Encoding" ":" 1#transfer-coding
		transfer-coding         = "chunked" | transfer-extension
		transfer-extension      = token *( ";" parameter )
		parameter               = attribute "=" value
		attribute               = token
		value                   = token | quoted-string
*/

	ACCEPT("Transfer-Encoding: chunked");

	ACCEPT("Transfer-Encoding: chunked, transfer-extension");
	ACCEPT("Transfer-Encoding: chunked, transfer-extension, transfer-extension;attr=value");
	ACCEPT("Transfer-Encoding: chunked, transfer-extension, transfer-extension;attr=value, transfer-extension;attr=\"value\";attr=\"value\"");

}


DEFINE_TEST(Trailer)
{
/*
		Trailer  = "Trailer" ":" 1#field-name
		field-name     = token
*/

	ACCEPT("Trailer: field-name");
	ACCEPT("Trailer: field-name, field-name");
}

DEFINE_TEST(Pragma)
{
/*
		Pragma            = "Pragma" ":" 1#pragma-directive
		pragma-directive  = "no-cache" | extension-pragma
		extension-pragma  = token [ "=" ( token | quoted-string ) ]
*/

	ACCEPT("Pragma: no-cache");
	ACCEPT("Pragma: no-cache, extension-pragma");
	ACCEPT("Pragma: no-cache, extension-pragma, extension-pragma=token");
	ACCEPT("Pragma: no-cache, extension-pragma, extension-pragma=token, extension-pragma=\"value\"");
}

DEFINE_TEST(Date)
{
/*
		Date  = "Date" ":" HTTP-date
*/
	ACCEPT("Date:Tue, 15 Nov 1994 08:12:31 GMT");
	ACCEPT("Date:Tuesday, 15-Nov-94 08:12:31 GMT");
	ACCEPT("Date:Tue Nov 15 08:12:31 1994");
}

DEFINE_TEST(Connection)
{
/*
		Connection = "Connection" ":" 1#(connection-token)
		connection-token  = token
*/

	ACCEPT("Connection: close");

	ACCEPT("Connection: close, token");
}

DEFINE_TEST(Cache_Control)
{
/*
		Cache-Control   = "Cache-Control" ":" 1#cache-directive

		cache-directive = cache-request-directive
         | cache-response-directive

		cache-request-directive =
           "no-cache"                          ; Section 14.9.1
         | "no-store"                          ; Section 14.9.2
         | "max-age" "=" delta-seconds         ; Section 14.9.3, 14.9.4
         | "max-stale" [ "=" delta-seconds ]   ; Section 14.9.3
         | "min-fresh" "=" delta-seconds       ; Section 14.9.3
         | "no-transform"                      ; Section 14.9.5
         | "only-if-cached"                    ; Section 14.9.4
         | cache-extension                     ; Section 14.9.6

		cache-response-directive =
           "public"                               ; Section 14.9.1
         | "private" [ "=" <"> 1#field-name <"> ] ; Section 14.9.1
         | "no-cache" [ "=" <"> 1#field-name <"> ]; Section 14.9.1
         | "no-store"                             ; Section 14.9.2
         | "no-transform"                         ; Section 14.9.5
         | "must-revalidate"                      ; Section 14.9.4
         | "proxy-revalidate"                     ; Section 14.9.4
         | "max-age" "=" delta-seconds            ; Section 14.9.3
         | "s-maxage" "=" delta-seconds           ; Section 14.9.3
         | cache-extension                        ; Section 14.9.6

		cache-extension = token [ "=" ( token | quoted-string ) ]
*/

	ACCEPT("Cache-Control: no-cache");
	ACCEPT("Cache-Control: no-cache, no-store");
	ACCEPT("Cache-Control: no-cache, no-store, max-age=12345");
	ACCEPT("Cache-Control: no-cache, no-store, max-age=12345, max-stale");
	ACCEPT("Cache-Control: no-cache, no-store, max-age=12345, max-stale=12345");
	ACCEPT("Cache-Control: no-cache, no-store, max-age=12345, max-stale=12345, min-fresh=12345, no-transform, only-if-cached, extension");
	ACCEPT("Cache-Control: no-cache, no-store, max-age=12345, max-stale=12345, min-fresh=12345, no-transform, only-if-cached, extension=token");
	ACCEPT("Cache-Control: no-cache, no-store, max-age=12345, max-stale=12345, min-fresh=12345, no-transform, only-if-cached, extension=\"value\"");

	ACCEPT("Cache-Control: public");
	ACCEPT("Cache-Control: public, private");
	ACCEPT("Cache-Control: public, private=\"field\"");
	ACCEPT("Cache-Control: public, private=\"field,field\"");
	ACCEPT("Cache-Control: public, private=\"field,field\", no-cache");
	ACCEPT("Cache-Control: public, private=\"field,field\", no-cache=\"field\"");
	ACCEPT("Cache-Control: public, private=\"field,field\", no-cache=\"field, field\"");
	ACCEPT("Cache-Control: public, private=\"field,field\", no-cache=\"field, field\", no-store");
	ACCEPT("Cache-Control: public, private=\"field,field\", no-cache=\"field, field\", no-store, no-transform");
	ACCEPT("Cache-Control: public, private=\"field,field\", no-cache=\"field, field\", no-store, no-transform, must-revalidate");
	ACCEPT("Cache-Control: public, private=\"field,field\", no-cache=\"field, field\", no-store, no-transform, must-revalidate, proxy-revalidate");
	ACCEPT("Cache-Control: public, private=\"field,field\", no-cache=\"field, field\", no-store, no-transform, must-revalidate, proxy-revalidate, max-age=12345");
	ACCEPT("Cache-Control: public, private=\"field,field\", no-cache=\"field, field\", no-store, no-transform, must-revalidate, proxy-revalidate, s-max-age=12345");
}

DEFINE_TEST(domain)
{
/*
		domain      =  sub-domain *("." sub-domain)
		sub-domain  =  domain-ref / domain-literal
		domain-ref  =  atom
		domain-literal =  "[" *(dtext / quoted-pair) "]"
		atom        =  1*<any CHAR Expect specials, SPACE and CTLs>
		specials    =  "(" / ")" / "<" / ">" / "@"  ; Must be in quoted-
                 /  "," / ";" / ":" / "\" / <">  ;  string, to use
                 /  "." / "[" / "]"              ;  within a word.
*/

	ACCEPT("subdomain");
	ACCEPT("subdomain.subdomain");
	ACCEPT("subdomain.subdomain.[literal]");

	REFUSE("subdomain.");
	REFUSE("subdomain..subdomain");

	ACCEPT("subdomain.subdomain.[literal \\] string]");

}

DEFINE_TEST(addr_spec)
{
/*
		addr-spec   =  local-part "@" domain
		local-part  =  word *("." word)
		word        =  atom / quoted-string
*/

	ACCEPT("someaddr@domain");
	ACCEPT("some.addr@subdomain.subdomain");

	ACCEPT("some.\"..addr.\"@subdomain.subdomain");
	ACCEPT("some.\"@addr\"@subdomain.subdomain");

	ACCEPT("'test@[127.0.0.1'\\\x1f]");

	REFUSE("some..addr@domain");
	REFUSE("some.@domain");
	REFUSE("some@addr@domain");
}

DEFINE_TEST(mailbox)
{
/*
		mailbox     =  addr-spec /  phrase route-addr
		phrase      =  1*word
		route-addr  =  "<" [route] addr-spec ">"
		route       =  1#("@" domain) ":"
*/

	ACCEPT("some.addr@subdomain.subdomain");
	ACCEPT("name<some.addr@subdomain.subdomain>");
	ACCEPT("name<@doman:some.addr@subdomain.subdomain>");
	ACCEPT("name<@doman,@domain:some.addr@subdomain.subdomain>");
}

DEFINE_TEST(User_Agent)
{
/*
		User-Agent     = "User-Agent" ":" 1*( product | comment )
		product         = token ["/" product-version]
*/

	ACCEPT("User-Agent: CERN-LineMode/2.15 libwww/2.17b3");

	ACCEPT("User-Agent: token/product-version");
	ACCEPT("User-Agent: token/product-version (comment)");
}

DEFINE_TEST(TE)
{
/*
		TE        = "TE" ":" #( t-codings )
		t-codings = "trailers" | ( transfer-extension [ accept-params ] )
		transfer-extension      = token *( ";" parameter )
		parameter               = attribute "=" value
		attribute               = token
		value                   = token | quoted-string
		accept-params  = ";" "q" "=" qvalue *( accept-extension )
		qvalue         = ( "0" [ "." 0*3DIGIT ] )
                      | ( "1" [ "." 0*3("0") ] )
		accept-extension = ";" token [ "=" ( token | quoted-string ) ]

*/
	ACCEPT("TE: deflate");
	ACCEPT("TE:");
	ACCEPT("TE: trailers, deflate;q=0.5");

	ACCEPT("TE: deflate;param=value");
	ACCEPT("TE: deflate;param=value, deflate;param=value;param=\"value\", deflate;param=value;q=0.123");
	ACCEPT("TE: deflate;param=value, deflate;param=value;param=\"value\", deflate;param=value;q=0.123;token");
	ACCEPT("TE: deflate;param=value, deflate;param=value;param=\"value\", deflate;param=value;q=0.123;token;token=value");
	ACCEPT("TE: deflate;param=value, deflate;param=value;param=\"value\", deflate;param=value;q=0.123;token;token=value;token=\"value\"");
}

DEFINE_TEST(Referer)
{
/*
	Referer        = "Referer" ":" ( absoluteURI | relativeURI )
*/

	ACCEPT("Referer:http://www.w3.org/hypertext/DataSources/Overview.html");
	ACCEPT("Referer:/hypertext/DataSources/Overview.html");
	ACCEPT("Referer:hypertext/DataSources/Overview.html");

	// absoluteURI
	ACCEPT("Referer:scheme://some-host");
	ACCEPT("Referer:scheme://user:path@some-host/");
	ACCEPT("Referer:scheme://user:path@some-host/path/to/file");
	ACCEPT("Referer:scheme://user:path@some-host/path/to/dir/");
	ACCEPT("Referer:scheme://user:path@some-host/path/to/dir/?query");

	ACCEPT("Referer:scheme:/path/to/absolute/file");
	ACCEPT("Referer:scheme:/path/to/absolute/dir/");
	ACCEPT("Referer:scheme:/path/to/absolute/dir/?query");

	ACCEPT("Referer:scheme:relative/path");
	ACCEPT("Referer:scheme::/first/segment/can/include/colons");
	ACCEPT("Referer:scheme::/first/segment/can/include/colons?query");

	REFUSE("Referer:scheme:");
	ACCEPT("Referer:scheme:?query");


	// relativeURI
	ACCEPT("Referer://some-host");
	ACCEPT("Referer://user:path@some-host/");
	ACCEPT("Referer://user:path@some-host/path/to/file");
	ACCEPT("Referer://user:path@some-host/path/to/dir/");
	ACCEPT("Referer://user:path@some-host/path/to/dir/?query");

	ACCEPT("Referer:/path/to/absolute/file");
	ACCEPT("Referer:/path/to/absolute/dir/");
	ACCEPT("Referer:/path/to/absolute/dir/?query");

	ACCEPT("Referer:relative/path");
	REFUSE("Referer::/first/segment/can/not/include/colons");
	REFUSE("Referer::/first/segment/can/not/include/colons?query");

	REFUSE("Referer:");
	REFUSE("Referer:?query");
}

DEFINE_TEST(Range)
{
/*
		Range = "Range" ":" ranges-specifier
		ranges-specifier = byte-ranges-specifier
		byte-ranges-specifier = bytes-unit "=" byte-range-set
		bytes-unit       = "bytes"
		byte-range-set  = 1#( byte-range-spec | suffix-byte-range-spec )
		byte-range-spec = first-byte-pos "-" [last-byte-pos]
		first-byte-pos  = 1*DIGIT
		last-byte-pos   = 1*DIGIT
		suffix-byte-range-spec = "-" suffix-length
		suffix-length = 1*DIGIT
*/
	ACCEPT("Range:bytes=123-");
	ACCEPT("Range:bytes=123-, 123-456");
	ACCEPT("Range:bytes=123-, 123-456, -456");
}

DEFINE_TEST(Proxy_Authorization)
{
/*
		Proxy-Authorization     = "Proxy-Authorization" ":" credentials
		credentials = auth-scheme #auth-param
 		auth-scheme    = token
		auth-param     = token "=" ( token | quoted-string )
*/

	ACCEPT("Proxy-Authorization:aaa");
	ACCEPT("Proxy-Authorization:aaa bbb=ccc");
	ACCEPT("Proxy-Authorization:aaa bbb=ccc  , ddd=eee");
	ACCEPT("Proxy-Authorization:aaa bbb=ccc\r\n ,\r\n ddd=eee");
	ACCEPT("Proxy-Authorization:aaa bbb=\"ccc\r\n ,\r\n ddd=eee\"");

	REFUSE("Proxy-Authorization:aaa bbb=ccc ddd=eee");
	REFUSE("Proxy-Authorization:aaa bbb=\"ccc\r\n ,\r\n ddd\"=eee");
	REFUSE("Proxy-Authorization:aaa bbb=\"ccc\r\n ,\r\n ddd=\"eee");

}

DEFINE_TEST(Max_Forwards)
{
/*
		Max-Forwards   = "Max-Forwards" ":" 1*DIGIT
*/

	ACCEPT("Max-Forwards:123");
}

DEFINE_TEST(If_Unmodified_Since)
{
/*
		If-Unmodified-Since = "If-Unmodified-Since" ":" HTTP-date
*/

	ACCEPT("If-Unmodified-Since:Sat, 29 Oct 1994 19:43:31 GMT");
	ACCEPT("If-Unmodified-Since:Tuesday, 15-Nov-94 08:12:31 GMT");
	ACCEPT("If-Unmodified-Since:Tue Nov 15 08:12:31 1994");
}


DEFINE_TEST(If_Range)
{
/*
		If-Range = "If-Range" ":" ( entity-tag | HTTP-date )
		entity-tag = [ weak ] opaque-tag
		weak       = "W/"
		opaque-tag = quoted-string
*/

	ACCEPT("If-Range:\"tag\"");
	ACCEPT("If-Range:W/\"tag\"");

	ACCEPT("If-Range:Sat, 29 Oct 1994 19:43:31 GMT");
	ACCEPT("If-Range:Tuesday, 15-Nov-94 08:12:31 GMT");
	ACCEPT("If-Range:Tue Nov 15 08:12:31 1994");
}

DEFINE_TEST(If_None_Match)
{
/*
		If-None-Match = "If-None-Match" ":" ( "*" | 1#entity-tag )
*/

	ACCEPT("If-None-Match:*");
	ACCEPT("If-None-Match:\"tag\"");
	ACCEPT("If-None-Match:\"tag\", W/\"tag\"");
}

DEFINE_TEST(If_Modified_Since)
{
/*
		If-Modified-Since = "If-Modified-Since" ":" HTTP-date
*/

	ACCEPT("If-Modified-Since:Sat, 29 Oct 1994 19:43:31 GMT");
	ACCEPT("If-Modified-Since:Tuesday, 15-Nov-94 08:12:31 GMT");
	ACCEPT("If-Modified-Since:Tue Nov 15 08:12:31 1994");
}

DEFINE_TEST(If_Match)
{
/*
		If-Match = "If-Match" ":" ( "*" | 1#entity-tag )
*/

	ACCEPT("If-Match:*");
	ACCEPT("If-Match:\"tag\"");
	ACCEPT("If-Match:\"tag\", W/\"tag\"");
}

DEFINE_TEST(Host)
{
/*
		Host = "Host" ":" host [ ":" port ]
		h  host          = hostname | IPv4address
*/

	ACCEPT("Host:www.w3.org");

//	ACCEPT("Host:[0:0a:0Ab:0Abc:9999:0000:F:aaaa]");
//	ACCEPT("Host:[0:0a:0Ab:0Abc:9999:0000:0.10.100.255]");
//
//	ACCEPT("Host:[v1F.ip:future]");

	ACCEPT("Host:0.10.100.255");
}

DEFINE_TEST(From)
{
/*
		From   = "From" ":" mailbox
*/
	ACCEPT("From:webmaster@w3.org");

	ACCEPT("From:some.addr@subdomain.subdomain");
	ACCEPT("From:name<some.addr@subdomain.subdomain>");
	ACCEPT("From:name<@doman:some.addr@subdomain.subdomain>");
	ACCEPT("From:name<@doman,@domain:some.addr@subdomain.subdomain>");
}

DEFINE_TEST(Expect)
{
/*
		Expect       =  "Expect" ":" 1#expectation

		expectation  =  "100-continue" | expectation-extension
		expectation-extension =  token [ "=" ( token | quoted-string ) *expect-params ]
		expect-params =  ";" token [ "=" ( token | quoted-string ) ]
*/

	REFUSE("Expect:");
	ACCEPT("Expect:100-continue");
	ACCEPT("Expect:100-continue,token");
	ACCEPT("Expect:100-continue,token,token=token");
	ACCEPT("Expect:100-continue,token,token=token,token=\"token\"");
	ACCEPT("Expect:100-continue,token,token=token,token=\"token\",token=token;token");
	ACCEPT("Expect:100-continue,token,token=token,token=\"token\",token=token;token,token=token;token=token");
	ACCEPT("Expect:100-continue,token,token=token,token=\"token\",token=token;token,token=token;token=token,token=token;token=\"token\"");
	ACCEPT("Expect:100-continue,token,token=token,token=\"token\",token=\"token\";token,token=\"token\";token=token,token=\"token\";token=\"token\"");
}

DEFINE_TEST(Authorization)
{
/*
		Authorization  = "Authorization" ":" credentials
		credentials = auth-scheme #auth-param
 		auth-scheme    = token
		auth-param     = token "=" ( token | quoted-string )
*/

	ACCEPT("Authorization:aaa");
	ACCEPT("Authorization:aaa bbb=ccc");
	ACCEPT("Authorization:aaa bbb=ccc  , ddd=eee");
	ACCEPT("Authorization:aaa bbb=ccc\r\n ,\r\n ddd=eee");
	ACCEPT("Authorization:aaa bbb=\"ccc\r\n ,\r\n ddd=eee\"");

	REFUSE("Authorization:aaa bbb=ccc ddd=eee");
	REFUSE("Authorization:aaa bbb=\"ccc\r\n ,\r\n ddd\"=eee");
	REFUSE("Authorization:aaa bbb=\"ccc\r\n ,\r\n ddd=\"eee");
}

DEFINE_TEST(Accept_Language)
{
/*
		Accept-Language = "Accept-Language" ":" 1#( language-range [ ";" "q" "=" qvalue ] )
		language-range  = ( ( 1*8ALPHA *( "-" 1*8ALPHA ) ) | "*" )
*/
	ACCEPT("Accept-Language: da, en-gb;q=0.8, en;q=0.7");
	ACCEPT("Accept-Language: da, en-gb;q=0.8, en;q=0.7, *");

	ACCEPT("Accept-Language: a,ab,abc,abcd,abcde,abcdef,abcdefg,abcdefgh");
	REFUSE("Accept-Language: abcdefghg");

}

DEFINE_TEST(Accept_Encoding)
{
/*
		Accept-Encoding  = "Accept-Encoding" ":" 1#( codings [ ";" "q" "=" qvalue ] )
		codings          = ( content-coding | "*" )
		content-coding   = token
*/

	REFUSE("Accept-Encoding:");

	ACCEPT("Accept-Encoding: compress, gzip");
	ACCEPT("Accept-Encoding: *");
	ACCEPT("Accept-Encoding: compress;q=0.5, gzip;q=1.0");
	ACCEPT("Accept-Encoding: gzip;q=1.0");
	ACCEPT("Accept-Encoding: gzip;q=1.0, identity;q=0.5");
	ACCEPT("Accept-Encoding: gzip;q=1.0, identity;q=0.5, *;q=0");
}

DEFINE_TEST(Accept_Charset)
{
/*
	Accept-Charset = "Accept-Charset" ":" 1#( ( charset | "*" )[ ";" "q" "=" qvalue ] )
	charset = token
*/

	ACCEPT("Accept-Charset: *, **, iso-8859-5, unicode-1-1;q=0.8");
}

DEFINE_TEST(Accept)
{

//		Accept         = "Accept" ":" #( media-range [ accept-params ] )
//		media-range    = ( "*/*"
//                        | ( type "/" "*" )
//                        | ( type "/" subtype )
//                        ) *( ";" parameter )
//		accept-params  = ";" "q" "=" qvalue *( accept-extension )
//		accept-extension = ";" token [ "=" ( token | quoted-string ) ]

	ACCEPT("Accept:");
	ACCEPT("Accept: */*");
	ACCEPT("Accept: **/**");
	ACCEPT("Accept: */*, type/*;q=0.000");
	ACCEPT("Accept: */*, type/*;q=0.000;token");
	ACCEPT("Accept: */*, type/type;q=0.000;token;token=token");
	ACCEPT("Accept: */*, type/type;attr=value;q=0.000;token;token=token;token=\"token\"");
	ACCEPT("Accept: */*, type/type;attr=value;attr=\"value\";q=0.000;token;token=token;token=\"token\"");
}

DEFINE_TEST(Last_Modified)
{
/*
		Last-Modified  = "Last-Modified" ":" HTTP-date
*/

	ACCEPT("Last-Modified:Sat, 29 Oct 1994 19:43:31 GMT");
	ACCEPT("Last-Modified:Tuesday, 15-Nov-94 08:12:31 GMT");
	ACCEPT("Last-Modified:Tue Nov 15 08:12:31 1994");

}

DEFINE_TEST(Expires)
{
/*
		Expires  = "Last-Modified" ":" HTTP-date
*/

	ACCEPT("Expires:Sat, 29 Oct 1994 19:43:31 GMT");
	ACCEPT("Expires:Tuesday, 15-Nov-94 08:12:31 GMT");
	ACCEPT("Expires:Tue Nov 15 08:12:31 1994");

}

DEFINE_TEST(Content_Type)
{
/*
		Content-Type   = "Content-Type" ":" media-type
		media-type     = type "/" subtype *( ";" parameter )
		type           = token
		subtype        = token
*/

	ACCEPT("Content-Type:text/html;charset=ISO-8859-4");
}

DEFINE_TEST(Content_Range)
{
/*
		Content-Range = "Content-Range" ":" content-range-spec
		content-range-spec      = byte-content-range-spec
		byte-content-range-spec = bytes-unit SP byte-range-resp-spec "/" ( instance-length | "*" )
		byte-range-resp-spec = (first-byte-pos "-" last-byte-pos) | "*"
		first-byte-pos  = 1*DIGIT
		last-byte-pos   = 1*DIGIT
		instance-length           = 1*DIGIT
*/

	ACCEPT("Content-Range:bytes 123-456/12345");
	ACCEPT("Content-Range:bytes 123-456/*");
	ACCEPT("Content-Range:bytes */12345");
	ACCEPT("Content-Range:bytes */*");

}

DEFINE_TEST(Content_MD5)
{
/*
	Content-MD5   = "Content-MD5" ":" md5-digest
	md5-digest   = <base64 of 128 bit MD5 digest as per RFC 1864>
*/

	ACCEPT("Content-MD5:22CharsConsitOfAlnum+/==");
}


DEFINE_TEST(Content_Location)
{
/*
		Content-Location = "Content-Location" ":" ( absoluteURI | relativeURI )
*/

	ACCEPT("Content-Location:http://www.w3.org/hypertext/DataSources/Overview.html");
	ACCEPT("Content-Location:/hypertext/DataSources/Overview.html");
	ACCEPT("Content-Location:hypertext/DataSources/Overview.html");

	// absoluteURI
	ACCEPT("Content-Location:scheme://some-host");
	ACCEPT("Content-Location:scheme://user:path@some-host/");
	ACCEPT("Content-Location:scheme://user:path@some-host/path/to/file");
	ACCEPT("Content-Location:scheme://user:path@some-host/path/to/dir/");
	ACCEPT("Content-Location:scheme://user:path@some-host/path/to/dir/?query");

	ACCEPT("Content-Location:scheme:/path/to/absolute/file");
	ACCEPT("Content-Location:scheme:/path/to/absolute/dir/");
	ACCEPT("Content-Location:scheme:/path/to/absolute/dir/?query");

	ACCEPT("Content-Location:scheme:relative/path");
	ACCEPT("Content-Location:scheme::/first/segment/can/include/colons");
	ACCEPT("Content-Location:scheme::/first/segment/can/include/colons?query");

	REFUSE("Content-Location:scheme:");
	ACCEPT("Content-Location:scheme:?query");


	// relativeURI
	ACCEPT("Content-Location://some-host");
	ACCEPT("Content-Location://user:path@some-host/");
	ACCEPT("Content-Location://user:path@some-host/path/to/file");
	ACCEPT("Content-Location://user:path@some-host/path/to/dir/");
	ACCEPT("Content-Location://user:path@some-host/path/to/dir/?query");

	ACCEPT("Content-Location:/path/to/absolute/file");
	ACCEPT("Content-Location:/path/to/absolute/dir/");
	ACCEPT("Content-Location:/path/to/absolute/dir/?query");

	ACCEPT("Content-Location:relative/path");
	REFUSE("Content-Location::/first/segment/can/not/include/colons");
	REFUSE("Content-Location::/first/segment/can/not/include/colons?query");

	REFUSE("Content-Location:");
	REFUSE("Content-Location:?query");
}


DEFINE_TEST(Content_Length)
{
/*
		Content-Length    = "Content-Length" ":" 1*DIGIT
*/
	ACCEPT("Content-Length:3495");
}


DEFINE_TEST(Content_Language)
{
/*
		Content-Language  = "Content-Language" ":" 1#language-tag
		language-tag  = primary-tag *( "-" subtag )
        primary-tag   = 1*8ALPHA
        subtag        = 1*8ALPHA
*/

	ACCEPT("Content-Language: da, en-gb, en");

	ACCEPT("Content-Language: a,ab,abc,abcd,abcde,abcdef,abcdefg,abcdefgh");
	REFUSE("Content-Language: abcdefghg");
}


DEFINE_TEST(Content_Encoding)
{
/*
		Content-Encoding  = "Content-Encoding" ":" 1#content-coding
		content-coding   = token
*/

	ACCEPT("Content-Encoding: gzip");
	ACCEPT("Content-Encoding: gzip,deflate");
}


DEFINE_TEST(Allow)
{
/*
		Allow   = "Allow" ":" #Method
		Method         = "OPTIONS"                ; Section 9.2
                      | "GET"                    ; Section 9.3
                      | "HEAD"                   ; Section 9.4
                      | "POST"                   ; Section 9.5
                      | "PUT"                    ; Section 9.6
                      | "DELETE"                 ; Section 9.7
                      | "TRACE"                  ; Section 9.8
                      | "CONNECT"                ; Section 9.9
                      | extension-method
		extension-method = token
*/

	ACCEPT("Allow: GET, HEAD, PUT");
}

DEFINE_TEST(Request)
{
/*
	Request       = Request-Line              ; Section 5.1
	                        *(( general-header        ; Section 4.5
	                         | request-header         ; Section 5.3
	                         | entity-header ) CRLF)  ; Section 7.1
	                        CRLF
	                        [ message-body ]          ; Section 4.3
*/

	ACCEPT("GET /aaa/bbb/cccc HTTP/1.1\r\n"
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

	ACCEPT("GET / HTTP/1.1\r\n"
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

	ACCEPT("GET /extern_js/f/CgJqYRICanArMEU4ASwrMFo4AiwrMA44FywrswFjgcLCswGTggLCswJTjKiAEsKzA1OAQsKzBAOBIsKzBBOAUsKzAYOAUsKzAmOAssgAIXkAIV/IjFQqMVzCyU.js HTTP/1.1\r\n"
		"Host:www.google.co.jp\r\n"
		"User-Agent:Mozilla/5.0 (X11; U; Linux i686; ja; rv:1.9.2.8) Gecko/20100722 Firefox/3.4.4\r\n"
		"Accept:*/*\r\n"
		"Accept-Language:ja,en-us;q=0.7,en;q=0.3\r\n"
		"Accept-Encoding:gzip,deflate\r\n"
		"Accept-Charset:Shift_JIS,utf-8;q=0.7,*;q=0.7\r\n"
		"Keep-Alive:115\r\n"
		"Connection:keep-alive\r\n"
		"Referer:http://www.google.co.jp/\r\n"
		"Cookie:PREF=ID=2bdd9aa98503:TM=114720:LM=614720:S=dLlh6FV3n-Z9; NID=37=m9UB21ePGXjT0IalYJC1tiCxPCbi-DyromZQhesAx7HJaL9XjffosMP1wSNDNayh28seAwJfGi4PduwQbyxG9yDYoblORavoS4IWIfyGdP9ii\r\n"
		"\r\n");

	ACCEPT("GET /generate_204 HTTP/1.1\r\n"
		"Host:clients1.google.co.jp\r\n"
		"User-Agent:Mozilla/5.0 (X11; U; Linux i686; ja; rv:1.9.2.8) Gecko/20100722 Firefox/3.5.0\r\n"
		"Accept:image/png,image/*;q=0.8,*/*;q=0.5\r\n"
		"Accept-Language:ja,en-us;q=0.7,en;q=0.3\r\n"
		"Accept-Encoding:gzip,deflate\r\n"
		"Accept-Charset:Shift_JIS,utf-8;q=0.7,*;q=0.7\r\n"
		"Keep-Alive:115\r\n"
		"Connection:keep-alive\r\n"
		"Referer:http://www.google.co.jp/\r\n"
		"Cookie:PREF=ID=2bdd9aa98503:TM=114720:LM=614720:S=dLlh6FV3n-Z9; NID=37=m9UB21ePGXjT0IalYJC1tiCxPCbi-DyromZQhesAx7HJaL9XjffosMP1wSNDNayh28seAwJfGi4PduwQbyxG9yDYoblORavoS4IWIfyGdP9ii\r\n"
		"\r\n");

	REFUSE("GET /csi?v=3 HTTP/1.1\r\n"
//	ACCEPT("GET /csi?v=3&s=webhp&action=&e=1729,1868,2522,2565&ei=W6FUTP6VAWU3vziBA&expi=1259,1868,2522,2565&imc=0&imn=0&imp=0&rt=prt.30,xjsls.48,xjses.155,xjsee.168,xjs.172,ol.202,iml.30 HTTP/1.1\r\n"
//		"Host:www.google.co.jp\r\n"
//		"User-Agent:Mozilla/5.0 (X11; U; Linux i686; ja; rv:1.9.2.8) Gecko/20100722 Firefox/3.3.4\r\n"
//		"Accept:image/png,image/*;q=0.8,*/*;q=0.5\r\n"
//		"Accept-Language:ja,en-us;q=0.7,en;q=0.3\r\n"
//		"Accept-Encoding:gzip,deflate\r\n"
//		"Accept-Charset:Shift_JIS,utf-8;q=0.7,*;q=0.7\r\n"
//		"Keep-Alive:115\r\n"
//		"Connection:keep-alive\r\n"
//		"Referer:http://www.google.co.jp/\r\n"
//		"Cookie:PREF=ID=2bdd9aa98503:TM=114720:LM=614720:S=dLlh6FV3n-Z9; NID=37=m9UB21ePGXjT0IalYJC1tiCxPCbi-DyromZQhesAx7HJaL9XjffosMP1wSNDNayh28seAwJfGi4PduwQbyxG9yDYoblORavoS4IWIfyGdP9ii\r\n"
		"\r\n");

}


