#pragma once
#include "Singleton.h"

class IGameApp;

namespace Core
{
    void RunApplication(IGameApp& app, const wchar_t* className);
    void TerminateApplication(IGameApp& game);

    void CalculateFrameStats();
}

#define CREATE_APPLICATION( app_class ) \
    IGameApp* app = app_class::GetApp(); \
    Core::RunApplication( *app, L#app_class ); \
    app_class::DestroyApp(); \
    return 0; \

class CGameCore
{
};

