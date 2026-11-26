#pragma once

#include "Common.h"
#include "Rect.h"
#include "RenderDevice.h"

namespace Sexy
{
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

    class Graphics3D
    {
    public:
        Graphics* mGraphics;
        RenderDevice3D* mRenderDevice;
        HRenderContext mRenderContext;

    public:
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
}
