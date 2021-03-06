///////////////////////////////////////////////////////////////////////////////
// main.cpp
// ========
// main driver
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2006-06-28
// UPDATED: 2006-07-08
///////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN             // exclude rarely-used stuff from Windows headers


#include "resource.h"
#include <windows.h>
#include <commctrl.h>                   // common controls
#include <commdlg.h>
#include <stdio.h>
#include "elf.h"
#include "pages.h"
#include "CFileStream.h"


#define TB_MAX_BUTTON 7
#define MAXTAB 10


extern int RunSilent(LPSTR cfg);


class App
{
    HWND hwnd;
    HINSTANCE hInst;

    HWND toolBar;
    HWND tabView;
    HWND treeView;

    HWND TabList[MAXTAB];
    int nTab;
    Page *pCurrPage;
    CFileStream *pFileStream;

    TBBUTTON* build_toolbar(int &sz);
    HTREEITEM insertTreeItem(const char* str, LPARAM lParam = 0, HTREEITEM parent = TVI_ROOT, HTREEITEM insertAfter = TVI_LAST, int imageIndex = 0, int selectedImageIndex = 0)const;
public:
    App(HINSTANCE hinst);
    void Init();
    ~App();
    int MainMessageLoop();
    DWORD GetFileSignature(LPCSTR path);
    void Init_ELF_Page(LPSTR fname);
    void Init_PE_Page(LPSTR fname);
    void Init_ANY_Page(LPSTR fname);
    static BOOL CALLBACK  DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void OnFileOpen();
    void OnFileClose();
    void OnFileExit();
    void OnNotify(WPARAM wParam, LPARAM lParam);
    void OnClose(){}
    void OnAbout();
};

HBITMAP hbmp;
HWND hTab1,hTab2,hTab3;

BOOL ShowLicence(HWND hEdit, LPCTSTR pszFileName)
{
	HANDLE hFile;
	BOOL bSuccess = FALSE;
    HFONT hf = (HFONT)GetStockObject(ANSI_VAR_FONT);
    SendMessage(hEdit,WM_SETFONT, (WPARAM)hf, TRUE);

	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;

		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize != 0xFFFFFFFF)
		{
			LPSTR pszFileText;

			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL)
			{
				DWORD dwRead;

				if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
				{
					pszFileText[dwFileSize] = 0; // Add null terminator
					if(SetWindowText(hEdit, pszFileText))
						bSuccess = TRUE; // It worked!
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

BOOL CALLBACK AboutDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HWND current;
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            HINSTANCE hInst = reinterpret_cast<HINSTANCE>(GetWindowLong(hwndDlg,GWL_HINSTANCE));
            HICON hic = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APPDEFAULT));
            SendMessage(hwndDlg, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hic);
            SendMessage(hwndDlg, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hic);

            hbmp = LoadBitmap(hInst,MAKEINTRESOURCE(IDI_BIN));
            if(hbmp == NULL) MessageBox(hwndDlg,"Error loading Bitmap","Error",MB_OK);
            HWND hBtn = GetDlgItem(hwndDlg,IDC_BTOK);

            TCITEM tie = {0};
            CHAR *Tab1 = "Cover";
            CHAR *Tab2 = "License";
            CHAR *Tab3 = "Disassembly";
            HWND hTab = GetDlgItem(hwndDlg, IDC_TABABOUT);

            tie.mask = TCIF_TEXT;
            tie.pszText = Tab1;
            TabCtrl_InsertItem(hTab,0,&tie);
            tie.pszText = Tab2;
            TabCtrl_InsertItem(hTab,1,&tie);
            tie.pszText = Tab3;
            TabCtrl_InsertItem(hTab,2,&tie);

            RECT tr = {0};
            TabCtrl_GetItemRect(hTab,0,&tr);

            hTab3 = CreateWindowEx(WS_EX_STATICEDGE,"EDIT","",WS_CHILD|ES_MULTILINE|ES_READONLY|WS_VSCROLL,
                                 10,35,505,110,hTab,(HMENU)IDC_LICENCE2,hInst,NULL);
            hTab2 = CreateWindowEx(WS_EX_STATICEDGE,"EDIT","",WS_CHILD|ES_MULTILINE|ES_READONLY|WS_VSCROLL,
                                 10,35,505,110,hTab,(HMENU)IDC_LICENCE,hInst,NULL);
            hTab1 = CreateWindowEx(WS_EX_STATICEDGE,"STATIC","",WS_CHILD|WS_VISIBLE ,
                                 10,35,505,110,hTab,(HMENU)NULL,hInst,NULL);


            current = hTab1;
            ShowWindow(current,SW_SHOWNORMAL);

            return TRUE;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BITMAP bm;
            HDC hdc=BeginPaint(hwndDlg,&ps);
            HDC hdcMem=CreateCompatibleDC(hdc);
            HBITMAP hbmOld=(HBITMAP)SelectObject(hdcMem,hbmp);
            GetObject(hbmp,sizeof(bm),&bm);
            BitBlt(hdc,25,27,bm.bmWidth,bm.bmHeight,hdcMem,0,0,SRCCOPY);
            SelectObject(hdcMem,hbmOld);
            DeleteDC(hdcMem);
            EndPaint(hwndDlg,&ps);
            return 0;
        }
        case  WM_NOTIFY:
        {
            HWND hTab=GetDlgItem(hwndDlg,IDC_TABABOUT);
            LPNMHDR lpnmhdr=(LPNMHDR)lParam;
            if(lpnmhdr->code == TCN_SELCHANGE)
            {
                int iPage = TabCtrl_GetCurSel(hTab);

                switch(iPage)
                {
                    case 0:
                        ShowWindow(current, SW_HIDE);
                        ShowWindow(hTab1, SW_SHOW);
                        current = hTab1;
                        return 0;
                    case 1:
                        ShowLicence(hTab2, "License");
                        ShowWindow(current, SW_HIDE);
                        ShowWindow(hTab2, SW_SHOW);
                        current = hTab2;
                        return 0;
                    case 2:
                        ShowLicence(hTab3, "License_dasm");
                        ShowWindow(current, SW_HIDE);
                        ShowWindow(hTab3, SW_SHOW);
                        current = hTab3;
                        return 0;
                    default:break;
                }

            }
            return 0;
        }
        case WM_CLOSE:
            DeleteObject(hbmp);
            EndDialog(hwndDlg, 0);
            return 0;
    }
    return FALSE;
}

App::App(HINSTANCE hinst):hInst(hinst), pCurrPage(NULL), pFileStream(NULL), nTab(0)
{
    INITCOMMONCONTROLSEX commonCtrls;
    commonCtrls.dwSize = sizeof(commonCtrls);
    commonCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&commonCtrls);
    hwnd = CreateDialogParam(hInst, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)App::DialogProc, (LPARAM)this);

}

void App::Init()
{
    HMENU mn = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_APPMENU));
    SetMenu(hwnd, mn);

    toolBar = GetDlgItem(hwnd, IDC_TOOLBAR);
    tabView = GetDlgItem(hwnd, IDC_TABVIEW);
    treeView = GetDlgItem(hwnd, IDC_TREEVIEW);

    int sz;
    TBBUTTON *tbb = build_toolbar(sz);
    SendMessage(toolBar, TB_ADDBUTTONS, sz, (LPARAM)tbb);

    ShowWindow(hwnd, SW_SHOW);

}

App::~App()
{

}

int App::MainMessageLoop()
{
   MSG msg;

    while(GetMessage(&msg,NULL,0,0))
    {
        if (!::IsDialogMessage (hwnd, &msg))
        {
            ::TranslateMessage (&msg);
            ::DispatchMessage (&msg);
        }
    }

    return (int)msg.wParam;
}

BOOL CALLBACK  App::DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static App *app = NULL;
    switch(msg)
    {
        case WM_INITDIALOG:
            app = (App*)lParam;
            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDM_FILE_OPEN:
                    app->OnFileOpen();
                    break;
                case IDM_FILE_CLOSE:
                    app->OnFileClose();
                    break;
                case IDM_FILE_EXIT:
                    app->OnFileClose();
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case IDM_ABOUT:
                    app->OnAbout();
                    break;
            }
            break;

        case WM_NOTIFY:
            app->OnNotify(wParam, lParam);
            break;

        case WM_CLOSE:
            app->OnClose();
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return FALSE;
}

TBBUTTON* App::build_toolbar(int &sz)
{
    sz = TB_MAX_BUTTON;
    static  TBBUTTON tbb[TB_MAX_BUTTON];

    TBADDBITMAP tbab;
    SendMessage(toolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    tbab.hInst = HINST_COMMCTRL;
    tbab.nID = IDB_STD_LARGE_COLOR;
    SendMessage(toolBar, TB_ADDBITMAP, 0, (LPARAM)&tbab);
    SendMessage(toolBar, TB_AUTOSIZE, 0, 0);

    ZeroMemory(tbb, sizeof(tbb));
    tbb[0].fsStyle = BTNS_SEP;

    tbb[1].iBitmap = STD_FILEOPEN;
    tbb[1].fsState = TBSTATE_ENABLED;
    tbb[1].fsStyle = TBSTYLE_BUTTON;
    tbb[1].idCommand = IDM_FILE_OPEN;

    tbb[2].iBitmap = STD_FILESAVE;
    tbb[2].fsState = TBSTATE_ENABLED;
    tbb[2].fsStyle = TBSTYLE_BUTTON;
    tbb[2].idCommand = IDM_FILE_SAVEAS;

    tbb[3].fsStyle = BTNS_SEP;

    tbb[4].iBitmap = STD_PROPERTIES;
    tbb[4].fsState = TBSTATE_ENABLED;
    tbb[4].fsStyle = TBSTYLE_BUTTON;
    tbb[4].idCommand = IDM_PREFER;

    tbb[5].fsStyle = BTNS_SEP;

    tbb[6].iBitmap = STD_HELP;
    tbb[6].fsState = TBSTATE_ENABLED;
    tbb[6].fsStyle = TBSTYLE_BUTTON;
    tbb[6].idCommand = IDM_ABOUT;

    return tbb;
}

 HTREEITEM App::insertTreeItem(const char* str, LPARAM lParam, HTREEITEM parent, HTREEITEM insertAfter, int imageIndex, int selectedImageIndex)const
{ // build TVINSERTSTRUCT
    TVINSERTSTRUCT insertStruct;
    insertStruct.hParent = parent;
    insertStruct.hInsertAfter = insertAfter;        // handle to item or TVI_FIRST, TVI_LAST, TVI_ROOT
    insertStruct.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    insertStruct.item.pszText = (LPSTR)str;
    insertStruct.item.cchTextMax = lstrlen(str);
    insertStruct.item.iImage = imageIndex;                       // image index of ImageList
    insertStruct.item.iSelectedImage = selectedImageIndex;
    insertStruct.item.lParam = lParam;
    // insert the item
    HTREEITEM hTreeItem = (HTREEITEM)SendMessage(treeView, TVM_INSERTITEM, 0, (LPARAM)&insertStruct);

    // expand its parent
    HTREEITEM hParentItem = TreeView_GetNextItem(treeView, hTreeItem, TVGN_PARENT);

    if(hParentItem)
        SendMessage(treeView, TVM_EXPAND, (WPARAM)TVE_EXPAND, (LPARAM)hParentItem);


    return hTreeItem;
}

DWORD App::GetFileSignature(LPCSTR path)
{
    DWORD sign = 0;

    pFileStream = new CFileStream(path);

    sign = *((DWORD*)pFileStream->GetFile());
    if ((sign & 0xffff) == IMAGE_DOS_SIGNATURE)
        sign = sign & 0xffff;
    return sign;
}

void App::Init_ELF_Page(LPSTR fname)
{
    SetWindowPos(hwnd, NULL, 0, 0, 880, 640, SWP_NOMOVE);
    Elf32_Ehdr *target = (Elf32_Ehdr*)pFileStream->GetFile();
 //   HTREEITEM rootItem = treeView.insertItem(fname, TVI_ROOT, TVI_LAST, 2, 2);
    HTREEITEM rootItem = insertTreeItem("ELF32 header", ID_ELF_HDR, TVI_ROOT, TVI_LAST, 2, 2);
    HTREEITEM hItem = insertTreeItem("Section Hdr", ID_SHDR, rootItem, TVI_LAST);
    insertTreeItem("SymbolTab", ID_SYM, hItem, TVI_LAST);
    if (target->e_phnum)
        insertTreeItem("Program Hdr", ID_PHDR, rootItem, TVI_LAST, 1, 0);
    insertTreeItem("Relocation", ID_RELOC, rootItem, TVI_LAST, 1, 0);
    insertTreeItem("Hex editor", ID_HEXEDIT, rootItem, TVI_LAST, 1, 0);
    insertTreeItem("Disassembler", ID_TREE_DASM, rootItem, TVI_LAST, 1, 0);
    SendMessage(treeView, TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)rootItem);

    // Insert Tab page
    TCITEM tci = { 0 };
    tci.mask = TCIF_TEXT;
    tci.pszText = fname;
    tci.cchTextMax = lstrlen(fname);
    SendMessage(tabView, TCM_INSERTITEM, nTab++, (LPARAM)&tci);
}


void App::Init_PE_Page(LPSTR fname)
{
    SetWindowPos(hwnd, NULL, 0, 0, 900, 660, SWP_NOMOVE);
    Elf32_Ehdr *target = (Elf32_Ehdr*)pFileStream->GetFile();

    HTREEITEM rootItem = insertTreeItem("IMAGE_FILE_HEADER", ID_PE_HDR, TVI_ROOT, TVI_LAST, 2, 2);
    HTREEITEM hItem = insertTreeItem("Section Hdr", ID_SHDR, rootItem, TVI_LAST);
    insertTreeItem("Hex editor", ID_HEXEDIT, rootItem, TVI_LAST, 1, 0);
    insertTreeItem("Disassembler", ID_TREE_DASM, rootItem, TVI_LAST, 1, 0);
    SendMessage(treeView, TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)rootItem);

    // Insert Tab page
    TCITEM tci = { 0 };
    tci.mask = TCIF_TEXT;
    tci.pszText = fname;
    tci.cchTextMax = lstrlen(fname);
    SendMessage(tabView, TCM_INSERTITEM, nTab++, (LPARAM)&tci);

}

void App::Init_ANY_Page(LPSTR fname)
{
    Elf32_Ehdr *target = (Elf32_Ehdr*)pFileStream->GetFile();
 //   HTREEITEM rootItem = treeView.insertItem(fname, TVI_ROOT, TVI_LAST, 2, 2);
    HTREEITEM rootItem = insertTreeItem(fname, ID_ANY, TVI_ROOT, TVI_LAST, 2, 2);
    SendMessage(treeView, TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)rootItem);

}

void App::OnFileClose()
{
    if(pCurrPage)
        delete pCurrPage;
    pCurrPage = NULL;
    if(nTab > 0)
        nTab--;
//    int nSel = SendMessage(tabView, TCM_GETCURSEL, 0, 0);
    TabCtrl_DeleteItem(tabView,0);
    TreeView_DeleteAllItems(treeView);
}

void App::OnFileExit()
{
    TreeView_DeleteAllItems(treeView);
    TabCtrl_DeleteAllItems(tabView);
}

void App::OnFileOpen()
{
    CHAR path[MAX_PATH];
    CHAR fname[MAX_PATH];
    *fname = *path = '\0';//GetCurrentDirectory(MAX_PATH, buf);
    OPENFILENAME FileOpen;

    ZeroMemory(&FileOpen, sizeof(FileOpen));

    FileOpen.lStructSize = sizeof(FileOpen);
    FileOpen.hwndOwner = hwnd;
    FileOpen.hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
    FileOpen.lpstrFilter = "(*.*)\0*.*\0(*.exe)\0*.exe\0(*.dll)\0*.dll\0(*.obj)\0*.o\0(*.ko)\0*.ko\0";
    FileOpen.nMaxFile = MAX_PATH;
    FileOpen.lpstrFile = path;
    FileOpen.lpstrFileTitle = fname;
    FileOpen.nMaxFileTitle = MAX_PATH;
    FileOpen.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if(!GetOpenFileName(&FileOpen))
         return;

    switch(GetFileSignature(FileOpen.lpstrFileTitle))
    {
        case IMAGE_ELF_SIGNATURE:
            Init_ELF_Page(fname);
            break;
        case IMAGE_DOS_SIGNATURE:
            Init_PE_Page(fname);
            break;
        default:
            Init_ANY_Page(fname);
            break;
    }

}

void App::OnNotify(WPARAM wParam, LPARAM lParam)
{
    if(wParam == IDC_TREEVIEW && ((LPNMHDR)lParam)->code == TVN_SELCHANGED)
    {
        TV_ITEM iItem;
        iItem.mask = TVIF_HANDLE | TVIF_PARAM;
        iItem.hItem = TreeView_GetSelection(((LPNMHDR)lParam)->hwndFrom);
        TreeView_GetItem(((LPNMHDR)lParam)->hwndFrom, &iItem);

        switch(iItem.lParam)
        {
            case ID_ELF_HDR:
                if (pCurrPage) delete pCurrPage;
                pCurrPage = new EHDR_Page(tabView, pFileStream);
                break;
            case ID_SHDR:
                if (pCurrPage) delete pCurrPage;
                pCurrPage = new SHDR_Page(tabView, pFileStream);
                break;
            case ID_PHDR:
                if (pCurrPage) delete pCurrPage;
                pCurrPage = new PHDR_Page(tabView, pFileStream);
                break;
            case ID_SYM:
                if (pCurrPage) delete pCurrPage;
                pCurrPage = new SYM_Page(tabView, pFileStream);
                break;
            case ID_PE_HDR:
                if (pCurrPage) delete pCurrPage;
                pCurrPage = new PE_Page(tabView, pFileStream);
                break;
            case ID_ANY:
    printf("ID_ANY");
                break;
    /*        case ID_RELOC:
                break;
            case ID_HEXEDIT:
                if(pCurrPage) delete pCurrPage;
                pCurrPage = new HEX_Page(tabView.getHandle(), pFileStream);
                break;
            case ID_TREE_DASM:
                if(pCurrPage) delete pCurrPage;
                pCurrPage = new DASM_Page(tabView.getHandle(), pFileStream);
                break;*/
        }
    }
    else if (wParam == IDC_TABVIEW)
    {
        LPNMHDR lpnmhdr=(LPNMHDR)lParam;
        if(lpnmhdr->code == TCN_SELCHANGE)
        {
            int iPage = TabCtrl_GetCurSel(tabView);

            switch(iPage)
            {
                case 0:
                    printf("Tab %d %d\n",iPage,lpnmhdr->idFrom);
  //               ShowWindow(current,SW_HIDE);
  //               ShowWindow(hTab1,SW_SHOW);
  //               current=hTab1;
                   return;
                case 1:
                    printf("Tab %d %d\n",iPage,lpnmhdr->idFrom);
    //             ShowWindow(current,SW_HIDE);
    //             ShowWindow(hTab2,SW_SHOW);
    //             current=hTab2;
                   return;
            }
        }
    }

}

void App::OnAbout()
{
    DialogBox(hInst, MAKEINTRESOURCE(DLG_ABOUT), hwnd, AboutDlgProc);

}



///////////////////////////////////////////////////////////////////////////////
// main function of a windows application
///////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdArgs, int cmdShow)
{
    if(*cmdArgs)
    {
        return RunSilent(cmdArgs);
    }
     App app(hInst);
     app.Init();
     return app.MainMessageLoop();
}
