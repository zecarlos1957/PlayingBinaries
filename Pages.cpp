#include "resource.h"
#include <windows.h>
#include <commctrl.h>
#include "Pages.h"
#include "elf.h"
#include "dasm/dasmengine.h"


   char *EhdrStr[] =
    {
        "e_ident",
        "ei_class",
        "ei_data",
        "e_type",
        "e_machine",
        "e_version",
        "e_entry",
        "e_phoff",
        "e_shoff",
        "e_flags",
        "e_ehsize",
        "e_phentsize",
        "e_phnum",
        "e_shentsize",
        "e_shnum",
        "e_shstrndx",
        ""
    };

    char *PhdrStr[] =
    {
        "p_type",
        "p_offset",
        "p_vaddr",
        "p_paddr",
        "p_filesz",
        "p_memsz",
        "p_flags",
        "p_align",
        ""
    };

char *ABI[]=
{
    "System V",
    "HP-UX",
    "NetBSD",
    "Linux",
    "GNU Hurd",
    "-",
    "Solaris",
    "AIX",
    "IRIX",
    "FreeBSD",
    "Tru64 UNIX",
    "Novell Modesto",
    "Open BSD",
    "Open VMS",
    "NonStop Kernel",
    "AROS",
    "Fenix OS",
    "CloudABI"
};

char *SHDRTYPE[]=
{
    "SHT_NULL",
    "SHT_PROGBITS",
    "SHT_SYMTAB",
    "SHT_STRTAB",
    "SHT_RELA",
    "SHT_HASH",
    "SHT_DYNAMIC",
    "SHT_NOTE",
    "SHT_NOBITS",
    "SHT_REL",
    "SHT_SHLIB",
    "SHT_DYNSYM",
};

EHDR_Page::EHDR_Page(HWND hParent, CFileStream *pFile)
{
    target = (Elf32_Ehdr*)pFile->GetFile();
    hWnd = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(EHDR_PAGE),hParent, EHDR_Page::EhdrProc, (LPARAM)this);
    HdrData = GetDlgItem(hWnd, IDC_EHDR);
    HexEdit = GetDlgItem(hWnd, IDC_HEXDATA);
    ListView_SetExtendedListViewStyle(HdrData, LVS_EX_FULLROWSELECT);
    ListView_SetExtendedListViewStyle(HexEdit, LVS_EX_FULLROWSELECT);

    BuildHex();
    UpdateHdr();
    UpdateHex();
}

EHDR_Page::~EHDR_Page()
{
    ListView_DeleteAllItems(HexEdit);
    ListView_DeleteAllItems(HdrData);
    SendMessage(hWnd, WM_CLOSE, 0, 0);
}

BOOL CALLBACK  EHDR_Page::EhdrProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static Page *page = NULL;
    switch (msg)
    {
        case WM_INITDIALOG:
            page = (Page*)lParam;
            return TRUE;

        case WM_COMMAND:
            break;

        case WM_NOTIFY:

            break;

        case WM_CLOSE:
           DestroyWindow(hwnd);
            break;
    }
    return FALSE;
}

void EHDR_Page::BuildHex()
{
    char szText[32];
    LV_COLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;  // left align the column
    lvc.cx = 75;            // width of the column, in pixels
    lvc.pszText = "offset";
    ListView_InsertColumn(HexEdit, 0, &lvc);

    for(int idx = 0; idx < 16; idx++)
    {
        szText[0] = '0';
        sprintf(szText+1, "%X", idx);
        lvc.cx = 29;
        lvc.pszText = szText;
        ListView_InsertColumn(HexEdit, idx+1, &lvc);
    }
    lvc.cx = 140;
    lvc.pszText = "Text";
    ListView_InsertColumn(HexEdit, 17, &lvc);

}

void EHDR_Page::UpdateHex()
{
    HFONT hf = (HFONT)GetStockObject(ANSI_FIXED_FONT);
    SendMessage(HexEdit, WM_SETFONT, (WPARAM)hf, TRUE);
    ListView_DeleteAllItems(HexEdit);
    UCHAR* data = (UCHAR*)target;
    INT sz = sizeof(Elf32_Ehdr);
    DWORD idx = 0;

    for(int n = 0; n < sz; n += 16)
    {
        CHAR buf[32];
        CHAR line[64];
        LVITEM lvi;
        memset(&lvi, 0, sizeof(lvi));
        lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
        lvi.iItem = idx;
        int x= 0;

        ListView_InsertItem(HexEdit, &lvi);
        for(int i = 0; i < 18; i++)
        {
             if(i == 0)
            {
               lstrcpy(buf, "00000000");
                sprintf(buf+8, "%X", idx * 16);
                ListView_SetItemText(HexEdit, idx, 0, buf+lstrlen(buf)-8);
            }
            else if(i < 17)
            {
                int dx = idx * 16 + i - 1;
                lstrcpy(buf, "00");
                sprintf(buf + 2, "%X", data[dx]);
                ListView_SetItemText(HexEdit, idx, i, buf+lstrlen(buf)-2);
                if(data[dx] == 0)
                    line[x] = '.';
                else line[x] = data[dx];
                x++;
                if (dx + 1 >= sz)
                {
                    i = 16;
                }
            }
            else
            {
                line[x]='\0';
                ListView_SetItemText(HexEdit, idx, i, line);
            }

        }
        idx++;
    }

}

void EHDR_Page::UpdateHdr()
{
   char buffer[128];
    int nColumns = 2;
    Elf32_Ehdr* data = target;
    ListView_DeleteAllItems(HdrData);

    LVCOLUMN lvc = { 0 };
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    lvc.iSubItem = 0;
    lvc.cx = 100;
    lvc.pszText = "Name";
    ListView_InsertColumn(HdrData, 0, &lvc);
    lvc.cx = 150;
    lvc.iSubItem = 1;
    lvc.pszText = "Value";
    ListView_InsertColumn(HdrData, 1, &lvc);

    LVITEM lvi;
    lvi.mask = LVIF_TEXT;

    sprintf(buffer, "%c%c%c (version %d)", data->e_ident[1], data->e_ident[2], data->e_ident[3],
            (*(short*)&data->e_ident[6] )&0xff);
    lvi.iItem = 0;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[0];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 0;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "%s (version %d)", ABI[data->e_ident[7]],
                               (*(short*)&data->e_ident[8] )&0xff);
    lvi.iItem = 1;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[1];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 1;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

     sprintf(buffer, "%s / %s", data->e_ident[4] == 0x1 ? "32BITS":"64BITS",
                                data->e_ident[5] == 0x1 ? "LITTLE_ENDIAN":"BIG_ENDIAN");
    lvi.iItem = 2;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[2];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 2;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "%s ", data->e_type == 0x0 ? "ET_NONE":
                           data->e_type == 0x1 ? "ET_RELOC":
                           data->e_type == 0x2 ? "ET_EXEC":
                           data->e_type == 0x3 ? "ET_DYN":
                           data->e_type == 0x4 ? "ET_CORE":"UNKNOWN");

    lvi.iItem = 3;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[3];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 3;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "%s", data->e_machine == 0x03 ? "EM_386 (Intel 80386)":
                          data->e_machine == 0x06 ? "-- Intel MCU":
                          data->e_machine == 0x07 ? "EM_860 (Intel 80860)":
                          data->e_machine == 0x013 ? "-- Intel 80960":
                          data->e_machine == 0x028 ? "ARM":
                          data->e_machine == 0x03e ? "x86-64":"Other");
    lvi.iItem = 4;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[4];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 4;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "%d", data->e_version);
    lvi.iItem = 5;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[5];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 5;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "0x%x", data->e_entry);
    lvi.iItem = 6;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[6];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 6;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "0x%x", data->e_phoff);
    lvi.iItem = 7;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[7];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 7;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "0x%x", data->e_shoff);
    lvi.iItem = 8;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[8];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 8;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

/*   sprintf(buffer, "%s", data->e_flags == 0x1 ? "SHF_WRITE":
                            data->e_flags == 0x2 ? "SHF_ALLOC":
                            data->e_flags == 0x4 ? "SHF_EXECINSTR":
                            data->e_flags == 0xf0000000 ? "SHF_MASKPROC":"0");
 */
    sprintf(buffer, "0x%x", data->e_flags);
    lvi.iItem = 9;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[9];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 9;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "0x%x", data->e_ehsize);
    lvi.iItem = 10;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[10];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 10;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "0x%x", data->e_phentsize);
    lvi.iItem = 11;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[11];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 11;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "0x%x", data->e_phnum);
    lvi.iItem = 12;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[12];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 12;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "0x%x", data->e_shentsize);
    lvi.iItem = 13;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[13];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 13;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "0x%x", data->e_shnum);
    lvi.iItem = 14;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[14];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 14;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

    sprintf(buffer, "0x%x", data->e_shstrndx);
    lvi.iItem = 15;
    lvi.iSubItem = 0;
    lvi.pszText = EhdrStr[15];
    ListView_InsertItem(HdrData, &lvi);
    lvi.iItem = 15;
    lvi.iSubItem = 1;
    lvi.pszText = buffer;
    ListView_SetItem(HdrData, &lvi);

}


/*********************************/

SHDR_Page::SHDR_Page(HWND hParent,  CFileStream *pFile)
{
    target = (Elf32_Ehdr*)pFile->GetFile();
    hWnd = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(SHDR_PAGE), hParent, SHDR_Page::ShdrProc, (LPARAM)this);
    HdrData = GetDlgItem(hWnd, IDC_SHDR);
    HexEdit = GetDlgItem(hWnd, IDC_HEXDATA);
    ListView_SetExtendedListViewStyle(HexEdit, LVS_EX_FULLROWSELECT);

    BuildHex();
    BuildHdr();

}

SHDR_Page::~SHDR_Page()
{
    ListView_DeleteAllItems(HexEdit);
    ListView_DeleteAllItems(HdrData);
    SendMessage(hWnd, WM_CLOSE, 0, 0);
}

BOOL CALLBACK  SHDR_Page::ShdrProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static SHDR_Page *page = NULL;
    switch (msg)
    {
        case WM_INITDIALOG:
            page = (SHDR_Page*)lParam;
            return TRUE;

        case WM_COMMAND:
            break;

        case WM_NOTIFY:
            if ( ((LPNMHDR)lParam)->code == LVN_ITEMCHANGED && wParam == IDC_SHDR)
            {
                LPNMITEMACTIVATE lpit = (LPNMITEMACTIVATE) lParam;
                if((lpit->uNewState & LVIS_SELECTED)  && (lpit->uNewState & LVIS_FOCUSED))
                {
                    Elf32_Shdr* shdr = elf_section(page->GetTarget(), lpit->iItem + 1);
                    page->UpdateHex(((UCHAR*)page->GetTarget() + shdr->sh_offset), shdr->sh_size);
  //   printf("SubItem %d \n", lpit->iItem);//, shdr->sh_offset, shdr->sh_addr, shdr->sh_size);
                }
            }
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

    }
    return FALSE;
}

void SHDR_Page::BuildHdr()
{
    RECT rc;
    int nColumns = 6;
    int wColumns = 70;

    HINSTANCE hInst = (HINSTANCE)GetWindowLong(HdrData, GWL_HINSTANCE);

    ListView_SetExtendedListViewStyle(HdrData, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
    LVCOLUMN lvc = { 0 };

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;

    for (long nIndex = 0; nIndex < nColumns; nIndex++)
    {
        char buf[50];
        LoadString(hInst, IDS_SECTION_NAMES + nIndex, buf, 32);

        lvc.iSubItem = nIndex;
        if(nIndex == 4)lvc.cx = 100;
        else if(nIndex == 5)lvc.cx = 165;
        else lvc.cx = wColumns;
        lvc.pszText = buf;

        ListView_InsertColumn(HdrData, nIndex, &lvc);
    }
    LVITEM lvi;
    lvi.mask = LVIF_TEXT;
    int i = 0;
    char * shstrtab = NULL;

    for (unsigned int x = 0; x < (unsigned int)target->e_shentsize * target->e_shnum; x += target->e_shentsize)
    {
        Elf32_Shdr * shdr = (Elf32_Shdr *)((uintptr_t)target + (target->e_shoff + x));
        if (i == target->e_shstrndx)
            shstrtab = (char *)((uintptr_t)target + shdr->sh_offset);
        i++;
    }

    for (lvi.iItem = 0; lvi.iItem < target->e_shnum - 1; lvi.iItem++)
    {
        Elf32_Shdr* shdr = (Elf32_Shdr*)((uintptr_t)target + (target->e_shoff + ((lvi.iItem+1)*target->e_shentsize)));
        CHAR* name = shstrtab + shdr->sh_name;

        for (long nIndex = 0; nIndex < nColumns; nIndex++)
        {
            lvi.iSubItem = nIndex;

            if (!nIndex)  // item
            {
                lvi.pszText = name;
                SendMessage(HdrData, LVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&lvi));
            }
            else
            {// sub-item
                char value[32];
                switch(nIndex)
                {
                    case 1:
                        sprintf(value, "0x%x", shdr->sh_offset );
                        break;
                    case 2:
                        sprintf(value, "0x%x", shdr->sh_addr );
                        break;
                    case 3:
                        sprintf(value, "0x%x", shdr->sh_size );
                        break;
                    case 4:
                        if(shdr->sh_type > 0 && shdr->sh_type < 12)
                            sprintf(value, "%s", SHDRTYPE[shdr->sh_type]);
                        else sprintf(value, "0x%x", shdr->sh_type);
                        break;
                    case 5:
                    {
                        char buf[128];buf[0]='\0';
                        if (shdr->sh_flags & 0x1) lstrcat(buf, "SHF_WRITE|");
                        if (shdr->sh_flags & 0x2) lstrcat(buf, "SHF_ALLOC|");
                        if (shdr->sh_flags & 0x4) lstrcat(buf, "SHF_EXECINSTR|");
                        if (shdr->sh_flags == 0xf0000000) lstrcpy(buf, "SHF_MASLPROC|");
                        buf[lstrlen(buf)-1] ='\0';
                        sprintf(value, "%s", buf);
                    //    sprintf(value, "0x%x", shdr->sh_flags);
  //   printf( "flags 0x%x\n", shdr->sh_flags);
                         break;
                    }
                }
                lvi.pszText = value;
                SendMessage(HdrData, LVM_SETITEM, 0, reinterpret_cast<LPARAM>(&lvi));
            }
        }

    }

}

void SHDR_Page::BuildHex()
{

    char szText[32];
    LV_COLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;  // left align the column
    lvc.cx = 75;            // width of the column, in pixels
    lvc.pszText = "offset";
    ListView_InsertColumn(HexEdit, 0, &lvc);

    for(int idx = 0; idx < 16; idx++)
    {
        szText[0] = '0';
        sprintf(szText+1, "%X", idx);
        lvc.cx = 29;
        lvc.pszText = szText;
        ListView_InsertColumn(HexEdit, idx+1, &lvc);
    }
    lvc.cx = 140;
    lvc.pszText = "Text";
    ListView_InsertColumn(HexEdit, 17, &lvc);

}

void SHDR_Page::UpdateHex(UCHAR *data, DWORD sz)
{
   HFONT hf = (HFONT)GetStockObject(ANSI_FIXED_FONT);
    SendMessage(HexEdit,WM_SETFONT, (WPARAM)hf, TRUE);
    ListView_DeleteAllItems(HexEdit);
    DWORD idx = 0;

    for(int n = 0; n < sz; n += 16)
    {
        CHAR buf[32];
        CHAR line[64];
        LVITEM lvi;
        memset(&lvi, 0, sizeof(lvi));
        lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
        lvi.iItem = idx;
        int x = 0;

        ListView_InsertItem(HexEdit, &lvi);
        for(int i = 0; i < 18; i++)
        {
             if(i == 0)
            {
               lstrcpy(buf, "00000000");
                sprintf(buf+8, "%X", idx*16);
                ListView_SetItemText(HexEdit, idx, 0, buf+lstrlen(buf)-8);
            }
            else if(i < 17)
            {
                int dx = idx*16+i-1;
                lstrcpy(buf, "00");
                sprintf(buf+2, "%X", data[dx]);
                ListView_SetItemText(HexEdit, idx, i, buf+lstrlen(buf)-2);
                if(data[dx] == 0)// || data[dx] == 0x0e)
                    line[x] = '.';
                else line[x] = data[dx];
                x++;
                if (dx + 1 >= sz)
                {
                    i = 16;
                }
             }
           else
            {
                line[x]='\0';
                ListView_SetItemText(HexEdit, idx, i, line);
            }

        }
        idx++;
    }
}


/*********************************/

PHDR_Page::PHDR_Page(HWND hParent,  CFileStream *pFile)
{
    target = (Elf32_Ehdr*)pFile->GetFile();
    hWnd = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(PHDR_PAGE),hParent, PHDR_Page::PhdrProc, (LPARAM)this);
    ProgHdr = GetDlgItem(hWnd, IDC_PHDR);

    BuildPage();

}

PHDR_Page::~PHDR_Page()
{
    ListView_DeleteAllItems(ProgHdr);
    SendMessage(hWnd, WM_CLOSE, 0, 0);
}

BOOL CALLBACK  PHDR_Page::PhdrProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static Page *page = NULL;
    switch (msg)
    {
        case WM_INITDIALOG:
            page = (Page*)lParam;
            return TRUE;

        case WM_COMMAND:
            break;

         case WM_NOTIFY:

            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

    }
    return FALSE;
}

void PHDR_Page::BuildPage()
{
   int nColumns = 8;
    int wColumns = 70;

    HINSTANCE hInst = (HINSTANCE)GetWindowLong(ProgHdr, GWL_HINSTANCE);

    ListView_SetExtendedListViewStyle(ProgHdr, LVS_EX_FULLROWSELECT);
    LVCOLUMN lvc = { 0 };

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;

    for (long nIndex = 0; nIndex < nColumns; nIndex++)
    {
        char buf[50];
        LoadString(hInst, IDS_PROG_TYPE + nIndex, buf, 32);

        lvc.iSubItem = nIndex;
        if(nIndex == 0)lvc.cx = 90;
        else if(nIndex == 6)lvc.cx = 80;
        else lvc.cx = wColumns;
        lvc.pszText = buf;

        ListView_InsertColumn(ProgHdr, nIndex, &lvc);
    }
    LVITEM lvi;
    lvi.mask = LVIF_TEXT;

    for(int n = 0; n < target->e_phnum; n++)
    {
        char buffer[32];
        Elf32_Phdr *phdr = (Elf32_Phdr*)((char*)target + target->e_phoff + n * target->e_phentsize);
        lvi.iItem = n;

        for(int i = 0; i < nColumns; i++)
        {
            lvi.iSubItem = i;
            if(i == 0)
            {
                sprintf(buffer, "%s",phdr->p_type == 0x1 ? "PT_LAOD" :
                                       phdr->p_type == 0x2 ? "PT_DYNAMIC" :
                                       phdr->p_type == 0x3 ? "PT_INTERP" :
                                       phdr->p_type == 0x4 ? "PT_NOTE" :
                                       phdr->p_type == 0x5 ? "PT_SHLIB" :
                                       phdr->p_type == 0x6 ? "PT_PHDR" :"-");
                lvi.pszText = buffer;
                SendMessage(ProgHdr, LVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&lvi));
            }
            else
            {
                switch(i)
                {
                    case 1:
                        sprintf(buffer, "0x%x",phdr->p_offset);
                        lvi.pszText = buffer;
                        break;
                    case 2:
                        sprintf(buffer, "0x%x",phdr->p_vaddr);
                        lvi.pszText = buffer;
                        break;
                    case 3:
                        sprintf(buffer, "0x%x",phdr->p_paddr);
                        lvi.pszText = buffer;
                        break;
                    case 4:
                        sprintf(buffer, "0x%x",phdr->p_filesz);
                        lvi.pszText = buffer;
                        break;
                    case 5:
                        sprintf(buffer, "0x%x",phdr->p_memsz);
                        lvi.pszText = buffer;
                        break;
                    case 6:
                        buffer[0] = '\0';
                        if (phdr->p_flags & 0x1) lstrcat(buffer, "PF_X|");
                        if (phdr->p_flags & 0x2) lstrcat(buffer, "PF_W|");
                        if (phdr->p_flags & 0x4) lstrcat(buffer, "PF_R|");
                        buffer[lstrlen(buffer)-1] ='\0';
                        lvi.pszText = buffer;
                        break;
                    case 7:
                        sprintf(buffer, "0x%x",phdr->p_align);
                        lvi.pszText = buffer;
                        break;
                }
                SendMessage(ProgHdr, LVM_SETITEM, i, reinterpret_cast<LPARAM>(&lvi));

            }
        }
    }

}


/*********************************/

HEX_Page::HEX_Page(HWND hParent, CFileStream *pFile)
{
    target = (Elf32_Ehdr*)pFile->GetFile();
    hWnd = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(HEX_PAGE), hParent, HEX_Page::HexProc, (LPARAM)this);
    HexEdit = GetDlgItem(hWnd, IDC_HEXEDIT);
}

HEX_Page::~HEX_Page()
{
    ListView_DeleteAllItems(HexEdit);
    SendMessage(hWnd, WM_CLOSE, 0, 0);
}

BOOL CALLBACK  HEX_Page::HexProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static Page *page = NULL;
    switch (msg)
    {
        case WM_INITDIALOG:
            page = (Page*)lParam;
            return TRUE;

        case WM_COMMAND:
            break;

         case WM_NOTIFY:

            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

    }
    return FALSE;
}

/*********************************/

DASM_Page::DASM_Page(HWND hParent, CFileStream *pFile)
{
    target = (Elf32_Ehdr*)pFile->GetFile();
    hWnd = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(DASM_PAGE), hParent, DASM_Page::DasmProc, (LPARAM)this);
    Dasm = GetDlgItem(hWnd, IDC_DASM);
}

DASM_Page::~DASM_Page()
{
    SendMessage(hWnd, WM_CLOSE, 0, 0);
}

BOOL CALLBACK  DASM_Page::DasmProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static Page *page = NULL;
    switch (msg)
    {
        case WM_INITDIALOG:
            page = (Page*)lParam;
            return TRUE;
        case WM_COMMAND:
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

    }
    return FALSE;
}

/*****************************************/

SYM_Page::SYM_Page(HWND hwnd, CFileStream *pFile)
{
    target = (Elf32_Ehdr*)pFile->GetFile();
    hWnd = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(SYMBOL_PAGE),hwnd, SYM_Page::SymProc, (LPARAM)this);

    ChkObj = GetDlgItem(hWnd, IDC_CHKOBJ);
    ChkFunc = GetDlgItem(hWnd, IDC_CHKFUNC);
    ChkFile = GetDlgItem(hWnd, IDC_CHKFILE);
    SymbolList = GetDlgItem(hWnd, IDC_SYMBOLTAB);
    ChkSection = GetDlgItem(hWnd, IDC_CHKSECTION);
    CodeList = GetDlgItem(hWnd, IDC_DASM);

    HFONT hf = (HFONT)GetStockObject(ANSI_VAR_FONT);
    SendMessage(ChkObj, WM_SETFONT, (WPARAM)hf, TRUE);
    SendMessage(ChkFunc, WM_SETFONT, (WPARAM)hf, TRUE);
    SendMessage(ChkFile, WM_SETFONT, (WPARAM)hf, TRUE);
    SendMessage(ChkSection, WM_SETFONT, (WPARAM)hf, TRUE);

//    hf = (HFONT)GetStockObject(ANSI_FIXED_FONT);
 //   SendMessage(CodeList, WM_SETFONT, (WPARAM)hf, TRUE);

    ListView_SetExtendedListViewStyle(SymbolList, LVS_EX_FULLROWSELECT);
    CheckDlgButton(hWnd, IDC_CHKOBJ, BST_CHECKED);

    LVCOLUMN lvc = { 0 };
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    lvc.iSubItem = 0;
    lvc.cx = 60;
    lvc.pszText = "Address";
    ListView_InsertColumn(SymbolList, 0, &lvc);
    lvc.cx = 150;
    lvc.iSubItem = 1;
    lvc.pszText = "Name";
    ListView_InsertColumn(SymbolList, 1, &lvc);
    lvc.cx = 50;
    lvc.iSubItem = 2;
    lvc.pszText = "Type";
    ListView_InsertColumn(SymbolList, 2, &lvc);
    lvc.cx = 50;
    lvc.iSubItem = 3;
    lvc.pszText = "Bind";
    ListView_InsertColumn(SymbolList, 3, &lvc);
    UpdateData();

    ListView_SetExtendedListViewStyle(CodeList, LVS_EX_FULLROWSELECT);
    lvc = { 0 };
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    lvc.iSubItem = 0;
    lvc.cx = 70;
    lvc.pszText = "Address";
    ListView_InsertColumn(CodeList, 0, &lvc);
    lvc.cx = 110;
    lvc.iSubItem = 1;
    lvc.pszText = "Opcode";
    ListView_InsertColumn(CodeList, 1, &lvc);
    lvc.cx = 200;
    lvc.iSubItem = 2;
    lvc.pszText = "Instruction";
    ListView_InsertColumn(CodeList, 2, &lvc);

 }

void SYM_Page::UpdateData()
{
    ListView_DeleteAllItems(SymbolList);
    LVITEM lvi;
    ZeroMemory(&lvi, sizeof(lvi));
    lvi.mask = LVIF_TEXT;

    char *strtab = get_symbol_strtab(target);
    Elf32_Shdr *symtab = get_symbol_table(target);
    char buf[32];
    char mask = CheckBtnMask();
    if (mask == 0) mask = 1;
    DWORD idx = 0;
    for(int i = 1; i < (symtab->sh_size / symtab->sh_entsize); i++)
    {
        Elf32_Sym * symbol = (Elf32_Sym *)((DWORD)target + symtab->sh_offset + i * symtab->sh_entsize);
        char * name = (char*)(strtab + symbol->st_name);

 //       if(!((1 << ELF32_ST_TYPE(symbol->st_info)) & mask))
 //           continue;

        sprintf(buf, "0x%x", symbol->st_value);
        lvi.pszText = buf;
        lvi.iItem = idx++;
        lvi.iSubItem = 0;
        ListView_InsertItem(SymbolList, &lvi);

        lvi.pszText = name;
        lvi.iSubItem = 1;
        ListView_SetItem(SymbolList, &lvi);

        sprintf(buf, "0x%x", ELF32_ST_TYPE(symbol->st_info));
        lvi.pszText = buf;
        lvi.iSubItem = 2;
        ListView_SetItem(SymbolList, &lvi);

        sprintf(buf, "0x%x", ELF32_ST_BIND(symbol->st_info));
        lvi.pszText = buf;
        lvi.iSubItem = 3;
        ListView_SetItem(SymbolList, &lvi);
    }

}

SYM_Page::~SYM_Page()
{
    ListView_DeleteAllItems(SymbolList);
    ListView_DeleteAllItems(CodeList);
    SendMessage(hWnd, WM_CLOSE, 0, 0);
}

DWORD SYM_Page::CheckBtnMask()
{
    DWORD mask = 0;
    mask |= (IsDlgButtonChecked(hWnd, IDC_CHKOBJ) == BST_CHECKED) << 1;
    mask |= (IsDlgButtonChecked(hWnd, IDC_CHKFUNC) == BST_CHECKED) << 2;
    mask |= (IsDlgButtonChecked(hWnd, IDC_CHKSECTION) == BST_CHECKED) << 3;
    mask |= (IsDlgButtonChecked(hWnd, IDC_CHKFILE) == BST_CHECKED) << 4;
    return mask;
}


BOOL CALLBACK  SYM_Page::SymProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static SYM_Page *page = NULL;
    switch (msg)
    {
        case WM_INITDIALOG:
            page = (SYM_Page*)lParam;
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDC_CHKOBJ:
                case IDC_CHKFUNC:
                case IDC_CHKSECTION:
                case IDC_CHKFILE:
                    page->OnMaskButton(wParam, lParam);
                    break;
            }
            break;

        case WM_NOTIFY:
            page->OnNotify(wParam, lParam);
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            page = NULL;
            break;
    }
    return FALSE;
}

int SYM_Page::OnNotify(WPARAM wParam, LPARAM lParam)
{
    if ( ((LPNMHDR)lParam)->code == LVN_ITEMCHANGED && wParam == IDC_SYMBOLTAB)
    {
        LPNMITEMACTIVATE lpit = (LPNMITEMACTIVATE) lParam;
        if((lpit->uNewState & LVIS_SELECTED)  && (lpit->uNewState & LVIS_FOCUSED))
        {
            char *strtab = get_symbol_strtab(target);
            Elf32_Shdr *symtab = get_symbol_table(target);
            Elf32_Sym * symbol = (Elf32_Sym *)((DWORD)target + symtab->sh_offset + (lpit->iItem + 1)* symtab->sh_entsize);
            Elf32_Shdr *text = elf_section(target, 1);

            if(ELF32_ST_TYPE(symbol->st_info) == STT_FUNC)
            {
                Elf32_Sym * next_symbol = (Elf32_Sym *)((DWORD)target + symtab->sh_offset + (lpit->iItem + 2)* symtab->sh_entsize);
                DWORD nSize = next_symbol->st_value - symbol->st_value;
                DWORD *addr = (DWORD*)((DWORD)target - text->sh_addr + symbol->st_value + text->sh_offset);
                DoDisassembly(CodeList, addr, nSize, symbol->st_value, FALSE);
            }
        }
    }
    return 0;
}

int SYM_Page::OnMaskButton(WPARAM wParam, LPARAM lParam)
{
    if (HIWORD(wParam) == BN_CLICKED)
    {
        switch(LOWORD(wParam))
        {
            case IDC_CHKOBJ:
                if(IsDlgButtonChecked(hWnd, IDC_CHKOBJ) == BST_CHECKED)
                    CheckDlgButton(hWnd, IDC_CHKOBJ, BST_UNCHECKED);
                else
                    CheckDlgButton(hWnd, IDC_CHKOBJ, BST_CHECKED);
                break;
            case IDC_CHKFUNC:
                if(IsDlgButtonChecked(hWnd, IDC_CHKFUNC) == BST_CHECKED)
                    CheckDlgButton(hWnd, IDC_CHKFUNC, BST_UNCHECKED);
                else
                    CheckDlgButton(hWnd, IDC_CHKFUNC, BST_CHECKED);
                break;
            case IDC_CHKSECTION:
                if(IsDlgButtonChecked(hWnd, IDC_CHKSECTION) == BST_CHECKED)
                    CheckDlgButton(hWnd, IDC_CHKSECTION, BST_UNCHECKED);
                else
                    CheckDlgButton(hWnd, IDC_CHKSECTION, BST_CHECKED);
                break;
            case IDC_CHKFILE:
                if(IsDlgButtonChecked(hWnd, IDC_CHKFILE) == BST_CHECKED)
                    CheckDlgButton(hWnd, IDC_CHKFILE, BST_UNCHECKED);
                else
                    CheckDlgButton(hWnd, IDC_CHKFILE, BST_CHECKED);
                break;
       }
        UpdateData();
    }
    return 0;
}

PE_Page::PE_Page(HWND hwnd, CFileStream *pFile)
{
}

PE_Page::~PE_Page()
{
}


BOOL CALLBACK  PE_Page::PeProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static Page *page = NULL;
    switch (msg)
    {
        case WM_INITDIALOG:
            page = (Page*)lParam;
            return TRUE;
        case WM_COMMAND:
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

    }
    return FALSE;

}

