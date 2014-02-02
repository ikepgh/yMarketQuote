#define TAG_HELP \
"\33bFull list of Yahoo .csv formatting Tags:                  \n" \
"                                                              \n" \
"a \t   Ask                                                     \n" \
"a2\t  Average Daily Volume                                    \n" \
"a5\t  Ask Size                                                \n" \
"b \t   Bid                                                     \n" \
"b2\t  Ask (Real-time)                                         \n" \
"b3\t  Bid (Real-time)                                         \n" \
"b4\t  Book Value                                              \n" \
"b6\t  Bid Size                                                \n" \
"c \t   Change & Percent Change                                 \n" \
"c1\t  Change                                                  \n" \
"c3\t  Commission                                              \n" \
"c6\t  Change (Real-time)                                      \n" \
"c8\t  After Hours Change (Real-time)                          \n" \
"d \t   Dividend/Share                                          \n" \
"d1\t  Last Trade Date                                         \n" \
"d2\t  Trade Date                                              \n" \
"e \t   Earnings/Share                                          \n" \
"e1\t  Error Indication (returned for symbol changed/invalid)  \n" \
"e7\t  EPS Estimate Current Year                               \n" \
"e8\t  EPS Estimate Next Year                                  \n" \
"e9\t  EPS Estimate Next Quarter                               \n" \
"f6\t   Float Shares                                            \n" \
"g \t   Day's Low                                               \n" \
"h \t   Day's High                                              \n" \
"j \t    52-week Low                                             \n" \
"k \t   52-week High                                            \n" \
"g1\t  Holdings Gain Percent                                   \n" \
"g3\t  Annualized Gain                                         \n" \
"g4\t  Holdings Gain                                           \n" \
"g5\t  Holdings Gain Percent (Real-time)                       \n" \
"g6\t  Holdings Gain (Real-time)                               \n" \
"i \t    More Info                                               \n" \
"i5\t   Order Book (Real-time)                                  \n" \
"j1\t   Market Capitalization                                   \n" \
"j3\t   Market Cap (Real-time)                                  \n" \
"j4\t   EBITDA                                                  \n" \
"j5\t   Change From 52-week Low                                 \n" \
"j6\t   Percent Change From 52-week Low                         \n" \
"k1\t  Last Trade (Real-time) With Time                        \n" \
"k2\t  Change Percent (Real-time)                              \n" \
"k3\t  Last Trade Size                                         \n" \
"k4\t  Change From 52-week High                                \n" \
"k5\t  Percebt Change From 52-week High                        \n" \
"l \t    Last Trade (With Time)                                  \n" \
"l1\t   Last Trade (Price Only)                                 \n" \
"l2\t   High Limit                                              \n" \
"l3\t   Low Limit                                               \n" \
"m \t  Day's Range                                             \n" \
"m2\t Day's Range (Real-time)                                 \n" \
"m3\t 50-day Moving Average                                   \n" \
"m4\t 200-day Moving Average                                  \n" \
"m5\t Change From 200-day Moving Average                      \n" \
"m6\t Percent Change From 200-day Moving Average              \n" \
"m7\t Change From 50-day Moving Average                       \n" \
"m8\t Percent Change From 50-day Moving Average               \n" \
"n \t   Name                                                    \n" \
"n4\t  Notes                                                   \n" \
"o \t   Open                                                    \n" \
"p \t   Previous Close                                          \n" \
"p1\t  Price Paid                                              \n" \
"p2\t  Change in Percent                                       \n" \
"p5\t  Price/Sales                                             \n" \
"p6\t  Price/Book                                              \n" \
"q \t   Ex-Dividend Date                                        \n" \
"r \t    P/E Ratio                                               \n" \
"r1\t   Dividend Pay Date                                       \n" \
"r2\t   P/E Ratio (Real-time)                                   \n" \
"r5\t   PEG Ratio                                               \n" \
"r6\t   Price/EPS Estimate Current Year                         \n" \
"r7\t   Price/EPS Estimate Next Year                            \n" \
"s \t    Symbol                                                  \n" \
"s1\t   Shares Owned                                            \n" \
"s7\t   Short Ratio                                             \n" \
"t1\t   Last Trade Time                                         \n" \
"t6\t   Trade Links                                             \n" \
"t7\t   Ticker Trend                                            \n" \
"t8\t   1 yr Target Price                                       \n" \
"v \t    Volume                                                  \n" \
"v1\t   Holdings Value                                          \n" \
"v7\t   Holdings Value (Real-time)                              \n" \
"w \t   52-week Range                                           \n" \
"w1\t  Day's Value Change                                      \n" \
"w4\t  Day's Value Change (Real-time)                          \n" \
"x \t    Stock Exchange                                          \n" \
"y \t    Dividend Yield                                          \n" \
"                                                              \n" \
"\33bUsage Example: 'xnsd1t1cl1'                               \n" \
"                                                              \n" \
"\33iIt will return the following in order:                    \n" \
"                                                              \n" \
"Stock Exchange                                                \n" \
"Name                                                          \n" \
"Symbol                                                        \n" \
"Last Trade Date                                               \n" \
"Last Trade Time                                               \n" \
"Change & Percent Change                                       \n" \
"Last Trade (Price Only)                                       \n"


#define EXCHANGE_HELP \
"\33bExchanges:                                              \n" \
"                                                            \n" \
"\33bCountry          Exchange	                                             Suffix \n" \
"                                                                     \n" \
"Argentina      Buenos Aires Stock Exchange                .BA     \n" \
"Austria           Vienna Stock Exchange                            .VI      \n" \
"Australia        Australian Stock Exchange	                   .AX       \n" \
"Belgium         Brussels Stocks                                       .BR       \n" \
"Brazil	            BOVESPA-Sao Paolo Stock Exchange  .SA       \n" \
"Canada	        Toronto Stock Exchange                         .TO       \n" \
"Canada	        TSX Venture Exchange                            .V        \n" \
"Chile	            Santiago Stock Exchange                      .SN       \n" \
"China	           Shanghai Stock Exchange                     .SS       \n" \
"China	           Shenzhen Stock Exchange                    .SZ       \n" \
"Denmark         Copenhagen Stock Exchange              .CO       \n" \
"France	           Euronext                                                   .NX       \n" \
"France	           Paris Stock Exchange                            .PA       \n" \
"Germany         Berlin Stock Exchange                          .BE       \n" \
"Germany         Bremen Stock Exchange                      .BM       \n" \
"Germany         Dusseldorf Stock Exchange                 .DU       \n" \
"Germany         Frankfurt Stock Exchange                    .F        \n" \
"Germany         Hamburg Stock Exchange                    .HM       \n" \
"Germany         Hanover Stock Exchange                      .HA       \n" \
"Germany         Munich Stock Exchange                        .MU       \n" \
"Germany         Stuttgart Stock Exchange                     .SG       \n" \
"Germany         XETRA Stock Exchange                          .DE       \n" \
"Hong Kong	    Hong Kong Stock Exchange                 .HK       \n" \
"India	              Bombay Stock Exchange	                      .BO       \n" \
"India	              National Stock Exchange of India        .NS       \n" \
"Indonesia	      Jakarta Stock Exchange                        .JK       \n" \
"Israel	             Tel Aviv Stock Exchange                        .TA       \n" \
"Italy	               Milan Stock Exchange                            .MI       \n" \
"Japan	             Nikkei Indices                                           N/A       \n" \
"Mexico	            Mexico Stock Exchange                        .MX       \n" \
"Netherlands     Amsterdam Stock Exchange             .AS       \n" \
"New Zealand    New Zealand Stock Exchange          .NZ       \n" \
"Norway	            Oslo Stock Exchange                           .OL       \n" \
"Portugal	        Lisbon Stocks                                        .LS       \n" \
"Singapore	      Singapore Stock Exchange     	          .SI       \n" \
"South Korea      Korea Stock Exchange                       .KS       \n" \
"South Korea      KOSDAQ                                                 .KQ       \n" \
"Spain	              Barcelona Stock Exchange                 .BC       \n" \
"Spain	              Bilbao Stock Exchange	                       .BI       \n" \
"Spain	              Madrid Fixed Income Market              .MF       \n" \
"Spain	              Madrid SE C.A.T.S.                              .MC       \n" \
"Spain	              Madrid Stock Exchange                      .MA       \n" \
"Sweden             Stockholm Stock Exchange               .ST       \n" \
"Switzerland       Swiss Exchange                                  .SW       \n" \
"Taiwan	             Taiwan OTC Exchange                       .TWO      \n" \
"Taiwan	             Taiwan Stock Exchange                     .TW       \n" \
"UK                      FTSE Indices                                           N/A       \n" \
"UK                      London Stock Exchange                      .L        \n" \
"USA                    American Stock Exchange                  N/A       \n" \
"USA	                   BATS Exchange                                     N/A       \n" \
"USA	                   Chicago Board of Trade                     .CBT      \n" \
"USA	                   Chicago Mercantile Exchange           .CME      \n" \
"USA	                   Dow Jones Indexes                                N/A       \n" \
"USA                     NASDAQ Stock Exchange                     N/A       \n" \
"USA	                   New York Board of Trade                    .NYB      \n" \
"USA	                   New York Commodities Exchange    .CMX      \n" \
"USA	                   New York Mercantile Exchange	         .NYM      \n" \
"USA	                   New York Stock Exchange                     N/A       \n" \
"USA	                   OTC Bulletin Board Market                  .OB       \n" \
"USA	                   Pink Sheets                                             .PK       \n" \
"USA	                   S & P Indices                                            N/A       \n" \
"                                                            \n" \
"\33bUsage Example: ' csco.f ' or ' cis.f '                      \n" \
"                                                            \n" \
"\33iIt will return Cisco from the Frankfurt Stock Exchange  \n"
