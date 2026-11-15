#pragma once

#include <map>
#include <list>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class PakCollection;

class PakRecord
{
public:
    PakCollection*			mCollection;
    std::string				mFileName;
    FILETIME				mFileTime;
    int						mStartPos;
    int						mSize;
};

typedef std::map<std::string, PakRecord> PakRecordMap;

class PakCollection
{
public:
    HANDLE					mFileHandle;
    HANDLE					mMappingHandle;
    void*					mDataPtr;
};

typedef std::list<PakCollection> PakCollectionList;

struct PFILE
{
    PakRecord*				mRecord;
    int						mPos;
    FILE*					mFP;
};

struct PFindData
{
    HANDLE					mWHandle;
    std::string				mLastFind;
    std::string				mFindCriteria;
};

class PakInterfaceBase
{
public:
    virtual void Cleanup() = 0;
    virtual PFILE* FOpen(const wchar_t* theFileName, const wchar_t* anAccess, int searchOrder) { return NULL; }
    virtual PFILE* FOpen(const char* theFileName, const char* anAccess, int searchOrder) = 0;
    virtual int FClose(PFILE* theFile) = 0;
    virtual int FSeek(PFILE* theFile, long theOffset, int theOrigin) = 0;
    virtual int FTell(PFILE* theFile) = 0;
    virtual size_t FRead(void* thePtr, int theElemSize, int theCount, PFILE* theFile) = 0;
    virtual int FGetC(PFILE* theFile) = 0;
    virtual int UnGetC(int theChar, PFILE* theFile) = 0;
    virtual wchar_t* FGetS(wchar_t* thePtr, int theSize, PFILE* theFile) { return thePtr; }
    virtual char* FGetS(char* thePtr, int theSize, PFILE* theFile) = 0;
    virtual int FEof(PFILE* theFile) = 0;

    virtual HANDLE FindFirstFile(LPCTSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData) = 0;
    virtual BOOL FindNextFile(HANDLE hFindFile, LPWIN32_FIND_DATA lpFindFileData) = 0;
    virtual BOOL FindClose(HANDLE hFindFile) = 0;

    virtual int SetFileSearchOrder(int searchOrder) = 0;
    virtual int GetFileSearchOrder() = 0;
};

class PakInterface : public PakInterfaceBase
{
public:
    PakCollectionList		mPakCollectionList;
    PakRecordMap			mPakRecordMap;
#ifdef PAK_ERROR_CHECK
    std::string				mError;
#endif
    int mSearchOrder;

public:
    bool PFindNext(PFindData* theFindData, LPWIN32_FIND_DATA lpFindFileData);

public:
    PakInterface();
    ~PakInterface();

    bool AddPakFile(const std::string& theFileName);

    PFILE* FOpen_Pak(const char* theFileName, const char* anAccess);
    PFILE* FOpen_File(const char* theFileName, const char* anAccess);

    void Cleanup() override;
    PFILE* FOpen(const char* theFileName, const char* anAccess, int searchOrder) override;
    int FClose(PFILE* theFile) override;
    int FSeek(PFILE* theFile, long theOffset, int theOrigin) override;
    int FTell(PFILE* theFile) override;
    size_t FRead(void* thePtr, int theElemSize, int theCount, PFILE* theFile) override;
    int FGetC(PFILE* theFile) override;
    int UnGetC(int theChar, PFILE* theFile) override;
    char* FGetS(char* thePtr, int theSize, PFILE* theFile) override;
    int FEof(PFILE* theFile) override;

    HANDLE FindFirstFile(LPCTSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData) override;
    BOOL FindNextFile(HANDLE hFindFile, LPWIN32_FIND_DATA lpFindFileData) override;
    BOOL FindClose(HANDLE hFindFile) override;

    int SetFileSearchOrder(int searchOrder) override;
    int GetFileSearchOrder() override;
};

extern PakInterface* gPakInterface;

static HANDLE gPakFileMapping = nullptr;
static PakInterfaceBase** gPakInterfaceP = nullptr;

static PakInterfaceBase* GetPakPtr()
{
    if (gPakFileMapping == nullptr)
    {
        char aName[256];
        sprintf(aName, "gPakInterfaceP_%d", GetCurrentProcessId());
        gPakFileMapping = CreateFileMappingA((HANDLE)INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(PakInterface*), aName);
        gPakInterfaceP = (PakInterfaceBase**)MapViewOfFile(gPakFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(PakInterface*));
    }

    return *gPakInterfaceP;
}
