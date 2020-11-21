#ifndef _CFILESTREAM_H_
#define _CFILESTREAM_H_

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class CFileStream
{
private:
    char FilePath[MAX_PATH];
    ifstream File; 
    unsigned char *Mem; 
    int FileSize;
    BOOL Dirty;
public:

    CFileStream(const char *FilePath);
    
    ~CFileStream(void);

    bool SaveFile();
    void SetDirty(BOOL flag){Dirty = flag;}
    void SetCharByIndex(unsigned char Byte,int Index);

    unsigned char GetFileByIndex(int Byte);

    unsigned char *GetFile(void);

    int GetFileSize(void);

};

#endif