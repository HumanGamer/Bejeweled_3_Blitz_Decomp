#include "GraphicsRecorder.h"

using namespace Sexy;

GraphicsRecorder* Sexy::gGR = nullptr;

GraphicsRecorder::GraphicsRecorder()
{
    mIgnoreDraws = false;
    mRecordDraws = false;
    mTimestamp = 0;
}

GraphicsRecorder::~GraphicsRecorder() = default;
