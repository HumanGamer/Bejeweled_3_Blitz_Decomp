#include "PakInterface.h"
#include <Windows.h>
#include <direct.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

enum
{
    FILEFLAGS_END = 0x80
};

PakInterface* gPakInterface = new PakInterface();

PakInterface::PakInterface()
{
    // TODO: Implement
}

PakInterface::~PakInterface() = default;

bool PakInterface::AddPakFile(const std::string &theFileName)
{
    // TODO: Implement
    return false;
}
