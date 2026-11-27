#pragma once

#include "Common.h"
#include "Image.h"
#include "RenderDevice.h"

#define OPTIMIZE_SOFTWARE_DRAWING
#ifdef OPTIMIZE_SOFTWARE_DRAWING
extern bool gOptimizeSoftwareDrawing;
#endif

namespace Sexy
{
    const ulong MEMORYCHECK_ID = 0x4BEEFADE;

    class SexyAppBase;

    class MemoryImage : public Image, public RenderDevice
    {
    public:
        ulong* mBits;
        int mBitsChangedCount;

        ulong* mColorTable;
        uchar* mColorIndices;

        bool mForcedMode;
        bool mHasTrans;
        bool mHasAlpha;
        bool mIsVolatile;
        bool mPurgeBits;
        bool mWantPal;
        bool mDither16;

        ulong* mNativeAlphaData;
        uchar* mRLAlphaData;
        uchar* mRLAdditiveData;

        bool mBitsChanged;
        SexyAppBase* mApp;

        struct TriRep
        {
            struct Tri
            {
                struct Point
                {
                    float u;
                    float v;
                };

                union
                {
                    struct
                    {
                        Point p0;
                        Point p1;
                        Point p2;
                    };

                    Point p[3];
                };
            };

            struct Level
            {
                struct Region
                {
                    Rect mRect;
                    std::vector<Tri> mTris;
                };
                int mDetailX;
                int mDetailY;
                int mRegionWidth;
                int mRegionHeight;
                std::vector<Region> mRegions;
            };
            std::vector<Level> mLevels;
        };

        TriRep mNormalTriRep;
        TriRep mAdditiveTriRep;

    private:
        void Init();

    public:
        // TODO: Implement

        virtual void BitsChanged();
        virtual void CommitBits();

    public:
        MemoryImage();
        MemoryImage(SexyAppBase* theApp);
        MemoryImage(const MemoryImage& theMemoryImage);
        virtual ~MemoryImage();

        MemoryImage *AsMemoryImage() override;

        HRenderContext CreateContext(Image *theImage, const HRenderContext &theContext) override;
        void DeleteContext(const HRenderContext &context) override;
        void SetCurrentContext(const HRenderContext &context) override;

        void FillRect(const Rect& theRect, const Color& theColor, int theDrawMode) override;

        virtual ulong* GetBits();

        virtual void SetImageMode(bool hasTrans, bool hasAlpha);

        virtual bool BuildTriRep(TriRep* rep, bool additive, bool force);
        virtual bool BuildTriReps(bool force);
    };
}