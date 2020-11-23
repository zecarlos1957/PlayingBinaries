#include "resource.h"
#include <windows.h>
#include <commctrl.h>
#include "elf.h"


// return pointer to Section Header Table
  Elf32_Shdr *elf_sheader(Elf32_Ehdr *hdr)
{
    return (Elf32_Shdr *)((int)hdr + hdr->e_shoff);
}

//
  Elf32_Shdr *elf_section(Elf32_Ehdr *hdr, int idx)
{
    return &elf_sheader(hdr)[idx];
}

 CHAR* elf_str_table(Elf32_Ehdr *hdr)
{
    if (hdr->e_shstrndx == SHN_UNDEF)return NULL;
    return (CHAR*)hdr + elf_section(hdr, hdr->e_shstrndx)->sh_offset;
}

 CHAR *elf_lookup_string(Elf32_Ehdr *hdr, INT offset)
{
    CHAR *strtab = elf_str_table(hdr);
    if(strtab == NULL) return NULL;
    return strtab + offset;
}

 char *get_symbol_strtab(Elf32_Ehdr *target)
{
    char *shstrtab;
    {
        unsigned int i = 0;
        for (unsigned int x = 0; x < (unsigned int)target->e_shentsize * target->e_shnum; x += target->e_shentsize)
        {
            Elf32_Shdr * shdr = (Elf32_Shdr *)((DWORD)target + (target->e_shoff + x));
            if (i == target->e_shstrndx) {
                shstrtab = (char *)((DWORD)target + shdr->sh_offset);
            }
            i++;
        }
    }

    for (unsigned int x = 0; x < (unsigned int)target->e_shentsize * target->e_shnum; x += target->e_shentsize)
    {
        Elf32_Shdr * shdr = (Elf32_Shdr *)((DWORD)target + (target->e_shoff + x));
        if (shdr->sh_type == SHT_STRTAB && (!strcmp((char *)((DWORD)shstrtab + shdr->sh_name), ".strtab")))
             return (char *)((DWORD)target + shdr->sh_offset);
    }
    return NULL;
}

 Elf32_Shdr *get_symbol_table(Elf32_Ehdr *target)
{
    char *shstrtab;
    {
        unsigned int i = 0;
        for (unsigned int x = 0; x < (unsigned int)target->e_shentsize * target->e_shnum; x += target->e_shentsize)
        {
            Elf32_Shdr * shdr = (Elf32_Shdr *)((DWORD)target + (target->e_shoff + x));
            if (i == target->e_shstrndx) {
                shstrtab = (char *)((DWORD)target + shdr->sh_offset);
            }
            i++;
        }
    }

    for (unsigned int x = 0; x < (unsigned int)target->e_shentsize * target->e_shnum; x += target->e_shentsize)
    {
       Elf32_Shdr * shdr = (Elf32_Shdr *)((DWORD)target + (target->e_shoff + x));
       if (shdr->sh_type == SHT_SYMTAB && (!strcmp((char *)((DWORD)shstrtab + shdr->sh_name), ".symtab")))
            return shdr;
    }
    return NULL;
}


void EnumSymbols(Elf32_Ehdr *target, EnumProc fn, LPARAM lParam)
{
    if (!target)
        return;
    
    char *strtab = get_symbol_strtab(target);
    Elf32_Shdr *symtab = get_symbol_table(target);

    for(int i = 0; i < (symtab->sh_size / symtab->sh_entsize); i++)
    {
        Elf32_Sym * symbol = (Elf32_Sym *)((DWORD)target + symtab->sh_offset + i * symtab->sh_entsize);
        char * name = (char*)(strtab + symbol->st_name);

        if(fn(symbol, name, lParam) == 0)
            return;
    }
}
