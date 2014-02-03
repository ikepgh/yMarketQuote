Short:   yMarketQuote
Uploader:  ike@ezcyberspace.com (IKE)
Author:  ikepgh
Type:  biz/misc
Version: 0.0.8
Architecture:  MorphOS
Requires:  BetterString.mcc, TextEditor.mcc
Distribution: Any


INTRODUCTION
------------

Features:

* Downloads stock market quotes from Yahoo Finance
* Saves output file in "ram:" in '.csv' format

Thanks for your interest!

Send email/feedback to:

ike@ezcyberspace.com

INSTALLATION
------------

Just place the yMarketQuote program wherever you'd like on your hard drive

USAGE
-----

See Help menus in the program for a list of Stock Exchanges and Yahoo Tags
Also, look at the 'hot help' bubbles to get an idea of program usage...

VERSION HISTORY
--------------- 

0.0.8 - first public release (SkyDog build)
0.0.7 - source cleanup, added preferences settings
0.0.6 - reworked internals, case, menus, startup, etc.
0.0.5 - added help dialogs, main program finally working correctly
0.0.4 - switched to betterstring.mcc and texteditor.mcc
0.0.3 - interface enhancements and stocks/tags now selectable
0.0.2 - downloads hard coded data and dumps to stdout
0.0.1 - yMarketQuote was born listening to the Duane Allman Box Set

RECOMPILING
-----------

Dependencies: libcurl 7.xx.x+

Compile: gcc -o yMarketQuote ymarketquote.c -lcurl -s -Wall

Enclosed version was built with libcurl 7.34.0 (without ssl and many others)
Take a look at 'libcurl-build-options.txt' to get an idea of what not to
include to get the program size down from 4.3MBs to only about 300k...

Project is available on SourceForge...

https://sourceforge.net/projects/ymarketquote/

TO DO
-----

- Green/red indicators depending on market movement
- Auto update stocks setting
- Choice(s) of output formatting
- Other ideas?

LICENSE
-------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

ABOUT
-----

yMarketQuote is ©2014 IKE <ike@ezcyberspace.com>

icon by Ken Lester

Look at some of my other projects and ports @
http://www.ezcyberspace.com/programming.shtml
