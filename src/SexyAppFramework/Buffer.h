#pragma once

#include "Common.h"

namespace Sexy
{
    typedef std::vector<uchar> ByteVector;

    class Buffer
    {
    public:
        ByteVector mData;
        int mDataBitSize;
        mutable int mReadBitPos;
        mutable int mWriteBitPos;

    public:
        Buffer();
        virtual ~Buffer();
    };
}
