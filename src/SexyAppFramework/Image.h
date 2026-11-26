#pragma once

#include "Common.h"

namespace Sexy
{
    struct AnimInfo;

    enum ImageFlags
    {
        ImageFlag_MinimizeNumSubdivisions = 0x1,
        ImageFlag_Use64By64Subdivisions = 0x2,
        ImageFlag_UseA4R4G4B4 = 0x4,
        ImageFlag_UseA8R8G8B8 = 0x8,
        ImageFlag_RenderTarget = 0x10,
        ImageFlag_CubeMap = 0x20,
        ImageFlag_VolumeMap = 0x40,
        ImageFlag_NoTriRep = 0x80,
        ImageFlag_NoQuadRep = 0x80,
        ImageFlag_RTUseDefaultRenderMode = 0x100,
        REFLECT_ATTR$ENUM$FLAGS = 0xFFFFFFFF,
    };

    class Image
    {
    public:
        ImageFlags mImageFlags;
        void* mRenderData;
        bool mDrawn;
        std::string mFilePath;
        int mWidth;
        int mHeight;
        int mNumRows;
        int mNumCols;
        AnimInfo* mAnimInfo;

    public:
        Image();
        Image(const Image& theImage);
        virtual ~Image();

        int GetWidth();
        int GetHeight();

        // TODO: Implement
    };
}
