#pragma once

#include "Color.h"
#include "Common.h"
#include "Rect.h"

namespace Sexy
{
    class RenderDevice3D;
    class Color;
    class Image;
    class RenderDevice;
    class Font;
    class SexyMatrix3;
    class Transform;

    const int MAX_TEMP_SPANS = 8192;

    struct Edge
    {
        double mX;
        double mDX;
        int i;
        double b;
    };

    class Graphics3D;
    class Graphics;

    struct HRenderContext
    {
        union // TODO: This will have to be updated for 64-bit
        {
            DWORD mHandleDword;
            void* mHandlePtr;
        };
    };

    class GraphicsState
    {
    public:
        static Image mStaticImage;
        Image* mDestImage;
        float mTransX;
        float mTransY;
        float mScaleX;
        float mScaleY;
        float mScaleOrigX;
        float mScaleOrigY;
        Rect mClipRect;
        std::vector<Color> mPushedColorVector;
        Color mFinalColor;
        Color mColor;
        Font *mFont;
        int mDrawMode;
        bool mColorizeImages;
        bool mFastStretch;
        bool mWriteColoredString;
        bool mLinearBlend;
        bool mIs3D;

    public:
        void CopyStateFrom(const GraphicsState* theState);

        // TODO: Implement
    };

    typedef std::list<GraphicsState> GraphicsStateList;

    class Graphics : public GraphicsState
    {
    public:
        enum
        {
            DRAWMODE_NORMAL,
            DRAWMODE_ADDITIVE
        };
        RenderDevice* mRenderDevice;
        HRenderContext mRenderContext;
        Graphics3D* mGraphics3D;
        Edge* mPFActiveEdgeList;
        int mPFNumActiveEdges;
        int mPFNumVertices;
        GraphicsStateList mStateStack;

    protected:
        void InitRenderInfo(const Graphics* theGraphics);

        void CalcFinalColor();

        void SetAsCurrentContext();
        // TODO: Implement

    public:
        Graphics(const Graphics& theGraphics);
        Graphics(Image* theDestImage = nullptr);
        virtual ~Graphics();

        // TODO: Implement

        void SetFont(Font* theFont);
        Font* GetFont();

        void SetColor(const Color& theColor);
        const Color& GetColor();

        // TODO: Implement

        void FillRect(int theX, int theY, int theWidth, int theHeight);
        void FillRect(const Rect& theRect);

        // TODO: Implement

        void DrawString(const SexyString& theString, int theX, int theY);

        // TODO: Implement
    };

    class Graphics3D
    {
    public:
        Graphics* mGraphics;
        RenderDevice3D* mRenderDevice;
        HRenderContext mRenderContext;

    public:
        Graphics3D(Graphics* theGraphics, RenderDevice3D* theRenderDevice, const HRenderContext& theContext);

        // TODO: Implement
    };
}
