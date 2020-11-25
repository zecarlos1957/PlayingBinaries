#ifndef _HEXEDIT_
#define _HEXEDIT_


#include "CFileStream.h"
#include "elf.h"

typedef int(*SymFunc)(Elf32_Sym *Sym, char *name, LPARAM lp);

void EnumSymbols(Elf32_Ehdr *target, SymFunc fn, LPARAM lp);

    class Page
    {
    protected:
        HWND hWnd;
        Elf32_Ehdr* target;
    public:
        Page(){}
        Elf32_Ehdr* GetTarget(){return target;}
        virtual ~Page(){}
    };

    class EHDR_Page: public Page
    {
        HWND HexEdit;
        HWND HdrData;
        void BuildHex();
    public:
        EHDR_Page(HWND hwnd, CFileStream *pFile);
       ~EHDR_Page();
        static BOOL CALLBACK  EhdrProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void UpdateHex();
        void UpdateHdr();
    };

    class SHDR_Page: public Page
    {
        HWND HdrData;
        HWND HexEdit;
    public:
        SHDR_Page(HWND hwnd, CFileStream *pFile);
        ~SHDR_Page();
        static BOOL CALLBACK  ShdrProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void BuildHdr();
        void BuildHex();
        void UpdateHex(UCHAR *data, DWORD sz);
    };


    class PHDR_Page: public Page
    {
        HWND ProgHdr;

    public:
        PHDR_Page(HWND hwnd, CFileStream *pFile);
       ~PHDR_Page();
        static BOOL CALLBACK  PhdrProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void BuildPage();
    };


    class SYM_Page: public Page
    {
        HWND  SymbolList;
        HWND  CodeList;
        HWND  ChkObj;
        HWND  ChkFunc;
        HWND  ChkSection;
        HWND  ChkFile;
        DWORD CheckBtnMask();
        void UpdateData();
    public:
        SYM_Page(HWND hwnd, CFileStream *pFile);
        ~SYM_Page();
        static BOOL CALLBACK  SymProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        int OnMaskButton(WPARAM wParam, LPARAM lParam);
        int OnNotify(WPARAM wParam, LPARAM lParam);
        void Translate(DWORD *code, DWORD dwSize);
   };

    class HEX_Page: public Page
    {
        HWND HexEdit;
    public:
        HEX_Page(HWND hwnd, CFileStream *pFile);
        ~HEX_Page();
        static BOOL CALLBACK  HexProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };

    class DASM_Page: public Page
    {
        HWND Dasm;
    public:
        DASM_Page(HWND hwnd, CFileStream *pFile);
        ~DASM_Page();
        static BOOL CALLBACK  DasmProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };

class PE_Page: public Page
{
    HWND PeHdr;
    HWND HexData;
public:
    PE_Page(HWND hwnd, CFileStream *pFile);
    ~PE_Page();
    static BOOL CALLBACK  PeProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif
