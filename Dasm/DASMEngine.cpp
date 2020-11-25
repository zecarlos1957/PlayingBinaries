
// ---------------------------------------------------
// Author: Shishir Bhat (www.shishirbhat.com)
// The MIT License (MIT)
// Copyright (c) 2016
//

#include "DasmEngine.h"
#include <windows.h>
#include <commctrl.h>

// Lookup table for instruction names
WCHAR awszOpcodeLUT1[][MAX_OPCODE_NAME_LEN + 1] =
{
    //     0              1              2              3              4              5          6          7              
    //     8              9              A              B              C              D          E          F
    /*0*/  L"add",        L"add",        L"add",        L"add",        L"add",        L"add",    L"push",   L"pop",
    /*0*/  L"or",         L"or",         L"or",         L"or",         L"or",         L"or",     L"push",   L"!twobyte",
    /*1*/  L"adc",        L"adc",        L"adc",        L"adc",        L"adc",        L"adc",    L"push",   L"pop",
    /*1*/  L"sbb",        L"sbb",        L"sbb",        L"sbb",        L"sbb",        L"sbb",    L"push",   L"pop",
    /*2*/  L"and",        L"and",        L"and",        L"and",        L"and",        L"and",    L"sges",   L"daa",
    /*2*/  L"sub",        L"sub",        L"sub",        L"sub",        L"sub",        L"sub",    L"sgcs",   L"das",
    /*3*/  L"xor",        L"xor",        L"xor",        L"xor",        L"xor",        L"xor",    L"sgds",   L"aaa",
    /*3*/  L"cmp",        L"cmp",        L"cmp",        L"cmp",        L"cmp",        L"cmp",    L"sgds",   L"aas",
    /*4*/  L"inc",        L"inc",        L"inc",        L"inc",        L"inc",        L"inc",    L"inc",    L"inc",
    /*4*/  L"dec",        L"dec",        L"dec",        L"dec",        L"dec",        L"dec",    L"dec",    L"dec",
    /*5*/  L"push",       L"push",       L"push",       L"push",       L"push",       L"push",   L"push",   L"push",
    /*5*/  L"pop",        L"pop",        L"pop",        L"pop",        L"pop",        L"pop",    L"pop",    L"pop",
    //     0              1              2              3              4              5          6          7              
    //     8              9              A              B              C              D          E          F
    /*6*/  L"pushad",     L"popad",      L"bound",      L"arpl",       L"!sgfs",      L"!sggs",  L"!szop",  L"!szad",
    /*6*/  L"push",       L"imul",       L"push",       L"imul",       L"ins",        L"ins",    L"outs",   L"outs",
    /*7*/  L"jo",         L"jno",        L"jb",         L"jnb",        L"je",         L"jne",    L"jbe",    L"ja",
    /*7*/  L"js",         L"jns",        L"jpe",        L"jpo",        L"jl",         L"jge",    L"jle",    L"jg",
    /*8*/  L"",           L"",           L"",           L"",           L"test",       L"test",   L"xchg",   L"xchg",
    /*8*/  L"mov",        L"mov",        L"mov",        L"mov",        L"mov",        L"lea",    L"mov",    L"pop",
    /*9*/  L"nop",        L"xchg",       L"xchg",       L"xchg",       L"xchg",       L"xchg",   L"xchg",   L"xchg",
    /*9*/  L"cwd",        L"cdq",        L"callf",      L"wait",       L"pushfd",     L"popfd",  L"sahf",   L"lahf",
    /*A*/  L"mov",        L"mov",        L"mov",        L"mov",        L"movs",       L"movs",   L"cmps",   L"cmps",
    /*A*/  L"test",       L"test",       L"stos",       L"stos",       L"lods",       L"lods",   L"scas",   L"scas",
    /*B*/  L"mov",        L"mov",        L"mov",        L"mov",        L"mov",        L"mov",    L"mov",    L"mov",
    /*B*/  L"mov",        L"mov",        L"mov",        L"mov",        L"mov",        L"mov",    L"mov",    L"mov",
    /*C*/  L"!shift",     L"!shift",     L"ret",        L"ret",        L"les",        L"lds",    L"mov",    L"mov",
    /*C*/  L"enter",      L"leave",      L"retf",       L"retf",       L"int",        L"int",    L"into",   L"iretd",
    /*D*/  L"!shift",     L"!shift",     L"!shift",     L"!shift",     L"aam",        L"aad",    L"salc",   L"xlat",
    /*D*/  L"!fpu",       L"!fpu",       L"!fpu",       L"!fpu",       L"!fpu",       L"!fpu",   L"!fpu",   L"!fpu",
    /*E*/  L"loopnz",     L"loopz",      L"loop",       L"jecxz",      L"in",         L"in",     L"out",    L"out",
    /*E*/  L"call",       L"jmp",        L"jmpf",       L"jmp",        L"in",         L"in",     L"out",    L"out",
    /*F*/  L"lock",       L"int1",       L"repne",      L"repe",       L"hlt",        L"cmc",    L"!",      L"!",
    /*F*/  L"clc",        L"stc",        L"cli",        L"sti",        L"cld",        L"std",    L"!inc",   L"!"
};

// Lookup table for instruction mnemonics(2byte opcodes)
WCHAR awszOpcodeLUT2[][MAX_OPCODE_NAME_LEN + 1] =
{
    //     0              1              2              3              4              5              6              7              
    //     8              9              A              B              C              D              E              F
    /*0*/  L"!mult",      L"!mult",      L"lar",        L"lsl",        L"",           L"",           L"clts",       L"",
    /*0*/  L"invd",       L"wbinvd",     L"",           L"ud2",        L"",           L"nop",        L"",           L"",
    /*1*/  L"movups",     L"movups",     L"movlps",     L"movlps",     L"unpcklps",   L"unpckhps",   L"movhps",     L"movhps",
    /*1*/  L"!pfsse",     L"nop",        L"nop",        L"nop",        L"nop",        L"nop",        L"nop",        L"nop",
    /*2*/  L"mov",        L"mov",        L"mov",        L"mov",        L"",           L"",           L"",           L"",
    /*2*/  L"movaps",     L"movaps",     L"cvtpi2ps",   L"movntps",    L"cvttps2pi",  L"cvtps2pi",   L"ucomiss",    L"comiss",
    /*3*/  L"wrmsr",      L"rdtsc",      L"rdmsr",      L"rdpmc",      L"sysenter",   L"sysexit",    L"",           L"sse",
    /*3*/  L"3byte",      L"",           L"3byte",      L"",           L"",           L"",           L"",           L"",
    /*4*/  L"cmovo",      L"cmovno",     L"cmovb",      L"cmovae",     L"cmove",      L"cmovne",     L"cmovna",     L"cmova",
    /*4*/  L"cmovs",      L"cmovns",     L"cmovp",      L"cmovpo",     L"cmovl",      L"cmovge",     L"cmovle",     L"cmovg",
    /*5*/  L"movmskps",   L"sqrtps",     L"rsqrtps",    L"rcpps",      L"andps",      L"andnps",     L"orps",       L"xorps",
    /*5*/  L"addps",      L"mulps",      L"cvtps2pd",   L"cvtdq2ps",   L"subps",      L"minps",      L"divps",      L"maxps",
    /*6*/  L"punpcklbw",  L"punpcklwd",  L"punpckldq",  L"packsswb",   L"pcmpgtb",    L"pcmpgtw",    L"pcmpgtd",    L"packuswb",
    /*6*/  L"punpckhbw",  L"punpckhwd",  L"punpckhdq",  L"packssdw",   L"mmx",        L"mmx",        L"movd",       L"movq",
    /*7*/  L"pshufw",     L"!mult",      L"!mult",      L"!mult",      L"pcmpeqb",    L"pcmpeqw",    L"pcmpeqd",    L"emms",
    /*7*/  L"mmx",        L"mmx",        L"",           L"",           L"mmx",        L"mmx",        L"movd",       L"movq",
    //     0              1              2              3              4              5              6              7              
    //     8              9              A              B              C              D              E              F
    /*8*/  L"jo",         L"jno",        L"jb",         L"jnb",        L"je",         L"jne",        L"jbe",        L"ja",
    /*8*/  L"js",         L"jns",        L"jpe",        L"jpo",        L"jl",         L"jge",        L"jle",        L"jg",
    /*9*/  L"seto",       L"setno",      L"setb",       L"setnb",      L"sete",       L"setne",      L"setbe",      L"seta",
    /*9*/  L"sets",       L"setns",      L"setpe",      L"setpo",      L"setl",       L"jge",        L"setle",      L"setg",
    /*A*/  L"push",       L"pop",        L"cpuid",      L"bt",         L"shld",       L"shld",       L"",           L"",
    /*A*/  L"push",       L"pop",        L"rsm",        L"bts",        L"shrd",       L"shrd",       L"!mult",      L"imul",
    /*B*/  L"cmpxchg",    L"cmpxchg",    L"lss",        L"btr",        L"lfs",        L"lgs",        L"movzx",      L"movzx",
    /*B*/  L"popcnt",     L"ud",         L"!btx",       L"btc",        L"bsf",        L"bsr",        L"movsx",      L"movsx",
    /*C*/  L"xadd",       L"xadd",       L"cmpps",      L"sse",        L"pinsrw",     L"pextrw",     L"shufps",     L"cmpxchg8b",
    /*C*/  L"bswap",      L"bswap",      L"bswap",      L"bswap",      L"bswap",      L"bswap",      L"bswap",      L"bswap",
    /*D*/  L"sse",        L"psrlw",      L"psrld",      L"psrlq",      L"sse",        L"pmullw",     L"sse",        L"pmovmskb",
    /*D*/  L"psubusb",    L"psubusw",    L"pminub",     L"pand",       L"paddusb",    L"paddusw",    L"pmaxub",     L"pandn",
    /*E*/  L"pavgb",      L"psraw",      L"psrad",      L"pavgw",      L"pmulhuw",    L"pmulhuw",    L"cvttpd2dq",  L"movntq",
    /*E*/  L"psubsb",     L"psubsw",     L"pminsw",     L"por",        L"paddsb",     L"paddsw",     L"pmaxsw",     L"pxor",
    /*F*/  L"sse",        L"psllw",      L"pslld",      L"psllq",      L"sse",        L"pmaddwd",    L"psadbw",     L"maskmovq",
    /*F*/  L"psubb",      L"psubw",      L"psubd",      L"mmx",        L"paddb",      L"paddw",      L"paddd",      L"!",

};

// ** FPU Instructions **
WCHAR awszFPUOpcodeLUTRegEx[][MAX_OPCODE_NAME_LEN + 1] = // Instructions that use only the 'reg' field as opcode extension
{
    //         0          1          2          3              4              5              6              7
    /*D8,0*/   L"fadd",   L"fmul",   L"fcom",   L"fcomp",      L"fsub",       L"fsubr",      L"fdiv",       L"fdivr",
    /*D9,1*/   L"fld",    L"!",      L"fst",    L"fstp",       L"fldenv",     L"fldcw",      L"fnstenv",    L"fnstcw",
    /*DA,2*/   L"fiadd",  L"fimul",  L"ficom",  L"ficomp",     L"fisub",      L"fisubr",     L"fidiv",      L"fidivr",
    /*DB,3*/   L"fild",   L"!",      L"fist",   L"fistp",      L"!",          L"fld",        L"!",          L"fstp",
    /*DC,4*/   L"fadd",   L"fmul",   L"fcom",   L"fcomp",      L"fsub",       L"fsubr",      L"fdiv",       L"fdivr",
    /*DD,5*/   L"fld",    L"!",      L"fst",    L"fstp",       L"frstor",     L"!",          L"fnsave",     L"fnstsw",
    /*DE,6*/   L"fiadd",  L"fimul",  L"ficom",  L"ficomp",     L"fisub",      L"fisubr",     L"fidiv",      L"fidivr",
    /*DF,7*/   L"fild",   L"!",      L"fist",   L"fistp",      L"fbld",       L"fild",       L"fbstp",      L"fistp"
};

WCHAR awszFPUOpcodeLUTFullEx[][MAX_OPCODE_NAME_LEN + 1] = // Instructions that use full ModRM byte as opcode extension
{
    //         0              1              2              3              4              5              6              7
    //         8              9              A              B              C              D              E              F
    /*D8,C0*/  L"fadd",       L"fadd",       L"fadd",       L"fadd",       L"fadd",       L"fadd",       L"fadd",       L"fadd",
    /*D8,C8*/  L"fmul",       L"fmul",       L"fmul",       L"fmul",       L"fmul",       L"fmul",       L"fmul",       L"fmul",
    /*D8,D0*/  L"fcom",       L"fcom",       L"fcom",       L"fcom",       L"fcom",       L"fcom",       L"fcom",       L"fcom",
    /*D8,D8*/  L"fcomp",      L"fcomp",      L"fcomp",      L"fcomp",      L"fcomp",      L"fcomp",      L"fcomp",      L"fcomp",
    /*D8,E0*/  L"fsub",       L"fsub",       L"fsub",       L"fsub",       L"fsub",       L"fsub",       L"fsub",       L"fsub",
    /*D8,E8*/  L"fsubr",      L"fsubr",      L"fsubr",      L"fsubr",      L"fsubr",      L"fsubr",      L"fsubr",      L"fsubr",
    /*D8,F0*/  L"fdiv",       L"fdiv",       L"fdiv",       L"fdiv",       L"fdiv",       L"fdiv",       L"fdiv",       L"fdiv",
    /*D8,F8*/  L"fdivr",      L"fdivr",      L"fdivr",      L"fdivr",      L"fdivr",      L"fdivr",      L"fdivr",      L"fdivr",
    /*D9,C0*/  L"fld",        L"fld",        L"fld",        L"fld",        L"fld",        L"fld",        L"fld",        L"fld",
    /*D9,C8*/  L"fxch",       L"fxch",       L"fxch",       L"fxch",       L"fxch",       L"fxch",       L"fxch",       L"fxch",
    /*D9,D0*/  L"fnop",       L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*D9,D8*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*D9,E0*/  L"fchs",       L"fabs",       L"!",          L"!",          L"ftst",       L"fxam",       L"!",          L"!",
    /*D9,E8*/  L"fld1",       L"fldl2t",     L"fldl2e",     L"fldpi",      L"fldlg2",     L"fldln2",     L"fldz",       L"!",
    /*D9,F0*/  L"f2xm1",      L"fyl2x",      L"fptan",      L"fpatan",     L"fxtract",    L"fprem1",     L"fdecstp",    L"fincstp",
    /*D9,F8*/  L"fprem",      L"fyl2xp1",    L"fsqrt",      L"fsincos",    L"frndint",    L"fscale",     L"fsin",       L"fcos",
    /*DA,C0*/  L"fcmovb",     L"fcmovb",     L"fcmovb",     L"fcmovb",     L"fcmovb",     L"fcmovb",     L"fcmovb",     L"fcmovb",
    /*DA,C8*/  L"fcmove",     L"fcmove",     L"fcmove",     L"fcmove",     L"fcmove",     L"fcmove",     L"fcmove",     L"fcmove",
    /*DA,D0*/  L"fcmovbe",    L"fcmovbe",    L"fcmovbe",    L"fcmovbe",    L"fcmovbe",    L"fcmovbe",    L"fcmovbe",    L"fcmovbe",
    /*DA,D8*/  L"fcmovu",     L"fcmovu",     L"fcmovu",     L"fcmovu",     L"fcmovu",     L"fcmovu",     L"fcmovu",     L"fcmovu",
    /*DA,E0*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DA,E8*/  L"!",          L"fucompp",    L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DA,F0*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DA,F8*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DB,C0*/  L"fcmovnb",    L"fcmovnb",    L"fcmovnb",    L"fcmovnb",    L"fcmovnb",    L"fcmovnb",    L"fcmovnb",    L"fcmovnb",
    /*DB,C8*/  L"fcmovne",    L"fcmovne",    L"fcmovne",    L"fcmovne",    L"fcmovne",    L"fcmovne",    L"fcmovne",    L"fcmovne",
    /*DB,D0*/  L"fcmovnbe",   L"fcmovnbe",   L"fcmovnbe",   L"fcmovnbe",   L"fcmovnbe",   L"fcmovnbe",   L"fcmovnbe",   L"fcmovnbe",
    /*DB,D8*/  L"fcmovu",     L"fcmovu",     L"fcmovu",     L"fcmovu",     L"fcmovu",     L"fcmovu",     L"fcmovu",     L"fcmovu",
    /*DB,E0*/  L"!",          L"!",          L"fnclex",     L"fninit",     L"!",          L"!",          L"!",          L"!",
    /*DB,E8*/  L"fucomi",     L"fucomi",     L"fucomi",     L"fucomi",     L"fucomi",     L"fucomi",     L"fucomi",     L"fucomi",
    /*DB,F0*/  L"fcomi",      L"fcomi",      L"fcomi",      L"fcomi",      L"fcomi",      L"fcomi",      L"fcomi",      L"fcomi",
    /*DB,F8*/  L"!",          L"!",          L"fnclex",     L"fninit",     L"!",          L"!",          L"!",          L"!",
    /*DC,C0*/  L"fadd",       L"fadd",       L"fadd",       L"fadd",       L"fadd",       L"fadd",       L"fadd",       L"fadd",
    /*DC,C8*/  L"fmul",       L"fmul",       L"fmul",       L"fmul",       L"fmul",       L"fmul",       L"fmul",       L"fmul",
    /*DC,D0*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DC,D8*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DC,E0*/  L"fsubr",      L"fsubr",      L"fsubr",      L"fsubr",      L"fsubr",      L"fsubr",      L"fsubr",      L"fsubr",
    /*DC,E8*/  L"fsub",       L"fsub",       L"fsub",       L"fsub",       L"fsub",       L"fsub",       L"fsub",       L"fsub",
    /*DC,F0*/  L"fdivr",      L"fdivr",      L"fdivr",      L"fdivr",      L"fdivr",      L"fdivr",      L"fdivr",      L"fdivr",
    /*DC,F8*/  L"fdiv",       L"fdiv",       L"fdiv",       L"fdiv",       L"fdiv",       L"fdiv",       L"fdiv",       L"fdiv",
    /*DD,C0*/  L"ffree",      L"ffree",      L"ffree",      L"ffree",      L"ffree",      L"ffree",      L"ffree",      L"ffree",
    /*DD,C8*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DD,D0*/  L"fst",        L"fst",        L"fst",        L"fst",        L"fst",        L"fst",        L"fst",        L"fst",
    /*DD,D8*/  L"fstp",       L"fstp",       L"fstp",       L"fstp",       L"fstp",       L"fstp",       L"fstp",       L"fstp",
    /*DD,E0*/  L"fucom",      L"fucom",      L"fucom",      L"fucom",      L"fucom",      L"fucom",      L"fucom",      L"fucom",
    /*DD,E8*/  L"fucomp",     L"fucomp",     L"fucomp",     L"fucomp",     L"fucomp",     L"fucomp",     L"fucomp",     L"fucomp",
    /*DD,F0*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DD,F8*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DE,C0*/  L"faddp",      L"faddp",      L"faddp",      L"faddp",      L"faddp",      L"faddp",      L"faddp",      L"faddp",
    /*DE,C8*/  L"fmulp",      L"fmulp",      L"fmulp",      L"fmulp",      L"fmulp",      L"fmulp",      L"fmulp",      L"fmulp",
    /*DE,D0*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DE,D8*/  L"!",          L"fcompp",     L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DE,E0*/  L"fsubrp",     L"fsubrp",     L"fsubrp",     L"fsubrp",     L"fsubrp",     L"fsubrp",     L"fsubrp",     L"fsubrp",
    /*DE,E8*/  L"fsubp",      L"fsubp",      L"fsubp",      L"fsubp",      L"fsubp",      L"fsubp",      L"fsubp",      L"fsubp",
    /*DE,F0*/  L"fdivrp",     L"fdivrp",     L"fdivrp",     L"fdivrp",     L"fdivrp",     L"fdivrp",     L"fdivrp",     L"fdivrp",
    /*DE,F8*/  L"fdivp",      L"fdivp",      L"fdivp",      L"fdivp",      L"fdivp",      L"fdivp",      L"fdivp",      L"fdivp",
    /*DF,C0*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DF,C8*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DF,D0*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DF,D8*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DF,E0*/  L"fnstsw",     L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
    /*DF,E8*/  L"fucomip",    L"fucomip",    L"fucomip",    L"fucomip",    L"fucomip",    L"fucomip",    L"fucomip",    L"fucomip",
    /*DF,F0*/  L"fcomip",     L"fcomip",     L"fcomip",     L"fcomip",     L"fcomip",     L"fcomip",     L"fcomip",     L"fcomip",
    /*DF,F8*/  L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",          L"!",
};


// Array of function pointers to opcode handler functions.
// These are arranged according to the opcodes. Indexing is done
// this way: Opcode 0-3bits selects row and 4-7bits selects the 
// column.
// All opcode handlers take one BYTE argument(the opcode) 
// and return a BOOL value.
BOOL(*afpOpcHndlrs[16][16])(BYTE) =
{
    //     0                          1                          2                          3                      4
    //     5                          6                          7                          8                      9
    //     A                          B                          C                          D                      E
    //     F
    /*0*/  OPCHndlrALU_ADD,           OPCHndlrALU_ADD,           OPCHndlrALU_ADD,           OPCHndlrALU_ADD,       OPCHndlrALU_ADD,
    /*0*/  OPCHndlrALU_ADD,           OPCHndlrStack_PUSH,        OPCHndlrStack_POP,         OPCHndlrALU_OR,        OPCHndlrALU_OR,
    /*0*/  OPCHndlrALU_OR,            OPCHndlrALU_OR,            OPCHndlrALU_OR,            OPCHndlrALU_OR,        OPCHndlrStack_PUSH,
    /*0*/  OPCHndlr_2ByteHandler,
    /*1*/  OPCHndlrALU_ADD,           OPCHndlrALU_ADD,           OPCHndlrALU_ADD,           OPCHndlrALU_ADD,       OPCHndlrALU_ADD,
    /*1*/  OPCHndlrALU_ADD,           OPCHndlrStack_PUSH,        OPCHndlrStack_POP,         OPCHndlrALU_SUB,       OPCHndlrALU_SUB,
    /*1*/  OPCHndlrALU_SUB,           OPCHndlrALU_SUB,           OPCHndlrALU_SUB,           OPCHndlrALU_SUB,       OPCHndlrStack_PUSH,
    /*1*/  OPCHndlrStack_POP,
    /*2*/  OPCHndlrALU_AND,           OPCHndlrALU_AND,           OPCHndlrALU_AND,           OPCHndlrALU_AND,       OPCHndlrALU_AND,
    /*2*/  OPCHndlrALU_AND,           OPCHndlrPrefix_Ovride,     OPCHndlrALU_DAA,           OPCHndlrALU_SUB,       OPCHndlrALU_SUB,
    /*2*/  OPCHndlrALU_SUB,           OPCHndlrALU_SUB,           OPCHndlrALU_SUB,           OPCHndlrALU_SUB,       OPCHndlrPrefix_Ovride,
    /*2*/  OPCHndlrALU_DAS,
    /*3*/  OPCHndlrALU_XOR,           OPCHndlrALU_XOR,           OPCHndlrALU_XOR,           OPCHndlrALU_XOR,       OPCHndlrALU_XOR,
    /*3*/  OPCHndlrALU_XOR,           OPCHndlrPrefix_Ovride,     OPCHndlrALU_AAA,           OPCHndlrCC_CMP,        OPCHndlrCC_CMP,
    /*3*/  OPCHndlrCC_CMP,            OPCHndlrCC_CMP,            OPCHndlrCC_CMP,            OPCHndlrCC_CMP,        OPCHndlrPrefix_Ovride,
    /*3*/  OPCHndlrALU_AAS,
    /*4*/  OPCHndlrALU_INC,           OPCHndlrALU_INC,           OPCHndlrALU_INC,           OPCHndlrALU_INC,       OPCHndlrALU_INC,
    /*4*/  OPCHndlrALU_INC,           OPCHndlrALU_INC,           OPCHndlrALU_INC,           OPCHndlrALU_DEC,       OPCHndlrALU_DEC,
    /*4*/  OPCHndlrALU_DEC,           OPCHndlrALU_DEC,           OPCHndlrALU_DEC,           OPCHndlrALU_DEC,       OPCHndlrALU_DEC,
    /*4*/  OPCHndlrALU_DEC,
    /*5*/  OPCHndlrStack_PUSH,        OPCHndlrStack_PUSH,        OPCHndlrStack_PUSH,        OPCHndlrStack_PUSH,    OPCHndlrStack_PUSH,
    /*5*/  OPCHndlrStack_PUSH,        OPCHndlrStack_PUSH,        OPCHndlrStack_PUSH,        OPCHndlrStack_POP,     OPCHndlrStack_POP,
    /*5*/  OPCHndlrStack_POP,         OPCHndlrStack_POP,         OPCHndlrStack_POP,         OPCHndlrStack_POP,     OPCHndlrStack_POP,
    /*5*/  OPCHndlrStack_POP,
    /*6*/  OPCHndlrStack_PUSHxx,      OPCHndlrStack_POPxx,       OPCHndlrCC_BOUND,          OPCHndlrCC_ARPL,       OPCHndlrPrefix_Ovride,
    /*6*/  OPCHndlrPrefix_Ovride,     OPCHndlrPrefix_Ovride,     OPCHndlrPrefix_Ovride,     OPCHndlrStack_PUSH,    OPCHndlrALU_MUL,
    /*6*/  OPCHndlrStack_PUSH,        OPCHndlrALU_MUL,           OPCHndlrSysIO_INS,         OPCHndlrSysIO_INS,     OPCHndlrSysIO_OUTS,
    /*6*/  OPCHndlrSysIO_OUTS,
    /*7*/  OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,       OPCHndlrCC_JUMP,
    /*7*/  OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,       OPCHndlrCC_JUMP,
    /*7*/  OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,       OPCHndlrCC_JUMP,
    /*7*/  OPCHndlrCC_JUMP,
    //     0                          1                          2                          3                      4
    //     5                          6                          7                          8                      9
    //     A                          B                          C                          D                      E
    //     F
    /*8*/  OPCHndlrMulti_8x,          OPCHndlrMulti_8x,          OPCHndlrMulti_8x,          OPCHndlrMulti_8x,      OPCHndlrCC_TEST,
    /*8*/  OPCHndlrCC_TEST,           OPCHndlrMem_XCHG,          OPCHndlrMem_XCHG,          OPCHndlrMem_MOV,       OPCHndlrMem_MOV,
    /*8*/  OPCHndlrMem_MOV,           OPCHndlrMem_MOV,           OPCHndlrMem_MOV,           OPCHndlrMem_LEA,       OPCHndlrMem_MOV,
    /*8*/  OPCHndlrStack_POP,
    /*9*/  OPCHndlr_NOP,              OPCHndlrMem_XCHG,          OPCHndlrMem_XCHG,          OPCHndlrMem_XCHG,      OPCHndlrMem_XCHG,
    /*9*/  OPCHndlrMem_XCHG,          OPCHndlrMem_XCHG,          OPCHndlrMem_XCHG,          OPCHndlrMem_CWDE,      OPCHndlrMem_CDQ,
    /*9*/  OPCHndlrCC_CALL,           OPCHndlrSysIO_WAIT,        OPCHndlrStack_PUSHxx,      OPCHndlrStack_POPxx,   OPCHndlrMem_SAHF,
    /*9*/  OPCHndlrMem_LAHF,
    /*A*/  OPCHndlrMem_MOV,           OPCHndlrMem_MOV,           OPCHndlrMem_MOV,           OPCHndlrMem_MOV,       OPCHndlrMem_MOVS,
    /*A*/  OPCHndlrMem_MOVS,          OPCHndlrCC_CMPS,           OPCHndlrCC_CMPS,           OPCHndlrCC_TEST,       OPCHndlrCC_TEST,
    /*A*/  OPCHndlrMem_STOS,          OPCHndlrMem_STOS,          OPCHndlrMem_LODS,          OPCHndlrMem_LODS,      OPCHndlrCC_SCAS,
    /*A*/  OPCHndlrCC_SCAS,
    /*B*/  OPCHndlrMem_MOV,           OPCHndlrMem_MOV,           OPCHndlrMem_MOV,           OPCHndlrMem_MOV,       OPCHndlrMem_MOV,
    /*B*/  OPCHndlrMem_MOV,           OPCHndlrMem_MOV,           OPCHndlrMem_MOV,           OPCHndlrMem_MOV,       OPCHndlrMem_MOV,
    /*B*/  OPCHndlrMem_MOV,           OPCHndlrMem_MOV,           OPCHndlrMem_MOV,           OPCHndlrMem_MOV,       OPCHndlrMem_MOV,
    /*B*/  OPCHndlrMem_MOV,
    /*C*/  OPCHndlrALU_Shift,         OPCHndlrALU_Shift,         OPCHndlrCC_RETN,           OPCHndlrCC_RETN,       OPCHndlrMem_LES,
    /*C*/  OPCHndlrMem_LDS,           OPCHndlrMem_MOV,           OPCHndlrMem_MOV,           OPCHndlrStack_ENTER,   OPCHndlrStack_LEAVE,
    /*C*/  OPCHndlrCC_RETN,           OPCHndlrCC_RETN,           OPCHndlrSysIO_INT,         OPCHndlrSysIO_INT,     OPCHndlrSysIO_INT,
    /*C*/  OPCHndlrCC_IRETD,
    /*D*/  OPCHndlrALU_Shift,         OPCHndlrALU_Shift,         OPCHndlrALU_Shift,         OPCHndlrALU_Shift,     OPCHndlrALU_AAM,
    /*D*/  OPCHndlrALU_AAD,           OPCHndlrALU_SALC,          OPCHndlrMem_XLAT,          OPCHndlrFPU_All,       OPCHndlrFPU_All,
    /*D*/  OPCHndlrFPU_All,           OPCHndlrFPU_All,           OPCHndlrFPU_All,           OPCHndlrFPU_All,       OPCHndlrFPU_All,
    /*D*/  OPCHndlrFPU_All,
    /*E*/  OPCHndlrCC_CLOOP,          OPCHndlrCC_CLOOP,          OPCHndlrCC_CLOOP,          OPCHndlrCC_JUMP,       OPCHndlrSysIO_IN,
    /*E*/  OPCHndlrSysIO_IN,          OPCHndlrSysIO_OUT,         OPCHndlrSysIO_OUT,         OPCHndlrCC_CALL,       OPCHndlrCC_JUMP,
    /*E*/  OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,           OPCHndlrSysIO_INDX,        OPCHndlrSysIO_INDX,    OPCHndlrSysIO_OUTDX,
    /*E*/  OPCHndlrSysIO_OUTDX,
    /*F*/  OPCHndlrPrefix_LOCK,       OPCHndlrSysIO_IceBP,       OPCHndlrPrefix_CREP,       OPCHndlrPrefix_CREP,   OPCHndlrSysIO_HLT,
    /*F*/  OPCHndlrCC_EFLAGS,         OPCHndlrMulti_fx,          OPCHndlrMulti_fx,          OPCHndlrCC_EFLAGS,     OPCHndlrCC_EFLAGS,
    /*F*/  OPCHndlrCC_EFLAGS,         OPCHndlrCC_EFLAGS,         OPCHndlrCC_EFLAGS,         OPCHndlrCC_EFLAGS,     OPCHndlrMulti_IncDec,
    /*F*/  OPCHndlrMulti_FF

}; // (*afpOpcHndlrs[][])()

// Array of function pointers to 2byte opcode handlers
// These are arranged according to the opcodes. Indexing is done
// this way: Opcode(Byte2) 4-7bits selects row and 0-3bits selects the 
// column.
// All opcode handlers take one BYTE argument(the opcode) 
// and return a BOOL value.
BOOL(*afpOpcHndlrs2[16][16])(BYTE) =
{
    //     0                          1                          2                      3                          4
    //     5                          6                          7                      8                          9
    //     A                          B                          C                      D                          E
    //     F
    /*0*/  OPCHndlrSysIO_LdtTrS,      OPCHndlrSysIO_GdtIdMsw,    OPCHndlrMem_LAR,       OPCHndlrMem_LSL,           OPCHndlr_INVALID,
    /*0*/  OPCHndlr_INVALID,          OPCHndlrSysIO_CLTS,        OPCHndlr_INVALID,      OPCHndlrSysIO_INVD,        OPCHndlrSysIO_WBINVD,
    /*0*/  OPCHndlr_INVALID,          OPCHndlrSysIO_UD2,         OPCHndlr_INVALID,      OPCHndlr_NOP,              OPCHndlr_INVALID,
    /*0*/  OPCHndlr_INVALID,
    /*1*/  OPCHndlrSSE_Mov,           OPCHndlrSSE_Mov,           OPCHndlrSSE_Mov,       OPCHndlrSSE_Mov,           OPCHndlrSSE_Conv,
    /*1*/  OPCHndlrSSE_Conv,          OPCHndlrSSE_Mov,           OPCHndlrSSE_Mov,       OPCHndlrSSE_Prefetch18,    OPCHndlr_HNOP,
    /*1*/  OPCHndlr_HNOP,             OPCHndlr_HNOP,             OPCHndlr_HNOP,         OPCHndlr_HNOP,             OPCHndlr_HNOP,
    /*1*/  OPCHndlr_HNOP,
    /*2*/  OPCHndlrMem_MOVCrDr,       OPCHndlrMem_MOVCrDr,       OPCHndlrMem_MOVCrDr,   OPCHndlrMem_MOVCrDr,       OPCHndlr_INVALID,
    /*2*/  OPCHndlr_INVALID,          OPCHndlr_INVALID,          OPCHndlr_INVALID,      OPCHndlrSSE_Mov,           OPCHndlrSSE_Mov,
    /*2*/  OPCHndlrSSE_Conv,          OPCHndlrSSE_Mov,           OPCHndlrSSE_Conv,      OPCHndlrSSE_Conv,          OPCHndlrSSE_Cmp,
    /*2*/  OPCHndlrSSE_Cmp,
    /*3*/  OPCHndlrSysIO_WRMSR,       OPCHndlrSysIO_RDTSC,       OPCHndlrSysIO_RDMSR,   OPCHndlrSysIO_RDPMC,       OPCHndlrSysIO_SYSENTER,
    /*3*/  OPCHndlrSysIO_SYSEXIT,     OPCHndlr_INVALID,          OPCHndlrUnKwn_,        OPCHndlr_3ByteHandler,     OPCHndlr_INVALID,
    /*3*/  OPCHndlr_3ByteHandler,     OPCHndlr_INVALID,          OPCHndlr_INVALID,      OPCHndlr_INVALID,          OPCHndlr_INVALID,
    /*3*/  OPCHndlr_INVALID,
    /*4*/  OPCHndlrCC_CMOV,           OPCHndlrCC_CMOV,           OPCHndlrCC_CMOV,       OPCHndlrCC_CMOV,           OPCHndlrCC_CMOV,
    /*4*/  OPCHndlrCC_CMOV,           OPCHndlrCC_CMOV,           OPCHndlrCC_CMOV,       OPCHndlrCC_CMOV,           OPCHndlrCC_CMOV,
    /*4*/  OPCHndlrCC_CMOV,           OPCHndlrCC_CMOV,           OPCHndlrCC_CMOV,       OPCHndlrCC_CMOV,           OPCHndlrCC_CMOV,
    /*4*/  OPCHndlrCC_CMOV,
    /*5*/  OPCHndlrSSE_Mov,           OPCHndlrSSE_Arith,         OPCHndlrSSE_Arith,     OPCHndlrSSE_Arith,         OPCHndlrSSE_Logical,
    /*5*/  OPCHndlrSSE_Logical,       OPCHndlrSSE_Logical,       OPCHndlrSSE_Logical,   OPCHndlrSSE_Arith,         OPCHndlrSSE_Arith,
    /*5*/  OPCHndlrSSE_Conv,          OPCHndlrSSE_Conv,          OPCHndlrSSE_Arith,     OPCHndlrSSE_Logical,       OPCHndlrSSE_Arith,
    /*5*/  OPCHndlrSSE_Logical,
    /*6*/  OPCHndlrMMX_PConv,         OPCHndlrMMX_PConv,         OPCHndlrMMX_PConv,     OPCHndlrMMX_PConv,         OPCHndlrMMX_PCmp,
    /*6*/  OPCHndlrMMX_PCmp,          OPCHndlrMMX_PCmp,          OPCHndlrMMX_PConv,     OPCHndlrMMX_PConv,         OPCHndlrMMX_PConv,
    /*6*/  OPCHndlrMMX_PConv,         OPCHndlrMMX_PConv,         OPCHndlrUnKwn_MMX,     OPCHndlrUnKwn_MMX,         OPCHndlrMMX_PMov,
    /*6*/  OPCHndlrMMX_PMov,
    /*7*/  OPCHndlrMMX_PSHUFW,        OPCHndlrMMX_PMulti7x,      OPCHndlrMMX_PMulti7x,  OPCHndlrMMX_PMulti7x,      OPCHndlrMMX_PCmp,
    /*7*/  OPCHndlrMMX_PCmp,          OPCHndlrMMX_PCmp,          OPCHndlrMMX_EMMS,      OPCHndlrUnKwn_MMX,         OPCHndlrUnKwn_MMX,
    /*7*/  OPCHndlr_INVALID,          OPCHndlr_INVALID,          OPCHndlrUnKwn_SSE,     OPCHndlrUnKwn_SSE,         OPCHndlrMMX_PMov,
    /*7*/  OPCHndlrMMX_PMov,
    //     0                          1                          2                      3                          4
    //     5                          6                          7                      8                          9
    //     A                          B                          C                      D                          E
    //     F
    /*8*/  OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,       OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,
    /*8*/  OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,       OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,
    /*8*/  OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,       OPCHndlrCC_JUMP,           OPCHndlrCC_JUMP,
    /*8*/  OPCHndlrCC_JUMP,
    /*9*/  OPCHndlrCC_SETxx,          OPCHndlrCC_SETxx,          OPCHndlrCC_SETxx,      OPCHndlrCC_SETxx,          OPCHndlrCC_SETxx,
    /*9*/  OPCHndlrCC_SETxx,          OPCHndlrCC_SETxx,          OPCHndlrCC_SETxx,      OPCHndlrCC_SETxx,          OPCHndlrCC_SETxx,
    /*9*/  OPCHndlrCC_SETxx,          OPCHndlrCC_SETxx,          OPCHndlrCC_SETxx,      OPCHndlrCC_SETxx,          OPCHndlrCC_SETxx,
    /*9*/  OPCHndlrCC_SETxx,
    /*A*/  OPCHndlrStack_PUSH,        OPCHndlrStack_POP,         OPCHndlrSysIO_CPUID,   OPCHndlrMem_BT,            OPCHndlrALU_Shift,
    /*A*/  OPCHndlrALU_Shift,         OPCHndlr_INVALID,          OPCHndlr_INVALID,      OPCHndlrStack_PUSH,        OPCHndlrStack_POP,
    /*A*/  OPCHndlrSysIO_RSM,         OPCHndlrMem_BTS,           OPCHndlrALU_Shift,     OPCHndlrALU_Shift,         OPCHndlrSSE_MultiAE,
    /*A*/  OPCHndlrALU_MUL,
    /*B*/  OPCHndlrCC_CmpXchg,        OPCHndlrCC_CmpXchg,        OPCHndlrMem_LSS,       OPCHndlrMem_BTR,           OPCHndlrMem_LFS,
    /*B*/  OPCHndlrMem_LGS,           OPCHndlrMem_MOVZX,         OPCHndlrMem_MOVZX,     OPCHndlrUnKwn_,            OPCHndlrSysIO_UD2,
    /*B*/  OPCHndlrMulti_BitTestX,    OPCHndlrMem_BTC,           OPCHndlrMem_BSF,       OPCHndlrMem_BSR,           OPCHndlrMem_MOVSX,
    /*B*/  OPCHndlrMem_MOVSX,
    /*C*/  OPCHndlrALU_XADD,          OPCHndlrALU_XADD,          OPCHndlrSSE_Cmp,       OPCHndlrUnKwn_SSE,         OPCHndlrMMX_PINSRW,
    /*C*/  OPCHndlrMMX_PEXTRW,        OPCHndlrSSE_SHUFPS,        OPCHndlrCC_CmpXchg,    OPCHndlrMem_ByteSWAP,      OPCHndlrMem_ByteSWAP,
    /*C*/  OPCHndlrMem_ByteSWAP,      OPCHndlrMem_ByteSWAP,      OPCHndlrMem_ByteSWAP,  OPCHndlrMem_ByteSWAP,      OPCHndlrMem_ByteSWAP,
    /*C*/  OPCHndlrMem_ByteSWAP,
    /*D*/  OPCHndlrUnKwn_SSE,         OPCHndlrMMX_PShift,        OPCHndlrMMX_PShift,    OPCHndlrMMX_PShift,        OPCHndlrUnKwn_MMX,
    /*D*/  OPCHndlrMMX_PArith,        OPCHndlrUnKwn_SSE,         OPCHndlrMMX_PMOVMSKB,  OPCHndlrMMX_PArith,        OPCHndlrMMX_PArith,
    /*D*/  OPCHndlrMMX_PMaxMinAvg,    OPCHndlrMMX_PLogical,      OPCHndlrMMX_PArith,    OPCHndlrMMX_PArith,        OPCHndlrMMX_PMaxMinAvg,
    /*D*/  OPCHndlrMMX_PLogical,
    /*E*/  OPCHndlrMMX_PMaxMinAvg,    OPCHndlrMMX_PShift,        OPCHndlrMMX_PShift,    OPCHndlrMMX_PMaxMinAvg,    OPCHndlrMMX_PArith,
    /*E*/  OPCHndlrMMX_PArith,        OPCHndlrSSE_Conv,          OPCHndlrSSE_Mov,       OPCHndlrMMX_PArith,        OPCHndlrMMX_PArith,
    /*E*/  OPCHndlrMMX_PMaxMinAvg,    OPCHndlrMMX_PLogical,      OPCHndlrMMX_PArith,    OPCHndlrMMX_PArith,        OPCHndlrMMX_PMaxMinAvg,
    /*E*/  OPCHndlrMMX_PLogical,
    /*F*/  OPCHndlrUnKwn_SSE,         OPCHndlrMMX_PShift,        OPCHndlrMMX_PShift,    OPCHndlrMMX_PShift,        OPCHndlrUnKwn_MMX,
    /*F*/  OPCHndlrMMX_PArith,        OPCHndlrMMX_PArith,        OPCHndlrSSE_Mov,       OPCHndlrMMX_PArith,        OPCHndlrMMX_PArith,
    /*F*/  OPCHndlrMMX_PArith,        OPCHndlrUnKwn_SSE,         OPCHndlrMMX_PArith,    OPCHndlrMMX_PArith,        OPCHndlrMMX_PArith,
    /*F*/  OPCHndlrUnKwn_SSE

}; // (*afpOpcHndlrs2[][])()

/***************************************************
 *    FPU Instructions *
 **************************************************/

 // LookUpTable for instructions whose ModRM's value lies
 // between 00h and BFh. These instructions use only the
 // 'reg' field of the ModRM byte as the opcode extension.
BOOL(*afpFPUModRMRegEx[8][8])(BYTE) =
{
    /*D8,0*/    OPCHndlrFPU_FADD,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,
                OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,
    /*D9,1*/    OPCHndlrFPU_FLoad,     OPCHndlrFPU_Invalid,   OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,
                OPCHndlrFPU_CtlOp,     OPCHndlrFPU_CtlOp,     OPCHndlrFPU_CtlOp,     OPCHndlrFPU_CtlOp,
    /*DA,2*/    OPCHndlrFPU_FADD,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FCmpInts,  OPCHndlrFPU_FCmpInts,
                OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,
    /*DB,3*/    OPCHndlrFPU_FLoad,     OPCHndlrFPU_Invalid,   OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,
                OPCHndlrFPU_Invalid,   OPCHndlrFPU_FLoad,     OPCHndlrFPU_Invalid,   OPCHndlrFPU_FStore,
    /*DC,4*/    OPCHndlrFPU_FADD,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,
                OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,
    /*DD,5*/    OPCHndlrFPU_FLoad,     OPCHndlrFPU_Invalid,   OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,
                OPCHndlrFPU_CtlOp,     OPCHndlrFPU_Invalid,   OPCHndlrFPU_CtlOp,     OPCHndlrFPU_CtlOp,
    /*DE,6*/    OPCHndlrFPU_FADD,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FCmpInts,  OPCHndlrFPU_FCmpInts,
                OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,
    /*DF,7*/    OPCHndlrFPU_FLoad,     OPCHndlrFPU_Invalid,   OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,
                OPCHndlrFPU_FLoad,     OPCHndlrFPU_FLoad,     OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,
};

// LookUpTable for instructions whose ModRM's value lies
// between C0h and FFh. These instructions use the full
// ModRM byte as the opcode extension.
// #possible ModRM values between C0h and FFh = FFh - C0h + 1h = 40h
BOOL(*afpFPUModRMFullEx[8][0x40])(BYTE) =
{
    //         0                      1                      2                      3                      4                      5                      6                      7
    //         8                      9                      A                      B                      C                      D                      E                      F
    /*D8,C0*/  OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,
    /*D8,C8*/  OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,
    /*D8,D0*/  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,
    /*D8,D8*/  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,
    /*D8,E0*/  OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,
    /*D8,E8*/  OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,
    /*D8,F0*/  OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,
    /*D8,F8*/  OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,
    /*D9,C0*/  OPCHndlrFPU_FLoad,     OPCHndlrFPU_FLoad,     OPCHndlrFPU_FLoad,     OPCHndlrFPU_FLoad,     OPCHndlrFPU_FLoad,     OPCHndlrFPU_FLoad,     OPCHndlrFPU_FLoad,     OPCHndlrFPU_FLoad,
    /*D9,C8*/  OPCHndlrFPU_FXCH,      OPCHndlrFPU_FXCH,      OPCHndlrFPU_FXCH,      OPCHndlrFPU_FXCH,      OPCHndlrFPU_FXCH,      OPCHndlrFPU_FXCH,      OPCHndlrFPU_FXCH,      OPCHndlrFPU_FXCH,
    /*D9,D0*/  OPCHndlrFPU_FNOP,      OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*D9,D8*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*D9,E0*/  OPCHndlrFPU_FCHS,      OPCHndlrFPU_FABS,      OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_FTST,      OPCHndlrFPU_FXAM,      OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*D9,E8*/  OPCHndlrFPU_Const,     OPCHndlrFPU_Const,     OPCHndlrFPU_Const,     OPCHndlrFPU_Const,     OPCHndlrFPU_Const,     OPCHndlrFPU_Const,     OPCHndlrFPU_Const,     OPCHndlrFPU_Invalid,
    /*D9,F0*/  OPCHndlrFPU_LgExSc,    OPCHndlrFPU_LgExSc,    OPCHndlrFPU_Trig,      OPCHndlrFPU_Trig,      OPCHndlrFPU_FXTRACT,   OPCHndlrFPU_FPREM,     OPCHndlrFPU_Ctl,       OPCHndlrFPU_Ctl,
    /*D9,F8*/  OPCHndlrFPU_FPREM,     OPCHndlrFPU_LgExSc,    OPCHndlrFPU_FSQRT,     OPCHndlrFPU_Trig,      OPCHndlrFPU_FRNDINT,   OPCHndlrFPU_LgExSc,    OPCHndlrFPU_Trig,      OPCHndlrFPU_Trig,
    /*DA,C0*/  OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,
    /*DA,C8*/  OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,
    /*DA,D0*/  OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,
    /*DA,D8*/  OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,
    /*DA,E0*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DA,E8*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DA,F0*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DA,F8*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DB,C0*/  OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,
    /*DB,C8*/  OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,
    /*DB,D0*/  OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,
    /*DB,D8*/  OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,     OPCHndlrFPU_FCMOV,
    /*DB,E0*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Ctl,       OPCHndlrFPU_Ctl,       OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DB,E8*/  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,
    /*DB,F0*/  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,
    /*DB,F8*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DC,C0*/  OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,
    /*DC,C8*/  OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,
    /*DC,D0*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DC,D8*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DC,E0*/  OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,
    /*DC,E8*/  OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,
    /*DC,F0*/  OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,
    /*DC,F8*/  OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,
    /*DD,C0*/  OPCHndlrFPU_CtlOp,     OPCHndlrFPU_CtlOp,     OPCHndlrFPU_CtlOp,     OPCHndlrFPU_CtlOp,     OPCHndlrFPU_CtlOp,     OPCHndlrFPU_CtlOp,     OPCHndlrFPU_CtlOp,     OPCHndlrFPU_CtlOp,
    /*DD,C8*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DD,D0*/  OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,
    /*DD,D8*/  OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,    OPCHndlrFPU_FStore,
    /*DD,E0*/  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,
    /*DD,E8*/  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,
    /*DD,F0*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DD,F8*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DE,C0*/  OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,      OPCHndlrFPU_FADD,
    /*DE,C8*/  OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,      OPCHndlrFPU_FMUL,
    /*DE,D0*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DE,D8*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DE,E0*/  OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,
    /*DE,E8*/  OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,      OPCHndlrFPU_FSUB,
    /*DE,F0*/  OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,
    /*DE,F8*/  OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,      OPCHndlrFPU_FDIV,
    /*DF,C0*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DF,C8*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DF,D0*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DF,D8*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DF,E0*/  OPCHndlrFPU_CtlOp,     OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
    /*DF,E8*/  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,
    /*DF,F0*/  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,  OPCHndlrFPU_FCmpReal,
    /*DF,F8*/  OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,   OPCHndlrFPU_Invalid,
};

// IASD Vol2 - Table 3-1
// Look for #defines for indexes into these arrays
WCHAR awszRegCodes8[8][3] =     { L"al",    L"cl",  L"dl",  L"bl",  L"ah",  L"ch",  L"dh",  L"bh" };
WCHAR awszRegCodes16[8][3] =    { L"ax",    L"cx",  L"dx",  L"bx",  L"sp",  L"bp",  L"si",  L"di" };
WCHAR awszRegCodes32[8][4] =    { L"eax",   L"ecx", L"edx", L"ebx", L"esp", L"ebp", L"esi", L"edi" };

// IASD Vol2 - Table B-6: segment registers
WCHAR awszSRegCodes[8][3] =     { L"es",    L"cs",  L"ss",  L"ds",  L"fs",  L"gs",  L"!s",  L"!s" };

// IASD Vol2 - Table B-7: control registers
WCHAR awszCRegCodes[8][4] =     { L"cr0",   L"!c",  L"cr2", L"cr3", L"cr4", L"!c",  L"!c",  L"!c" };

// IASD Vol2 - Table B-7: debug registers
WCHAR awszDRegCodes[8][4] =     { L"dr0",   L"dr1", L"dr2", L"dr3", L"!d",  L"!d",  L"dr6", L"dr7" };

// IASD Vol1 - 8.1.1 MMX Registers
WCHAR awszMMXRegCodes[8][4] =   { L"mm0",   L"mm1", L"mm2", L"mm3", L"mm4", L"mm5", L"mm6", L"mm7" };

WCHAR awszXMMRegCodes[8][5] =   { L"xmm0",  L"xmm1", L"xmm2", L"xmm3", L"xmm4", L"xmm5", L"xmm6", L"xmm7" };

// operand pointer strings
static WCHAR awszPtrStr[][MAX_PTR_STR] = {
    L"",            L"byte ptr ",   L"word ptr ",   L"dword ptr ",
    L"fword ptr ",  L"qword ptr ",  L"tbyte ptr ",  L"mmword ptr ", 
    L"xmmword ptr " };

/*
 * ***********************************************************************
 * Static global variables that are used to manage the disassembler state
 * machine and its operation.
 * ***********************************************************************
 */

#ifdef DASM_BUILD_FOR_UNITTEST
#define MYSTATIC
#else
#define MYSTATIC static
#endif

 // We will read the code section byte by byte.
static BOOL g_f64bit;
MYSTATIC PBYTE pByteInCode = NULL;
static long lDelta = 0;

// Variables to store the whole opcode, high and low 4bits of the opcode
MYSTATIC BYTE bFullOpcode = 0;
MYSTATIC BYTE bOpcodeLow = 0;
MYSTATIC BYTE bOpcodeHigh = 0;

// Used only by FPU instructions
MYSTATIC BYTE bFPUModRM = 0;
MYSTATIC BYTE bFPUReg = 0;

// This is where each field of the current instruction being
// disassembled is stored.
// The information from this structure will finally be used to
// print out the disassembled instruction to the command line.
static INS_SPLIT insCurIns;

// No. of bytes in the current instruction; used to dump
// hex bytes by subtracting this from pByteInCode
static INT nBytesCurIns;

// String that is used to hold the asm instruction after disassembly
static WCHAR wszCurInsStr[MAX_INS_STRLEN + 1];
static WCHAR wszCurInsTempStr[MAX_INS_STRLEN + 1];

static DWORD lSize = 0;
// State machine control
static DASM_STATE dsNextState;

HWND DasmCodeView;

/*
 * **** End of static global variables ****
 */

/*
 * **** File local functions ****
 */

 // 16/32 bit offset based on address size prefix
BYTE ImmTypeFromAddrSize() {
    return (insCurIns.wPrefixTypes & PREFIX_ADSIZE) ? IMM_16BIT : IMM_32BIT;
}

BYTE ImmTypeFromOperandSize() {
    return (insCurIns.wPrefixTypes & PREFIX_OPSIZE) ? IMM_16BIT : IMM_32BIT;
}

BYTE ImmTypeFromWbitOperandSize() {
    if (insCurIns.bWBit == 0) {
        return IMM_8BIT;
    }
    return ImmTypeFromOperandSize();
}

// Register string (AL/BH/ECX/ESI/...) from reg code and address size prefix
WCHAR* RegStrFromAddrSize(int regCode) {
    return (insCurIns.wPrefixTypes & PREFIX_ADSIZE) ? awszRegCodes16[regCode] : awszRegCodes32[regCode];
}

// Register string (AL/BH/ECX/ESI/...) from reg code and operand size prefix
WCHAR* RegStrFromOperandSize(int regCode) {
    return (insCurIns.wPrefixTypes & PREFIX_OPSIZE) ? awszRegCodes16[regCode] : awszRegCodes32[regCode];
}

WCHAR* RegStrFromWbitOperandSize(int regCode) {
    if (insCurIns.bWBit == 0) {
        return awszRegCodes8[regCode];
    }
    return RegStrFromOperandSize(regCode);
}

WCHAR* PtrStrFromOperandSize() {
    if (insCurIns.bWBit == 0) {
        return awszPtrStr[PTR_STR_INDEX_BYTE];
    }
    return (insCurIns.wPrefixTypes & PREFIX_OPSIZE) ? awszPtrStr[PTR_STR_INDEX_WORD] : awszPtrStr[PTR_STR_INDEX_DWORD];
}

 /*
 * **** End of file local functions ****
 */

 /* ExecDisassembler()
  * Entry point for the DasmEngine. Calls DoDisassembly() for all the
  * code sections specified in the argument pCodeLocs.
  *
  * Args:
  *        pCodeLocs: Pointer to the struct containing the array of address
  *                    and size of all parts of code in the .text section.
  *        dwVirtCodeBase: DWORD value indicating the size of the
  *            code section. Required to stop the disassembler at the
  *            end of the code section(.text).
  *
  * RetVal:
  *        TRUE/FALSE depending on whether there was an error or not.
  *
  * Notes: Under construction
  */
BOOL ExecDisassembler(NCODE_LOCS *pCodeLocs, DWORD dwVirtCodeBase)
{
 /*   ASSERT(pCodeLocs != NULL);

    wprintf(L"\n\n**** Disassembly starting from FilePtr = 0x%08x ****\n",
        (long)pCodeLocs->aNonCodeLocs[0].dwFilePointer - (long)pCodeLocs->hFileBase);

    for (INT i = 0; i < pCodeLocs->nNonCodeParts; ++i)
    {
        if (pCodeLocs->aNonCodeLocs[i].iPartType == CODE_PART_CODE)
        {
            DoDisassembly((DWORD*)pCodeLocs->aNonCodeLocs[i].dwFilePointer,
                pCodeLocs->aNonCodeLocs[i].dwPartSize, dwVirtCodeBase, FALSE);
        }
    }
*/
    return FALSE;

}// ExecDisassembler()

void InsertString(HWND lv, CHAR *str, int iItem, int subItem)
{
    LVITEM lvi;
    ZeroMemory(&lvi, sizeof(lvi));
    lvi.mask = LVIF_TEXT;
    lvi.pszText = str;
    lvi.iItem = iItem;
    lvi.iSubItem = subItem;
    if(subItem)
        ListView_SetItem(lv, &lvi);
    else
        ListView_InsertItem(lv, &lvi);

}

/* DoDisassembly()
 * The DASM module calls this DoDisassembly() with a pointer to
 * the code section(.text) of the executable file. This is where
 * the entry point must be.
 * DoDisassembly() is modeled as a state machine.
 *
 * Args:
 *        pdwCodeSection: DWORD* pointer to the code section in the
 *            memory-mapped PE file being disassembled.
 *        dwSizeOfCodeSection: DWORD value indicating the size of the
 *            code section. Required to stop the disassembler at the
 *            end of the code section(.text).
 *
 *
 * RetVal:
 *        TRUE/FALSE depending on whether there was an error or not.
 */
BOOL DoDisassembly(HWND ViewList, DWORD *pdwCodeSection, DWORD dwSizeOfCodeSection, DWORD dwVirtCodeBase, BOOL f64bit)
{
    ASSERT(pdwCodeSection != NULL);

    g_f64bit = f64bit;
    lSize = 0;
    ListView_DeleteAllItems(ViewList);

    // We will read the code section byte by byte.
    pByteInCode = (BYTE*)pdwCodeSection;

    PBYTE pbEndOfCode = pByteInCode + dwSizeOfCodeSection;

    // State machine start-stop
    BOOL fRunning;

    lDelta = (long)dwVirtCodeBase - (long)pdwCodeSection;

    // Set the first state of the machine.
    dsNextState = DASM_STATE_RESET;

    // Set the state machine to start
    fRunning = TRUE;

    wprintf(L"\n* Disassembly of code section 0x%x\n", (DWORD)pByteInCode);
//return FALSE;
    // State machine is running until we encounter the end of code
    // section or an error occurs.
    while (1)
    {
        switch (dsNextState)
        {
        case DASM_STATE_RESET:
        {
            // Begin disassembly of a new instruction.
            // Clear out insCurIns.
            memset(&insCurIns, 0, sizeof(INS_SPLIT));
            nBytesCurIns = 0;

            // Check if we have reached the end of code section
            // before beginning decoding of the next byte
            if (pByteInCode >= pbEndOfCode)
                goto AFT_WHILE;
            else
                dsNextState = DASM_STATE_PREFIX;
            break;
        }

        case DASM_STATE_PREFIX:
        {
            if (!StatePrefix())
                dsNextState = DASM_STATE_ERROR;

            // If StatePrefix did not return FALSE, then the next state
            // was already determined in that function.

            break;
        }

        case DASM_STATE_OPCODE:
        {
            if (!StateOpcode())
            {
                //dsNextState = DASM_STATE_ERROR;

                logdbg(L"DoDisassembly(): OPCODE error. Continuing...");


                // simply print the offending opcode byte and move on
                StateDumpOnOpcodeError();

                // Let pByteInCode point to the byte after the current 
                // opcode/prefix byte processed
                pByteInCode = pByteInCode - nBytesCurIns + 1;
                dsNextState = DASM_STATE_RESET;
            }
            break;
        }

        case DASM_STATE_MOD_RM:
        {
            if (!StateModRM())
                dsNextState = DASM_STATE_ERROR;
            break;
        }

        case DASM_STATE_SIB:
        {
            if (!StateSIB())
                dsNextState = DASM_STATE_ERROR;
            break;
        }

        case DASM_STATE_DISP:
        {
            if (!StateDisp())
                dsNextState = DASM_STATE_ERROR;
            break;
        }

        case DASM_STATE_IMM:
        {
            if (!StateImm())
                dsNextState = DASM_STATE_ERROR;
            break;
        }

        case DASM_STATE_DUMP:
        {
            if (insCurIns.fpvCallback)
                insCurIns.fpvCallback();

            if (!StateDump(ViewList))
                dsNextState = DASM_STATE_ERROR;
            break;
        }

        case DASM_STATE_ERROR:
        {
            wprintf(L"DoDisassembly(): ERROR state. Aborting...\n");
            goto AFT_WHILE;
        }

        default:
        {
            // We will never reach here, typically.
            wprintf(L"DoDisassembly(): Invalid state: %d\n", dsNextState);
            goto AFT_WHILE;
        }

        }// switch(bCurState)

    }// while(1)

AFT_WHILE:
    logdbg(L"\n**** End of disassembly ****\n");
    logdbg(L"Code begin        : %08Xh\n", (DWORD)pdwCodeSection);
    logdbg(L"Code size         : %Xh\n", dwSizeOfCodeSection);
    logdbg(L"Last byte decoded : %08Xh\n", (DWORD)pByteInCode - 1);
    logdbg(L"Bytes decoded     : %Xh\n", (DWORD)pByteInCode - (DWORD)pdwCodeSection);

    return TRUE;
}// DoDisassembly

/* StateReset()
 * This is the state that the state machine begins to operate in.
 * This is the state that is reached after disassembling an instruction
 * and before starting the disassembly of the next instruction, unless
 * an error occured.
 *
 * Args:
 *
 * RetVal:
 *        TRUE/FALSE depending on whether there was an error or not.
 */
BOOL StateReset()
{
    return TRUE;
}

/* StatePrefix()
 * This state processes the prefix(es) in the instruction.
 *
 * Args:
 *
 * RetVal:
 *        TRUE/FALSE depending on whether there was an error or not.
 */
BOOL StatePrefix()
{
    BYTE nPrefixes = 0;

    // There may be upto 4 prefixes of 1byte each.
    // We must scan forward until we find a byte that
    // is not a prefix.
    while (Util_IsPrefix(*pByteInCode))
    {
        ++nPrefixes;

        switch (*pByteInCode)
        {
        case 0xf0:
        case 0xf2:
        case 0xf3:
            insCurIns.wPrefixTypes |= PREFIX_LOCKREP;
            insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] = *pByteInCode;
            break;

        case 0x2e:
        case 0x36:
        case 0x3e:
        case 0x26:
        case 0x64:
        case 0x65:
            insCurIns.wPrefixTypes |= PREFIX_SEG;
            insCurIns.abPrefixes[PREFIX_INDEX_SEG] = *pByteInCode;
            break;

        case 0x66:
            insCurIns.wPrefixTypes |= PREFIX_OPSIZE;
            insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] = *pByteInCode;
            break;

        case 0x67:
            insCurIns.wPrefixTypes |= PREFIX_ADSIZE;
            insCurIns.abPrefixes[PREFIX_INDEX_ADSIZE] = *pByteInCode;
            break;

        default:
            wprintf(L"StatePrefix(): Unrecognized prefix type %u\n", *pByteInCode);
            break;
        }// switch(*pByteInCode)

        logdbg(L"PREFIX: %xh", *pByteInCode);

        // We have read 1byte
        ++pByteInCode;
        ++nBytesCurIns;
    }

    insCurIns.nPrefix = nPrefixes;
    if (nPrefixes > 0)
        insCurIns.fPrefix = TRUE;

    // No more prefixes to process.
    // pByteInCode is now pointing to an opcode.
    dsNextState = DASM_STATE_OPCODE;

    return TRUE;

}// StatePrefix()


/* StateOpcode()
 * This state processes the opcode(s) in the instruction.
 *
 * Args:
 *
 * RetVal:
 *        TRUE/FALSE depending on whether there was an error or not.
 */
BOOL StateOpcode()
{
    // Lookup instruction string using opcode
    WORD wIndex;

    // Differentiate whether it is a 1byte/2byte opcode
    if (*pByteInCode == OPC_2BYTE_ESCAPE)
    {
        return OPCHndlr_2ByteHandler(*pByteInCode);
    }

    // 1byte opcode
    bFullOpcode = *pByteInCode;

    // Separate out the first and second 4bits and use them
    // as indexes into the afpOpcHndlrs function pointer matrix
    bOpcodeLow = *pByteInCode & 0x0f;
    bOpcodeHigh = (*pByteInCode & 0xf0) >> 4;

    ++pByteInCode;
    ++nBytesCurIns;

    // store the d and w bits in INS_SPLIT
    Util_GetDWBits(bFullOpcode, &insCurIns.bDBit, &insCurIns.bWBit);

    // Construct the output string starting with the instruction mnemonic
    wIndex = (bOpcodeHigh * 16) + bOpcodeLow;
    wsprintfW(wszCurInsStr, L"%s", awszOpcodeLUT1[wIndex]);

    // function call
    return afpOpcHndlrs[bOpcodeHigh][bOpcodeLow](bFullOpcode);

}// StateOpcode()


/* OPCHndlr_2ByteHandler()
 * This state processes the 2byte opcodes.
 *
 * Args:
 *
 * RetVal:
 *        TRUE/FALSE depending on whether there was an error or not.
 */
BOOL OPCHndlr_2ByteHandler(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);
#ifdef _DEBUG
    if (bOpcode != 0x0f)
    {
        wprintf(L"OPCHndlr_2ByteHandler(): Not a 2byte escape code: %xh\n", bOpcode);
        return FALSE;
    }
#endif

    WORD wIndex;

    // Go past 0x0f
    ++pByteInCode;
    ++nBytesCurIns;

    // pByteInCode is now pointing to the second byte of the opcode;
    // i.e., the byte after 0x0f
    bFullOpcode = *pByteInCode;
    ++pByteInCode;
    ++nBytesCurIns;

    // pByteInCode now points to the byte after the second opcode byte

    // Separate out the first and second 4bits and use them
    // as indexes into the afpOpcHndlrs2 function pointer matrix
    bOpcodeLow = bFullOpcode & 0x0f;
    bOpcodeHigh = (bFullOpcode & 0xf0) >> 4;

    // store the d and w bits in INS_SPLIT
    Util_GetDWBits(bFullOpcode, &insCurIns.bDBit, &insCurIns.bWBit);

    // Construct the output string starting with the instruction mnemonic
    wIndex = (bOpcodeHigh * 16) + bOpcodeLow;
    wsprintfW(wszCurInsStr, L"%s", awszOpcodeLUT2[wIndex]);

    // function call
    return afpOpcHndlrs2[bOpcodeHigh][bOpcodeLow](bFullOpcode);

}// OPCHndlr_2ByteHandler()


/* OPCHndlr_3ByteHandler()
 * This state processes the 3byte opcodes.
 *
 * Args:
 *
 * RetVal:
 *        TRUE/FALSE depending on whether there was an error or not.
 */
BOOL OPCHndlr_3ByteHandler(BYTE bOpcode)
{
    // pByteInCode is now pointing to the third byte of the opcode;
    // i.e., the byte after 0x3a or 0x38
    bFullOpcode = *pByteInCode;
    ++pByteInCode;
    ++nBytesCurIns;

    if (bOpcode == 0x38)
    {
        switch (bFullOpcode)
        {
        case 0xDB:
        case 0xDC:
        case 0xDD:
        case 0xDE:
        case 0xDF: return OPCHndlrAES(bFullOpcode); break;
        default:
            wprintf(L"OPCHndlr_3ByteHandler(): Invalid third byte %xh %xh\n", bOpcode, bFullOpcode);
            return FALSE;
        }
    }
    else if (bOpcode == 0x3A)
    {
        switch (bFullOpcode)
        {
        case 0xDF: return OPCHndlrAES(bFullOpcode); break;
        default:
            wprintf(L"OPCHndlr_3ByteHandler(): Invalid third byte %xh %xh\n", bOpcode, bFullOpcode);
            return FALSE;
        }
    }
    else
    {
        wprintf(L"OPCHndlr_3ByteHandler(): Not a 3byte escape code: %xh\n", bOpcode);
        return FALSE;
    }

}// OPCHndlr32ByteHandler()

/*
 * OPCHndlrFPU_All()
 * This function is called for all opcodes between D8h and DFh,
 * which are the FPU instructions.
 *
 */
BOOL OPCHndlrFPU_All(BYTE bOpcode)
{
    // All FPU instructions have 0xD as the high nibble
    ASSERT(bOpcodeHigh == 0xD);

    // Figure out how this instruction uses the ModRM byte:
    // - All bits as opcode extension OR
    // - Only 'reg' field as opcode extension

#ifndef DASM_BUILD_FOR_UNITTEST
    bFPUModRM = *pByteInCode;
#endif

    // pByteInCode now points to the next instruction.
    // pByteInCode need not be modified in any of the
    // individual FPU instruction functions.

    if (bFPUModRM <= 0xBF)
        return OPCHndlrFPU_ModRMRegEx(bOpcode);

    // ModRM lies between C0h and FFh
    return OPCHndlrFPU_ModRMFullEx(bOpcode);
}

BOOL OPCHndlrFPU_ModRMRegEx(BYTE bOpcode)
{
    ASSERT(bFPUModRM >= 0x00 && bFPUModRM <= 0xBF);    // always??

    // Value of ModRM is stored into global variable bFPUModRM
    // pByteInCode is now pointing to the ModRM byte

    INT iRowIndex;
    INT iInsStrIndex;

    Util_SplitModRMByte(bFPUModRM, NULL, &bFPUReg, NULL);
    ASSERT(bFPUReg >= 0 && bFPUReg <= 7);

    /*
     * Subtract the opcode by 0xD8 to obtain a zero-based
     * opcode to use as row index into afpFPUModRMRegEx.
     * Use the bReg value as the column index.
     */
    iRowIndex = bOpcode - 0xD8;

    iInsStrIndex = iRowIndex * 8 + bFPUReg;
    lstrcpyW(wszCurInsStr, awszFPUOpcodeLUTRegEx[iInsStrIndex]);

    return afpFPUModRMRegEx[iRowIndex][bFPUReg](bOpcode);
}

BOOL OPCHndlrFPU_ModRMFullEx(BYTE bOpcode)
{
    ASSERT(bFPUModRM >= 0xC0 && bFPUModRM <= 0xFF);    // always??

    // Value of ModRM is stored into global variable bFPUModRM

    /*
     * pByteInCode is now pointing to the ModRM byte.
     * Move past the ModRM byte because it is not used
     * to calculate the effective address but is just an
     * opcode extension. We are going to DUMP state next.
     */
    ++pByteInCode;
    ++nBytesCurIns;

    /*
     * Subtract the opcode by 0xD8 to obtain a zero-based
     * opcode to use as row index into afpFPUModRMRegEx.
     * Subtract the ModRM value by 0xC0 to obtain a zero-based
     * ModRM to use as column index.
     */
    BYTE bRowIndex = bOpcode - 0xD8;
    BYTE bColIndex = bFPUModRM - 0xC0;
    INT iInsStrIndex = bRowIndex * 0x40 + bColIndex;

    lstrcpyW(wszCurInsStr, awszFPUOpcodeLUTFullEx[iInsStrIndex]);

    return afpFPUModRMFullEx[bRowIndex][bColIndex](bOpcode);
}


/*
 * *************************
 *        Begin MODRM state
 * *************************
 */

 /* StateModRM()
  * This state processes the ModRM byte in the instruction.
  *
  * Args:
  *
  * RetVal:
  *        TRUE/FALSE depending on whether there was an error or not.
  */
BOOL StateModRM()
{
    BYTE bModRM;
    BYTE bMod;
    BYTE bReg;
    BYTE bRM;
    BOOL fRetVal = TRUE;

    bModRM = *pByteInCode;
    ++pByteInCode;
    ++nBytesCurIns;
    logdbg(L"StateModRM(): %xh", bModRM);

    // pByteInCode now points to either a disp/SIB/IMM

    insCurIns.bModRM = bModRM;

    // split the ModRM byte into individual fields
    Util_SplitModRMByte(bModRM, &bMod, &bReg, &bRM);

    // Check the ModRM type
    if (insCurIns.bModRMType == MODRM_TYPE_DIGIT)
    {
        // Only Mod+RM bits are to be evaluated
        // Only 1 operand specified using ModRM byte
        fRetVal = MODRM_fTypeDigit(bModRM, bMod, bRM);
    }

    else if (insCurIns.bModRMType == MODRM_TYPE_R)
    {
        // Two operands specified by Mod+RM and Reg bits
        fRetVal = MODRM_fTypeR(bModRM, bMod, bReg, bRM);
    }
    else
    {
#ifdef _DEBUG
        wprintf(L"StateModRM(): Invalid bModRMType %d\n", insCurIns.bModRMType);
        fRetVal = FALSE;
#endif
    }

    // dsNextState has been determined in fSplIns()/TypeR()/TypeDigit()

    return fRetVal;

}// StateModRM


/*
 * MODRM_fTypeDigit()
 *    Reg field is the opcode extension. Mod+RM fields give the effective
 *    address of the operand. Next state will be set if this returns TRUE.
 */
BOOL MODRM_fTypeDigit(BYTE /*bModRM*/, BYTE bMod, BYTE bRM)
{
    BYTE bOprSize;
    OPRTYPE_RETVAL oprType;

    /*
     * Only one operand from ModRM byte and that is stored as the src str
     */

    if (insCurIns.fSpecialInstruction)// && 
       //insCurIns.bSplInsType == SPL_INS_TYPE_SIZEHINT)
    {
        bOprSize = insCurIns.bOperandSizeSrc;
    }
    else
        bOprSize = MODRM_bGetOperandSize();

    // Set bImmType if required
    if (insCurIns.fImm && insCurIns.bImmType == IMM_UNDEF)
        insCurIns.bImmType = bOprSize;

    // If we have a special instruction with specific regs
    // No, specific regs is specified using /r ModRM type; never /digit
    // 081812: MMX regs are specified using 'reg' field also.
    oprType = MODRM_GetOperandFromModRM(bMod, bRM, bOprSize, insCurIns.bRegTypeSrc,
        insCurIns.wszCurInsStrSrc, _countof(insCurIns.wszCurInsStrSrc));

    if (oprType == OPRTYPE_ERROR)
    {
        logdbg(L"MODRM_fTypeDigit(): Error retrieving operand: Mod = %d : RM = %d", bMod, bRM);
        return FALSE;
    }

    insCurIns.fSrcStrSet = TRUE;
    insCurIns.OprTypeSrc = oprType;

    switch (oprType)
    {
    case OPRTYPE_MEM:
    {
        // Set ptr str
        insCurIns.pwszPtrStr = awszPtrStr[bOprSize];

        // Before going to DUMP, check whether there is a imm value
        if (insCurIns.fImm)
            dsNextState = DASM_STATE_IMM;
        else
            dsNextState = DASM_STATE_DUMP;
        break;
    }

    case OPRTYPE_MEM8:    // We have a displacement and must go to DISP state next
    {
        insCurIns.pwszPtrStr = awszPtrStr[bOprSize];
        insCurIns.fDisp = TRUE;
        insCurIns.bDispType = DISP_8BIT;
        dsNextState = DASM_STATE_DISP;
        break;
    }

    case OPRTYPE_MEM32:    // disp32 follows
    {
        insCurIns.pwszPtrStr = awszPtrStr[bOprSize];
        insCurIns.fDisp = TRUE;
        insCurIns.bDispType = DISP_32BIT;
        dsNextState = DASM_STATE_DISP;
        break;
    }

    case OPRTYPE_SIB:    // No displacement
    {
        insCurIns.pwszPtrStr = awszPtrStr[bOprSize];
        dsNextState = DASM_STATE_SIB;
        break;
    }

    case OPRTYPE_SIB8:    // SIB follows, followed by disp8
    {
        insCurIns.pwszPtrStr = awszPtrStr[bOprSize];
        insCurIns.fSIB = TRUE;
        insCurIns.fDisp = TRUE;
        insCurIns.bDispType = DISP_8BIT;
        dsNextState = DASM_STATE_SIB;    // SIB state must check for fDisp and transition
        break;
    }

    case OPRTYPE_SIB32:    // SIB follows, followed by disp32
    {
        insCurIns.pwszPtrStr = awszPtrStr[bOprSize];
        insCurIns.fSIB = TRUE;
        insCurIns.fDisp = TRUE;
        insCurIns.bDispType = DISP_32BIT;
        dsNextState = DASM_STATE_SIB;    // SIB state must check for fDisp and transition
        break;
    }

    // todo: check whether there is a SIB byte and then the disp32
    case OPRTYPE_DISP32:    // disp32 follows
    {
        insCurIns.pwszPtrStr = awszPtrStr[bOprSize];
        insCurIns.fDisp = TRUE;
        insCurIns.bDispType = DISP_32BIT;
        dsNextState = DASM_STATE_DISP;
        break;
    }

    case OPRTYPE_REG:
    {
        // Before going to DUMP, check whether there is a imm value
        if (insCurIns.fImm)
            dsNextState = DASM_STATE_IMM;
        else
            dsNextState = DASM_STATE_DUMP;
        break;
    }

    default:
        wprintf(L"MODRM_fTypeDigit(): Invalid oprType %d\n", oprType);
        return FALSE;

    }// switch(oprType)

    return TRUE;
}// MODRM_fTypeDigit()


/*
 * MODRM_fTypeR()
 *        ** D-bit implications **
 *    If dbit = 1, reg    <-- Mod+RM    : src = Mod+RM
 *    If dbit = 0, Mod+RM    <-- reg        : src = Reg
 *
 *    Next state will be set if this returns TRUE.
 */
BOOL MODRM_fTypeR(BYTE /*bModRM*/, BYTE bMod, BYTE bReg, BYTE bRM)
{
    ASSERT(bMod >= 0 && bMod <= 3);
    ASSERT(bRM >= 0 && bRM <= 7);
    ASSERT(bReg >= 0 && bReg <= 7);

    // 2 different variables in order to handle fSplInstruction
    BYTE bOprSizeSrc;
    BYTE bOprSizeDes;

    // Eliminating redundant switch()
    BYTE bLocalOprSize;
    OPRTYPE_RETVAL LocalOprType;


    if (insCurIns.fSpecialInstruction)
    {
        bOprSizeSrc = insCurIns.bOperandSizeSrc;
        bOprSizeDes = insCurIns.bOperandSizeDes;
#ifdef _DEBUG
        if (insCurIns.fImm && insCurIns.bImmType == IMM_UNDEF)
        {
            wprintf(L"MODRM_fTypeR(): fSpl bImmType UNDEF\n");
            return FALSE;
        }
#endif
    }
    else
    {
        bOprSizeSrc = bOprSizeDes = MODRM_bGetOperandSize();
        if (insCurIns.fImm && insCurIns.bImmType == IMM_UNDEF)
        {
            insCurIns.bImmType = bOprSizeSrc;
        }
    }

    // There are two operands specified using the ModRM byte.
    // Get source and destination operands
    if (insCurIns.bDBit == 0)
    {
        // src = Reg; des = Mod+RM

        // If we have a special instruction with specific regs
        //if(insCurIns.fSpecialInstruction && insCurIns.bSplInsType == SPL_INS_TYPE_REG)
        //{
        //    insCurIns.OprTypeSrc = MODRM_GetOperandFromReg(bReg, bOprSizeSrc, insCurIns.bRegTypeSrc, 
        //                                insCurIns.wszCurInsStrSrc, _countof(insCurIns.wszCurInsStrSrc));
        //    insCurIns.OprTypeDes = MODRM_GetOperandFromModRM(bMod, bRM, bOprSizeDes, insCurIns.bRegTypeDest, 
        //                                insCurIns.wszCurInsStrDes, _countof(insCurIns.wszCurInsStrDes));
        //}
        //else
        //{
        //    insCurIns.OprTypeSrc = MODRM_GetOperandFromReg(bReg, bOprSizeSrc, REGTYPE_GREG, 
        //                                insCurIns.wszCurInsStrSrc, _countof(insCurIns.wszCurInsStrSrc));
        //    insCurIns.OprTypeDes = MODRM_GetOperandFromModRM(bMod, bRM, bOprSizeDes, insCurIns.bRegTypeDest, 
        //                                insCurIns.wszCurInsStrDes, _countof(insCurIns.wszCurInsStrDes));
        //}

        insCurIns.OprTypeSrc = MODRM_GetOperandFromReg(bReg, bOprSizeSrc, insCurIns.bRegTypeSrc,
            insCurIns.wszCurInsStrSrc, _countof(insCurIns.wszCurInsStrSrc));
        insCurIns.OprTypeDes = MODRM_GetOperandFromModRM(bMod, bRM, bOprSizeDes, insCurIns.bRegTypeDest,
            insCurIns.wszCurInsStrDes, _countof(insCurIns.wszCurInsStrDes));

        LocalOprType = insCurIns.OprTypeDes;
        bLocalOprSize = bOprSizeDes;

        //insCurIns.fSrcStrSet = TRUE;
        //insCurIns.fDesStrSet = TRUE;
        //switch(insCurIns.OprTypeDes)
        //{
        //    case OPRTYPE_MEM:
        //    {
        //        // Set ptr str
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeDes];
        //        // Before going to DUMP, check whether there is a imm value
        //        if(insCurIns.fImm)
        //        {
        //            dsNextState = DASM_STATE_IMM;
        //        }
        //        else
        //            dsNextState = DASM_STATE_DUMP;
        //        break;
        //    }
        //
        //    case OPRTYPE_MEM8:    // We have a displacement and must go to DISP state next
        //    {
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeDes];
        //        insCurIns.fDisp = TRUE;
        //        insCurIns.bDispType = DISP_8BIT;
        //        dsNextState = DASM_STATE_DISP;
        //        break;
        //    }

        //    case OPRTYPE_MEM32:    // disp32 follows
        //    {
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeDes];
        //        insCurIns.fDisp = TRUE;
        //        insCurIns.bDispType = DISP_32BIT;
        //        dsNextState = DASM_STATE_DISP;
        //        break;
        //    }

        //    case OPRTYPE_SIB:    // No displacement
        //    {
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeDes];
        //        dsNextState = DASM_STATE_SIB;
        //        break;
        //    }

        //    case OPRTYPE_SIB8:    // SIB follows, followed by disp8
        //    {
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeDes];
        //        insCurIns.fSIB = TRUE;
        //        insCurIns.fDisp = TRUE;
        //        insCurIns.bDispType = DISP_8BIT;
        //        dsNextState = DASM_STATE_SIB;    // SIB state must check for fDisp and transition
        //        break;
        //    }

        //    case OPRTYPE_SIB32:    // SIB follows, followed by disp32
        //    {
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeDes];
        //        insCurIns.fSIB = TRUE;
        //        insCurIns.fDisp = TRUE;
        //        insCurIns.bDispType = DISP_32BIT;
        //        dsNextState = DASM_STATE_SIB;    // SIB state must check for fDisp and transition
        //        break;
        //    }

        //    case OPRTYPE_DISP32:    // disp32 follows
        //    {    
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeDes];
        //        insCurIns.fDisp = TRUE;
        //        insCurIns.bDispType = DISP_32BIT;
        //        dsNextState = DASM_STATE_DISP;
        //        break;
        //    }

        //    case OPRTYPE_REG:
        //    {
        //        // Before going to DUMP, check whether there is a imm value
        //        if(insCurIns.fImm)
        //            dsNextState = DASM_STATE_IMM;
        //        else
        //            dsNextState = DASM_STATE_DUMP;
        //        break;
        //    }

        //    default:
        //        wprintf(L"MODRM_fTypeR(): Invalid oprType %d\n", insCurIns.OprTypeDes);
        //        return FALSE;

        //}// switch(oprType)
    }
    else
    {
        // src = Mod+RM; des = Reg

        // If we have a special instruction with specific regs
        //if(insCurIns.fSpecialInstruction && insCurIns.bSplInsType == SPL_INS_TYPE_REG)
        //{
        //    insCurIns.OprTypeSrc = MODRM_GetOperandFromModRM(bMod, bRM, bOprSizeSrc, insCurIns.bRegTypeSrc,
        //                                insCurIns.wszCurInsStrSrc, _countof(insCurIns.wszCurInsStrSrc));
        //    insCurIns.OprTypeDes = MODRM_GetOperandFromReg(bReg, bOprSizeDes, insCurIns.bRegTypeDest, 
        //                                insCurIns.wszCurInsStrDes, _countof(insCurIns.wszCurInsStrDes));
        //}
        //else
        //{
        //    insCurIns.OprTypeSrc = MODRM_GetOperandFromModRM(bMod, bRM, bOprSizeSrc, insCurIns.bRegTypeSrc,
        //                                insCurIns.wszCurInsStrSrc, _countof(insCurIns.wszCurInsStrSrc));
        //    insCurIns.OprTypeDes = MODRM_GetOperandFromReg(bReg, bOprSizeDes, insCurIns.bRegTypeDest, 
        //                                insCurIns.wszCurInsStrDes, _countof(insCurIns.wszCurInsStrDes));
        //}

        insCurIns.OprTypeSrc = MODRM_GetOperandFromModRM(bMod, bRM, bOprSizeSrc, insCurIns.bRegTypeSrc,
            insCurIns.wszCurInsStrSrc, _countof(insCurIns.wszCurInsStrSrc));
        insCurIns.OprTypeDes = MODRM_GetOperandFromReg(bReg, bOprSizeDes, insCurIns.bRegTypeDest,
            insCurIns.wszCurInsStrDes, _countof(insCurIns.wszCurInsStrDes));

        LocalOprType = insCurIns.OprTypeSrc;
        bLocalOprSize = bOprSizeSrc;

        //insCurIns.fSrcStrSet = TRUE;
        //insCurIns.fDesStrSet = TRUE;

        //switch(insCurIns.OprTypeSrc)
        //{
        //    case OPRTYPE_MEM:
        //    {
        //        // Set ptr str
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeSrc];
        //        // Before going to DUMP, check whether there is a imm value
        //        if(insCurIns.fImm)
        //            dsNextState = DASM_STATE_IMM;
        //        else
        //            dsNextState = DASM_STATE_DUMP;
        //        break;
        //    }
        //
        //    case OPRTYPE_MEM8:    // We have a displacement and must go to DISP state next
        //    {
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeSrc];
        //        insCurIns.fDisp = TRUE;
        //        insCurIns.bDispType = DISP_8BIT;
        //        dsNextState = DASM_STATE_DISP;
        //        break;
        //    }

        //    case OPRTYPE_MEM32:    // disp32 follows
        //    {
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeSrc];
        //        insCurIns.fDisp = TRUE;
        //        insCurIns.bDispType = DISP_32BIT;
        //        dsNextState = DASM_STATE_DISP;
        //        break;
        //    }

        //    case OPRTYPE_SIB:    // No displacement
        //    {
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeSrc];
        //        dsNextState = DASM_STATE_SIB;
        //        break;
        //    }

        //    case OPRTYPE_SIB8:    // SIB follows, followed by disp8
        //    {
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeSrc];
        //        insCurIns.fSIB = TRUE;
        //        insCurIns.fDisp = TRUE;
        //        insCurIns.bDispType = DISP_8BIT;
        //        dsNextState = DASM_STATE_SIB;    // SIB state must check for fDisp and transition
        //        break;
        //    }

        //    case OPRTYPE_SIB32:    // SIB follows, followed by disp32
        //    {
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeSrc];
        //        insCurIns.fSIB = TRUE;
        //        insCurIns.fDisp = TRUE;
        //        insCurIns.bDispType = DISP_32BIT;
        //        dsNextState = DASM_STATE_SIB;    // SIB state must check for fDisp and transition
        //        break;
        //    }

        //    case OPRTYPE_DISP32:    // disp32 follows
        //    {
        //        insCurIns.pwszPtrStr = awszPtrStr[bOprSizeSrc];
        //        insCurIns.fDisp = TRUE;
        //        insCurIns.bDispType = DISP_32BIT;
        //        dsNextState = DASM_STATE_DISP;
        //        break;
        //    }

        //    case OPRTYPE_REG:
        //    {
        //        // Before going to DUMP, check whether there is a imm value
        //        if(insCurIns.fImm)
        //            dsNextState = DASM_STATE_IMM;
        //        else
        //            dsNextState = DASM_STATE_DUMP;
        //        break;
        //    }

        //    default:
        //        wprintf(L"MODRM_fTypeDigit(): Invalid oprType %d\n", insCurIns.OprTypeSrc);
        //        return FALSE;

        //}// switch(oprType)

    }// if(insCurIns.bDBit == 0)

    insCurIns.fSrcStrSet = TRUE;
    insCurIns.fDesStrSet = TRUE;

    switch (LocalOprType)
    {
    case OPRTYPE_MEM:
    {
        // Set ptr str
        insCurIns.pwszPtrStr = awszPtrStr[bLocalOprSize];
        // Before going to DUMP, check whether there is a imm value
        if (insCurIns.fImm)
            dsNextState = DASM_STATE_IMM;
        else
            dsNextState = DASM_STATE_DUMP;
        break;
    }

    case OPRTYPE_MEM8:    // We have a displacement and must go to DISP state next
    {
        insCurIns.pwszPtrStr = awszPtrStr[bLocalOprSize];
        insCurIns.fDisp = TRUE;
        insCurIns.bDispType = DISP_8BIT;
        dsNextState = DASM_STATE_DISP;
        break;
    }

    case OPRTYPE_MEM32:    // disp32 follows
    {
        insCurIns.pwszPtrStr = awszPtrStr[bLocalOprSize];
        insCurIns.fDisp = TRUE;
        insCurIns.bDispType = DISP_32BIT;
        dsNextState = DASM_STATE_DISP;
        break;
    }

    case OPRTYPE_SIB:    // No displacement
    {
        insCurIns.pwszPtrStr = awszPtrStr[bLocalOprSize];
        dsNextState = DASM_STATE_SIB;
        break;
    }

    case OPRTYPE_SIB8:    // SIB follows, followed by disp8
    {
        insCurIns.pwszPtrStr = awszPtrStr[bLocalOprSize];
        insCurIns.fSIB = TRUE;
        insCurIns.fDisp = TRUE;
        insCurIns.bDispType = DISP_8BIT;
        dsNextState = DASM_STATE_SIB;    // SIB state must check for fDisp and transition
        break;
    }

    case OPRTYPE_SIB32:    // SIB follows, followed by disp32
    {
        insCurIns.pwszPtrStr = awszPtrStr[bLocalOprSize];
        insCurIns.fSIB = TRUE;
        insCurIns.fDisp = TRUE;
        insCurIns.bDispType = DISP_32BIT;
        dsNextState = DASM_STATE_SIB;    // SIB state must check for fDisp and transition
        break;
    }

    case OPRTYPE_DISP32:    // disp32 follows
    {
        insCurIns.pwszPtrStr = awszPtrStr[bLocalOprSize];
        insCurIns.fDisp = TRUE;
        insCurIns.bDispType = DISP_32BIT;
        dsNextState = DASM_STATE_DISP;
        break;
    }

    case OPRTYPE_REG:
    {
        // Before going to DUMP, check whether there is a imm value
        if (insCurIns.fImm)
            dsNextState = DASM_STATE_IMM;
        else
            dsNextState = DASM_STATE_DUMP;
        break;
    }

    default:
        wprintf(L"MODRM_fTypeR(): Invalid oprType %d\n", bLocalOprSize);
        return FALSE;

    }// switch(oprType)

    // dsNextState has been set

    return TRUE;
}


OPRTYPE_RETVAL MODRM_GetOperandFromModRM(BYTE bMod, BYTE bRM, BYTE bOprSize, BYTE bRegType,
    __out WCHAR *pwszOprStr, __in DWORD dwOprStrCount)
{
    ASSERT(bMod >= 0 && bMod <= 3);
    ASSERT(bRM >= 0 && bRM <= 7);
    ASSERT(pwszOprStr != NULL);

    switch (bMod)
    {
    case 0:    // Effective address is in one of the GPRs/is a disp32/in a SIB byte
    {        // depending on the value of the RM field.
        if (bRM == EFFADDR_DISP32)
            return OPRTYPE_DISP32;

        if (bRM == EFFADDR_SIB)
            return OPRTYPE_SIB;

        // [eax] / [ecx] / ...
        wsprintfW(pwszOprStr, L"%s", awszRegCodes32[bRM]);

        return OPRTYPE_MEM;
    }

    case 1:    // Effective address is in one of the GPRs with disp8/in a 
    {        // SIB byte with disp8 depending on the value of the RM field.            
        if (bRM == EFFADDR_SIB)
            return OPRTYPE_SIB8;

        // [eax] / [ecx] / ...
        wsprintfW(pwszOprStr, L"%s", awszRegCodes32[bRM]);

        return OPRTYPE_MEM8;
    }

    case 2:    // Effective address is in one of the GPRs with disp32/in a 
    {        // SIB byte with disp32 depending on the value of the RM field.            
        if (bRM == EFFADDR_SIB)
            return OPRTYPE_SIB32;

        // [eax] / [ecx] / ...
        wsprintfW(pwszOprStr, L"%s", awszRegCodes32[bRM]);

        return OPRTYPE_MEM32;
    }

    case 3: // Operand is a GPR/CR/DR/...
    {       // handle CR/DR/SegReg: C,D,S regs are in 'reg' field; never in Mod+RM fields
            // 081812: but MMX regs may be specified using Mod+RM bits. So include a check
            // for this. (added one function argument:bRegType).
        if (bRegType == REGTYPE_GREG || bRegType == REGTYPE_UNDEF)
        {
            if (bOprSize == OPERANDSIZE_8BIT)
                lstrcpyW(pwszOprStr, awszRegCodes8[bRM]);
            else if (bOprSize == OPERANDSIZE_16BIT)
                lstrcpyW(pwszOprStr, awszRegCodes16[bRM]);
            else if (bOprSize == OPERANDSIZE_32BIT)
                lstrcpyW(pwszOprStr, awszRegCodes32[bRM]);
        }
        else if (bRegType == REGTYPE_MMX)
        {
            lstrcpyW(pwszOprStr, awszMMXRegCodes[bRM]);
        }
        else if (bRegType == REGTYPE_XMM)
        {
            lstrcpyW(pwszOprStr, awszXMMRegCodes[bRM]);
        }
        else
        {
            wprintf(L"MODRM_GetOperandFromModRM(): Invalid bRegType %d\n", bRegType);
        }

        return OPRTYPE_REG;

    }// case 3

    default:
        wprintf(L"MODRM_GetOperandFromModRM(): Invalid bMod value %d\n", bMod);
        break;
    }// switch(bMod)

    return OPRTYPE_ERROR;

}// MODRM_GetOperandFromModRM()

OPRTYPE_RETVAL MODRM_GetOperandFromReg(BYTE bReg, BYTE bOprSize, BYTE bRegType,
    __out WCHAR *pwszOprStr, __in DWORD dwOprStrCount)
{
    ASSERT(bReg >= 0 && bReg <= 7);
    ASSERT(pwszOprStr != NULL);

    // Operand is one of the GPR/CR/DR/SegReg...
    // handle CR/DR/SegReg
    // Yes, C,D,S regs are encoded in the 'reg' field
    // 081812: added check for MMX register
    switch (bRegType)
    {
    case REGTYPE_UNDEF:
    case REGTYPE_GREG:
    {
        if (bOprSize == OPERANDSIZE_8BIT)
            lstrcpyW(pwszOprStr, awszRegCodes8[bReg]);
        else if (bOprSize == OPERANDSIZE_16BIT)
            lstrcpyW(pwszOprStr, awszRegCodes16[bReg]);
        else if (bOprSize == OPERANDSIZE_32BIT)
            lstrcpyW(pwszOprStr, awszRegCodes32[bReg]);
        else
        {
            wprintf(L"MODRM_GetOperandFromReg(): Invalid bOprSize %u\n", bOprSize);
            return OPRTYPE_ERROR;
        }
        break;
    }// UNDEF, GREG

    case REGTYPE_SREG:
        lstrcpyW(pwszOprStr, awszSRegCodes[bReg]);
        break;

    case REGTYPE_CREG:
        lstrcpyW(pwszOprStr, awszCRegCodes[bReg]);
        break;

    case REGTYPE_DREG:
        lstrcpyW(pwszOprStr, awszDRegCodes[bReg]);
        break;

    case REGTYPE_MMX:
        lstrcpyW(pwszOprStr, awszMMXRegCodes[bReg]);
        break;

    case REGTYPE_XMM:
        lstrcpyW(pwszOprStr, awszXMMRegCodes[bReg]);
        break;

    default:
        wprintf(L"MODRM_GetOperandFromReg(): Invalid bRegType %u\n", bRegType);
        return OPRTYPE_ERROR;

    }// switch(bRegType)

    return OPRTYPE_REG;
}


BOOL MODRM_fSetPtrStr(BYTE bOperandSize, __out WCHAR *pwszPtrStr, DWORD dwPtrStrCount)
{
    ASSERT(OPERANDSIZE_UNDEF <= bOperandSize && bOperandSize <= OPERANDSIZE_64BIT);

    if (bOperandSize != OPERANDSIZE_UNDEF)
    {
        // Operand size already determined earlier;
        // maybe a fSplInstruction!
        lstrcpyW(pwszPtrStr, awszPtrStr[bOperandSize]);
        return TRUE;
    }

    // If bOperandSize has not been determined earlier, then we must
    // determine it using wBit and opsize attrib
    lstrcpyW(pwszPtrStr, PtrStrFromOperandSize());

    return TRUE;
}

/*
 * MODRM_bGetOperandSize()
 *    Returns the operand size based on the value of wBit and
 *    opsize attrib. This function must be NOT be used for
 *    fSplInstructions!
 */
BYTE MODRM_bGetOperandSize()
{
    if (!insCurIns.fWBitAbsent && insCurIns.bWBit == 0)
        return OPERANDSIZE_8BIT;

    if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
        return OPERANDSIZE_16BIT;

    return OPERANDSIZE_32BIT;
}

/*
 * *************************
 *        Begin SIB state
 * *************************
 */

 /* StateSIB()
  * This state processes the SIB byte in the instruction.
  *
  * Args:
  *
  * RetVal:
  *        TRUE/FALSE depending on whether there was an error or not.
  */
BOOL StateSIB()
{
    BYTE bSIB, bScale, bIndex, bBase;

    bSIB = *pByteInCode;
    ++pByteInCode;
    ++nBytesCurIns;
    insCurIns.bSIB = bSIB;
    logdbg(L"StateSIB(): %X", bSIB);

    // SplitModRM can be used because the structure of both
    // ModRM and SIB byte is the same.
    Util_SplitModRMByte(bSIB, &bScale, &bIndex, &bBase);

    // _DEBUG only
    ASSERT(bScale >= 0 && bScale <= 3);
    ASSERT(bIndex >= 0 && bIndex <= 7);
    ASSERT(bBase >= 0 && bBase <= 7);

    //                                                   I   S    B
    //    Eg.: 89 04 95 D0 60 D5 00    mov        dword ptr [edx*4+0D560D0h],eax
    // Above, B=base=disp32
    //

    // Determine the scale index string
    switch (bScale)
    {
    case 0:    // no scaling
        if (bIndex == 4)        // no index either
        {
            insCurIns.wszScaleIndex[0] = 0;    // must not print this in DUMP
            break;
        }

        // todo: Add '[' ']' in DUMP state: done
        wsprintfW(insCurIns.wszScaleIndex, L"%s",
            awszRegCodes32[bIndex]);
        break;

    case 1:    // scale 2
        if (bIndex == 4)        // Nothing
        {
            insCurIns.wszScaleIndex[0] = 0;    // must not print this in DUMP
            break;
        }

        wsprintfW(insCurIns.wszScaleIndex, L"%s*2",
            awszRegCodes32[bIndex]);
        break;

    case 2:    // scale 4
        if (bIndex == 4)        // Nothing
        {
            insCurIns.wszScaleIndex[0] = 0;    // must not print this in DUMP
            break;
        }

        wsprintfW(insCurIns.wszScaleIndex, L"%s*4",
            awszRegCodes32[bIndex]);
        break;

    case 3:    // scale 8
        if (bIndex == 4)        // Nothing
        {
            insCurIns.wszScaleIndex[0] = 0;    // must not print this in DUMP
            break;
        }

        wsprintfW(insCurIns.wszScaleIndex, L"%s*8", awszRegCodes32[bIndex]);
        break;

    default:    // We generally wouldn't reach here.
        wprintf(L"StateSIB(): Invalid bIndex %d\n", bIndex);
        return FALSE;

    }// switch(bScale)

    // Determine base
    if (bBase == 5)
    {
        BYTE bMod;

        /* *****************************************************************************
         *    NOTE: Table 2-3 IASD Vol-2
         *    1. The [*] nomenclature(base=5) means a disp32 with no base if MOD is 00,
         *  [EBP] otherwise. This provides the following addressing modes:
         *    disp32[index] (MOD=00).
         *    disp8[EBP][index] (MOD=01).
         *    disp32[EBP][index] (MOD=10).
         * *****************************************************************************/

         // Get Mod field of ModRM byte: pByteInCode-2 because we have moved past
         // the SIB byte by now.
        Util_SplitModRMByte(*(pByteInCode - 2), &bMod, NULL, NULL);
        ASSERT(bMod >= 0 && bMod <= 2);    // ASSERT always??
        if (bMod == 0)
        {
            insCurIns.wszBase[0] = 0;
            insCurIns.bDispType = DISP_32BIT;
        }
        else
        {    // ebp is base; disp type has been set in MODRM already
            lstrcpyW(insCurIns.wszBase, awszRegCodes32[REGCODE_EBP]);
            //if(bMod == 1)
            //    insCurIns.bDispType = DISP_8BIT;
            //else
            //    insCurIns.bDispType = DISP_32BIT;
        }
        insCurIns.fDisp = TRUE;
        dsNextState = DASM_STATE_DISP;
    }
    else
    {
        // todo: return value
        lstrcpyW(insCurIns.wszBase, awszRegCodes32[bBase]);

        if (insCurIns.fDisp)
            // No need to determine size of disp here; done earlier.
            dsNextState = DASM_STATE_DISP;
        else if (insCurIns.fImm)
            // No need to determine size of imm here; done earlier.
            dsNextState = DASM_STATE_IMM;
        else
            dsNextState = DASM_STATE_DUMP;
    }

    return TRUE;

}// StateSIB()


/*
 * *************************
 *        Begin DISP state
 * *************************
 */

 /* StateDisp()
  * This state processes the Displacement byte in the instruction.
  *
  * Args:
  *
  * RetVal:
  *        TRUE/FALSE depending on whether there was an error or not.
  */
BOOL StateDisp()
{
    // pByteInCode must now be pointing to the disp byte
    // insCurIns.bDispType tells us the size of disp to read

    ASSERT(insCurIns.bDispType == DISP_8BIT
        || insCurIns.bDispType == DISP_32BIT);

    if (insCurIns.bDispType == DISP_8BIT)
    {
        insCurIns.iDisp = (char)*pByteInCode;
        ++pByteInCode;
        ++nBytesCurIns;
    }
    else if (insCurIns.bDispType == DISP_32BIT)
    {
        insCurIns.iDisp = (INT)*((INT*)pByteInCode);
        pByteInCode += sizeof(INT);
        nBytesCurIns += sizeof(INT);
    }

    if (insCurIns.fImm)
    {
        dsNextState = DASM_STATE_IMM;
#ifdef _DEBUG
        if (insCurIns.bImmType == IMM_UNDEF)
        {
            wprintf(L"StateDisp(): bImmType UNDEF\n");
            return FALSE;
        }
#endif
        return TRUE;
    }

    // No imm byte: no other byte to process for current instruction
    dsNextState = DASM_STATE_DUMP;
    return TRUE;

}// StateDisp()


/*
 * *************************
 *        Begin IMM state
 * *************************
 */

 /* StateImm()
  * This state processes the Immediate value byte in the instruction.
  *
  * Args:
  *
  * RetVal:
  *        TRUE/FALSE depending on whether there was an error or not.
  */
BOOL StateImm()
{
    // Read the bImmType size of data from pByteInCode

    if (insCurIns.fCodeOffset)
    {
        // Offset is added to the EIP, which is nothing but the 
        // value of pByteInCode after reading the offset value
        // because pByteInCode will then be pointing to the next
        // instruction.

        // Offset is a signed value

        // Store the offset value in insCurIns.iImm and let DUMP take care of 
        // calculating the actual address

        if (insCurIns.bImmType == IMM_8BIT)
        {
            // Do not write insCurIns.iImm = (INT)*pByteInCode : causes compiler to emit
            // a movzx instruction but we need a movsx really
            insCurIns.iImm = (INT)(CHAR)(*pByteInCode);
            ++pByteInCode;
            ++nBytesCurIns;
        }
        else if (insCurIns.bImmType == IMM_16BIT)
        {
            insCurIns.iImm = (SHORT)(*(SHORT*)pByteInCode);
            pByteInCode += sizeof(SHORT);
            nBytesCurIns += sizeof(SHORT);
        }
        else if (insCurIns.bImmType == IMM_32BIT)
        {
            insCurIns.iImm = (INT)(*(INT*)pByteInCode);
            pByteInCode += sizeof(INT);
            nBytesCurIns += sizeof(INT);
        }
        else
        {
            wprintf(L"StateImm(): Invalid code offset size %u\n", insCurIns.bImmType);
            return FALSE;
        }

        dsNextState = DASM_STATE_DUMP;
        return TRUE;
    }

    if (insCurIns.fDataOffset)
    {
        // moffs8/moffs16/moffs32

        // The data offset encoded as an immediate value can be either
        // 16/32 bits.
        if (insCurIns.bImmType == IMM_16BIT)
        {
            insCurIns.iImm = (SHORT)(*(SHORT*)pByteInCode);
            pByteInCode += sizeof(SHORT);
            nBytesCurIns += sizeof(SHORT);
        }
        else if (insCurIns.bImmType == IMM_32BIT)
        {
            insCurIns.iImm = (INT)(*(INT*)pByteInCode);
            pByteInCode += sizeof(INT);
            nBytesCurIns += sizeof(INT);
        }
        else
        {
            wprintf(L"StateImm(): Invalid data offset size %u\n", insCurIns.bImmType);
            return FALSE;
        }

        dsNextState = DASM_STATE_DUMP;
        return TRUE;
    }// if fDataOffset

    // Not a code/data offset, just an immediate value, which is signed(!).
    // Store into insCurIns.iImm and let DUMP take care of printing the value.
    if (insCurIns.bImmType == IMM_8BIT)
    {
        if (insCurIns.fImmSignEx)
            insCurIns.iImm = (INT)(CHAR)*pByteInCode;
        else
            insCurIns.iImm = (INT)*pByteInCode;
        ++pByteInCode;
        ++nBytesCurIns;
    }
    else if (insCurIns.bImmType == IMM_16BIT)
    {
        insCurIns.iImm = (SHORT)(*(SHORT*)pByteInCode);
        pByteInCode += sizeof(SHORT);
        nBytesCurIns += sizeof(SHORT);
    }
    else if (insCurIns.bImmType == IMM_32BIT)
    {
        insCurIns.iImm = (INT)(*(INT*)pByteInCode);
        pByteInCode += sizeof(INT);
        nBytesCurIns += sizeof(INT);
    }
    else
    {
        wprintf(L"StateImm(): Invalid bImmType %d\n", insCurIns.bImmType);
        return FALSE;
    }

    dsNextState = DASM_STATE_DUMP;

    return TRUE;
}// StateImm()


/*
 * *************************
 *        Begin DUMP state
 * *************************
 */

BOOL StateDumpOnOpcodeError()
{
    // 1: address
    wprintf(L"  %08X: ", (UINT)(pByteInCode - nBytesCurIns) + lDelta);
    wprintf(L"%02X\n", *(pByteInCode - nBytesCurIns));
    return TRUE;
}

BOOL StateDump(HWND ListView)
{
    CHAR Str[128];
    WCHAR wBuf[128];
    static WCHAR awszPrefixStr[][8] = { L"lock", L"repne", L"repe" };
    static CHAR aszPrefixStr[][8] = { "lock", "repne", "repe" };
    static INT aiPrefixStrNChars[] = { 4, 5, 4 };

    WCHAR *pwszSegStr = NULL;    // segment register, if any

    register INT nBytesPrinted = 0;
    INT nCharsMnemPrinted = 0;

    // Eg.: 0040106F: 68 AD DE 00 00        push        0DEADh

    // Eg.: 0040108F: 89 04 95 D0 60 D5 00    mov            dword ptr [edx*4+0D560D0h],eax

    // 1: address
    sprintf(Str, " %08X: ", (UINT)((pByteInCode - nBytesCurIns) + lDelta));

    InsertString(ListView, Str, lSize, 0);

    // 2: Bytes that make up the current instruction
    // Print the first 6 bytes on the current line; if there are more
    // bytes remaining, then print them on the next line, later.

    LPSTR ptr = Str;
    for (PBYTE pBytes = pByteInCode - nBytesCurIns;
        (pBytes < pByteInCode) && (nBytesPrinted < 6);
        ++pBytes, ++nBytesPrinted)
    {
        wprintf(L"%02X ", *pBytes);
         sprintf(ptr, "%02X ", *pBytes);
         ptr+=3;
    }
    
    InsertString(ListView, Str, lSize, 1);
    Str[0]='\0';
    // Padding if there are < 6 bytes in the current instruction
    // 3 space chars in each iteration because each byte printed
    // occupies 3spaces. Eg.: "88 8C 05 "
    for (INT nSpaces = nBytesCurIns; nSpaces < 6; ++nSpaces)
    {
        wprintf(L"   ");
    }

    /*
     * Keeping track of the number of characters as part of the
     * instruction mnemonic so that the correct amount of padding
     * can be printed and sp the operands are properly aligned
     * in every instruction. Instruction mnemonic includes prefixes
     * and the opcode mnemonic itself.
     */

     // 3: Print LOCK, REPx prefixes, if any
    if (insCurIns.wPrefixTypes & PREFIX_LOCKREP)
    {
        switch (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP])
        {
        case 0xf0:    // lock
            sprintf(Str, "%s ", aszPrefixStr[0]);
            nCharsMnemPrinted += aiPrefixStrNChars[0] + 1;    // +1 for a space char
            break;

        case 0xf2:    // repne
            if (insCurIns.fSSEIns)    // no prefix if SSE prefix
                break;
            sprintf(Str, "%s ", aszPrefixStr[1]);
            nCharsMnemPrinted += aiPrefixStrNChars[1] + 1;
            break;

        case 0xf3:    // repe
            if (insCurIns.fSSEIns)    // no prefix if SSE prefix
                break;
            sprintf(Str, "%s ", aszPrefixStr[2]);
            nCharsMnemPrinted += aiPrefixStrNChars[2] + 1;
            break;

        default:
            logdbg(L"StateDump(): Invalid prefix %d\n", insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP]);
            break;
        }// switch()

    }// if there is a lock/rep prefix

    // 4: Instruction mnemonic
        wprintf(L"%s", wszCurInsStr);
        wcstombs(Str+lstrlen(Str), wszCurInsStr, sizeof(Str));
    nCharsMnemPrinted += wcslen(wszCurInsStr);

    // Now print padding spaces if required
    for (register INT nPad = nCharsMnemPrinted; nPad <= NUM_CHARS_MNEMONIC; ++nPad)
    {
        wprintf(L" ");
        sprintf(Str+lstrlen(Str), " ");
    }

    // 5: Operands
    if (insCurIns.fModRM)
    {
        // Destination operand first
        switch (insCurIns.OprTypeDes)
        {
        case OPRTYPE_ERROR:
            // Instructions that do not have a ModRM/SIB byte would not
            // set the OprTypeDes and it will be OPRTYPE_ERROR(=0).
            // Or have a /digit ModRM byte, in which case it specifies only
            // one operand and that is in the src str.
            break;

        case OPRTYPE_REG:    // eax, ax, al, ...
            wprintf(L"%s", insCurIns.wszCurInsStrDes);
            wcstombs(Str+lstrlen(Str), insCurIns.wszCurInsStrDes, sizeof(Str));
            break;

        case OPRTYPE_MEM:    // dword ptr [eax], word ptr [ebx], ...
        {
            Dump_GetSegmentPrefix(insCurIns.abPrefixes[PREFIX_INDEX_SEG], &pwszSegStr);
             wprintf( L"%s%s[%s]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.wszCurInsStrDes);
            wsprintfW(wBuf, L"%s%s[%s]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.wszCurInsStrDes);
            wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
            break;
        }

        case OPRTYPE_MEM8:    // dword ptr [eax-4h], word ptr [ebx+10h], ...
        {
            Dump_GetSegmentPrefix(insCurIns.abPrefixes[PREFIX_INDEX_SEG], &pwszSegStr);
            if (insCurIns.iDisp >= 0)    // must include a '+'
            {
                wprintf(L"%s%s[%s+%02Xh]", insCurIns.pwszPtrStr, pwszSegStr,
                    insCurIns.wszCurInsStrDes, insCurIns.iDisp);
                 wsprintfW(wBuf,L"%s%s[%s+%02Xh]", insCurIns.pwszPtrStr, pwszSegStr,
                    insCurIns.wszCurInsStrDes, insCurIns.iDisp);
            wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
            }
            else
            {
                // Get the positive representation of the negative displacement and put a 
                // '-' sign preceding it while printing. This makes it easier to spot
                // positive and negative displacements (especially from ebp: diff b/w
                // locals and arguments).
                BYTE chDisp = (BYTE)insCurIns.iDisp;
                Util_TwosComplementByte(chDisp, &chDisp);        // same variable as two parameters I know!
                 wprintf( L"%s%s[%s-%02Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.wszCurInsStrDes, chDisp);
               wsprintfW(wBuf, L"%s%s[%s-%02Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.wszCurInsStrDes, chDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
            }
            break;
        }// case MEM8

        case OPRTYPE_MEM32:
        {
            Dump_GetSegmentPrefix(insCurIns.abPrefixes[PREFIX_INDEX_SEG], &pwszSegStr);

            // dumpbin appears to be doing it(2's compl thing) only for disp8, not disp32
            wprintf(L"%s%s[%s+%Xh]", insCurIns.pwszPtrStr, pwszSegStr,
                insCurIns.wszCurInsStrDes, insCurIns.iDisp);
            wsprintfW(wBuf, L"%s%s[%s+%Xh]", insCurIns.pwszPtrStr, pwszSegStr,
                insCurIns.wszCurInsStrDes, insCurIns.iDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
            break;
        }// case MEM32

        /* Note:
         * There may be a displacement when Mod=0; although MODRM state does not know this,
         * SIB state does and sets fDisp=TRUE. So yes, we can combine all SIB oprtypes.
         */
        case OPRTYPE_SIB8:
        {
            if (insCurIns.fDisp)
            {
                BYTE chDisp;

                if (insCurIns.wszBase[0] == 0)
                {
                    if (insCurIns.wszScaleIndex[0] == 0)    // only a disp
                    {
                        wprintf(L"%s[%02Xh]", insCurIns.pwszPtrStr, insCurIns.iDisp);
                        wsprintfW(wBuf, L"%s[%02Xh]", insCurIns.pwszPtrStr, insCurIns.iDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                    else    // scaled-index+disp
                    {
                        if (insCurIns.iDisp >= 0)
                        {
                            wprintf(L"%s[%s+%02Xh]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex, insCurIns.iDisp);
                           wsprintfW(wBuf, L"%s[%s+%02Xh]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex, insCurIns.iDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                        }
                        else
                        {
                            chDisp = (BYTE)insCurIns.iDisp;
                            Util_TwosComplementByte(chDisp, &chDisp);        // same variable as two parameters I know!
                            wprintf(L"%s[%s-%02Xh]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex, chDisp);
                            wsprintfW(wBuf, L"%s[%s-%02Xh]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex, chDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                        }
                    }
                }
                else
                {
                    if (insCurIns.wszScaleIndex[0] == 0)    // base + disp
                    {
                        if (insCurIns.iDisp >= 0)
                        {
                            wprintf(L"%s[%s+%02Xh]", insCurIns.pwszPtrStr, insCurIns.wszBase, insCurIns.iDisp);
                            wsprintfW(wBuf, L"%s[%s+%02Xh]", insCurIns.pwszPtrStr, insCurIns.wszBase, insCurIns.iDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                        }
                        else
                        {
                            chDisp = (BYTE)insCurIns.iDisp;
                            Util_TwosComplementByte(chDisp, &chDisp);        // same variable as two parameters I know!
                            wprintf(L"%s[%s-%02Xh]", insCurIns.pwszPtrStr, insCurIns.wszBase, chDisp);
                            wsprintfW(wBuf, L"%s[%s-%02Xh]", insCurIns.pwszPtrStr, insCurIns.wszBase, chDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                        }
                    }
                    else    // base+scaled-index+disp
                    {
                        if (insCurIns.iDisp >= 0)
                        {
                            wprintf(L"%s[%s+%s+%02Xh]", insCurIns.pwszPtrStr,
                                insCurIns.wszBase, insCurIns.wszScaleIndex, insCurIns.iDisp);
                            wsprintfW(wBuf, L"%s[%s+%s+%02Xh]", insCurIns.pwszPtrStr,
                                insCurIns.wszBase, insCurIns.wszScaleIndex, insCurIns.iDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                        }
                        else
                        {
                            chDisp = (BYTE)insCurIns.iDisp;
                            Util_TwosComplementByte(chDisp, &chDisp);        // same variable as two parameters I know!
                            wprintf(L"%s[%s+%s-%02Xh]", insCurIns.pwszPtrStr,
                                insCurIns.wszBase, insCurIns.wszScaleIndex, chDisp);
                            wsprintfW(wBuf, L"%s[%s+%s-%02Xh]", insCurIns.pwszPtrStr,
                                insCurIns.wszBase, insCurIns.wszScaleIndex, chDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                        }
                    }
                }// if base
            }
            else
            {
                if (insCurIns.wszBase[0] == 0)
                {
                    if (insCurIns.wszScaleIndex[0] == 0)    // todo: no scale, no index, no base!! error??
                    {
                        wprintf(L"StateDump(): SIB no base, no SI, no disp!!\n");
                        return FALSE;
                    }
                    else    // scaled-index
                    {
                        wprintf(L"%s[%s]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex);
                        wsprintfW(wBuf, L"%s[%s]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                }
                else
                {
                    if (insCurIns.wszScaleIndex[0] == 0)    // todo: only base??
                    {
                        wprintf(L"%s[%s]", insCurIns.pwszPtrStr, insCurIns.wszBase);
                       wsprintfW(wBuf, L"%s[%s]", insCurIns.pwszPtrStr, insCurIns.wszBase);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                    else    // base + scaled-index
                    {
                        wprintf(L"%s[%s+%s]", insCurIns.pwszPtrStr, insCurIns.wszBase, insCurIns.wszScaleIndex);
                        wsprintfW(wBuf, L"%s[%s+%s]", insCurIns.pwszPtrStr, insCurIns.wszBase, insCurIns.wszScaleIndex);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                }
            }// if fDisp

            break;
        }// SIB8

        case OPRTYPE_SIB:
        case OPRTYPE_SIB32:
        {
            if (insCurIns.fDisp)
            {
                if (insCurIns.wszBase[0] == 0)
                {
                    if (insCurIns.wszScaleIndex[0] == 0)    // only a disp
                    {
                        wprintf(L"%s[%Xh]", insCurIns.pwszPtrStr, insCurIns.iDisp);
                        wsprintfW(wBuf, L"%s[%Xh]", insCurIns.pwszPtrStr, insCurIns.iDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                    else    // scaled-index+disp
                    {
                        wprintf(L"%s[%s+%Xh]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex, insCurIns.iDisp);
                        wsprintfW(wBuf, L"%s[%s+%Xh]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex, insCurIns.iDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                }
                else
                {
                    if (insCurIns.wszScaleIndex[0] == 0)    // base + disp
                    {
                        wprintf(L"%s[%s+%Xh]", insCurIns.pwszPtrStr, insCurIns.wszBase, insCurIns.iDisp);
                        wsprintfW(wBuf, L"%s[%s+%Xh]", insCurIns.pwszPtrStr, insCurIns.wszBase, insCurIns.iDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                    else    // base+scaled-index+disp
                    {
                        wprintf(L"%s[%s+%s+%Xh]", insCurIns.pwszPtrStr,
                            insCurIns.wszBase, insCurIns.wszScaleIndex, insCurIns.iDisp);
                        wsprintfW(wBuf, L"%s[%s+%s+%Xh]", insCurIns.pwszPtrStr,
                            insCurIns.wszBase, insCurIns.wszScaleIndex, insCurIns.iDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                }
            }
            else
            {
                if (insCurIns.wszBase[0] == 0)
                {
                    if (insCurIns.wszScaleIndex[0] == 0)    // todo: no scale, no index, no base!! error??
                    {
                        wprintf(L"StateDump(): SIB no base, no SI, no disp!!\n");
                        return FALSE;
                    }
                    else    // scaled-index
                    {
                        wprintf(L"%s[%s]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex);
                         wsprintfW(wBuf, L"%s[%s]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex);
               wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                }
                else
                {
                    if (insCurIns.wszScaleIndex[0] == 0)    // todo: only base??
                    {
                        wprintf(L"%s[%s]", insCurIns.pwszPtrStr, insCurIns.wszBase);
                        wsprintfW(wBuf, L"%s[%s]", insCurIns.pwszPtrStr, insCurIns.wszBase);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                    else    // base + scaled-index
                    {
                        wprintf(L"%s[%s+%s]", insCurIns.pwszPtrStr, insCurIns.wszBase, insCurIns.wszScaleIndex);
                        wsprintfW(wBuf, L"%s[%s+%s]", insCurIns.pwszPtrStr, insCurIns.wszBase, insCurIns.wszScaleIndex);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                }
            }// if fDisp

            break;
        }// case SIB/SIB32

        case OPRTYPE_DISP32:    // ds:[disp32]    ds:[0040BD08h]
        {
            if (insCurIns.wPrefixTypes & PREFIX_SEG)
            {
                // We have a segment override prefix. Get it.
                Dump_GetSegmentPrefix(insCurIns.abPrefixes[PREFIX_INDEX_SEG], &pwszSegStr);

                wprintf(L"%s%s:[%08Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.iDisp);
                wsprintfW(wBuf, L"%s%s:[%08Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.iDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
            }
            else
            {
                // Default to "ds:" if there is no segment override prefix
                wprintf(L"%s%s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_DS], insCurIns.iDisp);
                wsprintfW(wBuf, L"%s%s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_DS], insCurIns.iDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
            }

            //switch(insCurIns.abPrefixes[PREFIX_INDEX_SEG])
            //{
            //    case OPC_PREFIX_SEGOVR_ES:
            //        wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_ES], insCurIns.iDisp);
            //        break;

            //    case OPC_PREFIX_SEGOVR_CS:
            //        wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_CS], insCurIns.iDisp);
            //        break;

            //    case OPC_PREFIX_SEGOVR_SS:
            //        wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_SS], insCurIns.iDisp);
            //        break;

            //    case OPC_PREFIX_SEGOVR_DS:
            //        wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_DS], insCurIns.iDisp);
            //        break;

            //    case OPC_PREFIX_SEGOVR_FS:
            //        wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_FS], insCurIns.iDisp);
            //        break;

            //    case OPC_PREFIX_SEGOVR_GS:
            //        wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_GS], insCurIns.iDisp);
            //        break;

            //    default:    // redundant but makes it easy to understand
            //        wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_DS], insCurIns.iDisp);
            //        break;
            //}
        }// case OPRTYPE_DISP32

        default:
            break;

        }// switch(OprTypeDes)

        // ',' between dest and src operands only if both are present
        if (insCurIns.fDesStrSet && insCurIns.fSrcStrSet)
        {
            wprintf(L",");
            sprintf(Str+lstrlen(Str), ", ");
        }

        // Now the source operand
        switch (insCurIns.OprTypeSrc)
        {
        case OPRTYPE_ERROR:
            // Instructions that do not have a ModRM/SIB byte would not
            // set the OprTypeDes and it will be OPRTYPE_ERROR(=0).
            break;

        case OPRTYPE_REG:    // eax, ax, al, ...
            wprintf(L"%s", insCurIns.wszCurInsStrSrc);
                wcstombs(Str+lstrlen(Str), insCurIns.wszCurInsStrSrc, sizeof(Str));
            break;

        case OPRTYPE_MEM:    // dword ptr [eax], word ptr [ebx], ...
        {
            Dump_GetSegmentPrefix(insCurIns.abPrefixes[PREFIX_INDEX_SEG], &pwszSegStr);
            wprintf(L"%s%s[%s]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.wszCurInsStrSrc);
            wsprintfW(wBuf, L"%s%s[%s]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.wszCurInsStrSrc);
             wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
             break;
        }

        case OPRTYPE_MEM8:    // dword ptr [eax-4h], word ptr [ebx+10h], ...
        {
            Dump_GetSegmentPrefix(insCurIns.abPrefixes[PREFIX_INDEX_SEG], &pwszSegStr);
            if (insCurIns.iDisp >= 0)    // must include a '+'
            {
                wprintf(L"%s%s[%s+%Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.wszCurInsStrSrc, insCurIns.iDisp);
                wsprintfW(wBuf, L"%s%s[%s+%Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.wszCurInsStrSrc, insCurIns.iDisp);
             wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
            }
            else
            {
                // Get the positive representation of the negative displacement and put a 
                // '-' sign preceding it while printing. This makes it easier to spot
                // positive and negative displacements (especially from ebp: diff b/w
                // locals and arguments).
                BYTE chDisp = (BYTE)insCurIns.iDisp;
                Util_TwosComplementByte(chDisp, &chDisp);        // same variable as two parameters I know!
                wprintf(L"%s%s[%s-%Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.wszCurInsStrSrc, chDisp);
                wsprintfW(wBuf, L"%s%s[%s-%Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.wszCurInsStrSrc, chDisp);
             wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
            }
            break;
        }// case MEM8

        case OPRTYPE_MEM32:
        {
            Dump_GetSegmentPrefix(insCurIns.abPrefixes[PREFIX_INDEX_SEG], &pwszSegStr);

            // dumpbin appears to be doing it(2's compl thing) only for disp8, not disp32
            wprintf(L"%s%s[%s+%Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.wszCurInsStrSrc, insCurIns.iDisp);
            wsprintfW(wBuf, L"%s%s[%s+%Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.wszCurInsStrSrc, insCurIns.iDisp);
             wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
            break;
        }// case MEM32

        case OPRTYPE_SIB:
        case OPRTYPE_SIB8:
        case OPRTYPE_SIB32:
        {
            if (insCurIns.fDisp)
            {
                if (insCurIns.wszBase[0] == 0)
                {
                    if (insCurIns.wszScaleIndex[0] == 0)    // only a disp
                    {
                        wprintf(L"%s[%Xh]", insCurIns.pwszPtrStr, insCurIns.iDisp);
                        wsprintfW(wBuf, L"%s[%Xh]", insCurIns.pwszPtrStr, insCurIns.iDisp);
                        wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                    else    // scaled-index+disp
                    {
                        wprintf(L"%s[%s+%Xh]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex, insCurIns.iDisp);
                        wsprintfW(wBuf, L"%s[%s+%Xh]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex, insCurIns.iDisp);
                        wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                }
                else
                {
                    if (insCurIns.wszScaleIndex[0] == 0)    // base + disp
                    {
                        wprintf(L"%s[%s+%Xh]", insCurIns.pwszPtrStr, insCurIns.wszBase, insCurIns.iDisp);
                        wsprintfW(wBuf, L"%s[%s+%Xh]", insCurIns.pwszPtrStr, insCurIns.wszBase, insCurIns.iDisp);
                        wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                    else    // base+scaled-index+disp
                    {
                        wprintf(L"%s[%s+%s+%Xh]", insCurIns.pwszPtrStr,
                                  insCurIns.wszBase, insCurIns.wszScaleIndex, insCurIns.iDisp);
                        wsprintfW(wBuf, L"%s[%s+%s+%Xh]", insCurIns.pwszPtrStr,
                                  insCurIns.wszBase, insCurIns.wszScaleIndex, insCurIns.iDisp);
                        wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                }
            }
            else
            {
                if (insCurIns.wszBase[0] == 0)
                {
                    if (insCurIns.wszScaleIndex[0] == 0)    // todo: no scale, no index, no base!! error??
                    {
                        wprintf(L"StateDump(): SIB no base, no SI, no disp!!\n");
                        return FALSE;
                    }
                    else    // scaled-index
                    {
                        wprintf(L"%s[%s]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex);
                        wsprintfW(wBuf, L"%s[%s]", insCurIns.pwszPtrStr, insCurIns.wszScaleIndex);
                        wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                }
                else
                {
                    if (insCurIns.wszScaleIndex[0] == 0)    // todo: only base??
                    {
                        wprintf(L"%s[%s]", insCurIns.pwszPtrStr, insCurIns.wszBase);
                        wsprintfW(wBuf, L"%s[%s]", insCurIns.pwszPtrStr, insCurIns.wszBase);
                        wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                    else    // base + scaled-index
                    {
                        wprintf(L"%s[%s+%s]", insCurIns.pwszPtrStr, insCurIns.wszBase, insCurIns.wszScaleIndex);
                        wsprintfW(wBuf, L"%s[%s+%s]", insCurIns.pwszPtrStr, insCurIns.wszBase, insCurIns.wszScaleIndex);
                        wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
                    }
                }
            }// if fDisp

            break;
        }// case SIB8/32

        case OPRTYPE_DISP32:    // ds:[disp32]    ds:[0040BD08h]
        {
            if (insCurIns.wPrefixTypes & PREFIX_SEG)
            {
                // We have a segment override prefix. Get it.
                Dump_GetSegmentPrefix(insCurIns.abPrefixes[PREFIX_INDEX_SEG], &pwszSegStr);

                wprintf(L"%s%s:[%08Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.iDisp);
                wsprintfW(wBuf, L"%s%s:[%08Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.iDisp);
                 wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
            }
            else
            {
                // Default to "ds:" if there is no segment override prefix
                wprintf(L"%s%s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_DS], insCurIns.iDisp);
                wsprintfW(wBuf, L"%s%s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_DS], insCurIns.iDisp);
                wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
            }

            //    switch(insCurIns.abPrefixes[PREFIX_INDEX_SEG])
            //    {
            //        case OPC_PREFIX_SEGOVR_ES:
            //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_ES], insCurIns.iDisp);
            //            break;

            //        case OPC_PREFIX_SEGOVR_CS:
            //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_CS], insCurIns.iDisp);
            //            break;

            //        case OPC_PREFIX_SEGOVR_SS:
            //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_SS], insCurIns.iDisp);
            //            break;

            //        case OPC_PREFIX_SEGOVR_DS:
            //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_DS], insCurIns.iDisp);
            //            break;

            //        case OPC_PREFIX_SEGOVR_FS:
            //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_FS], insCurIns.iDisp);
            //            break;

            //        case OPC_PREFIX_SEGOVR_GS:
            //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_GS], insCurIns.iDisp);
            //            break;

            //        default:    // redundant but makes it easy to understand
            //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, awszSRegCodes[SREGCODE_DS], insCurIns.iDisp);
            //            break;
            //    }
        }// case OPRTYPE_DISP32

        default:
            break;

        }// switch(OprTypeDes)

    }
    else
    {
        // There was no ModRM byte to specify the operand(s).
        // Options:
        // - Operand(s) are determined in OPCODE state
        // - Data offset is present (src/des)
        // - Imm value is src
        // - Code offset is src

        if (insCurIns.fDesStrSet)
        {
            wprintf(L"%s", insCurIns.wszCurInsStrDes);
                wcstombs(Str+lstrlen(Str), insCurIns.wszCurInsStrDes, sizeof(Str));
        }
        else if (insCurIns.fDataOffset && insCurIns.fSrcStrSet)
        {
            // des not determined earlier but src has been determined
            // and there is a data offset. So the data offset must be
            // the des operand
            Dump_DataOffset();
            insCurIns.fDesStrSet = TRUE;    // we have a des operand: print ',' later
        }

        if (insCurIns.fSrcStrSet)
        {
            // ',' between dest and src operands only if both are present
            if (insCurIns.fDesStrSet)
            {
                 wprintf(L",");
                 sprintf(Str+lstrlen(Str),", ");
            }
            wprintf(L"%s", insCurIns.wszCurInsStrSrc);
                wcstombs(Str+lstrlen(Str), insCurIns.wszCurInsStrSrc, sizeof(Str));
        }
        else if (insCurIns.fDataOffset)
        {
            // data offset must be the src operand
            if (insCurIns.fDesStrSet)
             {
                    wprintf(L",");
                    sprintf(Str+lstrlen(Str),", ");
             }
            Dump_DataOffset();
            insCurIns.fSrcStrSet = TRUE;    // we have a src operand: print ',' later
        }

    }// if fModRM

    // Now the third operand
    if (insCurIns.fOpr3StrSet)
    {
        wprintf(L",%s", insCurIns.wszCurInsStrOpr3);
        wcstombs(Str+lstrlen(Str), insCurIns.wszCurInsStrOpr3, sizeof(Str));
    }

    // 6: Immediate value: imm or code or data offset
    if (insCurIns.fImm)
    {                                    //  ud   8bit       16bit     32bit     48bit
        static WCHAR awszFormatSpec[][6] = { L"", L"%02Xh", L"%04Xh", L"%08Xh", L"%Xh" };

        // todo: ASSERT in Release mode too??
        ASSERT(insCurIns.bImmType >= IMM_UNDEF && insCurIns.bImmType <= IMM_48BIT);    // Array bounds checking

        // Preceding comma(',') if there was a prior operand
        if (insCurIns.fDesStrSet || insCurIns.fSrcStrSet)    // checking src also because /digit type
        {
            wprintf(L",");   sprintf(Str+lstrlen(Str),", ");                             // sets oprstr in src
        }

        wprintf(awszFormatSpec[insCurIns.bImmType], insCurIns.iImm);
    }
    else if (insCurIns.fCodeOffset)
    {
        // Preceding comma(',') if there was a prior operand
        if (insCurIns.fDesStrSet || insCurIns.fSrcStrSet)    // checking src also because /digit type
        {
            wprintf(L",");   sprintf(Str+lstrlen(Str),", ");                             // sets oprstr in src
        }

        LONG lTemp = lDelta + (LONG)pByteInCode;

        // 32bit address always; so %08X
        wprintf(L"%08X", insCurIns.iImm + lTemp);
        wsprintfW(wBuf, L"%08X", insCurIns.iImm + lTemp);
        wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
    }
    //else if(insCurIns.fDataOffset)
    //{
    //    if(insCurIns.fSrcStrSet || insCurIns.fDesStrSet)    // checking both because A3 C4 ... MOV eax,
    //        wprintf(L",");

    //    /*
    //     * By default, the offset is in the ds segment but this maybe
    //     * changed using any one of the segment override prefixes.
    //     * Sample string:
    //     *        dword ptr ds:[0040B000h]
    //     *        word ptr ss:[0040B000h]
    //     */
    //     if( ! (insCurIns.wPrefixTypes & PREFIX_SEG) )    // ! higher precedence than &
    //     {
    //        // No segment override prefix present, default to ds
    //        if(insCurIns.bImmType == IMM_16BIT)
    //            wprintf(L"%s ds:[%04Xh]", insCurIns.pwszPtrStr, insCurIns.iImm);
    //        else
    //            wprintf(L"%s ds:[%08Xh]", insCurIns.pwszPtrStr, insCurIns.iImm);
    //     }
    //     else
    //     {
    //        // There is a segment override prefix. Print the correct one
    //        // by reading the stored prefix type in abPrefixes.
    //        switch(insCurIns.abPrefixes[PREFIX_INDEX_SEG])
    //        {
    //            case OPC_PREFIX_SEGOVR_ES:
    //                if(insCurIns.bImmType == IMM_16BIT)
    //                    wprintf(L"%s %s:[%04Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_ES], insCurIns.iImm);
    //                else
    //                    wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_ES], insCurIns.iImm);
    //                break;

    //            case OPC_PREFIX_SEGOVR_CS:
    //                if(insCurIns.bImmType == IMM_16BIT)
    //                    wprintf(L"%s %s:[%04Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_CS], insCurIns.iImm);
    //                else
    //                    wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_CS], insCurIns.iImm);
    //                break;

    //            case OPC_PREFIX_SEGOVR_SS:
    //                if(insCurIns.bImmType == IMM_16BIT)
    //                    wprintf(L"%s %s:[%04Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_SS], insCurIns.iImm);
    //                else
    //                    wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_SS], insCurIns.iImm);
    //                break;

    //            case OPC_PREFIX_SEGOVR_DS:
    //                if(insCurIns.bImmType == IMM_16BIT)
    //                    wprintf(L"%s %s:[%04Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_DS], insCurIns.iImm);
    //                else
    //                    wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_DS], insCurIns.iImm);
    //                break;
    //            
    //            case OPC_PREFIX_SEGOVR_FS:
    //                if(insCurIns.bImmType == IMM_16BIT)
    //                    wprintf(L"%s %s:[%04Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_FS], insCurIns.iImm);
    //                else
    //                    wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_FS], insCurIns.iImm);
    //                break;
    //            
    //            case OPC_PREFIX_SEGOVR_GS:
    //                if(insCurIns.bImmType == IMM_16BIT)
    //                    wprintf(L"%s %s:[%04Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_GS], insCurIns.iImm);
    //                else
    //                    wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_GS], insCurIns.iImm);
    //                break;

    //            default:    // Not a critical error
    //                wprintf(L"StateDump(): Invalid segovr prefix %xh\n", insCurIns.abPrefixes[PREFIX_INDEX_SEG]);
    //                if(insCurIns.bImmType == IMM_16BIT)
    //                    wprintf(L"%s <>:[%04Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_ES], insCurIns.iImm);
    //                else
    //                    wprintf(L"%s <>:[%08Xh]", insCurIns.pwszPtrStr, 
    //                                awszSRegCodes[SREGCODE_ES], insCurIns.iImm);
    //                break;
    //        }// switch(abPrefixes)
    //     
    //     }// if PREFIX_SEG
    //}// if fDataOffset

    // 7: Go to next line
    wprintf(L"\n");

    // 8: Check if there are more bytes of the current instruction
    // to print.
    if (nBytesCurIns > 6)
    {
        // In the earlier print loop, nBytesPrinted is incremented after 
        // printing and before checking termination condition, so 
        // its value maybe greater than how many bytes were actually printed
        nBytesPrinted = 6;

        // Padding for address field
        wprintf(L"            ");    // 12 spaces

        // Start from where we left off earlier
        register int nNewPrinted = 0;
        for (PBYTE pBytes = pByteInCode - nBytesCurIns + nBytesPrinted;
            pBytes < pByteInCode && nNewPrinted < nBytesCurIns;
            ++pBytes, ++nNewPrinted)
        {
            wprintf(L"%02X ", *pBytes);
            wsprintfW(wBuf, L"%02X ", *pBytes);
        wcstombs(Str+lstrlen(Str), wBuf, sizeof(Str));
        }
        wprintf(L"\n");
    }

    InsertString(ListView, Str, lSize, 2);

    lSize++;
    dsNextState = DASM_STATE_RESET;
    return TRUE;

}

void Dump_DataOffset()
{
    //if(insCurIns.fDesStrSet)
    //    wprintf(L",");

    /*
     * By default, the offset is in the ds segment but this maybe
     * changed using any one of the segment override prefixes.
     * Sample string:
     *        dword ptr ds:[0040B000h]
     *        word ptr ss:[0040B000h]
     */
    if (!(insCurIns.wPrefixTypes & PREFIX_SEG))    // ! higher precedence than &
    {
        // No segment override prefix present, default to ds
        if (insCurIns.bImmType == IMM_16BIT)
            wprintf(L"%sds:[%04Xh]", insCurIns.pwszPtrStr, insCurIns.iImm);
        else
            wprintf(L"%sds:[%08Xh]", insCurIns.pwszPtrStr, insCurIns.iImm);
    }
    else
    {
        // There is a segment override prefix. Print the correct one
        // by reading the stored prefix type in abPrefixes.

        WCHAR *pwszSegStr = NULL;

        Dump_GetSegmentPrefix(insCurIns.abPrefixes[PREFIX_INDEX_SEG], &pwszSegStr);
        if (insCurIns.bImmType == IMM_16BIT)
            wprintf(L"%s%s:[%04Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.iImm);
        else
            wprintf(L"%s%s:[%08Xh]", insCurIns.pwszPtrStr, pwszSegStr, insCurIns.iImm);

        //switch(insCurIns.abPrefixes[PREFIX_INDEX_SEG])
        //{
        //    case OPC_PREFIX_SEGOVR_ES:
        //        if(insCurIns.bImmType == IMM_16BIT)
        //            wprintf(L"%s %s:[%04Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_ES], insCurIns.iImm);
        //        else
        //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_ES], insCurIns.iImm);
        //        break;

        //    case OPC_PREFIX_SEGOVR_CS:
        //        if(insCurIns.bImmType == IMM_16BIT)
        //            wprintf(L"%s %s:[%04Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_CS], insCurIns.iImm);
        //        else
        //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_CS], insCurIns.iImm);
        //        break;

        //    case OPC_PREFIX_SEGOVR_SS:
        //        if(insCurIns.bImmType == IMM_16BIT)
        //            wprintf(L"%s %s:[%04Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_SS], insCurIns.iImm);
        //        else
        //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_SS], insCurIns.iImm);
        //        break;

        //    case OPC_PREFIX_SEGOVR_DS:
        //        if(insCurIns.bImmType == IMM_16BIT)
        //            wprintf(L"%s %s:[%04Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_DS], insCurIns.iImm);
        //        else
        //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_DS], insCurIns.iImm);
        //        break;
        //        
        //    case OPC_PREFIX_SEGOVR_FS:
        //        if(insCurIns.bImmType == IMM_16BIT)
        //            wprintf(L"%s %s:[%04Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_FS], insCurIns.iImm);
        //        else
        //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_FS], insCurIns.iImm);
        //        break;
        //        
        //    case OPC_PREFIX_SEGOVR_GS:
        //        if(insCurIns.bImmType == IMM_16BIT)
        //            wprintf(L"%s %s:[%04Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_GS], insCurIns.iImm);
        //        else
        //            wprintf(L"%s %s:[%08Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_GS], insCurIns.iImm);
        //        break;

        //    default:    // Not a critical error
        //        wprintf(L"StateDump(): Invalid segovr prefix %xh\n", insCurIns.abPrefixes[PREFIX_INDEX_SEG]);
        //        if(insCurIns.bImmType == IMM_16BIT)
        //            wprintf(L"%s <>:[%04Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_ES], insCurIns.iImm);
        //        else
        //            wprintf(L"%s <>:[%08Xh]", insCurIns.pwszPtrStr, 
        //                        awszSRegCodes[SREGCODE_ES], insCurIns.iImm);
        //        break;
        //}// switch(abPrefixes)

    }// if PREFIX_SEG
    return;
}

// Dump_GetSegmentPrefix()
// Returns the segment register string given the segment override 
// prefix value as parameter. If there is no segment override 
// prefix/invalid is sent as the parameter, an empty string is returned.
void Dump_GetSegmentPrefix(BYTE bSegPrefixVal, __out WCHAR **ppwszPrefixStr)
{
    static WCHAR awszSegRegs[][4] = { L"", L"es:", L"cs:", L"ss:", L"ds:", L"fs:", L"gs:" };

    switch (bSegPrefixVal)
    {
    case OPC_PREFIX_SEGOVR_ES: *ppwszPrefixStr = awszSegRegs[1]; break;
    case OPC_PREFIX_SEGOVR_CS: *ppwszPrefixStr = awszSegRegs[2]; break;
    case OPC_PREFIX_SEGOVR_SS: *ppwszPrefixStr = awszSegRegs[3]; break;
    case OPC_PREFIX_SEGOVR_DS: *ppwszPrefixStr = awszSegRegs[4]; break;
    case OPC_PREFIX_SEGOVR_FS: *ppwszPrefixStr = awszSegRegs[5]; break;
    case OPC_PREFIX_SEGOVR_GS: *ppwszPrefixStr = awszSegRegs[6]; break;
    default: *ppwszPrefixStr = awszSegRegs[0]; break;
    }// switch(abPrefixes[])
    return;
}

/*
 * *************************
 *    Begin Opcode handlers
 * *************************
 */

 /*
  *    ** PUSH **
  * FF /6    PUSH r/m16     Push r/m16
  * FF /6    PUSH r/m32     Push r/m32
  * 50+rw    PUSH r16       Push r16
  * 50+rd    PUSH r32       Push r32
  * 6A        PUSH imm8     Push imm8
  * 68        PUSH imm16    Push imm16
  * 68        PUSH imm32    Push imm32
  * 0E        PUSH CS       Push CS
  * 16        PUSH SS       Push SS
  * 1E        PUSH DS       Push DS
  * 06        PUSH ES       Push ES
  * 0F A0    PUSH FS        Push FS    // 2byte opcode
  * 0F A8    PUSH GS        Push GS    // 2byte opcode
  */
BOOL OPCHndlrStack_PUSH(BYTE bOpcode)
{
    logdbg(L"%s(): %xh", __FUNCTIONW__, bOpcode);

    // pByteInCode is now pointing to the byte following the opcode

    if (!(bOpcode >= 0x50 && bOpcode <= 0x57))
    {
        switch (bOpcode)
        {
        case 0xff:    // opsize attrib can be used and w-bit=1 always
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            dsNextState = DASM_STATE_MOD_RM;
            break;
        }

        case 0x6a:    // PUSH imm8
        {
            // size of immediate value is 8bits
            insCurIns.fImm = TRUE;
            insCurIns.bImmType = IMM_8BIT;
            dsNextState = DASM_STATE_IMM;
            break;
        }

        case 0x68:    // PUSH imm16 / PUSH imm32
        {
            // Size of immediate value is 16/32bits depending on the
            // presence of the operand size override prefix
            insCurIns.fImm = TRUE;
            insCurIns.bImmType = ImmTypeFromOperandSize();
            dsNextState = DASM_STATE_IMM;
            break;
        }

        case 0x0e:    // PUSH CS
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"cs");
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
            break;
        }

        case 0x16:    // PUSH SS
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"ss");
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
            break;
        }

        case 0x1e:    // PUSH DS
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"ds");
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
            break;
        }

        case 0x06:    // PUSH ES
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"es");
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
            break;
        }

        case 0xa0:    // 2byte opcode. 0fh has already been read. PUSH FS
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"fs");
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
            break;
        }

        case 0xa8:    // 2byte opcode. 0fh has already been read. PUSH GS
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"gs");
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
            break;
        }

        // No default statement required.
        // If opcode is not recognized at all, then
        // fRecognized will be FALSE and this function
        // will return FALSE.
        default:
        {
            wprintf(L"OPCHndlrStack_PUSH(): Invalid opcode %xh\n", bOpcode);
            return FALSE;
        }

        }// switch(bOpcode)
    }
    else {
        // Opcode is between 50h to 57h: PUSH reg
        // Get the register name
        WORD wReg = bOpcode - 0x50;
        lstrcpyW(insCurIns.wszCurInsStrSrc, RegStrFromOperandSize(wReg));
        insCurIns.fSrcStrSet = TRUE;

        // Next state is DUMP because there are no more 
        // bytes to process for the current instruction.

        dsNextState = DASM_STATE_DUMP;

    }// if-else

    return TRUE;
}

/*    ** POP **
 * 8F /0    POP m16
 * 8F /0    POP m32
 * 58+ rw    POP r16
 * 58+ rd    POP r32
 * 1F        POP DS
 * 07        POP ES
 * 17        POP SS
 * 0F A1    POP FS    // 2byte opcodes
 * 0F A9    POP GS
 */
BOOL OPCHndlrStack_POP(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // POP opcodes
    if (!(bOpcode >= 0x58 && bOpcode <= 0x5f))
    {
        switch (bOpcode)
        {
        case 0x8f: // POP m32 / POP m16    // opsize attrib can be used, w=1 always
        {
            // there is a ModRM byte
            insCurIns.fModRM = TRUE;
            dsNextState = DASM_STATE_MOD_RM;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            break;
        }

        case 0x1f: // POP DS
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"ds");
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
            break;
        }

        case 0x07: // POP ES
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"es");
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
            break;
        }

        case 0x17: // POP SS
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"ss");
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
            break;
        }

        case 0xa1: // 2byte opcodes: 0fh has already been read. POP FS
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"fs");
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
            break;
        }

        case 0xa9: // 2byte opcodes: 0fh has already been read. POP GS
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"gs");
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
            break;
        }

        default:
        {
            wprintf(L"OPCHndlrStack_POP(): Invalid opcode %xh\n", bOpcode);
            return FALSE;
        }

        }// switch(bOpcode)
    }
    else {
        // Opcode is between 58h to 5fh: POP reg

        // Get the register name index
        WORD wReg = bOpcode - 0x58;
        lstrcpyW(insCurIns.wszCurInsStrSrc, RegStrFromOperandSize(wReg));
        insCurIns.fSrcStrSet = TRUE;

        // Next state is DUMP because there are no more
        // bytes to process for the current instruction.
        dsNextState = DASM_STATE_DUMP;

    }// if ! bOpcode >= 0x58 && bOpcode <= 0x5f

    return TRUE;
}

/*    ** PUSHxx **
 * 60        PUSHA/PUSHAD
 * 9c        PUSHF/PUSHFD
 */
BOOL OPCHndlrStack_PUSHxx(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // If operand size prefix is present, then 16bit mnemonic is used.
    // Remember that 32bit mnemonic is already written in StateOpcode().
    if (bOpcode == 0x60) {
        if (insCurIns.wPrefixTypes & PREFIX_OPSIZE) {
            wsprintfW(wszCurInsStr, L"pusha");
        }
    }
    else if (bOpcode == 0x9c) {
        if (insCurIns.wPrefixTypes & PREFIX_OPSIZE) {
            wsprintfW(wszCurInsStr, L"pushf");
        }
    }
    else {
        wprintf(L"OPCHndlrStack_PUSHxx(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** POPxx **
 * 61    POPA/POPAD
 * 9D    POPF/POPFD
 */
BOOL OPCHndlrStack_POPxx(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // If operand size prefix is present,
    // then 16bit mnemonic is used.
    // Remember that 32bit mnemonic is already written
    // in StateOpcode()
    if (bOpcode == 0x61)
    {
        if (insCurIns.wPrefixTypes & PREFIX_OPSIZE) {
            wsprintfW(wszCurInsStr, L"popa");
        }
    }
    else if (bOpcode == 0x9d)
    {
        if (insCurIns.wPrefixTypes & PREFIX_OPSIZE) {
            wsprintfW(wszCurInsStr, L"popf");
        }
    }
    else
    {
        wprintf(L"OPCHndlrStack_POPxx(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*
 *        ** ENTER **
 * C8 iw 00        ENTER imm16,0
 * C8 iw 01        ENTER imm16,1
 * C8 iw ib        ENTER imm16,imm8
 */
BOOL OPCHndlrStack_ENTER(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Because there are two immediate values in this instruction
    // we will handle it here itself and not in IMM state

    // pByteInCode is pointing to the byte following opcode,
    // that is the first imm16 value

    WORD wImm16;
    BYTE bImmSecond;

    wImm16 = *((WORD*)pByteInCode);
    pByteInCode += 2;

    bImmSecond = *pByteInCode;
    ++pByteInCode;

    nBytesCurIns += sizeof(WORD) + sizeof(BYTE);    // imm16 and 0/1/imm8

    // imm16 into des str and imm8 into src str
    wsprintfW(insCurIns.wszCurInsStrDes, L"%04Xh", wImm16);
    wsprintfW(insCurIns.wszCurInsStrSrc, L"%02Xh", bImmSecond);
    insCurIns.fSrcStrSet = TRUE;
    insCurIns.fDesStrSet = TRUE;

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*
 * C9 LEAVE
 * C9 LEAVE
 */
BOOL OPCHndlrStack_LEAVE(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

// Arithmetic instruction handlers

/*        ** ADD/ADC **
 *
 * 04 ib        ADD AL,imm8
 * 05 iw        ADD AX,imm16
 * 05 id        ADD EAX,imm32
 * 80 /0 ib     ADD r/m8,imm8
 * 81 /0 iw     ADD r/m16,imm16
 * 81 /0 id     ADD r/m32,imm32
 * 83 /0 ib     ADD r/m16,imm8        with sign-extension
 * 83 /0 ib     ADD r/m32,imm8        with sign-extension
 * 00 /r        ADD r/m8,r8
 * 01 /r        ADD r/m16,r16
 * 01 /r        ADD r/m32,r32
 * 02 /r        ADD r8,r/m8
 * 03 /r        ADD r16,r/m16
 * 03 /r        ADD r32,r/m32
 *
 * 14 ib        ADC AL,imm8
 * 15 iw        ADC AX,imm16
 * 15 id        ADC EAX,imm32
 * 80 /2 ib     ADC r/m8,imm8
 * 81 /2 iw     ADC r/m16,imm16
 * 81 /2 id     ADC r/m32,imm32
 * 83 /2 ib     ADC r/m16,imm8 Add with CF sign-extended imm8 to r/m16
 * 83 /2 ib     ADC r/m32,imm8 Add with CF sign-extended imm8 into r/m32
 * 10 /r        ADC r/m8,r8
 * 11 /r        ADC r/m16,r16
 * 11 /r        ADC r/m32,r32
 * 12 /r        ADC r8,r/m8
 * 13 /r        ADC r16,r/m16
 * 13 /r        ADC r32,r/m32
 */
BOOL OPCHndlrALU_ADD(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    /*
     * 80 - 83 is part of Multi instruction opcodes and is first handled
     * in OPCHndlrMulti_8x() and then this function is called.
     * So we can be sure that bOpcode is an ADD instruction only and
     * we don't have to check the reg field of the ModRM byte.
     */

    switch (bOpcode)
    {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x04:
    case 0x05:
    case 0x14:
    case 0x15:
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = ImmTypeFromWbitOperandSize();

        // Construct the output string
        lstrcpyW(insCurIns.wszCurInsStrDes, RegStrFromWbitOperandSize(REGCODE_AX));
        insCurIns.fDesStrSet = TRUE;

        dsNextState = DASM_STATE_IMM;
        break;

    case 0x80:
    case 0x81:
        // wbit and opsize attrib used to set bImmType in MODRM
        insCurIns.fImm = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x83:    // with sign-extension
        insCurIns.bImmType = IMM_8BIT;
        insCurIns.fImm = TRUE;
        insCurIns.fImmSignEx = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    default:
        wprintf(L"OPCHndlrALU_ADD(): Invalid opcode: %x\n", bOpcode);
        return FALSE;

    }// switch(bOpcode)
    return TRUE;
}

/*        ** SUB/SBB **
 * 1C ib    SBB AL,imm8
 * 1D iw    SBB AX,imm16
 * 1D id    SBB EAX,imm32
 *
 * 80 /3 ib     SBB r/m8,imm8
 * 81 /3 iw     SBB r/m16,imm16
 * 81 /3 id     SBB r/m32,imm32
 * 83 /3 ib     SBB r/m16,imm8        Subtract with borrow sign-extended imm8 from r/m16
 * 83 /3 ib     SBB r/m32,imm8        Subtract with borrow sign-extended imm8 from r/m32
 *
 * 18 /r    SBB r/m8,r8
 * 19 /r    SBB r/m16,r16
 * 19 /r    SBB r/m32,r32
 * 1A /r    SBB r8,r/m8
 * 1B /r    SBB r16,r/m16
 * 1B /r    SBB r32,r/m32
 *
 * 2C ib SUB    AL,imm8
 * 2D iw SUB    AX,imm16
 * 2D id SUB    EAX,imm32
 *
 * 80 /5 ib     SUB r/m8,imm8
 * 81 /5 iw     SUB r/m16,imm16
 * 81 /5 id     SUB r/m32,imm32
 * 83 /5 ib     SUB r/m16,imm8        Subtract sign-extended imm8 from r/m16
 * 83 /5 ib     SUB r/m32,imm8        Subtract sign-extended imm8 from r/m32
 *
 * 28 /r    SUB r/m8,r8
 * 29 /r    SUB r/m16,r16
 * 29 /r    SUB r/m32,r32
 * 2A /r    SUB r8,r/m8
 * 2B /r    SUB r16,r/m16
 * 2B /r    SUB r32,r/m32
 *
 */
BOOL OPCHndlrALU_SUB(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    switch (bOpcode)
    {
    case 0x1c:
    case 0x1d:
    case 0x2c:
    case 0x2d:
    {
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = ImmTypeFromWbitOperandSize();
        lstrcpyW(insCurIns.wszCurInsStrDes, RegStrFromWbitOperandSize(REGCODE_AX));
        insCurIns.fDesStrSet = TRUE;
        dsNextState = DASM_STATE_IMM;
        break;
    }

    case 0x80:    // wbit and opsize attrib can be used to determine size of operands
    case 0x81:
        insCurIns.fImm = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x83:    // imm8 always src with sign-extension
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        insCurIns.fImmSignEx = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x18:    // wbit and opsize attrib can be used
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x28:
    case 0x29:
    case 0x2a:
    case 0x2b:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    }// switch(bOpcode)
    return TRUE;
}

/*    ** MUL/IMUL **
 * F6 /5        IMUL r/m8
 * F7 /5        IMUL r/m16
 * F7 /5        IMUL r/m32
 * 0F AF /r     IMUL r16,r/m16
 * 0F AF /r     IMUL r32,r/m32
 * 6B /r ib     IMUL r16,r/m16,imm8        word register = r/m16 * sign-extended immediate byte
 * 6B /r ib     IMUL r32,r/m32,imm8        doubleword register = r/m32 * sign-extended immediate byte
 * 6B /r ib     IMUL r16,imm8            word register =  word register * sign-extended immediate byte
 * 6B /r ib     IMUL r32,imm8            doubleword register =  doubleword register * signextended immediate byte
 * 69 /r iw     IMUL r16,r/m16,imm16    word register = r/m16 * immediate word
 * 69 /r id     IMUL r32,r/m32,imm32    doubleword register = r/m32 * immediate doubleword
 * 69 /r iw     IMUL r16,imm16            word register = r/m16 * immediate word
 * 69 /r id     IMUL r32,imm32            doubleword register = r/m32 * immediate doubleword
 *
 * F6 /4    MUL r/m8    Unsigned multiply (AX = AL * r/m8)
 * F7 /4    MUL r/m16    Unsigned multiply (DX:AX = AX * r/m16)
 * F7 /4    MUL r/m32    Unsigned multiply (EDX:EAX = EAX * r/m32)
 */
BOOL OPCHndlrALU_MUL(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Check when adding MUL instructions
    // Checked: difference is in the reg field of ModRM byte
    // and appropriate mnemonic will have been set by the 
    // OPCHndlrMulti_fx() function.
    switch (bOpcode)
    {
    case 0xf6:    // wBit and opsize attrib can be used
    case 0xf7:
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        break;

    case 0xaf:    // 2byte opcode
        // opsize attrib can be used
        insCurIns.bModRMType = MODRM_TYPE_R;
        break;

    case 0x6b:
        insCurIns.bModRMType = MODRM_TYPE_R;
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        insCurIns.fImmSignEx = TRUE;
        break;

    case 0x69:
        insCurIns.bModRMType = MODRM_TYPE_R;
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = ImmTypeFromWbitOperandSize();
        break;

    default:
        wprintf(L"OPCHndlrALU_MUL(): Invalid opcode %xh\n", bOpcode);
        return FALSE;

    }// switch(bOpcode)

    // all variations have a ModRM byte
    insCurIns.fModRM = TRUE;
    dsNextState = DASM_STATE_MOD_RM;

    return TRUE;
}

/*    ** DIV **
 * F6 /6    DIV r/m8
 * F7 /6    DIV r/m16
 * F7 /6    DIV r/m32
 *
 * F6 /7    IDIV r/m8
 * F7 /7    IDIV r/m16
 * F7 /7    IDIV r/m32
 */
BOOL OPCHndlrALU_DIV(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // The reg/opcode field in the ModRM byte is different for
    // div and idiv, so the OPCHndlrMulti_fx() function would
    // have printed the correct mnemonic into wszCurInsStr.
    switch (bOpcode)
    {
    case 0xf6:    // wBit and opsize attrib can be used
    case 0xf7:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        break;

    default:
        wprintf(L"OPCHndlrALU_DIV(): Invalid opcode %xh\n", bOpcode);
        return FALSE;

    }// switch(bOpcode)

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** INC **
 * FE /0    INC r/m8
 * FF /0    INC r/m16
 * FF /0    INC r/m32
 * 40+ rw    INC r16
 * 40+ rd    INC r32
 */
BOOL OPCHndlrALU_INC(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    WORD wReg = bOpcode - OPC_INC_BEG;
    if (OPC_INC_BEG <= bOpcode && bOpcode <= OPC_INC_END)
    {
        lstrcpyW(insCurIns.wszCurInsStrSrc, RegStrFromOperandSize(wReg));
        insCurIns.fSrcStrSet = TRUE;
        dsNextState = DASM_STATE_DUMP;
    }
    else if (bOpcode == 0xfe || bOpcode == 0xff)
    {
        // wBit and opsize attrib can be used
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
    }
    else
    {
        wprintf(L"OPCHndlrALU_INC(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    return TRUE;
}

/*    ** DEC **
 * FE /1    DEC r/m8
 * FF /1    DEC r/m16
 * FF /1    DEC r/m32
 * 48+rw    DEC r16
 * 48+rd    DEC r32
 */
BOOL OPCHndlrALU_DEC(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    WORD wReg = bOpcode - OPC_DEC_BEG;
    if (OPC_DEC_BEG <= bOpcode && bOpcode <= OPC_DEC_END)
    {
        lstrcpyW(insCurIns.wszCurInsStrSrc, RegStrFromOperandSize(wReg));
        insCurIns.fSrcStrSet = TRUE;
        dsNextState = DASM_STATE_DUMP;
    }
    else if (bOpcode == 0xfe || bOpcode == 0xff)
    {
        // wBit and opsize attrib can be used
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
    }
    else
    {
        wprintf(L"OPCHndlrALU_DEC(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    return TRUE;
}

/*    ** Rotate / Shift **
 *    * This is gonna be a big one! *
 *
 * C0 /0 ib     ROL r/m8,imm8
 * C0 /2 ib     RCL r/m8,imm8
 * C0 /3 ib     RCR r/m8,imm8
 * C1 /2 ib     RCL r/m16,imm8
 * C1 /2 ib     RCL r/m32,imm8
 * C1 /0 ib     ROL r/m16,imm8
 * C1 /0 ib     ROL r/m32,imm8
 * C0 /1 ib     ROR r/m8,imm8
 * C1 /1 ib     ROR r/m16,imm8
 * C1 /1 ib     ROR r/m32,imm8
 * C1 /3 ib     RCR r/m16,imm8
 * C1 /3 ib     RCR r/m32,imm8
 *
 * D0 /2    RCL r/m8,1
 * D2 /2    RCL r/m8,CL
 * D1 /2    RCL r/m16,1
 * D3 /2    RCL r/m16,CL
 * D1 /2    RCL r/m32,1
 * D3 /2    RCL r/m32,CL
 *
 * D0 /3    RCR r/m8,1
 * D2 /3    RCR r/m8,CL
 * D1 /3    RCR r/m16,1
 * D3 /3    RCR r/m16,CL
 * D1 /3    RCR r/m32,1
 * D3 /3    RCR r/m32,CL
 *
 * D0 /0    ROL r/m8,1
 * D2 /0    ROL r/m8,CL
 * D1 /0    ROL r/m16,1
 * D3 /0    ROL r/m16,CL
 * D1 /0    ROL r/m32,1
 * D3 /0    ROL r/m32,CL
 *
 * D0 /1    ROR r/m8,1
 * D2 /1    ROR r/m8,CL
 * D1 /1    ROR r/m16,1
 * D3 /1    ROR r/m16,CL
 * D1 /1    ROR r/m32,1
 * D3 /1    ROR r/m32,CL
 *
 * 0F A4 SHLD r/m16,r16,imm8    Shift r/m16 to left imm8 places while shifting bits from r16 in from the right
 * 0F A5 SHLD r/m16,r16,CL      Shift r/m16 to left CL places while shifting bits from r16 in from the right
 * 0F A4 SHLD r/m32,r32,imm8    Shift r/m32 to left imm8 places while shifting bits from r32 in from the right
 * 0F A5 SHLD r/m32,r32,CL      Shift r/m32 to left CL places while shifting bits from r32 in from the right
 *
 * 0F AC SHRD r/m16,r16,imm8    Shift r/m16 to right imm8 places while shifting bits from r16 in from the left
 * 0F AD SHRD r/m16,r16,CL      Shift r/m16 to right CL places while shifting bits from r16 in from the left
 * 0F AC SHRD r/m32,r32,imm8    Shift r/m32 to right imm8 places while shifting bits from r32 in from the left
 * 0F AD SHRD r/m32,r32,CL      Shift r/m32 to right CL places while shifting bits from r32 in from the left
 *
 */
BOOL OPCHndlrALU_Shift(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    if (bOpcodeHigh != 0xA)    // not shrd or shld
        if (!OPCHndlrALU_Shift_SetInsStr(bOpcode))
            return FALSE;

    switch (bOpcode)
    {
    case 0xc0:
    case 0xc1:
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

        // Destination is either a reg/mem and it can be 
        // 8/16/32bits. This can be determined by w-bit,
        // Mod/RM combinations in the MODRM state.
        // Only difference is the source operand: CL/1
    case 0xd0:    // src = 1
    case 0xd1:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        wsprintfW(insCurIns.wszCurInsStrOpr3, L"1");
        insCurIns.fOpr3StrSet = TRUE;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0xd2:    // src = CL register
    case 0xd3:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        wsprintfW(insCurIns.wszCurInsStrOpr3, L"cl");
        insCurIns.fOpr3StrSet = TRUE;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0xa4:    // SHLD r/m16,r16,imm8
    case 0xac:    // SHRD r/m16,r16,imm8
        insCurIns.fSpecialInstruction = TRUE;        // No w-bit
        if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
            insCurIns.bOperandSizeDes = insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;
        else
            insCurIns.bOperandSizeDes = insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        insCurIns.fModRM = TRUE;
        insCurIns.fImm = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        insCurIns.bImmType = IMM_8BIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0xa5:    // shld ax,bx,cl
    case 0xad:    // shrd ax,bx,cl
        insCurIns.fSpecialInstruction = TRUE;        // No w-bit
        if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
            insCurIns.bOperandSizeDes = insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;
        else
            insCurIns.bOperandSizeDes = insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;

        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        insCurIns.fOpr3StrSet = TRUE;
        wsprintfW(insCurIns.wszCurInsStrOpr3, L"cl");
        dsNextState = DASM_STATE_MOD_RM;
        break;

    default:
        wprintf(L"OPCHndlrALU_Shift(): Invalid opcode %xh\n", bOpcode);
        return FALSE;

    }// switch(bOpcode)
    return TRUE;
}

// OpcodeEx        Ins
//        0        ROL
//        1        ROR
//        2        RCL
//        3        RCR
//        4        SHL/SAL
//        5        SHR
//        6        SAL
//        7        SAR
BOOL OPCHndlrALU_Shift_SetInsStr(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh %xh\n", __FUNCTIONW__, bOpcode, *pByteInCode);

    // Check the 'reg/opcode' field of the ModRM byte
    // This is the extension to the primary opcode.
    BYTE bOpcodeEx;

    static WCHAR awszMnemonics[][MAX_OPCODE_NAME_LEN + 1] = {
                    L"rol",    L"ror",    L"rcl",    L"rcr", L"shl", L"shr", L"sal", L"sar" };

    Util_SplitModRMByte(*pByteInCode, NULL, &bOpcodeEx, NULL);
    if (bOpcodeEx < 0 || bOpcodeEx > 7)
    {
        return FALSE;
    }

    wsprintfW(wszCurInsStr, awszMnemonics[bOpcodeEx]);

    return TRUE;
}

BOOL OPCHndlrALU_SALC(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

// Logical instructions

/*    ** OR **
 * 0C ib    OR AL,imm8
 * 0D iw    OR AX,imm16
 * 0D id    OR EAX,imm32
 *
 * 80 /1 ib     OR r/m8,imm8
 * 81 /1 iw     OR r/m16,imm16
 * 81 /1 id     OR r/m32,imm32
 * 83 /1 ib     OR r/m16,imm8        r/m16 OR imm8 (sign-extended)
 * 83 /1 ib     OR r/m32,imm8        r/m32 OR imm8 (sign-extended)
 *
 * 08 /r    OR r/m8,r8
 * 09 /r    OR r/m16,r16
 * 09 /r    OR r/m32,r32
 * 0A /r    OR r8,r/m8
 * 0B /r    OR r16,r/m16
 * 0B /r    OR r32,r/m32
 */
BOOL OPCHndlrALU_OR(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    switch (bOpcode)
    {
    case 0x0c:
    case 0x0d:
    {
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = ImmTypeFromWbitOperandSize();
        lstrcpyW(insCurIns.wszCurInsStrDes, RegStrFromWbitOperandSize(REGCODE_AX));
        insCurIns.fDesStrSet = TRUE;
        dsNextState = DASM_STATE_IMM;
        break;

    }// case 0x0d

    case 0x80:    // bImmType can be set in the MODRM state using wBit and opsize attrib
    case 0x81:
        insCurIns.fImm = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x83:  // 83 /1 ib        OR r/m16,imm8
                // 83 /1 ib        OR r/m32,imm8
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        insCurIns.fImmSignEx = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x08:  // 08 /r    OR r/m8,r8
    case 0x09:
    case 0x0a:
    case 0x0b:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    default:
        wprintf(L"OPCHndlrALU_OR(): Invalid opcode: %xh\n", bOpcode);
        return FALSE;

    }// switch(bOpcode)
    return TRUE;
}

/*        ** AND **
 * 24 ib        AND AL,imm8
 * 25 iw        AND AX,imm16
 * 25 id        AND EAX,imm32
 * 80 /4 ib     AND r/m8,imm8
 * 81 /4 iw     AND r/m16,imm16
 * 81 /4 id     AND r/m32,imm32
 * 83 /4 ib     AND r/m16,imm8    (sign-extended)
 * 83 /4 ib     AND r/m32,imm8  (sign-extended)
 * 20 /r        AND r/m8,r8
 * 21 /r        AND r/m16,r16
 * 21 /r        AND r/m32,r32
 * 22 /r        AND r8,r/m8
 * 23 /r        AND r16,r/m16
 * 23 /r        AND r32,r/m32
 */
BOOL OPCHndlrALU_AND(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    switch (bOpcode)
    {

    case 0x20:    // /r indicates that there is a ModRM byte next
    case 0x21:
    case 0x22:
    case 0x23:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x24:
    case 0x25:
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = ImmTypeFromWbitOperandSize();
        lstrcpyW(insCurIns.wszCurInsStrDes, RegStrFromWbitOperandSize(REGCODE_AX));
        insCurIns.fDesStrSet = TRUE;
        dsNextState = DASM_STATE_IMM;

        break;

    case 0x80:    // wbit and opsize attrib
    case 0x81:
        insCurIns.fImm = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x83:
        insCurIns.bImmType = IMM_8BIT;
        insCurIns.fImm = TRUE;
        insCurIns.fImmSignEx = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    default:
        wprintf(L"OPCHndlrALU_AND(): Invalid opcode: %xh\n", bOpcode);
        return FALSE;

    }// switch(bOpcode)
    return TRUE;
}

/*    ** XOR **
 * 34 ib    XOR AL,imm8
 * 35 iw    XOR AX,imm16
 * 35 id    XOR EAX,imm32
 *
 * 80 /6 ib     XOR r/m8,imm8
 * 81 /6 iw     XOR r/m16,imm16
 * 81 /6 id     XOR r/m32,imm32
 * 83 /6 ib     XOR r/m16,imm8        r/m16 XOR imm8 (sign-extended)
 * 83 /6 ib     XOR r/m32,imm8        r/m32 XOR imm8 (sign-extended)
 *
 * 30 /r    XOR r/m8,r8
 * 31 /r    XOR r/m16,r16
 * 31 /r    XOR r/m32,r32
 * 32 /r    XOR r8,r/m8
 * 33 /r    XOR r16,r/m16
 * 33 /r    XOR r32,r/m32
 */
BOOL OPCHndlrALU_XOR(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    switch (bOpcode)
    {
    case 0x34:
    case 0x35:
    {
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = ImmTypeFromWbitOperandSize();
        wsprintfW(insCurIns.wszCurInsStrDes, RegStrFromWbitOperandSize(REGCODE_AX));
        insCurIns.fDesStrSet = TRUE;
        dsNextState = DASM_STATE_IMM;
        break;

    }//case 0x35

    case 0x80:    // op size can be determined by wbit and op size prefix
    case 0x81:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        insCurIns.fImm = TRUE;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x83:
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        insCurIns.fImmSignEx = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x30:
    case 0x31:
    case 0x32:    // d-bit is set
    case 0x33:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    default:
        wprintf(L"OPCHndlrALU_XOR(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    return TRUE;
}

/*    ** NOT **
 * F6 /2    NOT r/m8    Reverse each bit of r/m8
 * F7 /2    NOT r/m16    Reverse each bit of r/m16
 * F7 /2    NOT r/m32    Reverse each bit of r/m32
 */
BOOL OPCHndlrALU_NOT(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_DIGIT;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** NEG **
 * F6 /3    NEG r/m8     Two's complement negate r/m8
 * F7 /3    NEG r/m16    Two's complement negate r/m16
 * F7 /3    NEG r/m32    Two's complement negate r/m32
 */
BOOL OPCHndlrALU_NEG(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_DIGIT;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

// Adjust instructions

/*    ** DAA **
 * 27    DAA        Decimal adjust AL after addition
 */
BOOL OPCHndlrALU_DAA(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** DAS **
 * 2F    DAS        Decimal adjust AL after subtraction
 */
BOOL OPCHndlrALU_DAS(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

BOOL OPCHndlrALU_AAA(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*
 * 3F    AAS
 */
BOOL OPCHndlrALU_AAS(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*
 * D4 0A    AAM
 * D4 ib    AAM     - not handled
 */
BOOL OPCHndlrALU_AAM(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // move past 0a after the opcode d4
    ++pByteInCode;

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*
 * D5 0A    AAD
 * D5 ib    AAD     - not handled
 */
BOOL OPCHndlrALU_AAD(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // move past 0a after the opcode d5
    ++pByteInCode;

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

// Memory operations

/*    ** XCHG **
 * 90+rw    XCHG AX,r16        Exchange r16 with AX
 * 90+rw    XCHG r16,AX        Exchange AX with r16
 * 90+rd    XCHG EAX,r32    Exchange r32 with EAX
 * 90+rd    XCHG r32,EAX    Exchange EAX with r32
 *
 * 86 /r XCHG r/m8,r8        Exchange r8 (byte register) with byte from r/m8
 * 86 /r XCHG r8,r/m8        Exchange byte from r/m8 with r8 (byte register)
 * 87 /r XCHG r/m16,r16        Exchange r16 with word from r/m16
 * 87 /r XCHG r16,r/m16        Exchange word from r/m16 with r16
 * 87 /r XCHG r/m32,r32        Exchange r32 with doubleword from r/m32
 * 87 /r XCHG r32,r/m32        Exchange doubleword from r/m32 with r32
 */
BOOL OPCHndlrMem_XCHG(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    if (OPC_XCHG_BEG <= bOpcode && bOpcode <= OPC_XCHG_END)
    {
        BYTE bReg = bOpcode - OPC_XCHG_BEG;
        wsprintfW(insCurIns.wszCurInsStrDes, RegStrFromOperandSize(REGCODE_AX));
        wsprintfW(insCurIns.wszCurInsStrSrc, RegStrFromOperandSize(bReg));
        insCurIns.fSrcStrSet = TRUE;
        insCurIns.fDesStrSet = TRUE;
        dsNextState = DASM_STATE_DUMP;
    }
    else if (bOpcode == 0x86 || bOpcode == 0x87)
    {
        // op size can be determined by w-bit and op size attrib
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        dsNextState = DASM_STATE_MOD_RM;
    }
    else
    {
        wprintf(L"OPCHndlrMem_XCHG(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }
    return TRUE;
}

/*    ** MOV **
 * 88 /r    MOV r/m8,r8         Move r8 to r/m8
 * 89 /r    MOV r/m16,r16       Move r16 to r/m16
 * 89 /r    MOV r/m32,r32       Move r32 to r/m32
 * 8A /r    MOV r8,r/m8         Move r/m8 to r8
 * 8B /r    MOV r16,r/m16       Move r/m16 to r16
 * 8B /r    MOV r32,r/m32       Move r/m32 to r32
 * 8C /r    MOV r/m16,Sreg**    Move segment register to r/m16
 * 8E /r    MOV Sreg,r/m16**    Move r/m16 to segment register
 *
 * A0    MOV AL,moffs8*         Move byte at (seg:offset) to AL
 * A1    MOV AX,moffs16*        Move word at (seg:offset) to AX
 * A1    MOV EAX,moffs32*       Move doubleword at (seg:offset) to EAX
 * A2    MOV moffs8*,AL         Move AL to (seg:offset)
 * A3    MOV moffs16*,AX        Move AX to (seg:offset)
 * A3    MOV moffs32*,EAX       Move EAX to (seg:offset)
 *
 * B0+ rb    MOV r8,imm8
 * B8+ rw    MOV r16,imm16
 * B8+ rd    MOV r32,imm32
 *
 * C6 /0    MOV r/m8,imm8
 * C7 /0    MOV r/m16,imm16
 * C7 /0    MOV r/m32,imm32
 *
 * * The moffs8, moffs16, and moffs32 operands specify a simple offset relative to the segment base, where
 * 8, 16, and 32 refer to the size of the data. The address-size attribute of the instruction determines
 * the size of the offset, either 16 or 32 bits.
 *
 * ** In 32-bit mode, the assembler may insert the 16-bit operand-size prefix with this instruction.
 *
 */
BOOL OPCHndlrMem_MOV(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    switch (bOpcode)
    {

    case 0x88:    // wBit and opsize attrib used
    case 0x89:
    case 0x8a:
    case 0x8b:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x8c:    // 8C /r    MOV r/m16,Sreg
    {
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        insCurIns.bOperandSizeDes = OPERANDSIZE_16BIT;

        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.bRegTypeSrc = REGTYPE_SREG;
        insCurIns.bRegTypeDest = REGTYPE_GREG;    // only if Mod+RM bits suggest a register destination
        dsNextState = DASM_STATE_MOD_RM;
        break;
    }

    case 0x8e:    // 8C /r    MOV Sreg,r/m16
    {
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;

        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.bRegTypeSrc = REGTYPE_GREG;    // only if Mod+RM bits suggest a register source
        insCurIns.bRegTypeDest = REGTYPE_SREG;
        dsNextState = DASM_STATE_MOD_RM;
        break;
    }

    case 0xa0:    // A0    MOV AL,moffs8*        Move byte at (seg:offset) to AL
    case 0xa1:    // A1    MOV AX,moffs16*
    {
        insCurIns.fDataOffset = TRUE;
        insCurIns.bImmType = ImmTypeFromAddrSize();
        insCurIns.pwszPtrStr = PtrStrFromOperandSize();
        wsprintfW(insCurIns.wszCurInsStrDes,   L"%s", RegStrFromWbitOperandSize(REGCODE_AX));
        insCurIns.fDesStrSet = TRUE;
        dsNextState = DASM_STATE_IMM;
        break;
    }

    case 0xa2:    // A2    MOV moffs8*,AL
    case 0xa3:    // A3    MOV moffs16*,AX
    {
        insCurIns.fDataOffset = TRUE;
        insCurIns.bImmType = ImmTypeFromAddrSize();
        insCurIns.pwszPtrStr = PtrStrFromOperandSize();
        wsprintfW(insCurIns.wszCurInsStrSrc,  L"%s", RegStrFromWbitOperandSize(REGCODE_AX));
        insCurIns.fSrcStrSet = TRUE;
        dsNextState = DASM_STATE_IMM;
        break;
    }

    case 0xc6:
    case 0xc7:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = ImmTypeFromWbitOperandSize();
        dsNextState = DASM_STATE_MOD_RM;
        break;

    default:
    {
        if (0xb0 <= bOpcode && bOpcode <= 0xb7)
        {
            BYTE bReg = bOpcode - 0xb0;
            wsprintfW(insCurIns.wszCurInsStrDes,  L"%s", awszRegCodes8[bReg]);
            insCurIns.fDesStrSet = TRUE;
            insCurIns.fImm = TRUE;
            insCurIns.bImmType = IMM_8BIT;
            dsNextState = DASM_STATE_IMM;
        }
        else if (0xb8 <= bOpcode && bOpcode <= 0xbf)
        {
            BYTE bReg = bOpcode - 0xb8;
            insCurIns.bImmType = ImmTypeFromOperandSize();
            wsprintfW(insCurIns.wszCurInsStrDes, L"%s", RegStrFromOperandSize(bReg));
            insCurIns.fDesStrSet = TRUE;
            insCurIns.fImm = TRUE;
            dsNextState = DASM_STATE_IMM;
        }
        else
        {
            wprintf(L"OPCHndlrMem_MOV(): Invalid opcode %xh\n", bOpcode);
            return FALSE;
        }

    }// default

    }// switch(bOpcode)
    return TRUE;
}

/*    ** LEA **
 * 8D /r    LEA r16,m    Store effective address for m in register r16
 * 8D /r    LEA r32,m    Store effective address for m in register r32
 */
BOOL OPCHndlrMem_LEA(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;

    /*
     * Manually set bDBit because it is insignificant for this instruction
     * but MODRM state depends on it having the correct value.
     * If we continue with bDBit = 0, then this instruction will be
     * decoded as LEA m, r32 :P
     */
    insCurIns.bDBit = 1;
    insCurIns.fSpecialInstruction = TRUE;
    if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_UNDEF;
        insCurIns.bOperandSizeDes = OPERANDSIZE_16BIT;
    }
    else
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_UNDEF;
        insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    }

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/* ** CWDE **
 * 98 CBW    AX = sign-extend of AL
 * 98 CWDE    EAX = sign-extend of AX
 */
BOOL OPCHndlrMem_CWDE(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Override the wszCurInsStr because mnemonic depends 
    // on the operand size
    if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
    {
        wsprintfW(wszCurInsStr,L"cbw");
    }
    else
    {
        wsprintfW(wszCurInsStr, L"cwde");
    }
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** CWD/CDQ **
 * 99    CWD        DX:AX = sign-extend of AX
 * 99    CDQ        EDX:EAX = sign-extend of EAX
 */
BOOL OPCHndlrMem_CDQ(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Override the wszCurInsStr because mnemonic depends 
    // on the operand size
    if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
    {
        wsprintfW(wszCurInsStr, L"cwd");
    }
    else
    {
        wsprintfW(wszCurInsStr, L"cdq");
    }
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** SAHF **
 * 9E    SAHF    Loads SF, ZF, AF, PF, and CF from AH into EFLAGS register
 */
BOOL OPCHndlrMem_SAHF(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

BOOL OPCHndlrMem_LAHF(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** MOVS **
 * A4    MOVSB    Move byte at address DS:(E)SI to address ES:(E)DI
 * A5    MOVSW    Move word at address DS:(E)SI to address ES:(E)DI
 * A5    MOVSD    Move doubleword at address DS:(E)SI to address ES:(E)DI
 */
BOOL OPCHndlrMem_MOVS(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);
    UNREFERENCED_PARAMETER(bOpcode);

    WCHAR *pwszxDI = RegStrFromAddrSize(REGCODE_DI);
    WCHAR *pwszxSI = RegStrFromAddrSize(REGCODE_SI);
    WCHAR *pwszPtrStr = NULL;

    // to print: movs    byte ptr es:[edi],byte ptr [esi]
    pwszPtrStr = PtrStrFromOperandSize();

    // destination: <ptrStr> es:[(e)di]
    wsprintfW(insCurIns.wszCurInsStrDes, L"%ses:[%s]", pwszPtrStr, pwszxDI);

    // source: <ptrStr> [(e)si]
    wsprintfW(insCurIns.wszCurInsStrSrc, L"%s[%s]", pwszPtrStr, pwszxSI);
    insCurIns.fSrcStrSet = TRUE;
    insCurIns.fDesStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** LODS **
 * AC    LODS m8    Load byte at address DS:(E)SI into AL
 * AD    LODS m16   Load word at address DS:(E)SI into AX
 * AD    LODS m32   Load doubleword at address DS:(E)SI into EAX
 * AC    LODSB      Load byte at address DS:(E)SI into AL
 * AD    LODSW      Load word at address DS:(E)SI into AX
 * AD    LODSD      Load doubleword at address DS:(E)SI into EAX
 */
BOOL OPCHndlrMem_LODS(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);
    UNREFERENCED_PARAMETER(bOpcode);

    WCHAR *pwszxSI = RegStrFromAddrSize(REGCODE_SI);
    WCHAR *pwszPtrStr = PtrStrFromOperandSize();

    // to print: lods        dword ptr [esi]
    wsprintfW(insCurIns.wszCurInsStrSrc, L"%s[%s]", pwszPtrStr, pwszxSI);
    insCurIns.fSrcStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** STOSx **
 * AA STOS m8    Store AL at address ES:(E)DI
 * AB STOS m16    Store AX at address ES:(E)DI
 * AB STOS m32    Store EAX at address ES:(E)DI
 * AA STOSB        Store AL at address ES:(E)DI
 * AB STOSW        Store AX at address ES:(E)DI
 * AB STOSD        Store EAX at address ES:(E)DI
 */
BOOL OPCHndlrMem_STOS(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);
    UNREFERENCED_PARAMETER(bOpcode);

    WCHAR *pwszxDI = RegStrFromAddrSize(REGCODE_DI);
    WCHAR *pwszPtrStr = PtrStrFromOperandSize();

    // to print: stos        word ptr es:[edi]
    wsprintfW(insCurIns.wszCurInsStrSrc, L"%ses:[%s]", pwszPtrStr, pwszxDI);
    insCurIns.fSrcStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** LES **
 * C4 /r    LES r16,m16:16    Load ES:r16 with far pointer from memory
 * C4 /r    LES r32,m16:32    Load ES:r32 with far pointer from memory
 */
BOOL OPCHndlrMem_LES(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);
    UNREFERENCED_PARAMETER(bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;

    insCurIns.fSpecialInstruction = TRUE;
    if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        insCurIns.bOperandSizeDes = OPERANDSIZE_16BIT;
    }
    else
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_48BIT;
        insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    }

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** LDS **
 * C5 /r    LDS r16,m16:16    Load DS:r16 with far pointer from memory
 * C5 /r    LDS r32,m16:32    Load DS:r32 with far pointer from memory
 */
BOOL OPCHndlrMem_LDS(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);
    UNREFERENCED_PARAMETER(bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;

    insCurIns.fSpecialInstruction = TRUE;
    if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        insCurIns.bOperandSizeDes = OPERANDSIZE_16BIT;
    }
    else
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_48BIT;
        insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    }

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** XLAT **
 * D7 XLAT m8    Set AL to memory byte DS:[(E)BX + unsigned AL]
 * D7 XLATB        Set AL to memory byte DS:[(E)BX + unsigned AL]
 */
BOOL OPCHndlrMem_XLAT(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    // XLAT m8 is only for documentation purposes in the code
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

// Control flow and Conditional instructions

/*    ** CMP **
 * 3C ib        CMP AL, imm8
 * 3D iw        CMP AX, imm16
 * 3D id        CMP EAX, imm32
 * 80 /7 ib        CMP r/m8, imm8
 * 81 /7 iw        CMP r/m16, imm16
 * 81 /7 id        CMP r/m32,imm32
 * 83 /7 ib        CMP r/m16,imm8
 * 83 /7 ib        CMP r/m32,imm8
 * 38 /r        CMP r/m8,r8
 * 39 /r        CMP r/m16,r16
 * 39 /r        CMP r/m32,r32
 * 3A /r        CMP r8,r/m8
 * 3B /r        CMP r16,r/m16
 * 3B /r        CMP r32,r/m32
 *
 * When an immediate value is used as an operand, it is signextended
 * to the length of the first operand.
 */
BOOL OPCHndlrCC_CMP(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    switch (bOpcode)
    {

    case 0x3c:
    case 0x3d:
    {
        insCurIns.bImmType = ImmTypeFromWbitOperandSize();
        wsprintfW(insCurIns.wszCurInsStrDes,  RegStrFromWbitOperandSize(REGCODE_AX));
        insCurIns.fDesStrSet = TRUE;
        insCurIns.fImm = TRUE;
        dsNextState = DASM_STATE_IMM;
        break;
    }

    case 0x38:    // /r
    case 0x39:
    case 0x3a:
    case 0x3b:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x80:
    case 0x81:
    {
        insCurIns.fImm = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;
    }

    case 0x83:
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        insCurIns.fImmSignEx = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    }// switch(bOpcode)

    return TRUE;
}

/*    ** BOUND **
 * 62 /r    BOUND r16,m16&16    Check if r16 (array index) is within bounds specified by m16&16
 * 62 /r    BOUND r32,m32&32    Check if r32 (array index) is within bounds specified by m32&32
 */
BOOL OPCHndlrCC_BOUND(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    insCurIns.fSpecialInstruction = TRUE;
    if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;    // ptr str selector
        insCurIns.bOperandSizeDes = OPERANDSIZE_16BIT;
    }
    else
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_64BIT;    // ptr str selector
        insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    }
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** ARPL **
 * 63 /r    ARPL r/m16,r16    Adjust RPL of r/m16 to not less than RPL of r16
 */
BOOL OPCHndlrCC_ARPL(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;
    insCurIns.bOperandSizeDes = OPERANDSIZE_16BIT;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    dsNextState = DASM_STATE_MOD_RM;

    // pByteInCode is already pointing to ModRM byte

    return TRUE;
}

/*    ** Jcc **
 * 77 cb JA     rel8 Jump short if above (CF=0 and ZF=0)
 * 73 cb JAE    rel8 Jump short if above or equal (CF=0)
 * 72 cb JB     rel8 Jump short if below (CF=1)
 * 76 cb JBE    rel8 Jump short if below or equal (CF=1 or ZF=1)
 * E3 cb JCXZ   rel8 Jump short if CX register is 0
 * E3 cb JECXZ  rel8 Jump short if ECX register is 0
 * 74 cb JE     rel8 Jump short if equal (ZF=1)
 * 7F cb JG     rel8 Jump short if greater (ZF=0 and SF=OF)
 * 7D cb JGE    rel8 Jump short if greater or equal (SF=OF)
 * 7C cb JL     rel8 Jump short if less (SF<>OF)
 * 7E cb JLE    rel8 Jump short if less or equal (ZF=1 or SF<>OF)
 * 75 cb JNE    rel8 Jump short if not equal (ZF=0)
 * 71 cb JNO    rel8 Jump short if not overflow (OF=0)
 * 7B cb JNP    rel8 Jump short if not parity (PF=0)
 * 79 cb JNS    rel8 Jump short if not sign (SF=0)
 * 70 cb JO     rel8 Jump short if overflow (OF=1)
 * 7A cb JP     rel8 Jump short if parity (PF=1)
 * 78 cb JS     rel8 Jump short if sign (SF=1)
 *
 * 0F 87 cw/cd JA   rel16/32    Jump near if above (CF=0 and ZF=0)
 * 0F 83 cw/cd JAE  rel16/32    Jump near if above or equal (CF=0)
 * 0F 82 cw/cd JB   rel16/32    Jump near if below (CF=1)
 * 0F 86 cw/cd JBE  rel16/32    Jump near if below or equal (CF=1 or ZF=1)
 * 0F 84 cw/cd JE   rel16/32    Jump near if equal (ZF=1)
 * 0F 8F cw/cd JG   rel16/32    Jump near if greater (ZF=0 and SF=OF)
 * 0F 8D cw/cd JGE  rel16/32    Jump near if greater or equal (SF=OF)
 * 0F 8C cw/cd JL   rel16/32    Jump near if less (SF<>OF)
 * 0F 8E cw/cd JLE  rel16/32    Jump near if less or equal (ZF=1 or SF<>OF)
 * 0F 85 cw/cd JNE  rel16/32    Jump near if not equal (ZF=0)
 * 0F 81 cw/cd JNO  rel16/32    Jump near if not overflow (OF=0)
 * 0F 8B cw/cd JNP  rel16/32    Jump near if not parity (PF=0)
 * 0F 89 cw/cd JNS  rel16/32    Jump near if not sign (SF=0)
 * 0F 80 cw/cd JO   rel16/32    Jump near if overflow (OF=1)
 * 0F 8A cw/cd JP   rel16/32    Jump near if parity (PF=1)
 * 0F 88 cw/cd JS   rel16/32    Jump near if sign (SF=1)
 *
 * EB cb JMP rel8       Jump short, relative, displacement relative to next instruction
 * E9 cw JMP rel16      Jump near, relative, displacement relative to next instruction
 * E9 cd JMP rel32      Jump near, relative, displacement relative to next instruction
 * FF /4 JMP r/m16      Jump near, absolute indirect, address given in r/m16
 * FF /4 JMP r/m32      Jump near, absolute indirect, address given in r/m32
 * EA cd JMP ptr16:16   Jump far, absolute, address given in operand
 * EA cp JMP ptr16:32   Jump far, absolute, address given in operand
 * FF /5 JMP m16:16     Jump far, absolute indirect, address given in m16:16
 * FF /5 JMP m16:32     Jump far, absolute indirect, address given in m16:32
 */
BOOL OPCHndlrCC_JUMP(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    if (OPC_JMP_BEG <= bOpcode && bOpcode <= OPC_JMP_END)
    {
        insCurIns.fCodeOffset = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        dsNextState = DASM_STATE_IMM;
    }
    else if (0x80 <= bOpcode && bOpcode <= 0x8f)    // 2byte opcodes
    {
        insCurIns.fCodeOffset = TRUE;
        insCurIns.bImmType = ImmTypeFromOperandSize();
        dsNextState = DASM_STATE_IMM;
    }
    else
    {
        switch (bOpcode)
        {
        case 0xe3:    // jcxz/jcexz
        {
            insCurIns.fCodeOffset = TRUE;
            if (insCurIns.wPrefixTypes & PREFIX_ADSIZE)
                wsprintfW(wszCurInsStr, L"%s", L"jcxz");
            insCurIns.bImmType = IMM_8BIT;
            dsNextState = DASM_STATE_IMM;
            break;
        }

        case 0xeb:
            insCurIns.fCodeOffset = TRUE;
            insCurIns.bImmType = IMM_8BIT;
            dsNextState = DASM_STATE_IMM;
            break;

        case 0xe9:
            insCurIns.fCodeOffset = TRUE;
            insCurIns.bImmType = ImmTypeFromWbitOperandSize();
            dsNextState = DASM_STATE_IMM;
            break;

        case 0xea:    // EA cd JMP ptr16:16 or EA cp JMP ptr16:32
        {
            insCurIns.fCodeOffset = TRUE;
            if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
                insCurIns.bImmType = IMM_32BIT;
            else
                insCurIns.bImmType = IMM_48BIT;
            dsNextState = DASM_STATE_IMM;
            break;
        }

        case 0xff:
        {
            BYTE bOpcodeEx;
            Util_SplitModRMByte(*pByteInCode, NULL, &bOpcodeEx, NULL);
            insCurIns.fModRM = TRUE;
            insCurIns.fSpecialInstruction = TRUE;
            if (bOpcodeEx == 4)
            {
                if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
                    insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;
                else
                    insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            }
            else if (bOpcodeEx == 5)
            {
                if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
                    insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
                else
                    insCurIns.bOperandSizeSrc = OPERANDSIZE_48BIT;
            }
            else
            {
                wprintf(L"OPCHndlrCC_JUMP(): Invalid opcode ex %d\n", bOpcodeEx);
                return FALSE;
            }
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            dsNextState = DASM_STATE_MOD_RM;
            break;
        }

        default:
            wprintf(L"OPCHndlrCC_JUMP(): Invalid opcode %xh\n", bOpcode);
            return FALSE;

        }// switch(bOpcode)

    }// if-else

    return TRUE;
}

/*    ** TEST **
 * A8 ib    TEST AL,imm8    AND imm8 with AL; set SF, ZF, PF according to result
 * A9 iw    TEST AX,imm16   AND imm16 with AX; set SF, ZF, PF according to result
 * A9 id    TEST EAX,imm32  AND imm32 with EAX; set SF, ZF, PF according to result
 *
 * F6 /0 ib     TEST r/m8,imm8      AND imm8 with r/m8; set SF, ZF, PF according to result
 * F7 /0 iw     TEST r/m16,imm16    AND imm16 with r/m16; set SF, ZF, PF according to result
 * F7 /0 id     TEST r/m32,imm32    AND imm32 with r/m32; set SF, ZF, PF according to result
 *
 * 84 /r    TEST r/m8,r8    AND r8 with r/m8; set SF, ZF, PF according to result
 * 85 /r    TEST r/m16,r16  AND r16 with r/m16; set SF, ZF, PF according to result
 * 85 /r    TEST r/m32,r32  AND r32 with r/m32; set SF, ZF, PF according to result
 */
BOOL OPCHndlrCC_TEST(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    switch (bOpcode)
    {
    case 0xa8:
    case 0xa9:
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = ImmTypeFromWbitOperandSize();
        wsprintfW(insCurIns.wszCurInsStrDes, RegStrFromWbitOperandSize(REGCODE_EAX));
        insCurIns.fDesStrSet = TRUE;
        dsNextState = DASM_STATE_IMM;
        break;

    case 0xf6:    // w-bit and op size prefix can determine the operands' size
    case 0xf7:
        insCurIns.fModRM = TRUE;
        insCurIns.fImm = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0x84:    // w-bit and op size prefix can determine the operands' size
    case 0x85:
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    default:
        wprintf(L"OPCHndlrCC_TEST(): Invalid opcode %xh\n", bOpcode);
        return FALSE;

    }// switch(bOpcode)
    return TRUE;
}

/*    ** CMPSx **
 * A6 CMPS m8, m8       Compares byte at address DS:(E)SI with byte at address
 *                            ES:(E)DI and sets the status flags accordingly
 * A7 CMPS m16, m16     Compares word at address DS:(E)SI with word at address
 *                            ES:(E)DI and sets the status flags accordingly
 * A7 CMPS m32, m32     Compares doubleword at address DS:(E)SI with doubleword
 *                            at address ES:(E)DI and sets the status flags accordingly
 * A6 CMPSB             Compares byte at address DS:(E)SI with byte at address
 *                            ES:(E)DI and sets the status flags accordingly
 * A7 CMPSW             Compares word at address DS:(E)SI with word at address
 *                            ES:(E)DI and sets the status flags accordingly
 * A7 CMPSD             Compares doubleword at address DS:(E)SI with doubleword
 *                            at address ES:(E)DI and sets the status flags accordingly
 */
BOOL OPCHndlrCC_CMPS(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    WCHAR *pwszxDI = RegStrFromAddrSize(REGCODE_DI);
    WCHAR *pwszxSI = RegStrFromAddrSize(REGCODE_SI);
    WCHAR *pwszPtrStr = PtrStrFromOperandSize();

    // to print: cmps        byte ptr [esi],byte ptr es:[edi]

    // destination: <ptrStr>[(e)si]
    wsprintfW(insCurIns.wszCurInsStrDes, L"%s[%s]", pwszPtrStr, pwszxSI);

    // source: <ptrStr> es:[(e)di]
    wsprintfW(insCurIns.wszCurInsStrSrc, L"%ses:[%s]", pwszPtrStr, pwszxDI);
    insCurIns.fSrcStrSet = TRUE;
    insCurIns.fDesStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** SCASx **
 * AE    SCAS m8     Compare AL with byte at ES:(E)DI and set status flags
 * AF    SCAS m16    Compare AX with word at ES:(E)DI and set status flags
 * AF    SCAS m32    Compare EAX with doubleword at ES(E)DI and set status flags
 * AE    SCASB       Compare AL with byte at ES:(E)DI and set status flags
 * AF    SCASW       Compare AX with word at ES:(E)DI and set status flags
 * AF    SCASD       Compare EAX with doubleword at ES:(E)DI and set status flags
 */
BOOL OPCHndlrCC_SCAS(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    WCHAR *pwszxDI = RegStrFromAddrSize(REGCODE_DI);
    WCHAR *pwszPtrStr = PtrStrFromOperandSize();

    // to print: scas   dword ptr es:[edi]
    wsprintfW(insCurIns.wszCurInsStrSrc, L"%ses:[%s]", pwszPtrStr, pwszxDI);
    insCurIns.fSrcStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** RET **
 * C3       RET         Near return to calling procedure
 * CB       RET         Far return to calling procedure
 * C2 iw    RET imm16   Near return to calling procedure and pop imm16 bytes from stack
 * CA iw    RET imm16   Far return to calling procedure and pop imm16 bytes from stack
 */
BOOL OPCHndlrCC_RETN(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    if (bOpcode == 0xc3 || bOpcode == 0xcb)
    {
        dsNextState = DASM_STATE_DUMP;
    }
    else if (bOpcode == 0xc2 || bOpcode == 0xca)
    {
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_16BIT;
        dsNextState = DASM_STATE_IMM;
    }
    else
    {
        wprintf(L"OPCHndlrCC_RETN(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }
    return TRUE;
}

BOOL OPCHndlrCC_IRETD(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // "iret" if opsize = 16bit
    // else, "iretd" is already written to 
    // wszCurInsStr
    if (insCurIns.wPrefixTypes & PREFIX_OPSIZE) {
        wsprintfW(wszCurInsStr, L"iret");
    }

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** LOOP/LOOPxx **
 * E2 cb    LOOP rel8       Decrement count; jump short if count != 0
 * E1 cb    LOOPE rel8      Decrement count; jump short if count != 0 and ZF=1
 * E1 cb    LOOPZ rel8      Decrement count; jump short if count != 0 and ZF=1
 * E0 cb    LOOPNE rel8     Decrement count; jump short if count != 0 and ZF=0
 * E0 cb    LOOPNZ rel8     Decrement count; jump short if count != 0 and ZF=0
 */
BOOL OPCHndlrCC_CLOOP(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Offset handled in IMM state
    insCurIns.fCodeOffset = TRUE;
    insCurIns.bImmType = IMM_8BIT;
    dsNextState = DASM_STATE_IMM;

    return TRUE;
}

/*    ** CALL **
 * E8 cw    CALL rel16      Call near, relative, displacement relative to next instruction
 * E8 cd    CALL rel32      Call near, relative, displacement relative to next instruction
 * FF /2    CALL r/m16      Call near, absolute indirect, address given in r/m16
 * FF /2    CALL r/m32      Call near, absolute indirect, address given in r/m32
 * 9A cd    CALL ptr16:16   Call far, absolute, address given in operand
 * 9A cp    CALL ptr16:32   Call far, absolute, address given in operand
 * FF /3    CALL m16:16     Call far, absolute indirect, address given in m16:16
 * FF /3    CALL m16:32     Call far, absolute indirect, address given in m16:32
 */
BOOL OPCHndlrCC_CALL(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    int iError = ERROR_SUCCESS;

    switch (bOpcode)
    {

    case 0xe8:
        insCurIns.bImmType = ImmTypeFromOperandSize();
        insCurIns.fCodeOffset = TRUE;
        dsNextState = DASM_STATE_IMM;
        break;

    case 0x9a:    // CALLF
        if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
        {
            // ptr 16bit, offset 16bit
            WORD wPtr;
            short sOffset;

            wPtr = *((WORD*)pByteInCode);
            pByteInCode += sizeof(WORD);

            sOffset = *((short*)pByteInCode);
            pByteInCode += sizeof(short);

            wsprintfW(wszCurInsTempStr, L"word ptr [%04xh]:%04xh", wPtr, sOffset);
            lstrcatW(wszCurInsStr, wszCurInsTempStr);

            if (iError) {
                wprintf(L"OPCHndlrCC_CALL(): 3 wcscat_s() error %d\n", iError);
            }
        }
        else
        {
            // ptr 16bit, offset 32bit
            WORD wPtr;
            INT iOffset;

            wPtr = *((WORD*)pByteInCode);
            pByteInCode += sizeof(WORD);

            iOffset = *((INT*)pByteInCode);
            pByteInCode += sizeof(INT);

            wsprintfW(wszCurInsTempStr, L"word ptr [%04xh]:%08xh", wPtr, iOffset);
            wcscat(wszCurInsStr, wszCurInsTempStr);

            if (iError) {
                wprintf(L"OPCHndlrCC_CALL(): 3 wcscat_s() error %d\n", iError);
            }
        }

        dsNextState = DASM_STATE_DUMP;
        break;

    case 0xff:
        BYTE bReg;

        // Determine whether it is FF /2 or FF /3 call
        Util_SplitModRMByte(*pByteInCode, NULL, &bReg, NULL);
        if (bReg == 2)
        {
            // CALL r/m16 or CALL r/m32
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        else if (bReg == 3)
        {
            // CALL m16:16 or CALL m16:32
            insCurIns.fSpecialInstruction = TRUE;
            if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
                insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            else
                insCurIns.bOperandSizeSrc = OPERANDSIZE_48BIT;
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        else
        {
            wprintf(L"OPCHndlrCC_CALL(): Invalid reg field value %d\n", bReg);
            return FALSE;
        }

        break;

    default:
        wprintf(L"OPCHndlrCC_CALL(): Invalid opcode %xh\n", bOpcode);
        return FALSE;

    }// switch(bOpcode)

    return TRUE;
}

/* ** EFLAGS **
 * F8    CLC        Clear CF flag
 * FC    CLD        Clear DF flag
 * FA    CLI        Clear Int flag
 * F5    CMC        Complement CF flag
 * F9    STC        Set CF flag
 * FD    STD        Set DF flag
 * FB    STI        Set interrupt flag; external, maskable interrupts enabled at the end of the next instruction
 */
BOOL OPCHndlrCC_EFLAGS(BYTE bOpcode)    // EFLAGS manipulators: CMC, CLC, STC, ...
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

// System and IO instructions

/*    ** INSx **
 * 6C INS m8, DX    Input byte from I/O port specified in DX into memory location specified in ES:(E)DI
 * 6D INS m16, DX    Input word from I/O port specified in DX into memory location specified in ES:(E)DI
 * 6D INS m32, DX    Input doubleword from I/O port specified in DX into memory location specified in ES:(E)DI
 *
 * 6C INSB    Input byte from I/O port specified in DX into memory location specified with ES:(E)DI
 * 6D INSW    Input word from I/O port specified in DX into memory location specified in ES:(E)DI
 * 6D INSD    Input doubleword from I/O port specified in DX into memory location specified in ES:(E)DI
 */
BOOL OPCHndlrSysIO_INS(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    // 004011CC: 6D        ins        dword ptr es:[edi],dx

    WCHAR *pwszxDI = RegStrFromAddrSize(REGCODE_DI);

    // {byte|word|dword} ptr es:[(e)di]
    wsprintfW(insCurIns.wszCurInsStrDes, L"%ses:[%s]", PtrStrFromOperandSize(), pwszxDI);

    // source is always dx: port address is in dx
    lstrcpyW(insCurIns.wszCurInsStrSrc, awszRegCodes16[REGCODE_DX]);
    insCurIns.fSrcStrSet = TRUE;
    insCurIns.fDesStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** OUTS **
 * 6E    OUTS DX, m8     Output byte from memory location specified in DS:(E)SI to I/O port specified in DX
 * 6F    OUTS DX, m16    Output word from memory location specified in DS:(E)SI to I/O port specified in DX
 * 6F    OUTS DX, m32    Output doubleword from memory location specified in DS:(E)SI to I/O port specified in DX
 */
BOOL OPCHndlrSysIO_OUTS(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    WCHAR *pwszxDI = RegStrFromAddrSize(REGCODE_DI);

    // {byte|word|dword} ptr ds:[(e)di]
    wsprintfW(insCurIns.wszCurInsStrSrc, L"%sds:[%s]", PtrStrFromOperandSize(), pwszxDI);

    // destination is always dx: port address is in dx
    lstrcpyW(insCurIns.wszCurInsStrDes, awszRegCodes16[REGCODE_DX]);
    insCurIns.fSrcStrSet = TRUE;
    insCurIns.fDesStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** WAIT **
 * 9B    WAIT    Check pending unmasked floating-point exceptions.
 * FWAIT is an alternate mnemonic.
 */
BOOL OPCHndlrSysIO_WAIT(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** intx **
 * CC        INT 3       Interrupt 3-trap to debugger
 * CD ib     INT imm8    Interrupt vector number specified by immediate byte
 * CE        intO        Interrupt 4-if overflow flag is 1
 */

BOOL OPCHndlrSysIO_INT(BYTE bOpcode)    //    int3/intn/intO
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    if (bOpcode == 0xcc)
    {
        wsprintfW(wszCurInsStr, L"int");
        wsprintfW(insCurIns.wszCurInsStrSrc, L"3");
        insCurIns.fSrcStrSet = TRUE;
        dsNextState = DASM_STATE_DUMP;
    }
    else if (bOpcode == 0xcd)
    {
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        dsNextState = DASM_STATE_IMM;
    }
    else if (bOpcode == 0xce)
    {
        dsNextState = DASM_STATE_DUMP;
    }
    else
    {
        wprintf(L"OPCHndlrSysIO_int(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    return TRUE;
}

BOOL OPCHndlrSysIO_IceBP(BYTE bOpcode)    // undocumented INT1
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** IN imm **
 * E4 ib    IN AL,imm8      Input byte from imm8 I/O port address into AL
 * E5 ib    IN AX,imm8      Input byte from imm8 I/O port address into AX
 * E5 ib    IN EAX,imm8     Input byte from imm8 I/O port address into EAX
 */
BOOL OPCHndlrSysIO_IN(BYTE bOpcode)    // IN imm
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // w, opsize attrib
    if (bOpcode == 0xE4 || bOpcode == 0xE5)
    {
        wsprintfW(insCurIns.wszCurInsStrDes,  RegStrFromWbitOperandSize(REGCODE_AX));
    }
    else
    {
        wprintf(L"OPCHndlrSysIO_IN(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }
    insCurIns.fDesStrSet = TRUE;
    insCurIns.fImm = TRUE;
    insCurIns.bImmType = IMM_8BIT;
    dsNextState = DASM_STATE_IMM;
    return TRUE;
}

/*    ** OUT **
 * E6 ib    OUT imm8, AL    Output byte in AL to I/O port address imm8
 * E7 ib    OUT imm8, AX    Output word in AX to I/O port address imm8
 * E7 ib    OUT imm8, EAX   Output doubleword in EAX to I/O port address imm8
 */
BOOL OPCHndlrSysIO_OUT(BYTE bOpcode)    // OUT imm
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    BYTE bPortAddr;

    // w, opsize attrib
    if (bOpcode == 0xE6 || bOpcode == 0xE7)
    {
        wsprintfW(insCurIns.wszCurInsStrSrc, RegStrFromWbitOperandSize(REGCODE_AX));
    }
    else
    {
        wprintf(L"OPCHndlrSysIO_OUT(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }
    insCurIns.fSrcStrSet = TRUE;

    // The imm8 value is the source!
    // This is not handled in either IMM or DUMP state
    // so we will do it here instead
    bPortAddr = *pByteInCode;
    ++pByteInCode;
    ++nBytesCurIns;
    wsprintfW(insCurIns.wszCurInsStrDes, L"%Xh", bPortAddr);
    insCurIns.fDesStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** IN xx,DX **
 * EC    IN AL,DX    Input byte from I/O port in DX into AL
 * ED    IN AX,DX    Input word from I/O port in DX into AX
 * ED    IN EAX,DX   Input doubleword from I/O port in DX into EAX
 */
BOOL OPCHndlrSysIO_INDX(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // w, opsize attrib
    if (bOpcode == 0xEC || bOpcode == 0xED)
    {
        wsprintfW(insCurIns.wszCurInsStrDes,  RegStrFromWbitOperandSize(REGCODE_AX));
    }
    else
    {
        wprintf(L"OPCHndlrSysIO_INDX(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    // source is always DX: port address is in DX
    lstrcpyW(insCurIns.wszCurInsStrSrc, awszRegCodes16[REGCODE_DX]);
    insCurIns.fDesStrSet = TRUE;
    insCurIns.fSrcStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** OUT xx,DX **
 * EE    OUT DX, AL    Output byte in AL to I/O port address in DX
 * EF    OUT DX, AX    Output word in AX to I/O port address in DX
 * EF    OUT DX, EAX   Output doubleword in EAX to I/O port address in DX
 */
BOOL OPCHndlrSysIO_OUTDX(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // w, opsize attrib
    if (bOpcode == 0xEE || bOpcode == 0xEF)
    {
        wsprintfW(insCurIns.wszCurInsStrSrc, RegStrFromWbitOperandSize(REGCODE_AX));
    }
    else
    {
        wprintf(L"OPCHndlrSysIO_OUT(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    // Dest is always DX: port address is in DX
    lstrcpyW(insCurIns.wszCurInsStrDes, awszRegCodes16[REGCODE_DX]);
    insCurIns.fDesStrSet = TRUE;
    insCurIns.fSrcStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

BOOL OPCHndlrSysIO_HLT(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

// Prefix handlers
BOOL OPCHndlrPrefix_Ovride(BYTE bOpcode)    // override prefixes
{
    // This function need not be called since prefixes are handled in 
    // PREFIX state

    wprintf(L"!! %s(): %xh\n", __FUNCTIONW__, bOpcode);

    return TRUE;
}

/*    ** LOCK **
 * F0    LOCK    Asserts LOCK# signal for duration of the accompanying instruction
 */
BOOL OPCHndlrPrefix_LOCK(BYTE bOpcode)
{
    // This function need not be called since prefixes are handled in 
    // PREFIX state

    wprintf(L"!! %s(): %xh\n", __FUNCTIONW__, bOpcode);

    return TRUE;
}

BOOL OPCHndlrPrefix_CREP(BYTE bOpcode)    // conditional repetition: REPE/REPNE
{
    // This function need not be called since prefixes are handled in 
    // PREFIX state

    wprintf(L"!! %s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

// Opcodes that may mean any one of multiple instructions

/*
 * ADD    /0
 * OR     /1
 * ADC    /2
 * SBB    /3
 * AND    /4
 * SUB    /5
 * XOR    /6
 * CMP    /7
 */
BOOL OPCHndlrMulti_8x(BYTE bOpcode)        // 0x80 - 0x83
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Check the 'reg/opcode' field of the ModRM byte
    // This is the extension to the primary opcode.
    BYTE bOpcodeEx;

    static WCHAR awszMnemonics[][MAX_OPCODE_NAME_LEN + 1] = {
                    L"add",    L"or",    L"adc",    L"sbb", L"and", L"sub", L"xor", L"cmp" };

    Util_SplitModRMByte(*pByteInCode, NULL, &bOpcodeEx, NULL);

    switch (bOpcodeEx)
    {
    case 0:    // add
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[0]);
        return OPCHndlrALU_ADD(bOpcode);

    case 2:    // adc
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[2]);
        return OPCHndlrALU_ADD(bOpcode);

    case 3:    // sbb
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[3]);
        return OPCHndlrALU_SUB(bOpcode);

    case 5:    // sub
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[5]);
        return OPCHndlrALU_SUB(bOpcode);

    case 1:    // or
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[1]);
        return OPCHndlrALU_OR(bOpcode);

    case 4:    // and
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[4]);
        return OPCHndlrALU_AND(bOpcode);

    case 6:    // xor
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[6]);
        return OPCHndlrALU_XOR(bOpcode);

    case 7:    // cmp
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[7]);
        return OPCHndlrCC_CMP(bOpcode);

    default:
        wprintf(L"OPCHndlrMulti_8x(): Invalid opcode extension %u\n", bOpcodeEx);
        break;
    }

    return FALSE;
}

/*
 *    ** DIV **
 * F6 /6    DIV r/m8
 * F7 /6    DIV r/m16
 * F7 /6    DIV r/m32
 *    ** IDIV **
 * F6 /7    IDIV r/m8
 * F7 /7    IDIV r/m16
 * F7 /7    IDIV r/m32
 *    ** IMUL **
 * F6 /5 IMUL r/m8
 * F7 /5 IMUL r/m16
 * F7 /5 IMUL r/m32
 *    ** MUL **
 * F6 /4    MUL r/m8
 * F7 /4    MUL r/m16
 * F7 /4    MUL r/m32
 *    ** NEG **
 * F6 /3    NEG r/m8
 * F7 /3    NEG r/m16
 * F7 /3    NEG r/m32
 *    ** NOT **
 * F6 /2    NOT r/m8
 * F7 /2    NOT r/m16
 * F7 /2    NOT r/m32
 *    ** TEST **
 * F6 /0 ib     TEST r/m8,imm8
 * F7 /0 iw     TEST r/m16,imm16
 * F7 /0 id     TEST r/m32,imm32
 */
BOOL OPCHndlrMulti_fx(BYTE bOpcode)        // 0xf6 and 0xf7
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Check the 'reg/opcode' field of the ModRM byte
    // This is the extension to the primary opcode.
    BYTE bOpcodeEx;

    static WCHAR awszMnemonics[][MAX_OPCODE_NAME_LEN + 1] = {
                    L"test", L"", L"not", L"neg", L"mul", L"imul", L"div", L"idiv" };

    Util_SplitModRMByte(*pByteInCode, NULL, &bOpcodeEx, NULL);

    switch (bOpcodeEx)
    {
    case 0:
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[0]);
        return OPCHndlrCC_TEST(bOpcode);

    case 2:
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[2]);
        return OPCHndlrALU_NOT(bOpcode);

    case 3:
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[3]);
        return OPCHndlrALU_NEG(bOpcode);

    case 4:
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[4]);
        return OPCHndlrALU_MUL(bOpcode);

    case 5:
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[5]);
        return OPCHndlrALU_MUL(bOpcode);

    case 6:
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[6]);
        return OPCHndlrALU_DIV(bOpcode);

    case 7:
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[7]);
        return OPCHndlrALU_DIV(bOpcode);

    default:
        wprintf(L"OPCHndlrMulti_fx(): Invalid opcode extension %u\n", bOpcodeEx);
        break;
    }

    return FALSE;
}

/*
 *    ** INC **
 * FE /0    INC r/m8
 *    ** DEC **
 * FE /1    DEC r/m8
 */
BOOL OPCHndlrMulti_IncDec(BYTE bOpcode)    // 0xfe: INC/DEC
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Check the 'reg/opcode' field of the ModRM byte
    // This is the extension to the primary opcode.
    BYTE bOpcodeEx;

    Util_SplitModRMByte(*pByteInCode, NULL, &bOpcodeEx, NULL);

    if (bOpcodeEx == 0)
    {
        wsprintfW(wszCurInsStr, L"%s", L"inc");
        return OPCHndlrALU_INC(bOpcode);
    }
    else if (bOpcodeEx == 1)
    {
        wsprintfW(wszCurInsStr, L"%s", L"dec");
        return OPCHndlrALU_DEC(bOpcode);
    }
    else
    {
        wprintf(L"OPCHndlrMulti_IncDec(): Invalid opcode extension %u\n", bOpcodeEx);
    }

    return FALSE;
}

/*
 *    ** CALL **
 * FF /2    CALL r/m16
 * FF /2    CALL r/m32
 * FF /3    CALL m16:16
 * FF /3    CALL m16:32
 *    ** DEC **
 * FF /1    DEC r/m16
 * FF /1    DEC r/m32
 *    ** INC **
 * FF /0    INC r/m16
 * FF /0    INC r/m32
 *    ** JMP **
 * FF /4    JMP r/m16
 * FF /4    JMP r/m32
 * FF /5    JMP m16:16
 * FF /5    JMP m16:32
 *    ** PUSH **
 * FF /6    PUSH r/m16
 * FF /6    PUSH r/m32
 */
BOOL OPCHndlrMulti_FF(BYTE bOpcode)        // 0xff
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Check the 'reg/opcode' field of the ModRM byte
    // This is the extension to the primary opcode.
    BYTE bOpcodeEx;

    static WCHAR awszMnemonics[][MAX_OPCODE_NAME_LEN + 1] = {
                    L"inc", L"dec", L"call", L"call", L"jmp", L"jmp", L"push" };

    Util_SplitModRMByte(*pByteInCode, NULL, &bOpcodeEx, NULL);

    switch (bOpcodeEx)
    {
    case 0:
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[0]);
        return OPCHndlrALU_INC(bOpcode);

    case 1:
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[1]);
        return OPCHndlrALU_DEC(bOpcode);

    case 2:
    case 3:
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[3]);
        return OPCHndlrCC_CALL(bOpcode);

    case 4:
    case 5:
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[5]);
        return OPCHndlrCC_JUMP(bOpcode);

    case 6:
        wsprintfW(wszCurInsStr, L"%s", awszMnemonics[6]);
        return OPCHndlrStack_PUSH(bOpcode);

    default:
        wprintf(L"OPCHndlrMulti_FF(): Invalid opcode extension %u\n", bOpcodeEx);
        break;
    }

    return FALSE;
}

/*    ** NOP **
 * 90    NOP
 * The NOP instruction is an alias mnemonic for the XCHG (E)AX, (E)AX instruction.
 */
BOOL OPCHndlr_NOP(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** Multi-byte NOP **
 * 0F 1F /0 NOP r/m16 Multi-byte no-operation instruction.
 * 0F 1F /0 NOP r/m32 Multi-byte no-operation instruction.
 */
BOOL OPCHndlr_HNOP(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Check if 'reg' is 0??

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_DIGIT;
    insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

// Instruction I don't know yet
BOOL OPCHndlrUnKwn_SALC(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    wprintf(L"OPCHndlrUnKwn_SALC(): %xh\n", bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

BOOL OPCHndlrUnKwn_ICE(BYTE bOpcode)    // ?? SysIO
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    wprintf(L"OPCHndlrUnKwn_ICE(): %xh\n", bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*******************
 * FPU instructions
 *******************/

 // FPU Basic Arithmetic

 /*    ** FADD **
  * D8 /0       FADD m32 real       Add m32real to ST(0) and store result in ST(0)
  * DC /0       FADD m64real        Add m64real to ST(0) and store result in ST(0)
  * D8 C0+i     FADD ST(0), ST(i)   Add ST(0) to ST(i) and store result in ST(0)
  * DC C0+i     FADD ST(i), ST(0)   Add ST(i) to ST(0) and store result in ST(i)
  * DE C0+i     FADDP ST(i), ST(0)  Add ST(0) to ST(i), store result in ST(i), and pop the register stack
  * DA /0       FIADD m32int        Add m32int to ST(0) and store result in ST(0)
  * DE /0       FIADD m16int        Add m16int to ST(0) and store result in ST(0)
  */
BOOL OPCHndlrFPU_FADD(BYTE bOpcode)    // FADDP/FIADD also
{
#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Redundant code inside switch but I want to have only
    // one switch statement instead of multiple switch and if-else

    switch (bOpcode)
    {
    case 0xD8:
    {
        if (bFPUModRM >= 0xC0 && bFPUModRM <= 0xC7)    // D8 C0+i
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xC0);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// D8h

    case 0xDA:
    {
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;
    }// DAh

    case 0xDC:
    {
        if (bFPUModRM >= 0xC0 && bFPUModRM <= 0xC7)    // DC C0+i
        {
            wsprintfW(insCurIns.wszCurInsStrDes, L"st(%d)", bFPUModRM - 0xC0);
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_64BIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// DCh

    case 0xDE:
    {
        if (bFPUModRM >= 0xC0 && bFPUModRM <= 0xC7)    // DE C0+i
        {
            wsprintfW(insCurIns.wszCurInsStrDes, L"st(%d)", bFPUModRM - 0xC0);
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// DEh

    default:
    {
        wprintf(L"OPCHndlrFPU_FADD(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }
    }// switch(bOpcode)

    return TRUE;
}

/*    ** FSUB **
 * D8 /4    FSUB m32real        Subtract m32real from ST(0) and store result in ST(0)
 * DC /4    FSUB m64real        Subtract m64real from ST(0) and store result in ST(0)
 * D8 E0+i  FSUB ST(0), ST(i)   Subtract ST(i) from ST(0) and store result in ST(0)
 * DC E8+i  FSUB ST(i), ST(0)   Subtract ST(0) from ST(i) and store result in ST(i)
 * DE E8+i  FSUBP ST(i), ST(0)  Subtract ST(0) from ST(i), store result in ST(i), and pop register stack
 * DA /4    FISUB m32int        Subtract m32int from ST(0) and store result in ST(0)
 * DE /4    FISUB m16int        Subtract m16int from ST(0) and store result in ST(0)
 */
BOOL OPCHndlrFPU_FSUB(BYTE bOpcode)    // + FSUBP/FISUB/FSUBR/FSUBRP/FISUBR
{
#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Redundant code inside switch but I want to have only
    // one switch statement instead of multiple switch and if-else

    switch (bOpcode)
    {
    case 0xD8:
    {
        if (bFPUModRM >= 0xE0 && bFPUModRM <= 0xE7)    // D8 E0+i
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xE0);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// D8h

    case 0xDA:
    {
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;
    }// DAh

    case 0xDC:
    {
        if (bFPUModRM >= 0xE8 && bFPUModRM <= 0xEF)    // DC E8+i
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xE8);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// DCh

    case 0xDE:
    {
        if (bFPUModRM >= 0xE8 && bFPUModRM <= 0xEF)
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xE8);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// DEh

    default:
    {
        wprintf(L"OPCHndlrFPU_FSUB(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }
    }// switch(bOpcode)

    return TRUE;
}

/*
 * D8 /1    FMUL m32real        Multiply ST(0) by m32real and store result in ST(0)
 * DC /1    FMUL m64real        Multiply ST(0) by m64real and store result in ST(0)
 * D8 C8+i  FMUL ST(0), ST(i)   Multiply ST(0) by ST(i) and store result in ST(0)
 * DC C8+i  FMUL ST(i), ST(0)   Multiply ST(i) by ST(0) and store result in ST(i)
 * DE C8+i  FMULP ST(i), ST(0)  Multiply ST(i) by ST(0), store result in ST(i), and pop the register stack.
 * DA /1    FIMUL m32int        Multiply ST(0) by m32int and store result in ST(0)
 * DE /1    FIMUL m16int        Multiply ST(0) by m16int and store result in ST(0)
 */
BOOL OPCHndlrFPU_FMUL(BYTE bOpcode)    // + FMULP/FIMUL
{
#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Redundant code inside switch but I want to have only
    // one switch statement instead of multiple switch and if-else

    switch (bOpcode)
    {
    case 0xD8:
    {
        if (bFPUModRM >= 0xC8 && bFPUModRM <= 0xCF)
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xC8);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// D8h

    case 0xDA:
    {
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;
    }// DAh

    case 0xDC:
    {
        if (bFPUModRM >= 0xC8 && bFPUModRM <= 0xCF)
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xC8);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// DCh

    case 0xDE:
    {
        if (bFPUModRM >= 0xC8 && bFPUModRM <= 0xCF)
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xC8);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// DEh

    default:
    {
        wprintf(L"OPCHndlrFPU_FMUL(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }
    }// switch(bOpcode)

    return TRUE;
}

/*
 * D8 /6    FDIV m32real        Divide ST(0) by m32real and store result in ST(0)
 * DC /6    FDIV m64real        Divide ST(0) by m64real and store result in ST(0)
 * D8 F0+i  FDIV ST(0), ST(i)   Divide ST(0) by ST(i) and store result in ST(0)
 * DC F8+i  FDIV ST(i), ST(0)   Divide ST(i) by ST(0) and store result in ST(i)
 *
 * DE F8+i  FDIVP ST(i), ST(0)  Divide ST(i) by ST(0), store result in ST(i), and pop the register stack
 *
 * DA /6    FIDIV m32int        Divide ST(0) by m32int and store result in ST(0)
 * DE /6    FIDIV m16int        Divide ST(0) by m16int and store result in ST(0)
 *
 * D8 /7    FDIVR m32real       Divide m32real by ST(0) and store result in ST(0)
 * DC /7    FDIVR m64real       Divide m64real by ST(0) and store result in ST(0)
 * D8 F8+i  FDIVR ST(0), ST(i)  Divide ST(i) by ST(0) and store result in ST(0)
 * DC F0+i  FDIVR ST(i), ST(0)  Divide ST(0) by ST(i) and store result in ST(i)
 * DE F0+i  FDIVRP ST(i), ST(0) Divide ST(0) by ST(i), store result in ST(i), and pop the register stack
 * DA /7    FIDIVR m32int       Divide m32int by ST(0) and store result in ST(0)
 * DE /7    FIDIVR m16int       Divide m16int by ST(0) and store result in ST(0)
 */
BOOL OPCHndlrFPU_FDIV(BYTE bOpcode)    // + FDIVR/FIDIV/FDIVP/FDIVRP/FIDIVR
{
#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Redundant code inside switch but I want to have only
    // one switch statement instead of multiple switch and if-else

    switch (bOpcode)
    {
    case 0xD8:
    {
        if (bFPUModRM >= 0xF0 && bFPUModRM <= 0xF7)    // FDIV
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xF0);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else if (bFPUModRM >= 0xF8 && bFPUModRM <= 0xFF)    // FDIVR
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xF8);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// D8h

    case 0xDA:
    {
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;
    }// DAh

    case 0xDC:
    {
        if (bFPUModRM >= 0xF0 && bFPUModRM <= 0xF7)    // FDIVR
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xF0);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else if (bFPUModRM >= 0xF8 && bFPUModRM <= 0xFF)    // FDIV
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xF8);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// DCh

    case 0xDE:
    {
        if (bFPUModRM >= 0xF0 && bFPUModRM <= 0xF7)    // FDIVRP
        {
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xF0);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else if (bFPUModRM >= 0xF8 && bFPUModRM <= 0xFF)    // FDIVP
        {
            wsprintfW(insCurIns.wszCurInsStrSrc,  L"st(%d)", bFPUModRM - 0xF8);
            wsprintfW(insCurIns.wszCurInsStrDes, L"st");
            insCurIns.fSrcStrSet = TRUE;
            insCurIns.fDesStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else
        {
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// DEh

    default:
    {
        wprintf(L"OPCHndlrFPU_FDIV(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }
    }// switch(bOpcode)

    return TRUE;
}

BOOL OPCHndlrFPU_FABS(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

BOOL OPCHndlrFPU_FCHS(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

BOOL OPCHndlrFPU_FSQRT(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

BOOL OPCHndlrFPU_FPREM(BYTE bOpcode)    // + FPREM1
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

BOOL OPCHndlrFPU_FRNDINT(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

BOOL OPCHndlrFPU_FXTRACT(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}


// FPU Load Constants
/*
 * D9 E8    FLD1
 * D9 E9    FLDL2T
 * D9 EA    FLDL2E
 * D9 EB    FLDPI
 * D9 EC    FLDLG2  Push log10(2) onto the FPU register stack.
 * D9 ED    FLDLN2  Push loge(2) onto the FPU register stack.
 * D9 EE    FLDZ    Push +0.0 onto the FPU register stack.
 *
 */
BOOL OPCHndlrFPU_Const(BYTE bOpcode)    // all constants
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}


// FPU Data Transfer

/*
 * D9 /0    FLD m32real     Push m32real onto the FPU register stack.
 * DD /0    FLD m64real     Push m64real onto the FPU register stack.
 * DB /5    FLD m80real     Push m80real onto the FPU register stack.
 * D9 C0+i  FLD ST(i)       Push ST(i) onto the FPU register stack.
 *
 * DF /0    FILD m16int     Push m16int onto the FPU register stack.
 * DB /0    FILD m32int     Push m32int onto the FPU register stack.
 * DF /5    FILD m64int     Push m64int onto the FPU register stack.
 *
 * DF /4    FBLD m80dec
 */
BOOL OPCHndlrFPU_FLoad(BYTE bOpcode)        // FLD/FILD/FBLD
{
#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    if (bFPUModRM >= 0xC0 && bFPUModRM <= 0xC7)
    {
        wsprintfW(insCurIns.wszCurInsStrSrc,  L"st(%d)", bFPUModRM - 0xC0);
        insCurIns.fSrcStrSet = TRUE;
        dsNextState = DASM_STATE_DUMP;
    }
    else
    {
        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        switch (bOpcode)
        {
        case 0xDF:    // 16bit/64bit/80bit
        {
            if (bFPUReg == 0)
                insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;
            else if (bFPUReg == 5)
                insCurIns.bOperandSizeSrc = OPERANDSIZE_64BIT;
            else if (bFPUReg == 4)
                insCurIns.bOperandSizeSrc = OPERANDSIZE_80BIT;
            else
            {
                wprintf(L"OPCHndlrFPU_FLoad(): Invalid 'reg' field %xh\n", bFPUReg);
                return FALSE;
            }
            break;
        }

        case 0xD9:    // m32real
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            break;
        }

        case 0xDD:    // m64real
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_64BIT;
            break;
        }

        case 0xDB:
        {
            if (bFPUReg == 0)
                insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            else if (bFPUReg == 5)
                insCurIns.bOperandSizeSrc = OPERANDSIZE_80BIT;
            else
            {
                wprintf(L"OPCHndlrFPU_FLoad(): Invalid 'reg' field %xh\n", bFPUReg);
                return FALSE;
            }
            break;
        }

        default:
        {
            wprintf(L"OPCHndlrFPU_FStore(): Invalid opcode %xh\n", bOpcode);
            return FALSE;
        }

        }// switch(bOpcode)
    }

    return TRUE;
}

/*    ** Store **
 * D9 /2    FST m32real        Copy ST(0) to m32real
 * DD /2    FST m64real        Copy ST(0) to m64real
 * DD D0+i    FST ST(i)        Copy ST(0) to ST(i)
 * D9 /3    FSTP m32real    Copy ST(0) to m32real and pop register stack
 * DD /3    FSTP m64real    Copy ST(0) to m64real and pop register stack
 * DB /7    FSTP m80real    Copy ST(0) to m80real and pop register stack
 * DD D8+i    FSTP ST(i)        Copy ST(0) to ST(i) and pop register stack
 *
 * DF /2 FIST m16int    Store ST(0) in m16int
 * DB /2 FIST m32int    Store ST(0) in m32int
 * DF /3 FISTP m16int    Store ST(0) in m16int and pop register stack
 * DB /3 FISTP m32int    Store ST(0) in m32int and pop register stack
 * DF /7 FISTP m64int    Store ST(0) in m64int and pop register stack
 *
 * DF /6 FBSTP m80bcd    Store ST(0) in m80bcd and pop ST(0).
 */
BOOL OPCHndlrFPU_FStore(BYTE bOpcode)        // FST/FSTP/FIST/FISTP/FBSTP
{
#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    if (bFPUModRM >= 0xD0 && bFPUModRM <= 0xD7)
    {
        // FST st(i)
        wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xD0);
        insCurIns.fSrcStrSet = TRUE;
        dsNextState = DASM_STATE_DUMP;
    }
    else if (bFPUModRM >= 0xD8 && bFPUModRM <= 0xDF)
    {
        // FSTP st(i)
        wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bFPUModRM - 0xD8);
        insCurIns.fSrcStrSet = TRUE;
        dsNextState = DASM_STATE_DUMP;
    }
    else
    {
        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        switch (bOpcode)
        {
        case 0xD9:
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            break;

        case 0xDD:
            insCurIns.bOperandSizeSrc = OPERANDSIZE_64BIT;
            break;

        case 0xDB:
            if (bFPUReg == 7)
                insCurIns.bOperandSizeSrc = OPERANDSIZE_80BIT;
            else    // DB /2 and DB /3
                insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;

            break;

        case 0xDF:
            if (bFPUReg == 7)
                insCurIns.bOperandSizeSrc = OPERANDSIZE_64BIT;
            else if (bFPUReg == 6)    // fbstp /6
                insCurIns.bOperandSizeSrc = OPERANDSIZE_80BIT;
            else    // DF /2 and DF /3
                insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;
            break;

        default:
        {
            wprintf(L"OPCHndlrFPU_FStore(): Invalid opcode %xh\n", bOpcode);
            return FALSE;
        }

        }// switch(bFPUReg)
    }

    return TRUE;
}

/*    ** FXCH **
 * D9 C8+i    FXCH ST(i)    Exchange the contents of ST(0) and ST(i)
 * D9 C9    FXCH        Exchange the contents of ST(0) and ST(1)
 */
BOOL OPCHndlrFPU_FXCH(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    ASSERT(bFPUModRM >= 0xC8 && bFPUModRM <= 0xCF);    // always??

    BYTE bFPUDataReg = bFPUModRM - 0xC8;

    wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%x)", bFPUDataReg);
    insCurIns.fSrcStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** FCMOVcc **
 * DA C0+i FCMOVB ST(0), ST(i)        Move if below (CF=1)
 * DA C8+i FCMOVE ST(0), ST(i)        Move if equal (ZF=1)
 * DA D0+i FCMOVBE ST(0), ST(i)        Move if below or equal (CF=1 or ZF=1)
 * DA D8+i FCMOVU ST(0), ST(i)        Move if unordered (PF=1)
 * DB C0+i FCMOVNB ST(0), ST(i)        Move if not below (CF=0)
 * DB C8+i FCMOVNE ST(0), ST(i)        Move if not equal (ZF=0)
 * DB D0+i FCMOVNBE ST(0), ST(i)    Move if not below or equal (CF=0 and ZF=0)
 * DB D8+i FCMOVNU ST(0), ST(i)        Move if not unordered (PF=0)
 */
BOOL OPCHndlrFPU_FCMOV(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    BYTE bSrcReg;

    // Determine the source FPU data register
    if (bFPUModRM < 0xC8)                // C0+i
        bSrcReg = bFPUModRM - 0xC0;
    else if (bFPUModRM < 0xD0)            // C8+i
        bSrcReg = bFPUModRM - 0xC8;
    else if (bFPUModRM < 0xD8)            // D0+i
        bSrcReg = bFPUModRM - 0xD0;
    else                                // D8+i
        bSrcReg = bFPUModRM - 0xD8;

    // source is st(i) and destination is st always
    wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bSrcReg);
    wsprintfW(insCurIns.wszCurInsStrDes, L"st");
    insCurIns.fSrcStrSet = TRUE;
    insCurIns.fDesStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}


// FPU Compare/Classify

/*    ** FCOM **
 * D8 /2    FCOM m32real    Compare ST(0) with m32real.
 * DC /2    FCOM m64real    Compare ST(0) with m64real.
 * D8 D0+i    FCOM ST(i)        Compare ST(0) with ST(i).
 * D8 /3    FCOMP m32real    Compare ST(0) with m32real and pop register stack.
 * DC /3    FCOMP m64real    Compare ST(0) with m64real and pop register stack.
 * D8 D8+i    FCOMP ST(i)        Compare ST(0) with ST(i) and pop register stack.
 * DE D9    FCOMPP            Compare ST(0) with ST(1) and pop register stack twice.
 *
 * DB F0+i FCOMI ST, ST(i) Compare ST(0) with ST(i) and set status flags accordingly
 * DF F0+i FCOMIP ST, ST(i) Compare ST(0) with ST(i), set status flags accordingly, and pop register stack
 * DB E8+i FUCOMI ST, ST(i) Compare ST(0) with ST(i), check for ordered values, and set status flags accordingly
 * DF E8+i FUCOMIP ST, ST(i) Compare ST(0) with ST(i), check for ordered values, set status flags accordingly, and pop register stack
 *
 * DD E0+i    FUCOM ST(i)        Compare ST(0) with ST(i)
 * DD E8+i    FUCOMP ST(i)    Compare ST(0) with ST(i) and pop register stack
 * DA E9    FUCOMPP            Compare ST(0) with ST(1) and pop register stack twice
 */
BOOL OPCHndlrFPU_FCmpReal(BYTE bOpcode)    // FCOM,P,PP/FUCOM,P,PP/FCOMI,IP
{
#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    BYTE bSrcReg;

    switch (bOpcode)
    {
    case 0xDA:
        dsNextState = DASM_STATE_DUMP;
        break;

    case 0xD8:
    {
        if (bFPUModRM >= 0xD0 && bFPUModRM <= 0xD7)        // D0+i
        {
            bSrcReg = bFPUModRM - 0xD0;
            wsprintfW(insCurIns.wszCurInsStrSrc,  L"st(%d)", bSrcReg);
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else if (bFPUModRM >= 0xD8 && bFPUModRM <= 0xDF)    // D8+i
        {
            bSrcReg = bFPUModRM - 0xD8;
            wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bSrcReg);
            insCurIns.fSrcStrSet = TRUE;
            dsNextState = DASM_STATE_DUMP;
        }
        else //if(bFPUReg == 2 || bFPUReg == 3)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            insCurIns.fSpecialInstruction = TRUE;
            insCurIns.fModRM = TRUE;
            insCurIns.bModRMType = MODRM_TYPE_DIGIT;
            dsNextState = DASM_STATE_MOD_RM;
        }
        break;
    }// D8h

    case 0xDB:
    case 0xDF:
    {
        if (bFPUModRM <= 0xF7)    // F0+i
            bSrcReg = bFPUModRM - 0xF0;
        else                    // E8+i
            bSrcReg = bFPUModRM - 0xE8;

        wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bSrcReg);
        wsprintfW(insCurIns.wszCurInsStrDes, L"st");
        insCurIns.fSrcStrSet = TRUE;
        insCurIns.fDesStrSet = TRUE;
        dsNextState = DASM_STATE_DUMP;
        break;
    }// DBh

    case 0xDC:
        insCurIns.bOperandSizeSrc = OPERANDSIZE_64BIT;
        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 0xDD:
    {
        if (bFPUModRM <= 0xE7)    // E0+i
            bSrcReg = bFPUModRM - 0xE0;
        else                    // E8+i
            bSrcReg = bFPUModRM - 0xE8;
        wsprintfW(insCurIns.wszCurInsStrSrc, L"st(%d)", bSrcReg);
        insCurIns.fSrcStrSet = TRUE;
        dsNextState = DASM_STATE_DUMP;
        break;
    }// DDh

    case 0xDE:
        dsNextState = DASM_STATE_DUMP;
        break;

    default:
        wprintf(L"OPCHndlrFPU_FCmpReal(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    return TRUE;
}

/*    ** Compare Int **
 * DE /2    FICOM m16int    Compare ST(0) with m16int
 * DA /2    FICOM m32int    Compare ST(0) with m32int
 * DE /3    FICOMP m16int    Compare ST(0) with m16int and pop stack register
 * DA /3    FICOMP m32int    Compare ST(0) with m32int and pop stack register
 */
BOOL OPCHndlrFPU_FCmpInts(BYTE bOpcode)    // FICOM,P
{
#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    /*
     * Opcode DE has 16bit int memory operand
     * Opcode DA has 32bit int memory operand
     */
    if (bOpcode == 0xDE)
        insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;
    else if (bOpcode == 0xDA)
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
    else
    {
        wprintf(L"OPCHndlrFPU_FCmpInts(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_DIGIT;
    dsNextState = DASM_STATE_MOD_RM;

    return TRUE;
}

/*    ** FTST **
 * D9 E4    FTST    Compare ST(0) with 0.0.
 */
BOOL OPCHndlrFPU_FTST(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** FXAM **
 * D9 E5    FXAM    Classify value or number in ST(0)
 */
BOOL OPCHndlrFPU_FXAM(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}


// FPU Trigonometric

/*    ** FPU Trigonometric Instructions *
 * D9 F2    FPTAN    Replace ST(0) with its tangent and push 1 onto the FPU stack.
 * D9 F3    FPATAN    Replace ST(1) with arctan(ST(1)/ST(0)) and pop the register stack.
 * D9 FB    FSINCOS Compute the sine and cosine of ST(0); replace ST(0)
 *                    with the sine, and push the cosine onto the register stack.
 * D9 FE    FSIN    Replace ST(0) with its sine.
 * D9 FF    FCOS    Replace ST(0) with its cosine
 */
BOOL OPCHndlrFPU_Trig(BYTE bOpcode)    // FSIN/FCOS/FSINCOS/FPTAN/FPATAN
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

// FPU Log, Exp, Scale
/*
 * D9 F0    F2XM1    Replace ST(0) with (2^ST(0) -�1)
 * D9 F1    FYL2X
 * D9 F9    FYL2XP1
 * D9 FD    FSCALE    Scale ST(0) by ST(1).
 */
BOOL OPCHndlrFPU_LgExSc(BYTE bOpcode)    // FYL2X/FYL2XP1/F2XM1/FSCALE
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

// FPU Control

/* Control instructions that have no operands
 *
 * DB E3    FNINIT
 * DB E2    FNCLEX
 * D9 F7    FINCSTP
 * D9 F6    FDECSTP
 *
 */
BOOL OPCHndlrFPU_Ctl(BYTE bOpcode)        // No operands
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/* Control instructions that have an operand
 *
 * D9 /5    FLDCW m2byte
 * D9 /7    FNSTCW m2byte
 * DD /7    FNSTSW m2byte
 * DF E0    FNSTSW AX
 * D9 /4    FLDENV m14/28byte
 * D9 /6    FNSTENV m14/28byte
 * DD /4    FRSTOR m94/108byte
 * DD /6    FNSAVE m94/108byte
 * DD C0+i    FFREE ST(i)
 */
BOOL OPCHndlrFPU_CtlOp(BYTE bOpcode)    // With operands
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // First, handle instructions that do not have a memory operand
    // FNSTSW
    if (bFPUModRM == 0xE0)
    {
        wsprintfW(insCurIns.wszCurInsStrSrc, awszRegCodes16[REGCODE_AX]);
        insCurIns.fSrcStrSet = TRUE;
        dsNextState = DASM_STATE_DUMP;
        return TRUE;
    }

    // FFREE
    if (bFPUModRM >= 0xC0 && bFPUModRM <= 0xC7)
    {
        BYTE bFPUDataReg = bFPUModRM - 0xC0;
        wsprintfW(insCurIns.wszCurInsStrSrc,  L"st(%d)", bFPUDataReg);
        insCurIns.fSrcStrSet = TRUE;
        dsNextState = DASM_STATE_DUMP;
        return TRUE;
    }

    // There is an operand in the ModRM byte
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_DIGIT;
    insCurIns.fSpecialInstruction = TRUE;

    // Determine operand size. Observe that only instructions with
    // 5 & 7 in the 'reg' field have m2byte operands. We do not 
    // display a PtrStr for other operand sizes.
    if (bFPUReg == 5 || bFPUReg == 7)
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;
    }
    // else insCurIns.bOperandSizeSrc = OPERANDSIZE_UNDEF;    // default

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

BOOL OPCHndlrFPU_FNOP(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

BOOL OPCHndlrFPU_Invalid(BYTE bOpcode)
{
#ifdef DASM_BUILD_FOR_UNITTEST
    wprintf(L"%-25s : %02X %02X\n", __FUNCTIONW__, bOpcode, bFPUModRM);
#endif
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    wprintf(L"OPCHndlrFPU_Invalid(): Invalid FPU opcode:modrm %xh:%xh\n", bOpcode, bFPUModRM);
    return FALSE;
}


/**********************************************
 * 0F xx opcodes
 *********************************************/

 // Arithmetic instructions

/*    ** XADD **
 * 0F C0    /r    XADD r/m8,r8    Exchange r8 and r/m8; load sum into r/m8.
 * 0F C1    /r    XADD r/m16,r16    Exchange r16 and r/m16; load sum into r/m16.
 * 0F C1    /r    XADD r/m32,r32    Exchange r32 and r/m32; load sum into r/m32.
 */
BOOL OPCHndlrALU_XADD(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // operand size can be determined by w-bit and op size attribute
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

// Logical instructions

// Memory

/*    ** LAR ** Load Access Rights
 * 0F 02 /r        LAR r16,r/m16    r16 = r/m16 masked by FF00H
 * 0F 02 /r        LAR r32,r/m32    r32 = r/m32 masked by 00FxFF00H
 */
BOOL OPCHndlrMem_LAR(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** LSL **
 * 0F 03 /r        LSL r16,r/m16    Load: r16 = segment limit, selector r/m16
 * 0F 03 /r        LSL r32,r/m32    Load: r32 = segment limit, selector r/m32)
 */
BOOL OPCHndlrMem_LSL(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // opsize attrib
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** MOV to/from control registers **
 * 0F 22 /r        MOV CR0,r32
 * 0F 22 /r        MOV CR2,r32
 * 0F 22 /r        MOV CR3,r32
 * 0F 22 /r        MOV CR4,r32
 * 0F 20 /r        MOV r32,CR0
 * 0F 20 /r        MOV r32,CR2
 * 0F 20 /r        MOV r32,CR3
 * 0F 20 /r        MOV r32,CR4
 *
 * 0F 21 /r        MOV r32, DR0-DR7
 * 0F 23 /r        MOV DR0-DR7,r32
 */
BOOL OPCHndlrMem_MOVCrDr(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    if (bOpcode == 0x22)
    {
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;

        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        insCurIns.bRegTypeSrc = REGTYPE_GREG;
        insCurIns.bRegTypeDest = REGTYPE_CREG;
        dsNextState = DASM_STATE_MOD_RM;
    }
    else if (bOpcode == 0x20)
    {
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;

        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
        insCurIns.bRegTypeSrc = REGTYPE_CREG;
        insCurIns.bRegTypeDest = REGTYPE_GREG;
        dsNextState = DASM_STATE_MOD_RM;
    }
    else if (bOpcode == 0x21)
    {
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;

        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
        insCurIns.bRegTypeSrc = REGTYPE_DREG;
        insCurIns.bRegTypeDest = REGTYPE_GREG;
        dsNextState = DASM_STATE_MOD_RM;
    }
    else if (bOpcode == 0x23)
    {
        insCurIns.fModRM = TRUE;
        insCurIns.bModRMType = MODRM_TYPE_R;

        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        insCurIns.bRegTypeSrc = REGTYPE_GREG;
        insCurIns.bRegTypeDest = REGTYPE_DREG;
        dsNextState = DASM_STATE_MOD_RM;
    }
    else
    {
        wprintf(L"OPCHndlrMem_MOVCrDr(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** BT **
 * 0F A3    BT r/m16,r16    Store selected bit in CF flag
 * 0F A3    BT r/m32,r32    Store selected bit in CF flag
 * IASD Vol2 Table B-10: ModRM type = /r
 *
 * 0F BA /4 ib    BT r/m16,imm8    Store selected bit in CF flag
 * 0F BA /4 ib    BT r/m32,imm8    Store selected bit in CF flag
 */
BOOL OPCHndlrMem_BT(BYTE bOpcode)            // Bit Test
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // There is a modRM byte in any case
    insCurIns.fModRM = TRUE;

    if (bOpcode == 0xa3)
        insCurIns.bModRMType = MODRM_TYPE_R;
    else if (bOpcode == 0xba)
    {
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
    }
    else
    {
        wprintf(L"OPCHndlrMem_BT(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** BTS **
 * 0F AB    BTS r/m16,r16    Store selected bit in CF flag and set
 * 0F AB    BTS r/m32,r32    Store selected bit in CF flag and set
 * IASD Vol2 Table B-10: ModRM type = /r
 *
 * 0F BA /5 ib    BTS r/m16,imm8 Store selected bit in CF flag and set
 * 0F BA /5 ib    BTS r/m32,imm8 Store selected bit in CF flag and set
 */
BOOL OPCHndlrMem_BTS(BYTE bOpcode)            // Bit Test and Set
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // There is a modRM byte in any case
    insCurIns.fModRM = TRUE;

    if (bOpcode == 0xab)
        insCurIns.bModRMType = MODRM_TYPE_R;
    else if (bOpcode == 0xba)
    {
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
    }
    else
    {
        wprintf(L"OPCHndlrMem_BTS(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** LSS **
 * 0F B2 /r        LSS r16,m16:16    Load SS:r16 with far pointer from memory
 * 0F B2 /r        LSS r32,m16:32    Load SS:r32 with far pointer from memory
 */
BOOL OPCHndlrMem_LSS(BYTE bOpcode)            // Load Full Pointer
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;

    insCurIns.fSpecialInstruction = TRUE;
    if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        insCurIns.bOperandSizeDes = OPERANDSIZE_16BIT;
    }
    else
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_48BIT;
        insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    }
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** BTR **
 * 0F B3    BTR r/m16,r16    Store selected bit in CF flag and clear
 * 0F B3    BTR r/m32,r32    Store selected bit in CF flag and clear
 * IASD Vol2 Table B-10: ModRM byte is present and is of type /r
 *
 * 0F BA /6 ib    BTR r/m16,imm8 Store selected bit in CF flag and clear
 * 0F BA /6 ib    BTR r/m32,imm8 Store selected bit in CF flag and clear
 */
BOOL OPCHndlrMem_BTR(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // There is a modRM byte in any case
    insCurIns.fModRM = TRUE;

    if (bOpcode == 0xb3)
        insCurIns.bModRMType = MODRM_TYPE_R;
    else if (bOpcode == 0xba)
    {
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
    }
    else
    {
        wprintf(L"OPCHndlrMem_BTR(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** LFS **
 * 0F B4 /r        LFS r16,m16:16    Load FS:r16 with far pointer from memory
 * 0F B4 /r        LFS r32,m16:32    Load FS:r32 with far pointer from memory
 */
BOOL OPCHndlrMem_LFS(BYTE bOpcode)            // Load Full Pointer
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;

    insCurIns.fSpecialInstruction = TRUE;
    if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        insCurIns.bOperandSizeDes = OPERANDSIZE_16BIT;
    }
    else
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_48BIT;
        insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    }

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** LGS **
 * 0F B5 /r        LGS r16,m16:16    Load GS:r16 with far pointer from memory
 * 0F B5 /r        LGS r32,m16:32    Load GS:r32 with far pointer from memory
 */
BOOL OPCHndlrMem_LGS(BYTE bOpcode)            // Load Full Pointer
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;

    insCurIns.fSpecialInstruction = TRUE;
    if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        insCurIns.bOperandSizeDes = OPERANDSIZE_16BIT;
    }
    else
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_48BIT;
        insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    }

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** MOVZX **
 * 0F B6 /r        MOVZX r16,r/m8        Move byte to word with zero-extension
 * 0F B6 /r        MOVZX r32,r/m8        Move byte to doubleword, zero-extension
 * 0F B7 /r        MOVZX r32,r/m16        Move word to doubleword, zero-extension
 */
BOOL OPCHndlrMem_MOVZX(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    insCurIns.fSpecialInstruction = TRUE;    // different sized source and dest operands
    if (bOpcode == 0xb6)
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_8BIT;
        if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
            insCurIns.bOperandSizeDes = OPERANDSIZE_16BIT;
        else
            insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    }
    else if (bOpcode == 0xb7)
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;
        insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    }
    else
    {
        wprintf(L"OPCHndlrMem_MOVSX(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** MOVSX **
 * 0F BE /r        MOVSX r16,r/m8        Move byte to word with sign-extension
 * 0F BE /r        MOVSX r32,r/m8        Move byte to doubleword, sign-extension
 * 0F BF /r        MOVSX r32,r/m16        Move word to doubleword, sign-extension
 */
BOOL OPCHndlrMem_MOVSX(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    insCurIns.fSpecialInstruction = TRUE;    // different sized source and dest operands
    if (bOpcode == 0xbe)
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_8BIT;
        if (insCurIns.wPrefixTypes & PREFIX_OPSIZE)
            insCurIns.bOperandSizeDes = OPERANDSIZE_16BIT;
        else
            insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    }
    else if (bOpcode == 0xbf)
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;
        insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    }
    else
    {
        wprintf(L"OPCHndlrMem_MOVSX(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** BTC **
 * 0F BB    BTC r/m16,r16    Store selected bit in CF flag and complement
 * 0F BB    BTC r/m32,r32    Store selected bit in CF flag and complement
 * IASD Vol2 Table B-10: ModRM byte is present and is of type /r
 *
 * 0F BA /7 ib    BTC r/m16,imm8    Store selected bit in CF flag and complement
 * 0F BA /7 ib    BTC r/m32,imm8    Store selected bit in CF flag and complement
 */
BOOL OPCHndlrMem_BTC(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // There is a modRM byte in any case
    insCurIns.fModRM = TRUE;

    if (bOpcode == 0xbb)
        insCurIns.bModRMType = MODRM_TYPE_R;
    else if (bOpcode == 0xba)
    {
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
    }
    else
    {
        wprintf(L"OPCHndlrMem_BTC(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** BSF **
 * 0F BC    BSF r16,r/m16    Bit scan forward on r/m16
 * 0F BC    BSF r32,r/m32    Bit scan forward on r/m32
 * IASD Vol2 Table B-10: ModRM byte is present and is of type /r
 */
BOOL OPCHndlrMem_BSF(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Destination is always a register but the opcode has d-bit = 0
    // So force d-bit=1
    insCurIns.bDBit = 1;
    insCurIns.fWBitAbsent = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** BSR **
 * 0F BD    BSR r16,r/m16    Bit scan reverse on r/m16
 * 0F BD    BSR r32,r/m32    Bit scan reverse on r/m32
 * IASD Vol2 Table B-10: ModRM byte is present and is of type /r
 */
BOOL OPCHndlrMem_BSR(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** BSWAP **
 * 0F C8+rd        BSWAP    r32 Reverses the byte order of a 32-bit register.
 */
BOOL OPCHndlrMem_ByteSWAP(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Get the register name index
    BYTE bReg = bOpcode - 0xC8;

    wsprintfW(insCurIns.wszCurInsStrSrc, awszRegCodes32[bReg]);
    insCurIns.fSrcStrSet = TRUE;
    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

// Control flow and Conditional

/* ** CMOVxx **
 * All have ModRM bytes and mnemonics are already written into
 * wszCurInsStr.
 * Operand size is 16/32bit depending on operandsize prefix.
 */
BOOL OPCHndlrCC_CMOV(BYTE bOpcode)        // Conditional move
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** SETxx **
 * IASD does not indicate the presence of a ModRM byte for these instructions
 * but the disassembly by dumpbin shows the usage of the ModRM byte and it
 * appears to be using only the Mod+RM bits.
 * Examples:
 *   00401129: 0F 9F 45 EB        setg        byte ptr [ebp-15h]
 *   0040112D: 0F 9F C0           setg        al
 *   00401130: 0F 9F C1           setg        cl
 *   00401133: 0F 92 C1           setb        cl
 *
 * Update: Table B-10 IASD Vol2 shows this is true. Reg = 000.
 */
BOOL OPCHndlrCC_SETxx(BYTE bOpcode)    // SET byte on condition
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_DIGIT;    // do not consider reg bits

    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.bOperandSizeSrc = OPERANDSIZE_8BIT;    // 8bit always

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** CMPXCHG **
 * 0F B0 /r        CMPXCHG r/m8,r8        Compare AL with r/m8. If equal, ZF is set and r8 is loaded
 *                                    into r/m8. Else, clear ZF and load r/m8 into AL.
 * 0F B1 /r        CMPXCHG r/m16,r16    Compare AX with r/m16. If equal, ZF is set and r16 is
 *                                    loaded into r/m16. Else, clear ZF and load r/m16 into AL
 * 0F B1 /r        CMPXCHG r/m32,r32    Compare EAX with r/m32. If equal, ZF is set and r32 is
 *                                    loaded into r/m32. Else, clear ZF and load r/m32 into AL
 *
 * 0F C7 /1 m64        CMPXCHG8B m64    Compare EDX:EAX with m64. If equal, set ZF and load
 *                                    ECX:EBX into m64. Else, clear ZF and load m64 into
 *                                    EDX:EAX.
 */
BOOL OPCHndlrCC_CmpXchg(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    if (bOpcode == 0xb0 || bOpcode == 0xb1)
        insCurIns.bModRMType = MODRM_TYPE_R;
    else if (bOpcode == 0xc7)
    {
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        insCurIns.fSpecialInstruction = TRUE;
        insCurIns.bOperandSizeSrc = OPERANDSIZE_64BIT;
    }

    insCurIns.fModRM = TRUE;
    dsNextState = DASM_STATE_MOD_RM;

    return TRUE;
}

// Sys or IO instructions

/*    ** **
 * 0F 00 /2        LLDT r/m16        Load segment selector r/m16 into LDTR
 * 0F 00 /3        LTR r/m16        Load r/m16 into task register
 * 0F 00 /0        SLDT r/m16        Stores segment selector from LDTR in r/m16
 * 0F 00 /0        SLDT r/m32        Store segment selector from LDTR in low-order 16 bits of r/m32
 * 0F 00 /1        STR r/m16        Stores segment selector from TR in r/m16
 * 0F 00 /4        VERR r/m16        Set ZF=1 if segment specified with r/m16 can be read
 * 0F 00 /5        VERW r/m16        Set ZF=1 if segment specified with r/m16 can be written
 */
BOOL OPCHndlrSysIO_LdtTrS(BYTE bOpcode)    // 0f 00 LLDT/SLDT/LTR/...
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    BYTE bReg;
    static WCHAR awszMnemonics[][MAX_OPCODE_NAME_LEN + 1] =
    { L"sldt", L"str", L"lldt", L"ltr", L"verr", L"verw" };

    Util_SplitModRMByte(*pByteInCode, NULL, &bReg, NULL);
    ASSERT(bReg >= 0 && bReg <= 5);    // todo: ASSERT_ALWAYS?
    wsprintfW(wszCurInsStr, L"%s", awszMnemonics[bReg]);

    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;    // even SLDT uses only 16bits
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_DIGIT;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** **
 * 0F 01 /6        LMSW r/m16        Loads r/m16 in machine status word of CR0
 * 0F 01 /7        INVLPG m        Invalidate TLB Entry for page that contains m;
 *                                m is a 16/32bit operand in memory. (opsize attrib may be used).
 * 0F 01 /4        SMSW r/m16        Store machine status word to r/m16
 * 0F 01 /4        SMSW r32/m16    Store machine status word in low-order 16 bits of r32/m16; high-order 16 bits of r32 are undefined
 *
 * ** The destination operand specifies a 6-byte memory location **
 * 0F 01 /0        SGDT m            Store GDTR to m
 * 0F 01 /1        SIDT m            Store IDTR to m
 * 0F 01 /2        LGDT m16&32        Load m into GDTR
 * 0F 01 /3        LIDT m16&32        Load m into IDTR
 *
 */
BOOL OPCHndlrSysIO_GdtIdMsw(BYTE bOpcode)    // 0f 01 LGDT/SGDT/LIDT/...
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    BYTE bReg;
    static WCHAR awszMnemonics[][MAX_OPCODE_NAME_LEN + 1] =
    { L"sgdt", L"sidt", L"lgdt", L"lidt", L"smsw", L"!", L"lmsw", L"invlpg" };

    Util_SplitModRMByte(*pByteInCode, NULL, &bReg, NULL);
    ASSERT(bReg != 5 && bReg >= 0 && bReg <= 7);
    wsprintfW(wszCurInsStr, L"%s", awszMnemonics[bReg]);
    switch (bReg)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        insCurIns.bOperandSizeSrc = OPERANDSIZE_48BIT;
        break;

    case 4:
    case 6:
        insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;
        break;

    case 7:
        insCurIns.bOperandSizeSrc = OPERANDSIZE_UNDEF;
        break;

    default:
        wprintf(L"OPCHndlrSysIO_GdtIdMsw(): Invalid opcode extension %u\n", bReg);
        return FALSE;
    }// switch(bReg)

    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_DIGIT;
    dsNextState = DASM_STATE_MOD_RM;

    return TRUE;
}

/*    ** CLTS **
 * 0F 06    CLTS    Clears TS flag in CR0
 */
BOOL OPCHndlrSysIO_CLTS(BYTE bOpcode)        // Clear Task-Switched Flag in CR0
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

BOOL OPCHndlrSysIO_INVD(BYTE bOpcode)        // Invalidate internal caches
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** WBINVD **
 * 0F 09    WBINVD
 */
BOOL OPCHndlrSysIO_WBINVD(BYTE bOpcode)        // write-back and invalidate cache
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** UD2 **
 * 0B    UD2        Raise invalid opcode exception
 */
BOOL OPCHndlrSysIO_UD2(BYTE bOpcode)        // undefined instruction
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** WRMSR **
 * 0F 30    WRMSR
 */
BOOL OPCHndlrSysIO_WRMSR(BYTE bOpcode)        // Write To Model Specific Register
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** RDTSC **
 * 0F 31    RDTSC
 */
BOOL OPCHndlrSysIO_RDTSC(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** RDMSR **
 * 0F 32    RDMSR    Load MSR specified by ECX into EDX:EAX
 */
BOOL OPCHndlrSysIO_RDMSR(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** RDPMC **
 * 0F 33    RDPMC
 */
BOOL OPCHndlrSysIO_RDPMC(BYTE bOpcode)        // Read Performance Monitoring Counters
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** SYSENTER **
 * 0F, 34    SYSENTER    Transition to System Call Entry Point
 */
BOOL OPCHndlrSysIO_SYSENTER(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** SYSEXIT **
 * 0F, 35    SYSEXIT        Transition from System Call Entry Point
 */
BOOL OPCHndlrSysIO_SYSEXIT(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** PREFETCHxx **
 *
 */
BOOL OPCHndlrSysIO_Prefetch(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    return FALSE;
}

/*    ** CPUID **
 * 0F A2    CPUID
 */
BOOL OPCHndlrSysIO_CPUID(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** RSM **
 * 0F AA    RSM        Resume operation of interrupted program
 */
BOOL OPCHndlrSysIO_RSM(BYTE bOpcode)        // Resume from System Mgmt mode
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

// Opcodes that may mean any one of multiple instructions

/*
 *    ** BT **
 * 0F BA /4 ib    BT r/m16,imm8
 * 0F BA /4 ib    BT r/m32,imm8
 *    ** BTC **
 * 0F BA /7 ib    BTC r/m16,imm8
 * 0F BA /7 ib    BTC r/m32,imm8
 *    ** BTR **
 * 0F BA /6 ib    BTR r/m16,imm8
 * 0F BA /6 ib    BTR r/m32,imm8
 *    ** BTS **
 * 0F BA /5 ib    BTS r/m16,imm8
 * 0F BA /5 ib    BTS r/m32,imm8
 */
BOOL OPCHndlrMulti_BitTestX(BYTE bOpcode)        // 0f ba
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // Check the 'reg/opcode' field of the ModRM byte
    // This is the extension to the primary opcode.
    BYTE bOpcodeEx;

    Util_SplitModRMByte(*pByteInCode, NULL, &bOpcodeEx, NULL);

    switch (bOpcodeEx)
    {
    case 4:
        wsprintfW(wszCurInsStr,  L"%s", L"bt");
        return OPCHndlrMem_BT(bOpcode);

    case 5:
        wsprintfW(wszCurInsStr, L"%s", L"bts");
        return OPCHndlrMem_BTS(bOpcode);

    case 6:
        wsprintfW(wszCurInsStr, L"%s", L"btr");
        return OPCHndlrMem_BTR(bOpcode);

    case 7:
        wsprintfW(wszCurInsStr, L"%s", L"btc");
        return OPCHndlrMem_BTC(bOpcode);

    default:
        wprintf(L"OPCHndlrMulti_BitTestX(): Invalid opcode extension %u\n", bOpcodeEx);
        break;
    }

    return FALSE;
}

/*******************
 * MMX instructions
 *******************/

 /* ** PADD, PSUB, PMUL, PSAD **
  * All arithmetic instructions operate on 64bit MMX operands.
  * Dest is always an MMX register. Source is always either an
  * MMX register or a quad-word operand in memory. All of them
  * have a /r ModRM byte following the opcode.
  */
BOOL OPCHndlrMMX_PArith(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    insCurIns.bOperandSizeSrc = insCurIns.bOperandSizeDes = OPERANDSIZE_MMX64;
    insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_MMX;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*
 * 64h, 65h, 66h: PCMPGT{B,W,D}
 * 74h, 75h, 76h: PCMPEQ{B,W,D}
 * Same as arithmetic instructions.
 */
BOOL OPCHndlrMMX_PCmp(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    insCurIns.bOperandSizeSrc = insCurIns.bOperandSizeDes = OPERANDSIZE_MMX64;
    insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_MMX;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** Conversion **
 * PUNPCKLBW, PUNPCKLDW, PUNPCKLDQ are special instructions(opsize).
 *
 * 0F 60 /r PUNPCKLBW mm,mm/m32        Interleave low-order bytes from mm and mm/m64 into mm.
 * 0F 61 /r PUNPCKLWD mm,mm/m32        Interleave low-order words from mm and mm/m64 into mm.
 * 0F 62 /r PUNPCKLDQ mm,mm/m32        Interleave low-order doublewords from mm and mm/m64 into mm.
 *
 * 0F 63 /r PACKSSWB mm,mm/m64
 * 0F 6B /r PACKSSDW mm,mm/m64
 *
 * 0F 67 /r PACKUSWB mm,mm/m64
 *
 * 0F 68 /r PUNPCKHBW mm,mm/m64        Interleave high-order bytes from mm and mm/m64 into mm.
 * 0F 69 /r PUNPCKHWD mm,mm/m64        Interleave high-order words from mm and mm/m64 intomm.
 * 0F 6A /r PUNPCKHDQ mm,mm/m64        Interleave high-order doublewords from mm and mm/m64 into mm.
 */
BOOL OPCHndlrMMX_PConv(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    // src opsize is 32bits only for PUNPCKL{BW,WD,DQ}
    if (bOpcode == 0x60 || bOpcode == 0x61 || bOpcode == 0x62)
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
    else
        insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
    insCurIns.bOperandSizeDes = OPERANDSIZE_MMX64;
    insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_MMX;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/* ** P{AND,ANDN,OR,XOR} **
 * Same as arithmetic instructions.
 */
BOOL OPCHndlrMMX_PLogical(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    insCurIns.bOperandSizeSrc = insCurIns.bOperandSizeDes = OPERANDSIZE_MMX64;
    insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_MMX;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/* ** **
 * 0F F1 /r        PSLLW mm, mm/m64    Shift words in mm left by amount specified in mm/m64, while shifting in zeroes.
 * 0F F2 /r        PSLLD mm, mm/m64    Shift doublewords in mm left by amount specified in mm/m64, while shifting in zeroes.
 * 0F F3 /r        PSLLQ mm, mm/m64    Shift mm left by amount specified in mm/m64, while shifting in zeroes.
 *
 * 0F 71 /6 ib    PSLLW mm, imm8        Shift words in mm left by imm8, while shifting in zeroes.
 * 0F 72 /6 ib    PSLLD mm, imm8        Shift doublewords in mm by imm8, while shifting in zeroes.
 * 0F 73 /6 ib    PSLLQ mm, imm8        Shift mm left by Imm8, while shifting in zeroes.
 *
 * 0F D1 /r        PSRLW mm, mm/m64    Shift words in mm right by amount specified in mm/m64 while shifting in zeroes.
 * 0F D2 /r        PSRLD mm, mm/m64    Shift doublewords in mm right by amount specified in mm/m64 while shifting in zeroes.
 * 0F D3 /r        PSRLQ mm, mm/m64    Shift mm right by amount specified in mm/m64 while shifting in zeroes.

 * 0F 71 /2 ib    PSRLW mm, imm8        Shift words in mm right by imm8.
 * 0F 72 /2 ib    PSRLD mm, imm8        Shift doublewords in mm right by imm8.
 * 0F 73 /2 ib    PSRLQ mm, imm8        Shift mm right by imm8 while shifting in zeroes.
 *
 * 0F E1 /r        PSRAW mm,mm/m64        Shift words in mm right by amount specified in mm/m64 while shifting in sign bits.
 * 0F E2 /r        PSRAD mm,mm/m64        Shift doublewords in mm right by amount specified in mm/m64 while shifting in sign bits.
 *
 * 0F 71 /4 ib PSRAW mm, imm8        Shift words in mm right by imm8 while shifting in sign bits
 * 0F 72 /4 ib PSRAD mm, imm8        Shift doublewords in mm right by imm8 while shifting in sign bits.
 */
BOOL OPCHndlrMMX_PShift(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // only 71h,72h,73h opcodes have an immediate operand

    insCurIns.bDBit = 1;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    if (bOpcode == 0x71 || bOpcode == 0x72 || bOpcode == 0x73)
    {
        insCurIns.bModRMType = MODRM_TYPE_DIGIT;
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
    }
    else
        insCurIns.bModRMType = MODRM_TYPE_R;

    insCurIns.bOperandSizeSrc = insCurIns.bOperandSizeDes = OPERANDSIZE_MMX64;
    insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_MMX;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/* 0F {71,72,73} Shift instructions
 *        /2        /4        /6
 *    71    psrlw    psraw    psllw
 *    72    psrld    psrad    pslld
 *    73    psrlq            psllq
 */
BOOL OPCHndlrMMX_PMulti7x(BYTE bOpcode)    // 
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    static WCHAR awszMnemonics[][MAX_OPCODE_NAME_LEN + 1] = {
                L"psrlw", L"psraw", L"psllw",
                L"psrld", L"psrad", L"pslld",
                L"psrlq", L"!",     L"psllq" };

    BYTE bOpcodeEx;
    INT iIndex;

    // get the 'reg' field from the ModRM byte
    Util_SplitModRMByte(*pByteInCode, NULL, &bOpcodeEx, NULL);

    // TODO: For 0x0F 0x71 opcode, this assert fails because bOpcodeEx is 1
    ASSERT(bOpcodeEx == 2 || bOpcodeEx == 4 || bOpcodeEx == 6);    // assert always?

    bOpcodeEx = bOpcodeEx / 2 - 1;    // get zero-based index
    iIndex = (bOpcode - 0x71) * 3 + bOpcodeEx;
    wsprintfW(wszCurInsStr, awszMnemonics[iIndex]);

    return OPCHndlrMMX_PShift(bOpcode);
}

/*    ** MOVx **
 * 0F 6E /r    MOVD mm, r/m32        Move doubleword from r/m32 to mm.
 * 0F 7E /r    MOVD r/m32, mm        Move doubleword from mm to r/m32.
 *
 * 0F 6F /r MOVQ mm, mm/m64        Move quadword from mm/m64 to mm.
 * 0F 7F /r MOVQ mm/m64, mm        Move quadword from mm to mm/m64.
 */
BOOL OPCHndlrMMX_PMov(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    // d-bit must be 1 for load and 0 for store instructions

    //insCurIns.bDBit = 1;    // always

    switch (bOpcodeHigh)
    {
    case 0x6:    // des = mm; src = r/m32 OR mm/m64    LOAD
    {
        insCurIns.bDBit = 1;
        insCurIns.bRegTypeDest = REGTYPE_MMX;
        insCurIns.bOperandSizeDes = OPERANDSIZE_MMX64;
        if (bOpcodeLow == 0xE)
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;    // used by MODRM only if mem-loc is des
        else
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;    // used by MODRM only if mem-loc is des
            insCurIns.bRegTypeSrc = REGTYPE_MMX;    // only if des is a reg
        }
        break;
    }

    case 0x7:    // src = mm; des = r/m32 OR mm/m64    STORE
    {
        insCurIns.bDBit = 0;
        insCurIns.bRegTypeSrc = REGTYPE_MMX;
        if (bOpcodeLow == 0xE)
            insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;    // used by MODRM only if mem-loc is src
        else
        {
            insCurIns.bOperandSizeDes = OPERANDSIZE_MMX64;    // used by MODRM only if mem-loc is src
            insCurIns.bRegTypeDest = REGTYPE_MMX;    // only if des is a reg
        }
        break;
    }
    }// switch(bOpcodeHigh)

    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

BOOL OPCHndlrMMX_EMMS(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

/*    ** PSHUFW **
 * 0F 70 /r ib PSHUFW mm1, mm2/m64, imm8
 * Shuffle the words in MM2/Mem based on the encoding in imm8 and store in MM1.
 */
BOOL OPCHndlrMMX_PSHUFW(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fImm = TRUE;
    insCurIns.bImmType = IMM_8BIT;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    insCurIns.bOperandSizeSrc = insCurIns.bOperandSizeDes = OPERANDSIZE_MMX64;
    insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_MMX;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** PINSRW **
 * 0F C4 /r ib PINSRW mm,r32/m16,imm8
 * Insert the word from the lower half of r32 or from Mem16 into the
 * position in MM pointed to by imm8 without touching the other words.
 */
BOOL OPCHndlrMMX_PINSRW(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fImm = TRUE;
    insCurIns.bImmType = IMM_8BIT;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    insCurIns.bOperandSizeDes = OPERANDSIZE_MMX64;    // doesn't matter since it is a MMX register
    insCurIns.bOperandSizeSrc = OPERANDSIZE_16BIT;    // m16
    insCurIns.bRegTypeDest = REGTYPE_MMX;
    insCurIns.bRegTypeSrc = REGTYPE_GREG;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** PEXTRW **
 * 0F C5 /r ib PEXTRW r32, mm, imm8
 * Extract the word pointed to by imm8 from MM and move it to a 32-bit integer register.
 */
BOOL OPCHndlrMMX_PEXTRW(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fImm = TRUE;
    insCurIns.bImmType = IMM_8BIT;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
    insCurIns.bRegTypeDest = REGTYPE_GREG;
    insCurIns.bRegTypeSrc = REGTYPE_MMX;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** PMOVMSKB **
 * 0F D7 /r        PMOVMSKB r32, mm    Move the byte mask of MM to r32.
 */
BOOL OPCHndlrMMX_PMOVMSKB(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
    /*
     * No need to set src opsize because it is always a MMX register.
     */
    insCurIns.bRegTypeDest = REGTYPE_GREG;
    insCurIns.bRegTypeSrc = REGTYPE_MMX;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** **
 * Same as arithmetic instructions.
 */
BOOL OPCHndlrMMX_PMaxMinAvg(BYTE bOpcode)    // PMINUB/PMAXUB/PMINSW/PMAXSW/PAVGB/PVGW
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
    /*
     * No need to set des opsize because it is always a MMX register.
     * Src maybe a MMX reg/m64 mem-loc.
     */
    insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_MMX;
    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*******************
 * SSE instructions
 *******************/
 /*
  * 66 0F 5E /r    DIVPD xmm1, xmm2/m128
  */
BOOL OPCHndlrSSE_Arith(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSSEIns = TRUE;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_XMM;

    switch (bOpcode)
    {
    case 0x58:    // add
    {
        if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF3)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
           wsprintfW(wszCurInsStr, L"addss");
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF2)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
           wsprintfW(wszCurInsStr, L"addsd");
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] == OPC_PREFIX_OPSIZE)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
            wsprintfW(wszCurInsStr, L"addpd");
        }
        else
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
        break;
    }

    case 0x59:    // mul
    {
        if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF3)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            wsprintfW(wszCurInsStr, L"mulss");
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF2)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
            wsprintfW(wszCurInsStr, L"mulsd");
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] == OPC_PREFIX_OPSIZE)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
            wsprintfW(wszCurInsStr, L"mulpd");
        }
        else
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
        break;
    }

    case 0x5C:    // sub
    {
        if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF3)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            wsprintfW(wszCurInsStr, L"subss");
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF2)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
            wsprintfW(wszCurInsStr, L"subsd");
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] == OPC_PREFIX_OPSIZE)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
            wsprintfW(wszCurInsStr, L"subpd");
        }
        else
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
        break;
    }

    case 0x5E:    // div
    {
        if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF3)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            wsprintfW(wszCurInsStr, L"divss");
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF2)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
            wsprintfW(wszCurInsStr, L"divsd");
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] == OPC_PREFIX_OPSIZE)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
            wsprintfW(wszCurInsStr,  L"divpd");
        }
        else
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
        break;
    }

    default:
        wprintf(L"OPCHndlrSSE_Arith(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }// switch(bOpcode)

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;
}

/*    ** UCOMISS/COMISS/CMPPS/CMPSS **
 * 0F C2    /r ib        CMPPS xmm1,xmm2/m128,imm8
 * F3 0F C2 /r ib        CMPSS xmm1,xmm2/m32,imm8
 * F2 0F C2 /r ib        CMPSD xmm1,xmm2/m64, imm8
 * 66 0F C2 /r ib        CMPPD xmm1,xmm2/m128, imm8
 *
 *        Pseudo-Op            Implementation
 * CMPEQSS        xmm1, xmm2    CMPSS xmm1,xmm2, 0
 * CMPLTSS        xmm1, xmm2    CMPSS xmm1,xmm2, 1
 * CMPLESS        xmm1, xmm2    CMPSS xmm1,xmm2, 2
 * CMPUNORDSS    xmm1, xmm2    CMPSS xmm1,xmm2, 3
 * CMPNEQSS        xmm1, xmm2    CMPSS xmm1,xmm2, 4
 * CMPNLTSS        xmm1, xmm2    CMPSS xmm1,xmm2, 5
 * CMPNLESS        xmm1, xmm2    CMPSS xmm1,xmm2, 6
 * CMPORDSS        xmm1, xmm2    CMPSS xmm1,xmm2, 7
 *
 * 0F 2F    /r COMISS xmm1,xmm2/m32
 * 66 0F 2F /r COMISD xmm1, xmm2/m64
 * 0F 2E    /r UCOMISS xmm1,xmm2/m32
 * 66 0F 2E /r UCOMISD xmm1, xmm2/m64
 */
BOOL OPCHndlrSSE_Cmp(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSSEIns = TRUE;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_XMM;

    switch (bOpcode)
    {
    case 0x2E:    // UCOMISS/UCOMISD
        if (insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] == OPC_PREFIX_OPSIZE)
        {
            wsprintfW(wszCurInsStr, L"ucomisd");
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
        }
        else
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        break;

    case 0x2F:    // COMISS/COMISD
        if (insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] == OPC_PREFIX_OPSIZE)
        {
            wsprintfW(wszCurInsStr, L"comisd");
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
        }
        else
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        break;

    case 0xC2:    // CMPPS or CMPSS
    {
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;

        if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF3)
        {
            // CMPSS
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF2)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
        }
        else
        {
            // CMPPS or CMPPD
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
        }
        break;
    }// case 0xC2

    default:
    {
        wprintf(L"OPCHndlrSSE_Cmp(): Invalid opcode %xh\n", bOpcode);
        return FALSE;
    }

    }// switch(bOpcode)

    // set the callback function pointer
    insCurIns.fpvCallback = vCALLBACK_SSECmp;

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    dsNextState = DASM_STATE_MOD_RM;

    return TRUE;
}

/*
 * To set the instruction mnemonic once IMM state has
 * read the value of the immediate byte.
 */

void vCALLBACK_SSECmp()
{
    static WCHAR awszCMPPSMnemonics[][MAX_OPCODE_NAME_LEN + 1] =
    { L"cmpeqps", L"cmpltps", L"cmpleps", L"cmpunordps",
        L"cmpneqps", L"cmpnltps", L"cmpbleps", L"cmpordps" };

    static WCHAR awszCMPSSMnemonics[][MAX_OPCODE_NAME_LEN + 1] =
    { L"cmpeqss", L"cmpltss", L"cmpless", L"cmpunordss",
        L"cmpneqss", L"cmpnltss", L"cmpnless", L"cmpordss" };

    static WCHAR awszCMPPDMnemonics[][MAX_OPCODE_NAME_LEN + 1] =
    { L"cmpeqpd", L"cmpltpd", L"cmplepd", L"cmpunordpd",
        L"cmpneqpd", L"cmpnltpd", L"cmpnlepd", L"cmpordpd" };

    static WCHAR awszCMPSDMnemonics[][MAX_OPCODE_NAME_LEN + 1] =
    { L"cmpeqsd", L"cmpltsd", L"cmplesd", L"cmpunordsd",
        L"cmpneqsd", L"cmpnltsd", L"cmpnlesd", L"cmpordsd" };

    if (insCurIns.iImm < 0 || insCurIns.iImm > 7)
    {
        wprintf(L"vCALLBACK_SSECmp(): Invalid imm value for CMPSS/CMPPS: %d\n", insCurIns.iImm);
        return;
    }

    // Now that we have a valid imm value, set the proper instruction mnemonic
    if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF3)
    {
        // CMPSS
        lstrcpyW(wszCurInsStr, awszCMPSSMnemonics[insCurIns.iImm]);
    }
    else if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF2)
    {
        // CMPSD
        lstrcpyW(wszCurInsStr,  awszCMPSDMnemonics[insCurIns.iImm]);
    }
    else if (insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] == OPC_PREFIX_OPSIZE)
    {
        // CMPPD
        lstrcpyW(wszCurInsStr,  awszCMPPDMnemonics[insCurIns.iImm]);
    }
    else
    {
        // CMPPS
        lstrcpyW(wszCurInsStr, awszCMPPSMnemonics[insCurIns.iImm]);
    }

    // Clear fImm so that the immediate is not printed by the DUMP state
    insCurIns.fImm = FALSE;

    return;
}

/*
 * 0F 15    /r        UNPCKHPS xmm1,xmm2/m128
 * 0F 14    /r        UNPCKLPS xmm1,xmm2/m128
 *
 * 0F 2A    /r        CVTPI2PS xmm, mm/m64
 * F3 0F 2A /r        CVTSI2SS xmm, r/m32
 * 66 0F 2A /r        CVTPI2PD xmm, mm/m64
 *
 * 0F 2C    /r        CVTTPS2PI mm, xmm/m64
 * F3 0F 2C /r        CVTTSS2SI r32, xmm/m32
 *
 * 0F 2D    /r        CVTPS2PI mm, xmm/m64
 * 66 0F 2D /r        CVTPD2PI mm, xmm/m128
 * F2 0F 2D /r        CVTSD2SI r32, xmm/m64
 *
 * 0F 5A    /r        CVTPS2PD xmm1, xmm2/m64
 * 66 0F 5A /r        CVTPD2PS xmm1, xmm2/m128
 * F2 0F 5A /r        CVTSD2SS xmm1, xmm2/m64
 *
 * 0F 5B    /r        CVTDQ2PS xmm1, xmm2/m128
 * 66 0F 5B /r        CVTPS2DQ xmm1, xmm2/m128
 *
 * F3 0F E6    /r        CVTDQ2PD xmm1, xmm2/m64        ; dword ints to fp
 * F2 0F E6    /r        CVTPD2DQ xmm1, xmm2/m128    ; fp to dword ints
 * 66 0F E6    /r        CVTTPD2DQ xmm1, xmm2/m128    ; with truncation
 */
BOOL OPCHndlrSSE_Conv(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSSEIns = TRUE;
    insCurIns.fSpecialInstruction = TRUE;

    switch (bOpcode)
    {
    case 0x14:
    case 0x15:
        insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
        insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_XMM;
        break;

    case 0x2A:
        insCurIns.bRegTypeDest = REGTYPE_XMM;
        if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF3)
        {
            wsprintfW(wszCurInsStr,  L"cvtsi2ss");
            insCurIns.bRegTypeSrc = REGTYPE_GREG;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] == OPC_PREFIX_OPSIZE)
        {
            wsprintfW(wszCurInsStr, L"cvtpi2pd");
            insCurIns.bRegTypeSrc = REGTYPE_MMX;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
        }
        else
        {
            insCurIns.bRegTypeSrc = REGTYPE_MMX;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
        }
        break;

    case 0x2C:
    {
        if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF3)
        {
            // CVTTSS2SI
            insCurIns.bRegTypeDest = REGTYPE_GREG;
            insCurIns.bRegTypeSrc = REGTYPE_XMM;
            insCurIns.bOperandSizeSrc = insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
            wsprintfW(wszCurInsStr, L"cvttss2si");
        }
        else
        {
            insCurIns.bRegTypeDest = REGTYPE_MMX;
            insCurIns.bRegTypeSrc = REGTYPE_XMM;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
        }
        break;
    }

    case 0x2D:
    {
        insCurIns.bRegTypeDest = REGTYPE_MMX;
        insCurIns.bRegTypeSrc = REGTYPE_XMM;
        if (insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] == OPC_PREFIX_OPSIZE)
        {
            wsprintfW(wszCurInsStr, L"cvtpd2pi");
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF2)
        {
            // F2 0F 2D /r        CVTSD2SI r32, xmm/m64
            insCurIns.bRegTypeDest = REGTYPE_GREG;
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
            insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
            wsprintfW(wszCurInsStr, L"cvtsd2si");
        }
        else
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
        break;
    }

    case 0x5A:    // 66 0F 5A /r        CVTPD2PS xmm1, xmm2/m128
                // F2 0F 5A /r        CVTSD2SS xmm1, xmm2/m64
    {
        insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_XMM;
        if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF2)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
            wsprintfW(wszCurInsStr, L"cvtsd2ss");
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] == OPC_PREFIX_OPSIZE)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
            wsprintfW(wszCurInsStr, L"cvtpd2ps");
        }
        else    // 0F 5A    /r        CVTPS2PD xmm1, xmm2/m64
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
        break;
    }

    case 0x5B:
    {
        insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
        insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_XMM;
        if (insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] == OPC_PREFIX_OPSIZE)
            wsprintfW(wszCurInsStr, L"cvtps2dq");
        break;
    }

    case 0xE6:
    {
        insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_XMM;
        if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF2)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
            wsprintfW(wszCurInsStr, L"cvtpd2dq");
        }
        else if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF3)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_MMX64;
            wsprintfW(wszCurInsStr,  L"cvtdq2pd");
        }
        else
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
        break;
    }

    }// switch(bOpcode)

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    dsNextState = DASM_STATE_MOD_RM;

    return TRUE;
}

/*
 * SSE:
 * ANDPS/ANDNPS/ORPS/XORPS
 * MINPS/MINSS/MAXPS/MAXSS
 *
 * SSE2:
 * ANDNPD
 */
BOOL OPCHndlrSSE_Logical(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSSEIns = TRUE;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_XMM;
    if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF3)
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
    else
        insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;

    switch (bOpcode)
    {
    case 0x55:
        if (insCurIns.abPrefixes[PREFIX_INDEX_OPSIZE] == OPC_PREFIX_OPSIZE)
           wsprintfW(wszCurInsStr, L"andnpd");
        break;
    }

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    dsNextState = DASM_STATE_MOD_RM;

    return TRUE;
}

/*    ** DataTransfer **
 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 ; Some SSE data transfer instructions like MOVAPS have duplicate
 ; opcodes for load and store operations. The d-bit matters in
 ; these cases. It controls how the source and destination operands
 ; are specified in the ModRM byte.
 ; Load is memory -> register and Store is register -> memory.
 ; The only way to specify a memory location using the ModRM byte
 ; is via the Mod+RM bits. So, a load instruction will always have
 ; operands specified like Mod+RM -> Reg, i.e., des = 'reg' bits
 ; and src = 'mod+rm' bits and a store instruction will always have
 ; Reg -> Mod+RM, i.e., des = 'mod+rm' bits and src = 'reg' bits.
 ; So a SSE load instruction must be processed as if the d-bit is 1
 ; and a SSE store instruction must be processed as if the d-bit is
 ; 0.
 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 */
BOOL OPCHndlrSSE_Mov(BYTE bOpcode)
{
    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.fSSEIns = TRUE;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;

    // dbit may be of significance too: load->d=1, store->d=0
    // regtype may be MMX/XMM/GREG
    // size may be 32/64/128

    switch (bOpcode)
    {
    case 0x10:    // load:xmm/m 128/32 MOVUPS, MOVSS
    {
        insCurIns.bDBit = 1;
        insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_XMM;
        if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF3)
        {
            insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
            wsprintfW(wszCurInsStr, L"movss");
        }
        else
            insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
        break;
    }

    case 0x11:    // store:xmm/m 128/32 MOVUPS, MOVSS
    {
        insCurIns.bDBit = 0;
        insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_XMM;
        if (insCurIns.abPrefixes[PREFIX_INDEX_LOCKREP] == OPC_PREFIX_SIMDF3)
        {
            insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;
            wsprintfW(wszCurInsStr, L"movss");
        }
        else
            insCurIns.bOperandSizeDes = OPERANDSIZE_SSE128;
        break;
    }

    case 0x12:    // load:m64 MOVLPS, MOVHLPS
    {
        insCurIns.bDBit = 1;
        insCurIns.bRegTypeDest = REGTYPE_XMM;

        // if ModRM byte specifies both register operands, it is a MOVHLPS instruction
        if (*pByteInCode >= 0xC0 && *pByteInCode <= 0xFF)
        {
            insCurIns.bRegTypeSrc = REGTYPE_XMM;
            wsprintfW(wszCurInsStr, L"movhlps");
        }
        insCurIns.bOperandSizeSrc = OPERANDSIZE_64BIT;
        break;
    }

    case 0x13:    // store:m64 MOVLPS
    {
        insCurIns.bDBit = 0;
        insCurIns.bRegTypeSrc = REGTYPE_XMM;
        insCurIns.bOperandSizeDes = OPERANDSIZE_64BIT;
        break;
    }

    case 0x16:    // load:m64 MOVHPS, MOVLHPS
    {
        insCurIns.bDBit = 1;
        insCurIns.bRegTypeDest = REGTYPE_XMM;

        // if ModRM byte specifies both register operands, it is a MOVLHPS instruction
        if (*pByteInCode >= 0xC0 && *pByteInCode <= 0xFF)
        {
            insCurIns.bRegTypeSrc = REGTYPE_XMM;
            wsprintfW(wszCurInsStr,  L"movlhps");
        }
        insCurIns.bOperandSizeSrc = OPERANDSIZE_64BIT;
        break;
    }

    case 0x17:    // store:m64 MOVHPS
    {
        insCurIns.bDBit = 0;
        insCurIns.bRegTypeSrc = REGTYPE_XMM;
        insCurIns.bOperandSizeDes = OPERANDSIZE_64BIT;
        break;
    }

    case 0x28:    // load:xmm/m128 MOVAPS
    {
        insCurIns.bDBit = 1;
        insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_XMM;
        insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
        break;
    }

    case 0x29:    // store:xmm/m128 MOVAPS
    {
        insCurIns.bDBit = 0;
        insCurIns.bRegTypeSrc = insCurIns.bRegTypeSrc = REGTYPE_XMM;
        insCurIns.bOperandSizeDes = OPERANDSIZE_SSE128;
        break;
    }

    case 0x2B:    // store:m128 MOVNTPS
    {
        insCurIns.bDBit = 0;
        insCurIns.bRegTypeSrc = REGTYPE_XMM;
        insCurIns.bOperandSizeDes = OPERANDSIZE_SSE128;
        break;
    }

    case 0x50:    // r32<-xmm MOVMSKPS d=1 verified
    {
        insCurIns.bDBit = 1;
        insCurIns.bRegTypeSrc = REGTYPE_XMM;
        insCurIns.bRegTypeDest = REGTYPE_GREG;
        insCurIns.bOperandSizeDes = OPERANDSIZE_32BIT;    // required for GPR
        break;
    }

    case 0xE7:    // m64<-mm MOVNTQ d=0
    {
        insCurIns.bDBit = 0;
        insCurIns.bRegTypeSrc = REGTYPE_MMX;
        insCurIns.bOperandSizeDes = OPERANDSIZE_64BIT;
        break;
    }

    case 0xF7:    // mm<-mm MASKMOVQ d=1
    {
        insCurIns.bDBit = 1;
        insCurIns.bRegTypeSrc = insCurIns.bRegTypeDest = REGTYPE_MMX;
        break;
    }

    default:
        wprintf(L"OPCHndlrSSE_Mov(): Invalid opcode %xh\n", bOpcode);
        return FALSE;

    }// switch(bOpcode)

    dsNextState = DASM_STATE_MOD_RM;

    return TRUE;
}

/*    ** SHUFPS **
 * 0F C6 /r ib    SHUFPS xmm1,xmm2/m128,imm8        Shuffle Single.
 */
BOOL OPCHndlrSSE_SHUFPS(BYTE bOpcode)
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    insCurIns.bDBit = 1;
    insCurIns.fSSEIns = TRUE;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_XMM;
    insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;

    insCurIns.fImm = TRUE;
    insCurIns.bImmType = IMM_8BIT;

    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    dsNextState = DASM_STATE_MOD_RM;

    return TRUE;
}

/*
 * FXSAVE    /0        m512
 * FXSTOR    /1        m512
 * LDMXCSR    /2        m32
 * STMXCSR    /3        m32
 * SFENCE    /7
 */
BOOL OPCHndlrSSE_MultiAE(BYTE bOpcode)    // FXSAVE/FXRSTOR/LDMXCSR/STMXCSR/SFENCE
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    static WCHAR awszAEMnemonics[][MAX_OPCODE_NAME_LEN + 1] = {
                    L"fxsave", L"fxstor", L"ldmxcsr", L"stmxcsr",
                    L"",L"", L"", L"sfence" };

    BYTE bOpcodeEx = 0xff;

    // Get the opcode extension
    Util_SplitModRMByte(*pByteInCode, NULL, &bOpcodeEx, NULL);

    insCurIns.bDBit = 1;
    insCurIns.fSSEIns = TRUE;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_DIGIT;

    // Not performing bound checks for bOpcodeEx because I have
    // accessed the awszARMnemonics array within the switch cases
    // where the bOpcodeEx has valid values.

    switch (bOpcodeEx)
    {
    case 0:
    case 1:
        lstrcpyW(wszCurInsStr, awszAEMnemonics[bOpcodeEx]);
        // insCurIns.bOperandSizeSrc = OPERANDSIZE_UNDEF;    // m512 so no ptrstr
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 2:
    case 3:
        lstrcpyW(wszCurInsStr, awszAEMnemonics[bOpcodeEx]);
        insCurIns.bOperandSizeSrc = OPERANDSIZE_32BIT;
        dsNextState = DASM_STATE_MOD_RM;
        break;

    case 7:
        lstrcpyW(wszCurInsStr,  awszAEMnemonics[bOpcodeEx]);
        dsNextState = DASM_STATE_DUMP;
        break;

    default:
        wprintf(L"OPCHndlrSSE_MultiAE(): Invalid opcode extension %d\n", bOpcodeEx);
        return FALSE;
    }

    return TRUE;
}

/*    ** PREFETCH* **
 * 0F 18 /1    PREFETCHT0 m8        Move data specified by address closer to the processor using the t0 hint.
 * 0F 18 /2 PREFETCHT1 m8        Move data specified by address closer to the processor using the t1 hint.
 * 0F 18 /3 PREFETCHT2 m8        Move data specified by address closer to the processor using the t2 hint.
 * 0F 18 /0 PREFETCHTNTA m8        Move data specified by address closer to the processor using the nta hint.
 */
BOOL OPCHndlrSSE_Prefetch18(BYTE bOpcode)    // 0F 18 PREFETCH{T0,T1,T2,NTA}
{
    DBG_UNREFERENCED_PARAMETER(bOpcode);

    logdbg(L"%s(): %xh\n", __FUNCTIONW__, bOpcode);

    static WCHAR awszPrefMnemonics[][MAX_OPCODE_NAME_LEN + 1] = {
                                L"prefetchnta", L"prefetcht0", L"prefetcht1", L"prefetcht2" };

    BYTE bOpcodeEx;

    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_DIGIT;

    // insCurIns.bOperandSizeSrc = OPERANDSIZE_UNDEF;    // dumpbin says so

    Util_SplitModRMByte(*pByteInCode, NULL, &bOpcodeEx, NULL);

    // No need to check < 0 because bOpcodeEx is unsigned char
    if (bOpcodeEx > 3)
    {
        wprintf(L"OPCHndlrSSE_Prefetch18(): Invalid opcode extension %d\n", bOpcodeEx);
        return FALSE;
    }

    lstrcpyW(wszCurInsStr,  awszPrefMnemonics[bOpcodeEx]);
    dsNextState = DASM_STATE_MOD_RM;

    return TRUE;
}

/*    ** AES Instructions **
 * 66 0F 38 DB /r        AESIMC xmm1, xmm2/m128
 * 66 0F 38 DC /r        AESENC xmm1, xmm2/m128
 * 66 0F 38 DD /r        AESENCLAST xmm1, xmm2/m128
 * 66 0F 38 DE /r        AESDEC xmm1, xmm2/m128
 * 66 0F 38 DF /r        AESDECLAST xmm1, xmm2/m128
 * 66 0F 3A DF /r ib    AESKEYGENASSIST xmm1,xmm2/m128, imm8
 */
BOOL OPCHndlrAES(BYTE bOpcodeThirdByte)
{
    wprintf(L"OPCHndlrAES(): %xh\n", bOpcodeThirdByte);

    static WCHAR awszAESMnemonics[][MAX_INS_OP_STR_LEN + 1] = {
                    L"aesimc", L"aesenc", L"aesenclast", L"aesdec", L"aesdeslast" };

    INT iIndex = 0;

    insCurIns.bDBit = 1;
    insCurIns.fSpecialInstruction = TRUE;
    insCurIns.fSSEIns = TRUE;
    insCurIns.bRegTypeDest = insCurIns.bRegTypeSrc = REGTYPE_XMM;
    insCurIns.bOperandSizeSrc = OPERANDSIZE_SSE128;
    insCurIns.fModRM = TRUE;
    insCurIns.bModRMType = MODRM_TYPE_R;
    if (bOpcodeThirdByte == 0xDF)
    {
        insCurIns.fImm = TRUE;
        insCurIns.bImmType = IMM_8BIT;
        wsprintfW(wszCurInsStr, L"aeskeygenassist");
    }
    else
    {
        iIndex = bOpcodeThirdByte - 0xDB;
        if (iIndex < 0 || iIndex >= _countof(awszAESMnemonics))
        {
            wprintf(L"OPCHndlrAES(): Invalid index %d calculated from opcode byte %xh\n", iIndex, bOpcodeThirdByte);
            return FALSE;
        }
        wsprintfW(wszCurInsStr, awszAESMnemonics[iIndex]);
    }

    dsNextState = DASM_STATE_MOD_RM;
    return TRUE;

}// OPCHndlrAES()

// Invalid opcodes handler
BOOL OPCHndlr_INVALID(BYTE bOpcode)
{
    wprintf(L"OPCHndlr_INVALID(): %xh\n", bOpcode);

    dsNextState = DASM_STATE_DUMP;
    return TRUE;
}

// Instructions I don't know yet
BOOL OPCHndlrUnKwn_SSE(BYTE bOpcode)
{
    wprintf(L"OPCHndlrUnKwn_SSE(): %xh\n", bOpcode);

    //dsNextState = DASM_STATE_DUMP;
    return FALSE;
}

BOOL OPCHndlrUnKwn_MMX(BYTE bOpcode)
{
    wprintf(L"OPCHndlrUnKwn_MMX(): %xh\n", bOpcode);

    //dsNextState = DASM_STATE_DUMP;
    return FALSE;
}

BOOL OPCHndlrUnKwn_(BYTE bOpcode)        // call this when you don't know anything about the opcode
{
    wprintf(L"OPCHndlrUnKwn_(): %xh\n", bOpcode);

    // dsNextState = DASM_STATE_DUMP;
    return FALSE;
}
