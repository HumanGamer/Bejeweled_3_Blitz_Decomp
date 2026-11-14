#pragma once

#include <Common.h>
#include <deque>

#include "Color.h"
#include "Image.h"
#include "Rect.h"

namespace Sexy
{
    class GraphicsOperation
    {
    public:
        int mType;
        Image* mImage;
        TRect<double> mDestRect; // TODO: Should this be FRect? or maybe DRect?
        Rect mSrcRect;
        Color mColor;
        int mDrawMode;
        float mFloat;
        TRect<double> mFRect; // TODO: Should this be FRect? or maybe DRect?

        // TODO: Implement
    };

    class GraphicsRecorder
    {
    public:
        int mTimestamp;
        bool mIgnoreDraws;
        bool mRecordDraws;
        std::deque<GraphicsOperation> mGraphicsOperations;

    public:
        GraphicsRecorder();
        virtual ~GraphicsRecorder();

        // TODO: Implement
    };

    extern GraphicsRecorder* gGR;
}
