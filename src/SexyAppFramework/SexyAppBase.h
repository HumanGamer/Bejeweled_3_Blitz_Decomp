#pragma once

#include "ButtonListener.h"
#include "Common.h"
#include "DialogListener.h"

namespace Sexy
{
    class SexyAppBase : public ButtonListener, DialogListener
    {
    public:
        // TODO: Implement

    public:
        SexyAppBase();
        virtual ~SexyAppBase();
        virtual void Start();
        virtual void Init();
        virtual void Shutdown();
    };
}
