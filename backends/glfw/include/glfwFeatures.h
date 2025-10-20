#pragma once
#include "pgrender/WindowManager.hpp"
#include "pgrender/InputSystem.hpp"
#include "pgrender/RenderCore.hpp"
#include <memory>

namespace pgrender::backends::glfw {

    class GLFWWindowManager : public IWindowManager {
    public:
        explicit GLFWWindowManager();
        ~GLFWWindowManager() override;

        // Ventanas
        WindowID createWindow(const WindowConfig& config) override;
        void destroyWindow(WindowID id) override;
        void closeAllWindows() override;

        IWindow* getWindow(WindowID id) override;
        const IWindow* getWindow(WindowID id) const override;
        std::vector<WindowID> getActiveWindows() const override;
        size_t getWindowCount() const override;
        bool hasOpenWindows() const override;

        // Contextos
        IGraphicsContext* getWindowContext(WindowID id) override;
        void setWindowContext(WindowID id, std::unique_ptr<IGraphicsContext> context) override;

        // Eventos
        void pollEvents() override;
        bool getEventForWindow(WindowID windowId, Event& event) override;
        void processWindowClosures() override;

        void setWindowEventCallback(WindowID id, WindowEventCallback callback) override;
        void setWindowEventFilter(WindowID id, EventFilter filter) override;
        void setWindowEventWatcher(WindowID id, EventFilter watcher) override;
        size_t getWindowQueueSize(WindowID id) const override;
        size_t getTotalQueuedEvents() const override;

        // Displays (stub)
        int getDisplayCount() const override;
        DisplayInfo getDisplayInfo(int index) const override;
        int getWindowDisplayIndex(WindowID windowId) const override;
        void centerWindowOnDisplay(WindowID windowId, int displayIndex) override;
        void setWindowPosition(WindowID, int, int) override {}
        void getWindowPosition(WindowID, int&, int&) const override {}
        void maximizeWindow(WindowID) override {}
        void minimizeWindow(WindowID) override {}
        void restoreWindow(WindowID) override {}
        void raiseWindow(WindowID) override {}
        void setFullscreen(WindowID, bool) override {}
        void setFullscreenDesktop(WindowID, bool) override {}
        void setWindowOpacity(WindowID, float) override {}
        float getWindowOpacity(WindowID) const override { return 1.0f; }
        void setBordered(WindowID, bool) override {}

    private:
        struct WindowData {
            std::unique_ptr<GLFWWindow> window;
            std::unique_ptr<GLFWGraphicsContext> context;
            WindowEventCallback callback;
        };
        std::unordered_map<WindowID, WindowData> m_windows;
    };

    class GLFWLibraryContext : public ILibraryContext {
    public:
        GLFWLibraryContext();
        ~GLFWLibraryContext();

        std::unique_ptr<IWindow> createWindow(const WindowConfig& config) override;
        std::unique_ptr<IGraphicsContext> createHeadlessContext(const ContextConfig& config) override;
        IEventSystem& getEventSystem() override;
        IWindowManager& getWindowManager() override;
        IAdvancedInputSystem& getInputSystem() override;

    private:
        GLFWWindowManager m_windowManager;
        SDL3AdvancedInputSystem m_inputSystem; // placeholder - podría ser propio
    };

} // namespace pgrender::backends::glfw
