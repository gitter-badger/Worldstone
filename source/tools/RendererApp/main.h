#pragma once
#include <SDL.h>
#include <atomic>
#include <fmt/format.h>

class BaseApp
{
private:
    std::atomic_bool stopRunning  = false;
    SDL_Window* mainWindow  = nullptr;
    int              windowWidth  = 1280;
    int              windowHeight = 720;

    int  init();
    bool initAppThread();

    void shutdown();
    void shutdownAppThread();

    void executeLoopOnce();

protected:
    virtual void executeAppLoopOnce();
    void requireExit() { stopRunning = true; }

public:
    BaseApp() { init(); }
    virtual ~BaseApp() { shutdown(); }

    void run();
    void runAppThread();
};