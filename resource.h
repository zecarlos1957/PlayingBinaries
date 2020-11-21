#ifndef _WIN32_IE
#define _WIN32_IE 0x500
#endif // _WIN32_IE

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500                     // for WM_MOUUSEWHEEL. Is this right?
#endif

#define IDC_STATIC (-1)


#define DLG_MAIN    100
#define DLG_ABOUT    1000
#define IDC_TABVIEW     101
#define IDC_TREEVIEW   102
#define IDC_TOOLBAR    103

#define EHDR_PAGE      1200
#define IDC_EHDR      1201
#define IDC_HEXDATA   1202
#define IDC_PEHDR     1203

#define SHDR_PAGE      300
#define IDC_SHDR      105
#define IDC_EDIT      106
#define IDC_HEX       107
#define IDC_HEXEDIT   108
#define IDC_HDRDATA   109
#define IDC_DASM      110
#define IDC_SYMLIST   111
#define IDC_PHDR   112

#define PHDR_PAGE      400

#define IDR_APPMENU  1300
#define IDM_FILE_NEW      200
#define IDM_FILE_OPEN     201
#define IDM_FILE_CLOSE    202
#define IDM_FILE_SAVE     203
#define IDM_FILE_SAVEAS   204
#define IDM_FILE_EXIT     205
#define IDM_ESHELLEX 206
#define IDM_DSHELLEX 207
#define IDM_PREFER   208
#define IDM_HELP     209
#define IDM_ABOUT    210
#define IDM_EXIT    210


#define IDS_SECTION_NAMES  1300
#define IDS_SECTION_FADDR  1301
#define IDS_SECTION_MADDR  1302
#define IDS_SECTION_SIZE   1303
#define IDS_SECTION_FLAG   1304
#define IDS_SECTION_INFO   1305

#define IDS_PROG_TYPE    1306
#define IDS_PROG_OFF     1307
#define IDS_PROG_VADDR   1308
#define IDS_PROG_PADDR   1309
#define IDS_PROG_FILESZ  1310
#define IDS_PROG_MEMSZ   1311
#define IDS_PROG_FLAGS   1312
#define IDS_PROG_ALIGN   1313

#define HEX_PAGE      500
#define DASM_PAGE      600
#define PE_PAGE       800
#define ID_EHDR 400
#define ID_SHDR 401
#define ID_PHDR 402
#define ID_SYM  403
#define ID_PEHDR 404
#define ID_RELOC 405
#define ID_HEXEDIT 406
#define ID_DASM 407
#define ID_ANY 408

#define IDI_FOLDEROPEN   501
#define IDI_APPDEFAULT    502
#define IDI_FOLDERCLOSED   503

#define IDI_BIN   1600
#define IDI_BINARIES 1601

#define SYMBOL_PAGE  700
#define IDC_CHKOBJ      701
#define IDC_CHKFUNC      702
#define IDC_CHKSECTION      703
#define IDC_CHKFILE      704
#define IDC_SYMBOLTAB    705
