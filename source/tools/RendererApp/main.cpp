#include "main.h"
#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx/platform.h>
#include <fmt/format.h>
#include <thread>

int BaseApp::init()
{
    stopRunning = false;

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fmt::print("Error: {}\n", SDL_GetError());
        return -1;
    }

    // Create the main window
    mainWindow = SDL_CreateWindow(
        // clang-format off
        "Main window",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        windowWidth, windowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        // clang-format on
        );
    if (!mainWindow) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating window: %s\n", SDL_GetError());
        return -1;
    }

    // Initialize the renderer
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(mainWindow, &wmi)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't get window information: %s", SDL_GetError());
        return -1;
    }

    bgfx::PlatformData pd;
    switch (wmi.subsystem)
    {
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
    case SDL_SYSWM_WINDOWS:
        pd.ndt = NULL;
        pd.nwh = wmi.info.win.window;
        break;
#endif
#if defined(SDL_VIDEO_DRIVER_X11)
    case SDL_SYSWM_X11:
        pd.ndt = wmi.info.x11.display;
        pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
        break;
#endif
#if defined(SDL_VIDEO_DRIVER_COCOA)
    case SDL_SYSWM_COCOA:
        pd.ndt = NULL;
        pd.nwh = wmi.info.cocoa.window;
        break;
#endif
#if defined(SDL_VIDEO_DRIVER_VIVANTE)
    case SDL_SYSWM_VIVANTE:
        pd.ndt = wmi.info.vivante.display;
        pd.nwh = wmi.info.vivante.window;
        break;
#endif
    default: SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unsupported window system");
    }
    // Let bgfx create the rendering context and back buffers
    pd.context      = NULL;
    pd.backBuffer   = NULL;
    pd.backBufferDS = NULL;
    bgfx::setPlatformData(pd);

    // Call this before init to tell bgfx this is the render thread
    bgfx::renderFrame();
    return 0;
}

bool BaseApp::initAppThread()
{
    if (!bgfx::init()) return false;
    bgfx::reset(uint32_t(windowWidth), uint32_t(windowHeight), BGFX_RESET_VSYNC);
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

    // Clear the window
    bgfx::frame();
    return true;
}

void BaseApp::shutdownAppThread() { bgfx::shutdown(); }

void BaseApp::shutdown()
{
    SDL_DestroyWindow(mainWindow);
    mainWindow = nullptr;

    SDL_Quit();
}

void BaseApp::executeLoopOnce()
{

    SDL_Event event;
    bgfx::renderFrame();
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) requireExit();
    }

    SDL_Delay(10); // Don't burn our CPU
}

void BaseApp::executeAppLoopOnce()
{
    // Set view 0 default viewport.
    bgfx::setViewRect(0, 0, 0, uint16_t(windowWidth), uint16_t(windowHeight));

    // This dummy draw call is here to make sure that view 0 is cleared
    // if no other draw calls are submitted to view 0.
    bgfx::touch(0);

    // Advance to next frame. Rendering thread will be kicked to
    // process submitted rendering primitives.
    bgfx::frame();
}

void BaseApp::run()
{
    std::thread appThread{&BaseApp::runAppThread, this};
    while (!stopRunning)
    {
        executeLoopOnce();
        SDL_Delay(10); // Don't burn our CPU
    }
    // Wait for destruction of the context
    while (bgfx::RenderFrame::NoContext != bgfx::renderFrame())
    {
    };
    appThread.join();
}

void BaseApp::runAppThread()
{
    if (!initAppThread()) return;
    while (!stopRunning)
    {
        executeAppLoopOnce();
    }
    shutdownAppThread();
    // End of the app thread
}


int main(int, char**)
{
    BaseApp app;
    app.run();
    return 0;
}
