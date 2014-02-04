/* yMarketQuote 0.0.8 by IKE (compiled with gcc 2.95.3 for MorphOS)
 * 
 * ike@ezcyberspace.com   Date: 2/1/14
 *
 * Connects to Yahoo and downloads market quotes
 *
 * gcc -o yMarketQuote ymarketquote.c -lcurl -s -Wall
 *
 */
#include <stdio.h>
#include <string.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <clib/gadtools_protos.h>
#include <MUI/TextEditor_mcc.h>
#include <MUI/BetterString_mcc.h>
#include <curl/curl.h>
#include "ymarketquote.h"

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Library  *MUIMasterBase;

#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))

// Buttons
#define GRAB        44
#define CLEARSTOCK  45
#define CLEARTAG    46
// Settings
#define SAVE        47
#define USE         48
#define CANCEL      49

enum {
	ADD_METHOD = 1,

	MEN_PROJECT, MEN_EXCHANGEHELP, MEN_TAGHELP, MEN_ABOUT, MEN_ABOUTMUI,
    MEN_QUIT, MEN_SETTINGS, MEN_PREFS, MEN_DELETE, MEN_MUIPREFS,
};

static struct NewMenu MenuData1[]=
{
	{NM_TITLE, "Project",                      0 , 0, 0,  (APTR)MEN_PROJECT      },
	{NM_ITEM,  "Stock Exchange Help",         "L", 0, 0,  (APTR)MEN_EXCHANGEHELP },
	{NM_ITEM,  "Yahoo Tag Help",              "P", 0, 0,  (APTR)MEN_TAGHELP      },
	{NM_ITEM,  NM_BARLABEL,                    0 , 0, 0,  (APTR)0                },
	{NM_ITEM,  "About...",                    "?", 0, 0,  (APTR)MEN_ABOUT        },
	{NM_ITEM,  "About MUI...",				   0,  0, 0,  (APTR)MEN_ABOUTMUI     },
	{NM_ITEM,  NM_BARLABEL,                    0 , 0, 0,  (APTR)0                },
	{NM_ITEM,  "Quit",                        "Q", 0, 0,  (APTR)MEN_QUIT         },
    {NM_TITLE, "Settings",       	    	   0 , 0, 0,  (APTR)MEN_SETTINGS     },
    {NM_ITEM,  "Preferences...",	           0 , 0, 0,  (APTR)MEN_PREFS        },
    {NM_ITEM,  "Delete 'stockquote.csv'",	   0 , 0, 0,  (APTR)MEN_DELETE       },
	{NM_ITEM,  NM_BARLABEL,                    0 , 0, 0,  (APTR)0                },
    {NM_ITEM,  "MUI...",	                   0 , 0, 0,  (APTR)MEN_MUIPREFS     },
    {NM_END,   NULL,                           0 , 0, 0,  (APTR)0                },
};

char about_text[] =
"\33cyMarketQuote © 2014 IKE\n
version 0.0.8 (2/1/14)\n
Connects to Yahoo and downloads market quotes\n
\n";

Object *App, *Win, *STR_quote, *STR_tag, *STR_tagsetting, *STR_stocksetting,
*STR_refreshsetting, *aboutwin;
BOOL running = TRUE;

APTR scroll_output, txt_output, scroll_taghelp, win_taghelp, txt_taghelp,
scroll_exchangehelp, win_exchangehelp, txt_exchangehelp, grab_gad, clear_quote, clear_tag,
but_save, but_use, but_cancel, win_settings;

// cURL
CURL *curl;
CURLcode code;
long res;
char *quote, *tag, *tag1, *quote1;
char host[1024];

// Read data file
FILE *fptr;
char filename[] = "ram:stockquote.csv";

// Write data file
char *ftext;
ULONG flen;

BOOL open_libs(void) {
	if ( !(IntuitionBase=(struct IntuitionBase *) OpenLibrary("intuition.library",39)) )
		return(0);

	if ( !(GfxBase=(struct GfxBase *) OpenLibrary("graphics.library",0)) ) {
		CloseLibrary((struct Library *)IntuitionBase);
		return(0);
	}

	if ( !(MUIMasterBase=OpenLibrary(MUIMASTER_NAME,19)) ) {
		CloseLibrary((struct Library *)GfxBase);
		CloseLibrary((struct Library *)IntuitionBase);
		return(0);
	}
	return(1);
}

void close_libs(void) {
	if (IntuitionBase)
		CloseLibrary((struct Library *)IntuitionBase);

	if (GfxBase)
		CloseLibrary((struct Library *)GfxBase);

	if (MUIMasterBase)
		CloseLibrary(MUIMasterBase);
}

void do_clear(void) {
	set(STR_quote, MUIA_String_Contents, 0);
}

void do_clear2(void) {
	set(STR_tag, MUIA_String_Contents, 0);
}

void error(void) {
	set (txt_output, MUIA_TextEditor_Contents, "Hmm...An error occurred! Could not connect to server...\n\nPossible reasons: No internet connection...or Yahoo site is down...");
}

int get_stocks() {

    set(txt_output, MUIA_TextEditor_Contents, "Connecting...");

	//host = "http://download.finance.yahoo.com/d/quotes.csv?s=pep+bp+msft+goog+csco+csco.f+xom+ge+c+pg&f=xnsd1t1cl1";

	get(STR_quote, MUIA_String_Contents, &quote);
	get(STR_tag, MUIA_String_Contents, &tag);

	strcpy(host, "http://download.finance.yahoo.com/d/quotes.csv?s=");
	strcat(host, quote); // "pep+bp+msft+goog+csco+csco.f+xom+ge+c+pg"
	strcat(host, "&f=");
	strcat(host, tag); // "xnsd1t1cl1"

	//printf("Final destination string : %s\n", host);

	curl = curl_easy_init();	

    if(!curl) {
		//fprintf(stderr, "error: curl_easy_init()\n");
		error();
        return -1;
    }

	curl_easy_setopt(curl, CURLOPT_URL, host);
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, stdout);

	freopen(filename, "w", stdout);

	code = curl_easy_perform(curl);
	
    if(code) {
		//fprintf(stderr, "error: %s\n", curl_easy_strerror(code));
		error();
		return -1;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);
	
    if(res != 200) {
		//fprintf(stderr, "error respose code: %ld\n", res);
		error();
		return res;
	}

	fclose(stdout);

	curl_easy_cleanup(curl);

	if((fptr = fopen(filename, "r")) == NULL) {
		printf("Can't open file\n");
		exit(1);
	}
	else {

		fseek(fptr, 0, SEEK_END);
		flen = ftell(fptr);
		fseek(fptr, 0, SEEK_SET);

		if(ftext = AllocVec(flen+16, MEMF_PUBLIC|MEMF_CLEAR)) {
			fread(ftext, 1, flen, fptr);
			set(txt_output, MUIA_TextEditor_Contents, ftext);
			FreeVec(ftext);
		}
	}
	fclose(fptr);

	return(0);
};

Object* build_gui(void) {

	App = MUI_NewObject(MUIC_Application,
		MUIA_Application_Author, (ULONG)"IKE",
		MUIA_Application_Base, (ULONG)"yMarketQuote",
		MUIA_Application_Copyright, (ULONG)"©2014, IKE",
		MUIA_Application_Description, (ULONG)"Connects to Yahoo and downloads market quotes",
		MUIA_Application_Title, (ULONG)"yMarketQuote",
		MUIA_Application_Version, (ULONG)"$VER: yMarketQuote-0.0.8 (2/1/14)",
		MUIA_Application_Window, (ULONG)(Win = MUI_NewObject(MUIC_Window,
			MUIA_Window_Title, (ULONG)"yMarketQuote", MUIA_Window_ID, MAKE_ID('M','A','I','N'),
			MUIA_Window_Menustrip, MUI_MakeObject(MUIO_MenustripNM, MenuData1, 0),

			WindowContents, VGroup,

			Child, ColGroup(3),
				Child, Label2("Stock Symbol(s):"),
				MUIA_ShortHelp, "Enter a stock symbol: i.e. 'goog' or multiple symbols with a '+' sign\n i.e, 'goog+csco.F+bp' etc...\n...see the 'Stock Exchange Help' Menu item for a list of stock exchange\n suffixes that can also be used...",
				Child, STR_quote = BetterStringObject, StringFrame, MUIA_CycleChain, TRUE, End,
				Child, clear_quote = KeyButton("Clear Symbol(s)",'s'),
			End,

			Child, ColGroup(3),
				Child, Label2("      Yahoo Tag(s):"),
				MUIA_ShortHelp, "Enter Yahoo Tag(s) string: i.e. 'xnsd1t1cl1'\n which will output: 'Stock Exchange, Name, Symbol, \nLast Trade Date, Last Trade Time, Change %, Last Trade Price'...\nsee 'www.gummy-stuff.org/Yahoo-data.htm' for more info...\n...or the 'Yahoo Tag Help' Menu item...",
				Child, STR_tag = BetterStringObject, StringFrame, MUIA_CycleChain, TRUE, End,
				Child, clear_tag = KeyButton("Clear Tag(s)",'t'),
			End,

			Child, HGroup,
				Child, txt_output = TextEditorObject,
				MUIA_TextEditor_Slider, scroll_output,
				MUIA_TextEditor_ReadOnly, TRUE,
				MUIA_TextEditor_Contents, "\33b\33c\n\nEnter Stock Symbol(s) and Tag(s) then press 'Get Quote!'",
				MUIA_ShortHelp, "Stock Quote\n Output Window",
			End,

			Child, scroll_output = ScrollbarObject, End,
			End,

			Child, HGroup, MUIA_Group_SameSize, TRUE,
				Child, Label2("                                                       "),
				Child, HGroup,
				Child, grab_gad = KeyButton("Get Quote!",'g'), MUIA_CycleChain, TRUE,
				End,
				Child, Label2("                                                       "),
			End,
	End,
TAG_END)),

// The Tag Help Window
	SubWindow, win_taghelp = WindowObject,
		MUIA_Window_Title, "Yahoo Tag Help",
		MUIA_Window_ID, MAKE_ID('H','E','L','P'),
			WindowContents, VGroup,

				Child, HGroup,
					Child, txt_taghelp = TextEditorObject,
					MUIA_TextEditor_Slider, scroll_taghelp,
					MUIA_TextEditor_ReadOnly, TRUE,
					MUIA_TextEditor_Contents, TAG_HELP,
				End,

				Child, scroll_taghelp = ScrollbarObject,
				End,
			End,
		End,
	End,

// The Stock Exchange Help Window
	SubWindow, win_exchangehelp = WindowObject,
		MUIA_Window_Title, "Stock Exchange Help",
		MUIA_Window_ID, MAKE_ID('E','X','C','G'),
			WindowContents, VGroup,

				Child, HGroup,
					Child, txt_exchangehelp = TextEditorObject,
					MUIA_TextEditor_Slider, scroll_exchangehelp,
					MUIA_TextEditor_ReadOnly, TRUE,
					MUIA_TextEditor_Contents, EXCHANGE_HELP,
				End,

				Child, scroll_exchangehelp = ScrollbarObject,
				End,
			End,
		End,
	End,
//TAG_END);
//}

// The Settings Window
	SubWindow, win_settings = WindowObject,
		MUIA_Window_Title, "Settings",
		MUIA_Window_ID, MAKE_ID('S','T','N','G'),
			WindowContents, VGroup,

				Child, ColGroup(2), GroupFrame,
					Child, Label2("Stock Symbol(s):"),
					Child, STR_stocksetting = BetterStringObject, StringFrame, MUIA_ExportID, 1, End,
					Child, Label2("Yahoo Tag(s):"),
					Child, STR_tagsetting = BetterStringObject, StringFrame, MUIA_ExportID, 2, End,
				//	  Child, Label2("Refresh (secs):"),
				//    Child, STR_refreshsetting = BetterStringObject, StringFrame, MUIA_ExportID, 3, End,
				End,

				Child, HGroup, MUIA_Group_SameSize, TRUE,
					Child, but_save   = MUI_MakeObject(MUIO_Button, "_Save"), 
					Child, but_use    = MUI_MakeObject(MUIO_Button, "_Use"), 
					Child, but_cancel = MUI_MakeObject(MUIO_Button, "_Cancel"), 
			End,
		End,
	End,
TAG_END);
}

void notifications(void) {

	// Load Preferences
	DoMethod(App, MUIM_Application_Load, MUIV_Application_Load_ENV);
	DoMethod(App, MUIM_Application_Load, MUIV_Application_Load_ENVARC);

	// Windows
	DoMethod(Win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	DoMethod(win_taghelp, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	win_taghelp, 3, MUIM_Set, MUIA_Window_Open, FALSE);

	DoMethod(win_exchangehelp, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	win_exchangehelp, 3, MUIM_Set, MUIA_Window_Open, FALSE);

	DoMethod(win_settings, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	win_settings, 3, MUIM_Set, MUIA_Window_Open, FALSE);

	// Prefs Buttons
	DoMethod(but_save, MUIM_Notify, MUIA_Pressed, FALSE,
	App, 2, MUIM_Application_ReturnID, SAVE);

	DoMethod(but_use, MUIM_Notify, MUIA_Pressed, FALSE,
	App, 2, MUIM_Application_ReturnID, USE);

	DoMethod(but_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
	App, 2, MUIM_Application_ReturnID, CANCEL);

	DoMethod(win_settings,MUIM_Window_SetCycleChain, STR_stocksetting,
    STR_tagsetting, /*STR_refreshsetting,*/ but_save, but_use, but_cancel,NULL);

    // Main Buttons
	DoMethod(clear_quote, MUIM_Notify, MUIA_Pressed, FALSE,
	App, 2, MUIM_Application_ReturnID, CLEARSTOCK);

	DoMethod(clear_tag, MUIM_Notify, MUIA_Pressed, FALSE,
	App, 2, MUIM_Application_ReturnID, CLEARTAG);

	DoMethod(grab_gad, MUIM_Notify, MUIA_Pressed, FALSE,
	App, 2, MUIM_Application_ReturnID, GRAB);

	DoMethod(txt_taghelp, MUIM_Notify, MUIA_TextEditor_Prop_Entries,
	MUIV_EveryTime, scroll_taghelp, 3, MUIM_Set, MUIA_Prop_Entries, MUIV_TriggerValue);

	set(txt_taghelp, MUIA_TextEditor_Slider, scroll_taghelp);

	DoMethod(txt_exchangehelp, MUIM_Notify, MUIA_TextEditor_Prop_Entries,
	MUIV_EveryTime, scroll_exchangehelp, 3, MUIM_Set, MUIA_Prop_Entries, MUIV_TriggerValue);

	set(txt_exchangehelp, MUIA_TextEditor_Slider, scroll_exchangehelp);

	DoMethod(txt_output, MUIM_Notify, MUIA_TextEditor_Prop_Entries,
	MUIV_EveryTime, scroll_output, 3, MUIM_Set, MUIA_Prop_Entries, MUIV_TriggerValue);

	set(txt_output, MUIA_TextEditor_Slider, scroll_output);

	get(STR_tagsetting, MUIA_String_Contents, &tag);
	get(STR_stocksetting, MUIA_String_Contents, &quote);

	set(STR_tag, MUIA_String_Contents, tag);
	set(STR_quote, MUIA_String_Contents, quote);
}

void main_loop(void) {

	ULONG signals = 0;

	set(Win, MUIA_Window_Open, TRUE);

	//automatically load saved stocks
	//get_stocks();

	while(running) {

		ULONG id = DoMethod(App, MUIM_Application_Input, &signals);

		switch(id) {

			case GRAB:
				get_stocks();
				break;

			case CLEARSTOCK:
				do_clear();
				break;

			case CLEARTAG:
				do_clear2();
				break;

			case MEN_EXCHANGEHELP:
				set(win_exchangehelp, MUIA_Window_Open, TRUE);
				break;
			
			case MEN_TAGHELP:
				set(win_taghelp, MUIA_Window_Open, TRUE);
				break;

			case MEN_ABOUT:
				MUI_Request(App, Win, 0, "About yMarketQuote", "*OK", about_text, NULL);
				break;

			case MEN_ABOUTMUI:
				if(!aboutwin) {
					aboutwin = AboutmuiObject, MUIA_Aboutmui_Application, App, End;
				}
				
                if(aboutwin)
					set(aboutwin,MUIA_Window_Open,TRUE);
				else
					DisplayBeep(0);
				break;

			case MEN_QUIT:
			case MUIV_Application_ReturnID_Quit:
				running = FALSE;
				break;

			case MEN_PREFS:
				set(win_settings, MUIA_Window_Open, TRUE);
				break;

			case SAVE: 
				get(STR_tagsetting, MUIA_String_Contents, &tag);
				get(STR_stocksetting, MUIA_String_Contents, &quote);

				set(STR_tag, MUIA_String_Contents, tag);
				set(STR_quote, MUIA_String_Contents, quote);
                  
				DoMethod(App, MUIM_Application_Save, MUIV_Application_Save_ENVARC);
				set(win_settings, MUIA_Window_Open, FALSE);
                get_stocks();
				break;

			case USE:
				get(STR_tagsetting, MUIA_String_Contents, &tag);
				get(STR_stocksetting, MUIA_String_Contents, &quote);

				set(STR_tag, MUIA_String_Contents, tag);
				set(STR_quote, MUIA_String_Contents, quote);

				DoMethod(App, MUIM_Application_Save, MUIV_Application_Save_ENV);
                get_stocks();
				break;

			case CANCEL:
				set(win_settings, MUIA_Window_Open, FALSE);
				break;

			case MEN_DELETE:
				remove("ram:stockquote.csv");

				break;

			case MEN_MUIPREFS:
				DoMethod(App, MUIM_Application_OpenConfigWindow, 0);
				break;
		}       
		if (running && signals) Wait(signals);
	}
	set(Win, MUIA_Window_Open, FALSE);
}

int main(void) {

	if(!open_libs()) {

		printf("Cannot open libs\n");
		return(0);
	}

	if (App = build_gui()) {

		notifications();
		main_loop();
		MUI_DisposeObject(App);
	}
	close_libs();
	return 0;
}
