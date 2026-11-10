#pragma once

#include <string>

namespace ImageLib
{
    void InitJPEG2000();
    void CloseJPEG2000();
    void SetJ2KCodecKey(const std::string& theKey);
}
