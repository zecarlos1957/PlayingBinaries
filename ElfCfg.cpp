#include "resource.h"
#include <windows.h>
#include "hexplore.h"

using namespace std;

char *getline(char *dst, char *src, char term)
{
    int i = 0;
    char *ptr = src;
    while(*ptr != term && *ptr)
    {
        ptr++;
        i++;
    }
    memcpy(dst,src, i);
    dst[i] = '\0';
    return dst;
}


int UpdateInfo(Elf32_Sym *Sym, char *name, LPARAM lParam)
{
    char *ptr = (char*)lParam;
    DWORD type = 0;
    char line[80];

    while(*(ptr-1))
    {
        getline(line, ptr,'\n');
    
        if(memcmp(ptr, "STT_", 4) == 0)
        {
            type = *(DWORD*)(ptr+4);
        }
        else if(memcmp(line, name, lstrlen(name)) == 0)
        {
            char iBind = ELF32_ST_BIND(Sym->st_info);
            char *BindStr = line + lstrlen(name);
            if(BindStr)
            {
                while(*BindStr == 0x20)BindStr++;
                if(memcmp(BindStr, "STB_LOCAL", 9) == 0)
                    iBind = STB_LOCAL;
                else if(memcmp(BindStr, "STB_GLOBAL", 10) == 0)
                    iBind = STB_GLOBAL;
                else if(memcmp(BindStr, "STB_WEAK", 8) == 0)
                    iBind = STB_WEAK;
            }

            switch(type)
            {
                case 0x59544f4e:
                    Sym->st_info = ELF32_ST_INFO(iBind, STT_NOTYPE);
                    break;
                case 0x454A424F:
                    Sym->st_info = ELF32_ST_INFO(iBind, STT_OBJECT);
                    break;
                case 0x434E5546:
                    Sym->st_info = ELF32_ST_INFO(iBind, STT_FUNC);
                    break;
                case 0x54434553:
                    Sym->st_info = ELF32_ST_INFO(iBind, STT_SECTION);
                    break;
                case 0x454C4946:
                    Sym->st_info = ELF32_ST_INFO(iBind, STT_FILE);
                    break;
            }
 //           printf("%s 0x%x %d\n", name, Sym->st_value, (int)Sym->st_info);
            return 1;
        }
        ptr += lstrlen(line)+1;
    }

    return 1;
}

int RunSilent(LPSTR cfg)
{
    if(!cfg)
        return 0;
    
    LPSTR ptr = cfg;
    
    while(*ptr != 0x20 && *ptr)ptr++;
    *ptr++ = '\0';
 
    CFileStream File(cfg);
    CFileStream BinFile(ptr);
    
    EnumSymbols((Elf32_Ehdr*)BinFile.GetFile(), UpdateInfo, (LPARAM)File.GetFile() );
    
    BinFile.SaveFile();

    return 0;
}
