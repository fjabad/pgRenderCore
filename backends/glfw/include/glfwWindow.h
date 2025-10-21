#pragma once
#include "pgrender/window.h"
#include <memory>

// Forward declarations - NO incluir GLFW
struct GLFWwindow;

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
		void* getNativeHandle() const override;

        std::unique_ptr<IGraphicsContext> createContext(const ContextConfig& config) override;

        void setRelativeMouseMode(bool enabled) override;
        bool getRelativeMouseMode() const override;
        void setMouseGrab(bool grabbed) override;
        bool getMouseGrab() const override;
		WindowID getWindowID() const override;
    private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

} // namespace pgrender::backends::glfw
