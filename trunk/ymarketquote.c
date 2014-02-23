/* yMarketQuote 0.1 by IKE (compiled with gcc 2.95.3/MorphOS)
 *
 * Connects to Yahoo and downloads market quotes
 *
 * ike@ezcyberspace.com   Date: 2/15/14
 *
 * NList/parsecsv code based on ViewCSV. Thanks Watertonian!
 *
 * gcc -o yMarketQuote ymarketquote.c -lcurl -s -Wall
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <clib/gadtools_protos.h>
#include <MUI/BetterString_mcc.h>
#include <MUI/NListview_mcc.h>
#include <curl/curl.h>
#include <SDI_hook.h>

char *version = "$VER: yMarketQuote-0.1";

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Library  *MUIMasterBase;

#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))

// Main Buttons
#define GRAB        44
#define CLEARSTOCK  45
#define CLEARTAG    46
// Settings Buttons
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
version 0.1 (2/15/14)\n
Connects to Yahoo and downloads market quotes\n
\n";

Object *STR_quote, *STR_tag, *STR_tagsetting, *STR_stocksetting,/**STR_refreshsetting,*/ *aboutwin;

APTR app, grab_gad, clear_quote, clear_tag, but_save, but_use, but_cancel, win_settings;

BOOL running = TRUE;

// cURL
CURL *curl;
CURLcode code;
long res;
char *quote, *tag;
char host[1024];
char filename[] = "ram:stockquote.csv";

BOOL Open_Libs(BOOL active) {

	if(active) {

		if ( !(IntuitionBase=(struct IntuitionBase *) OpenLibrary("intuition.library",39)) ) return(0);
		if ( !(GfxBase=(struct GfxBase *) OpenLibrary("graphics.library",0)) ) {

			CloseLibrary((struct Library *)IntuitionBase);
			return(0);
		};
		if ( !(MUIMasterBase=OpenLibrary(MUIMASTER_NAME,19)) ) {

			CloseLibrary((struct Library *)GfxBase);
			CloseLibrary((struct Library *)IntuitionBase);
			return(0);
		};
	}
	else {

		if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
		if (GfxBase) CloseLibrary((struct Library *)GfxBase);
		if (MUIMasterBase) CloseLibrary(MUIMasterBase);
		return(0);
	};
	return(1);
};

enum {list_csv, list_info, list_lastentry};
enum {group_main, group_toolbar, group_mainview, group_lastentry};
enum {window_main, window_lastentry};
enum {mui_notify, mui_list};
APTR lists[list_lastentry];
APTR listviews[list_lastentry];
APTR application();
APTR titleslider;
APTR groups[group_lastentry];
APTR windows[window_lastentry];
char maintitle[255];
char *wintitles[] = {"yMarketQuote - No file loaded", "File information", "Entry information", NULL};
enum {string_loaded, string_cols, string_rows, string_blankentry, string_titlebar, string_lastentry};
char *strings[] = {"Loaded", "Cols:", "Rows:", "", "BAR,", NULL};
char blank_entry[255];
enum{sepcode_comma, sepcode_tab, sepcode_semicolon, sepcode_cr, sepcode_quote, sepcode_custom, sepcode_lastentry};
char *sepcodes = ",\t;\n\"";
char *master_separator[]={",", "TAB", ";", "CR", "\" ","Custom", NULL};
char *labels_separator[sepcode_lastentry];
char *labels_escape[sepcode_lastentry];
char *labels_newline[sepcode_lastentry];
char *currentfile;
struct CSVfile
{
	char separators[3];
	int titlerow;
	int columns;
	int rows;
	int titlecolumn;
	int filesize;
	LONG *offsets;
	char *data;
	LONG blanktag;
};
LONG *csvoffsets;
int *rowlist, *infolist;
char *colinfolist[255];
char csvtitle[1024];
enum{csv_newfield, csv_escape, csv_newrow};
struct CSVfile myCSV=
{
	",\"\n",
	0,
	0,0,0,
	0,
	0,
	"No File Loaded\n",
};
int csvsort_col = 0;

// Hook declarations
HOOKPROTONH(MuiGadFunc, LONG, Object *app, LONG *args); MakeStaticHook(MuiGadHook, MuiGadFunc);
HOOKPROTONH(nlistFunc, long, char **array, LONG *args); MakeStaticHook(nlistHook, nlistFunc);
HOOKPROTONH(compareFunc, LONG, LONG cst1, LONG cst2); MakeStaticHook(compareHook, compareFunc);

// quickload function - Automagically allocate memory, load a file to it, and return memory pointer
#define quickfile_gauge(a) 
#define quickfile_set(a) 
#define quickfile_clear(a) 

char *quickfile_errors[] = {"No Error", "Memory Error", "File not found", "Save file error", "Wrong file type", NULL};
enum {quickfile_noerror, quickfile_memoryerror, quickfile_notfounderror, quickfile_saveerror, quickfile_wrongfile, quickfile_lastentry};
int quickfile_ecode = 0;
int quickfile_filesize = 0;

char *quickload(char *filebuffer, char *filename, char *header) {

	int ecode = 0;
	int filesize = 0;
	int headersize = 0;
	int count = 0;
	int headeroffset = 0;
	char tch = 0;
	FILE *inputfile;
	quickfile_ecode = quickfile_noerror;
	if((inputfile = fopen(filename, "r"))) {
		fseek(inputfile, 0, SEEK_END);
		filesize = (ftell(inputfile)+1)*sizeof(char);
		filebuffer = NULL;
		filebuffer = (char *)realloc((char *)filebuffer, filesize);
		quickfile_filesize = filesize;
		if(filebuffer == NULL) {
			filebuffer = (char *)realloc((char *)filebuffer, (strlen(quickfile_errors[quickfile_memoryerror])+1));
			sprintf(filebuffer, "%s", quickfile_errors[quickfile_memoryerror]);
		}
		else {
			fseek(inputfile, 0, SEEK_SET);
			if(header == NULL) {
				headersize=-1;
			}
			else {
				headersize = strlen(header);
			};
			quickfile_set(filesize);
			for(count = 0; count < filesize; count++) {
				quickfile_gauge(count);
				if(count == headersize) {
					headeroffset = count;
					filebuffer[count] = 0;
					if(strcmp(filebuffer, header)!= 0) {
						count = filesize;
						filebuffer = NULL;
						filebuffer = (char *)realloc((char *)filebuffer, (strlen(quickfile_errors[quickfile_wrongfile])+1));
						sprintf(filebuffer, "%s", quickfile_errors[quickfile_wrongfile]);
						ecode = quickfile_wrongfile;
                        quickfile_ecode = quickfile_wrongfile;
					};
				};
				if(ecode == quickfile_noerror) {
					tch = fgetc(inputfile);
						filebuffer[count-headeroffset] = tch;
				};
			};
			filebuffer[count-1] = 0;
			if(ecode == quickfile_noerror) {
				if(header == NULL) headersize = 0;
				filebuffer[filesize-headersize] = 0;
			};
			quickfile_clear(0);
		};
	}
	else {
		filebuffer = (char *)realloc((char *)filebuffer, (strlen(quickfile_errors[quickfile_notfounderror]+1)));
		sprintf(filebuffer, "%s", quickfile_errors[quickfile_notfounderror]);
		ecode = quickfile_notfounderror;
        quickfile_ecode = quickfile_notfounderror;
	};
	fclose(inputfile);
	return(filebuffer);
};

// dolist function
void dolist(int list_id, int list_entry) {
	LONG result;
    int count = 0;
	switch(list_id) {
		case list_csv:		  
			if(result) {
				DoMethod(lists[list_info], MUIM_Set,MUIA_Disabled, TRUE);
				DoMethod(lists[list_info], MUIM_NList_Clear);
				for(count = 0; count < myCSV.columns;count++) {
					infolist[count] = ((list_entry+1)*myCSV.columns)+count;
					DoMethod(lists[list_info], MUIM_NList_InsertSingle, infolist[count], MUIV_NList_Insert_Bottom);
				};
				DoMethod(lists[list_info] ,MUIM_Set, MUIA_Disabled, FALSE);
			};
		break;
	};
};

// parsecsv function
void parsecsv(char *csvfile) {
	int count = 0; int countlen = 0; BOOL seedata = TRUE; int currentcols = 0; int currentrows = 0; int fieldcount = 0;
	char etext[255];

	currentfile = NULL;
	currentfile = quickload(currentfile, csvfile, NULL);
	if(quickfile_ecode == quickfile_noerror) {
		sprintf(maintitle, "%s - %s", "yMarketQuote", csvfile);
		DoMethod(windows[window_main], MUIM_Set, MUIA_Window_Title, maintitle);
		DoMethod(listviews[list_csv], MUIM_Set, MUIA_Disabled, TRUE);
		countlen = strlen(currentfile);
		myCSV.filesize = countlen;
		myCSV.blanktag = countlen+1;
		myCSV.data = currentfile;
		myCSV.rows = 0;
		myCSV.columns = 0;
		quickfile_clear(0);
		quickfile_set(countlen);
		DoMethod(lists[list_csv], MUIM_NList_Clear);
		for(count = 0; count < countlen; count++) {
			quickfile_gauge(count);
			if(myCSV.data[count] == myCSV.separators[csv_newfield]) {
				if(seedata) {
					myCSV.data[count] = 0;
					currentcols++;
					if(currentcols > myCSV.columns) myCSV.columns = currentcols;
				};
			};
			if(myCSV.data[count] == myCSV.separators[csv_newrow]) {
				if(seedata) {
					myCSV.data[count] = 1;
					myCSV.rows++;
					currentcols++;
					if(currentcols > myCSV.columns) myCSV.columns = currentcols;
					currentcols = 0;
				};
			};
			if(myCSV.data[count] == myCSV.separators[csv_escape]) {
				if(seedata) seedata = FALSE;
				else seedata = TRUE;
			};
		};
		DoMethod(titleslider, MUIM_Set, MUIA_Slider_Max, myCSV.rows);
		sprintf(csvtitle, "");
		myCSV.titlerow = 1;
		for(count = 0; count < myCSV.columns; count++)sprintf(csvtitle, "%s%s", csvtitle, strings[string_titlebar]);
		csvoffsets = (LONG *)realloc((LONG *)csvoffsets, (sizeof(int)*(1+(myCSV.rows*myCSV.columns))));
		rowlist = (int *)realloc((int *)rowlist, (myCSV.rows*sizeof(int)));
		infolist = (int *)realloc((int *)infolist, (myCSV.columns*sizeof(int)));
		if((csvoffsets == NULL)||(rowlist == NULL)||(infolist == NULL)) {
			printf("MUI Memory error...\n");  
		}
		else {
			myCSV.offsets = csvoffsets;
			myCSV.offsets[0] = 0;
			fieldcount = 1;
			quickfile_clear(0);
			quickfile_set(countlen);
			currentcols = 0;
			currentrows = 0;
			for(count = 0; count < countlen; count++) {
				quickfile_gauge(count);
				if((unsigned char)myCSV.data[count] <= 1) {
					myCSV.offsets[fieldcount] = count+1;
					currentcols++;
					fieldcount++;
					if(myCSV.data[count] == 1) {
						while(currentcols < myCSV.columns) {
							myCSV.offsets[fieldcount] = count+1;
							myCSV.offsets[fieldcount-1] = myCSV.blanktag;
							fieldcount++; currentcols++;
						};
						myCSV.data[count] = 0;
						rowlist[currentrows] = currentrows;
						if(currentrows==0) DoMethod(lists[list_csv], MUIM_Set, MUIA_NList_Format, csvtitle);
						if((currentrows+1)!= myCSV.titlerow) DoMethod(lists[list_csv], MUIM_NList_InsertSingle, rowlist[currentrows]+1, MUIV_NList_Insert_Bottom);
						currentrows++;
						currentcols = 0;
					};
				};
			};
			quickfile_clear(0);
			sprintf(etext, "%s %s(%s%i %s%i)", csvfile, strings[string_loaded], strings[string_cols], myCSV.columns, strings[string_rows], myCSV.rows);
			DoMethod(listviews[list_csv], MUIM_Set, MUIA_Disabled, FALSE);
		};
	};
};

// get_stocks function - connects to Yahoo and downlads file to ram:
int get_stocks() {

	get(STR_quote, MUIA_String_Contents, &quote);
	get(STR_tag, MUIA_String_Contents, &tag);

	strcpy(host, "http://download.finance.yahoo.com/d/quotes.csv?s=");
	strcat(host, quote); // "pep+bp+msft+goog+csco+csco.f+xom+ge+c+pg"
	strcat(host, "&f=");
	strcat(host, tag); // "xnsd1t1cl1"

	curl = curl_easy_init();

    if(!curl) {
		//fprintf(stderr, "error: curl_easy_init()\n");
        printf("Error connecting...\n");
        return -1;
    }

	curl_easy_setopt(curl, CURLOPT_URL, host);
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, stdout);

	freopen(filename, "w", stdout);

	code = curl_easy_perform(curl);

    if(code) {
		//fprintf(stderr, "error: %s\n", curl_easy_strerror(code));
        printf("Error connecting...\n");
		return -1;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res);

    if(res != 200) {
		//fprintf(stderr, "error respose code: %ld\n", res);
        printf("Error connecting...\n");
		return res;
	}

	fclose(stdout);
	curl_easy_cleanup(curl);
    parsecsv(filename);
	return(0);
};

// notify function
void notify(int groupname, BOOL active) {
	if(groupname == group_main) {
		if(active) {

            // Load Preferences
			DoMethod(app, MUIM_Application_Load, MUIV_Application_Load_ENV);
			DoMethod(app, MUIM_Application_Load, MUIV_Application_Load_ENVARC);

			DoMethod(windows[window_main], MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 5, MUIM_CallHook, &MuiGadHook, mui_notify, group_main, FALSE);
			DoMethod(windows[window_main], MUIM_Set, MUIA_Window_Open, TRUE);
			DoMethod(lists[list_csv], MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime, app, 5, MUIM_CallHook, &MuiGadHook, mui_list, list_csv, MUIV_TriggerValue);

			// Prefs Window/Buttons
			DoMethod(win_settings, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			win_settings, 3, MUIM_Set, MUIA_Window_Open, FALSE);

			DoMethod(but_save, MUIM_Notify, MUIA_Pressed, FALSE,
			app, 2, MUIM_Application_ReturnID, SAVE);

			DoMethod(but_use, MUIM_Notify, MUIA_Pressed, FALSE,
			app, 2, MUIM_Application_ReturnID, USE);

			DoMethod(but_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
			app, 2, MUIM_Application_ReturnID, CANCEL);

			DoMethod(win_settings,MUIM_Window_SetCycleChain, STR_stocksetting,
    		STR_tagsetting, /*STR_refreshsetting,*/ but_save, but_use, but_cancel, NULL);

    		// Main Buttons
			DoMethod(clear_quote, MUIM_Notify, MUIA_Pressed, FALSE,
			app, 2, MUIM_Application_ReturnID, CLEARSTOCK);

			DoMethod(clear_tag, MUIM_Notify, MUIA_Pressed, FALSE,
			app, 2, MUIM_Application_ReturnID, CLEARTAG);

			DoMethod(grab_gad, MUIM_Notify, MUIA_Pressed, FALSE,
			app, 2, MUIM_Application_ReturnID, GRAB);

			get(STR_tagsetting, MUIA_String_Contents, &tag);
			get(STR_stocksetting, MUIA_String_Contents, &quote);

			set(STR_tag, MUIA_String_Contents, tag);
			set(STR_quote, MUIA_String_Contents, quote);
			}
			else {
				DoMethod(windows[window_main], MUIM_KillNotify, MUIA_Window_CloseRequest);
				DoMethod(lists[list_csv], MUIM_KillNotify, MUIA_NList_Active);
				DoMethod(windows[window_main], MUIM_Set, MUIA_Window_Open, FALSE);
				running = FALSE;
			};
	};
};

HOOKPROTONH(MuiGadFunc, LONG, Object *app, LONG *args) {
	int ecode = 0;
	switch (args[0]) {
		case mui_list:
			dolist(args[1], args[2]);
		break;
		case mui_notify:
		    notify(args[1], args[2]);
		break;
	}
	return(ecode);
};

HOOKPROTONH(compareFunc, LONG, LONG cst1, LONG cst2) {
	char string1[255]; char string2[255]; int ecode = 0;
	sprintf(string1, "%s", myCSV.data+myCSV.offsets[(cst1*myCSV.columns)+csvsort_col]);
	sprintf(string2, "%s", myCSV.data+myCSV.offsets[(cst2*myCSV.columns)+csvsort_col]);
	ecode = stricmp((char *)string2, (char *)string1);
	return(ecode);
};

HOOKPROTONH(nlistFunc, long, char **array, LONG *args) {
	int ecode = 0;
	int count = 0; int rowstart = 0;
	if(strcmp(array[0], "yMarketQuote") == 0) {
		if(myCSV.titlerow > 0) {
			rowstart = ((myCSV.titlerow)-1)*myCSV.columns;
			for(count = 0; count < myCSV.columns; count++) {
				array[count] = (char *)myCSV.data+myCSV.offsets[rowstart+count];
			};
		}
		else {
			for(count = 0; count < myCSV.columns; count++) {
				array[count] = "yMarketQuote";
			};
		};
	}
	else {
		rowstart = (((int)array[0])-1)*myCSV.columns;
		for(count = 0; count < myCSV.columns; count++) {
			if(myCSV.offsets[count+rowstart] > myCSV.filesize) array[count] = blank_entry;
			else array[count] = (char *)myCSV.data+myCSV.offsets[rowstart+count];
		};
	};
	return(ecode);
};

// initdata
BOOL initdata(int argC, char *argV[]) {
	int mostelements = 0; int count = 0; int ecode = 0;
	struct WBStartup *argmsg;
	if(sepcode_lastentry > mostelements) mostelements = sepcode_lastentry;
	if(window_lastentry > mostelements) mostelements = window_lastentry;
	for(count = 0; count < mostelements; count++) {
		if(count < sepcode_lastentry) {
			labels_separator[count] = master_separator[count];
			labels_escape[count] = master_separator[count];
			labels_newline[count] = master_separator[count];
		};
	};
	sprintf(maintitle, "%s", wintitles[window_main]);
	argmsg = (struct WBStartup *)argV;
	colinfolist[0] = version;
	colinfolist[1] = NULL;
	strings[string_blankentry] = blank_entry;
	labels_separator[sepcode_lastentry] = NULL;
	labels_escape[sepcode_lastentry] = NULL;
	labels_newline[sepcode_lastentry] = NULL;
	DoMethod(app, MUIM_Application_Load, MUIV_Application_Save_ENV);
	return(ecode);
};

APTR application() {
// Main Window
	lists[list_csv] = NListObject, MUIA_NList_Title, "yMarketQuote",
		MUIA_NList_CompareHook, &compareHook,
		MUIA_NList_DisplayHook, &nlistHook,
	End;
	listviews[list_csv] = NListviewObject,
		MUIA_Background, MUII_ListBack,
		MUIA_Frame, MUIV_Frame_InputList,
		MUIA_NListview_NList, lists[list_csv],
	End;
	titleslider = SliderObject,
		MUIA_Slider_Horiz, TRUE,
		MUIA_Slider_Max, 0,
		MUIA_Slider_Level, 0,
	End;
	groups[group_toolbar] = GroupObject,
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
	End;
	groups[group_mainview] = GroupObject,
		Child, listviews[list_csv],
		Child, HGroup, MUIA_Group_SameSize, TRUE,
			Child, Label2("                                                       "),
			Child, HGroup,
			Child, grab_gad = KeyButton("Get Quote!",'g'), MUIA_CycleChain, TRUE,
			End,
			Child, Label2("                                                       "),
		End,
	End;
	groups[group_main] = VGroup,
		Child, GroupObject, MUIA_Background, MUII_GroupBack,
			Child, groups[group_toolbar],
			Child, groups[group_mainview],
		End,
	End;
	windows[window_main] = WindowObject,
		MUIA_Window_Title, maintitle,
		MUIA_Window_ID , MAKE_ID('M','A','I','N'),
		MUIA_Window_AppWindow, TRUE,
        MUIA_Window_Menustrip, MUI_MakeObject(MUIO_MenustripNM, MenuData1, 0),
		WindowContents, groups[group_main], End;
		return(ApplicationObject,
		MUIA_Application_Title, "yMarketQuote",
		MUIA_Application_Version, version,
		MUIA_Application_Copyright, "©2014 IKE",
		MUIA_Application_Author, "IKE",
		MUIA_Application_Description, "Connects to Yahoo and downloads market quotes\n",
		MUIA_Application_Base, "yMarketQuote",
		SubWindow, windows[window_main],

// Settings Window
	SubWindow, win_settings = WindowObject,
		MUIA_Window_Title, "Settings",
		MUIA_Window_ID, MAKE_ID('S','T','N','G'),
			WindowContents, VGroup,
				Child, ColGroup(2), GroupFrame,
					Child, Label2("Stock Symbol(s):"),
					Child, STR_stocksetting = BetterStringObject, StringFrame, MUIA_ExportID, 1, End,
					Child, Label2("Yahoo Tag(s):"),
					Child, STR_tagsetting = BetterStringObject, StringFrame, MUIA_ExportID, 2, End,
				//	Child, Label2("Refresh (secs):"),
				//  Child, STR_refreshsetting = BetterStringObject, StringFrame, MUIA_ExportID, 3, End,
				End,
				Child, HGroup, MUIA_Group_SameSize, TRUE,
					Child, but_save   = MUI_MakeObject(MUIO_Button, "_Save"),
					Child, but_use    = MUI_MakeObject(MUIO_Button, "_Use"),
					Child, but_cancel = MUI_MakeObject(MUIO_Button, "_Cancel"),
				End,
			End,
	End,
End);
};

// Main program
int main(int argc, char *argv[]) {
    ULONG signals = 0;

	if (! Open_Libs(TRUE)) {
        printf("MUI Library error...\n"); 
        return(0);
	}
	if(!initdata(argc, argv)) app = application();
	
    if (!app) {
        printf("MUI Application error...\n");
        return(0);
	}
    notify(group_main, TRUE);

    while(running) {

        ULONG id = DoMethod(app, MUIM_Application_Input, &signals);

		switch(id) {

			case GRAB:
				get_stocks();
				break;

			case CLEARSTOCK:
				set(STR_quote, MUIA_String_Contents, 0);
				break;

			case CLEARTAG:
				set(STR_tag, MUIA_String_Contents, 0);
				break;

			case MEN_EXCHANGEHELP:				  
                parsecsv("PROGDIR:data/stockmarkets.csv");
				break;

			case MEN_TAGHELP:				 
                parsecsv("PROGDIR:data/yahootags.csv");
				break;

			case MEN_ABOUT:
				MUI_Request(app, windows[window_main], 0, "About yMarketQuote", "*OK", about_text, NULL);
				break;

			case MEN_ABOUTMUI:
				if(!aboutwin) {
					aboutwin = AboutmuiObject, MUIA_Aboutmui_Application, app, End;
				}
                if(aboutwin)
					set(aboutwin, MUIA_Window_Open, TRUE);
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

				DoMethod(app, MUIM_Application_Save, MUIV_Application_Save_ENVARC);
				set(win_settings, MUIA_Window_Open, FALSE);
                get_stocks();
				break;

			case USE:
				get(STR_tagsetting, MUIA_String_Contents, &tag);
				get(STR_stocksetting, MUIA_String_Contents, &quote);

				set(STR_tag, MUIA_String_Contents, tag);
				set(STR_quote, MUIA_String_Contents, quote);

				DoMethod(app, MUIM_Application_Save, MUIV_Application_Save_ENV);
                get_stocks();
				break;

			case CANCEL:
				set(win_settings, MUIA_Window_Open, FALSE);
				break;

			case MEN_DELETE:
		        remove(filename);
				break;

			case MEN_MUIPREFS:
				DoMethod(app, MUIM_Application_OpenConfigWindow, 0);
				break;
		}
		if (running && signals) Wait(signals);
	}
    notify(group_main, FALSE);
    MUI_DisposeObject(app);
	Open_Libs(FALSE);

    return 0;
}
