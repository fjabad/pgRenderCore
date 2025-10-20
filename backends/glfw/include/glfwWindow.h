#pragma once
#include "pgrender/Window.hpp"
#include "pgrender/backends/glfw/GLFWContext.hpp"
#include <GLFW/glfw3.h>
#include <memory>

namespace pgrender::backends::glfw {

    class GLFWWindow : public IWindow {
    public:
        explicit GLFWWindow(const WindowConfig& config);
        ~GLFWWindow() override;

        void show() override;
        void hide() override;
        void setTitle(const std::string& title) override;
        void setSize(uint32_t width, uint32_t height) override;
        void getSize(uint32_t& width, uint32_t& height) const override;
        bool shouldClose() const override;
        void* getNativeHandle() const override { return m_window; }

        std::unique_ptr<IGraphicsContext> createContext(const ContextConfig& config) override;

        void setRelativeMouseMode(bool enabled) override;
        bool getRelativeMouseMode() const override;
        void setMouseGrab(bool grabbed) override;
        bool getMouseGrab() const override;

        void markForClose();
        GLFWwindow* getGLFWHandle() const { return m_window; }

    private:
        GLFWwindow* m_window = nullptr;
        WindowConfig m_config;
        bool m_shouldClose = false;
    };

} // namespace pgrender::backends::glfw
