#pragma once

#include <map>
#include <list>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class PakInterfaceBase
{
public:

};

class PakInterface : public PakInterfaceBase
{
public:
    PakInterface();
    ~PakInterface();

    bool AddPakFile(const std::string& theFileName);
};

extern PakInterface* gPakInterface;
