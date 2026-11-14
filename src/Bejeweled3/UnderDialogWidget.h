#pragma once

#include <Common.h>
#include <Widget.h>

namespace Sexy
{
    class DDImage;

    class UnderDialogWidget : public Widget
    {
    public:
        DDImage* mShrunkScreen1;
        DDImage* mShrunkScreen2;

        UnderDialogWidget();
        ~UnderDialogWidget();
    };
}
