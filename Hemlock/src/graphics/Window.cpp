#include "stdafx.h"

#include "graphics\Window.h"

void hg::Window::init() {
    getAllowedDisplayResolutions();
    calculateAspectRatio();

    ui32 flags = SDL_WINDOW_OPENGL;
    if (m_settings.fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    if (m_settings.borderless) {
        flags |= SDL_WINDOW_BORDERLESS;
    }
    if (m_settings.resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }

    m_window = SDL_CreateWindow(getName(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, getWidth(), getHeight(), flags);
    if (m_window == nullptr) {
        puts("Oh bugger, couldn't make a window!");
    }

    m_context = SDL_GL_CreateContext(m_window);
    if (m_context == NULL) {
        puts("Gosh darn, couldn't make a GL Context!");
    }

    GLenum error = glewInit();
    if (error != GLEW_OK) {
        puts("Well I'll be, couldn't initialise Glew!");
    }

    printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));

    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

    if (m_settings.swapInterval == SwapInterval::V_SYNC) {
        SDL_GL_SetSwapInterval(1);
    } else {
        SDL_GL_SetSwapInterval(0);
    }
}

void hg::Window::getAllowedDisplayResolutions() {
    SDL_DisplayMode mode;
    ui32 displayCount = SDL_GetNumVideoDisplays();
    for (ui32 i = 0; i < displayCount; ++i) {
        std::vector<WindowDimensions> allowedResolutions;
        ui32 modeCount = SDL_GetNumDisplayModes(i);
        for (ui32 j = 0; j < modeCount; ++j) {
            SDL_GetDisplayMode(i, j, &mode);
            WindowDimensions resolution{ (ui32)mode.w, (ui32)mode.h };
            if (j == 0 || allowedResolutions.back() != resolution) {
                allowedResolutions.push_back(resolution);
            }
        }
        m_allowedResolutions[i] = allowedResolutions;
    }
}

void hg::Window::calculateAspectRatio() {
    std::function<ui32(ui32, ui32)> calculateGCD = [&calculateGCD](ui32 x, ui32 y) {
        if (y > x) return calculateGCD(y, x);
        if (y == 0) return x;

        return calculateGCD(y, x % y);
    };

    ui32 width = getWidth(), height = getHeight();
    ui32 gcd = calculateGCD(width, height);
    
    char* body = new char[5];
    snprintf(body, 5, "%d:%d", width / gcd, height / gcd);
    m_aspectRatioString = body;
}

void hg::Window::setName(char* name) {
    m_settings.name = name;
    SDL_SetWindowTitle(m_window, name);
}

void hg::Window::setDimensions(WindowDimensions dimensions) {
    if (m_settings.dimensions == dimensions) return;
    WindowDimensions temp = m_settings.dimensions;
    m_settings.dimensions = dimensions;
    SDL_SetWindowSize(m_window, dimensions.width, dimensions.height);
    onResize({ temp, dimensions });
}

void hg::Window::setWidth(ui32 width) {
    setDimensions({ width, getHeight() });
}

void hg::Window::setHeight(ui32 height) {
    setDimensions({ getWidth(), height });
}

void hg::Window::setFullscreen(bool fullscreen) {
    if (m_settings.fullscreen == fullscreen) return;
    m_settings.fullscreen = fullscreen;
    if (fullscreen) {
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
        onFullscreen();
    } else {
        SDL_SetWindowFullscreen(m_window, 0);
        onExitFullscreen();
    }
}

void hg::Window::setResizable(bool resizable) {
    if (m_settings.resizable == resizable) return;
    m_settings.resizable = resizable;
    SDL_SetWindowResizable(m_window, (SDL_bool)resizable);
}

void hg::Window::setBorderless(bool borderless) {
    if (m_settings.borderless == borderless) return;
    m_settings.borderless = borderless;
    SDL_SetWindowBordered(m_window, (SDL_bool)!borderless);
}

void hg::Window::setSwapInterval(hg::SwapInterval swapInterval) {
    if (m_settings.swapInterval == swapInterval) return;
    int vsync = swapInterval == hg::SwapInterval::V_SYNC ? 1 : 0;
    m_settings.swapInterval = swapInterval;
    SDL_GL_SetSwapInterval(vsync);
}

void hg::Window::sync() {
    // TODO(Matthew): Do we need to clear out untouched input events here? Should that even happen?

    SDL_GL_SwapWindow(m_window);

    // TODO(Matthew): Limit FPS if capped.
}