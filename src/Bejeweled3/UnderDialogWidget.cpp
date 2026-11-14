#include "UnderDialogWidget.h"

#include <DDImage.h>

using namespace Sexy;

UnderDialogWidget::UnderDialogWidget()
{
    mMouseVisible = false;
    mHasAlpha = true;
    mShrunkScreen1 = nullptr;
    mShrunkScreen2 = nullptr;
    mClip = false;
}

UnderDialogWidget::~UnderDialogWidget()
{
    delete mShrunkScreen1;
    delete mShrunkScreen2;
}
