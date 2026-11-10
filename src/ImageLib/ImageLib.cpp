#include "ImageLib.h"

#include <Windows.h>

using namespace ImageLib;

HMODULE gJ2KCodec = nullptr;
std::string gJ2KCodecKey = "Your registration here";

void ImageLib::InitJPEG2000()
{
    gJ2KCodec = LoadLibrary("j2k-codec.dll");
}

void ImageLib::CloseJPEG2000()
{
    if (gJ2KCodec != nullptr)
    {
        FreeLibrary(gJ2KCodec);
        gJ2KCodec = nullptr;
    }
}

void ImageLib::SetJ2KCodecKey(const std::string& theKey)
{
    gJ2KCodecKey = theKey;
}
