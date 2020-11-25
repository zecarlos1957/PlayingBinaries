
// ---------------------------------------------------
// Author: Shishir Bhat (www.shishirbhat.com)
// The MIT License (MIT)
// Copyright (c) 2016
//

#include "Logger.h"
#include "DasmAssert.h"

void PrintDebugString(wchar_t const *pszFmt, ...)
{
    wchar_t szBuffer[1024];

    HRESULT hr;
    va_list pArgs;

    va_start(pArgs, pszFmt);
    hr = wsprintfW(szBuffer, pszFmt, pArgs);
    va_end(pArgs);

    if (FAILED(hr) && (hr != 0))//STRSAFE_E_INSUFFICIENT_BUFFER))
    {
        ASSERT(!L"StringCchVPrintf failed!");
        goto errExit;
    }

    OutputDebugStringW(szBuffer);
    wprintf(szBuffer);
    return;

errExit:
    return;
}
