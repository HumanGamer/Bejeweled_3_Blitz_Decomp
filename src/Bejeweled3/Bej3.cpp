#include "Bej3App.h"

using namespace Sexy;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    gHInstance = hInstance;

    while (true)
    {
        gApp = new Bej3App();
        gApp->Init();
        gApp->Start();
        gApp->Shutdown();

        if (!gApp->mReInit)
            break;

        delete gApp;
    }

    return 0;
}