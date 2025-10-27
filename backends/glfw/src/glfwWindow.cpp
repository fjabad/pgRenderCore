#include "glfwWindow.h"
#include <stdexcept>
#include <GLFW/glfw3.h>
#include <glfwContext.h>

using NativeGLFWWindow = GLFWwindow;

namespace pgrender::backends::glfw {


	static void window_close_callback(GLFWwindow* window) {
		auto* instance = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
		if (instance) instance->markForClose();
	}

	class GLFWWindow::Impl {
	public:
		NativeGLFWWindow* window = nullptr;
		IWindow::Desc m_config;
		bool m_shouldClose = false;

		explicit Impl(const IWindow::Desc& config) : m_config(config) {
			//if (!glfwInit()) {
			//	throw std::runtime_error("Failed to initialize GLFW");
			//}

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_RESIZABLE, config.resizable ? GLFW_TRUE : GLFW_FALSE);

			window = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);
			if (!window) {
				throw std::runtime_error("Failed to create GLFW window");
			}

			glfwSetWindowUserPointer(window, this);
			glfwSetWindowCloseCallback(window, window_close_callback);
		}

		~Impl() {
			if (window) {
				glfwDestroyWindow(window);
			}
		}
	};

	GLFWWindow::GLFWWindow(const IWindow::Desc& config) :
		m_impl(std::make_unique<Impl>(config)) {
	}

	GLFWWindow::~GLFWWindow() = default;

	void GLFWWindow::show() { 
		glfwShowWindow(m_impl->window); 
	}
	
	void GLFWWindow::hide() { 
		glfwHideWindow(m_impl->window); 
	}

	void GLFWWindow::setTitle(const std::string& title) { 
		glfwSetWindowTitle(m_impl->window, title.c_str()); 
	}

	void GLFWWindow::setSize(uint32_t width, uint32_t height) { 
		glfwSetWindowSize(m_impl->window, width, height); 
	}

	void GLFWWindow::getSize(uint32_t& width, uint32_t& height) const {
		int w, h;
		glfwGetWindowSize(m_impl->window, &w, &h);
		width = w;
		height = h;
	}

	void* GLFWWindow::getNativeHandle() const
	{
		return m_impl->window;
	}

	std::unique_ptr<IGraphicsContext> GLFWWindow::createContext(const ContextConfig& config) {
		auto ctx = std::make_unique<GLFWGraphicsContext>(m_impl->window, config);
		glfwSwapInterval(this->m_impl->m_config.vsync ? 1 : 0);
		return ctx;
	}

	void GLFWWindow::setRelativeMouseMode(bool enabled) {
		glfwSetInputMode(m_impl->window, GLFW_CURSOR, enabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	bool GLFWWindow::getRelativeMouseMode() const {
		return glfwGetInputMode(m_impl->window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
	}

	void GLFWWindow::setMouseGrab(bool grabbed) {
		glfwSetInputMode(m_impl->window, GLFW_CURSOR, grabbed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	bool GLFWWindow::getMouseGrab() const {
		return glfwGetInputMode(m_impl->window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
	}

	WindowID GLFWWindow::getWindowID() const
	{
		return reinterpret_cast<WindowID>(m_impl->window);
	}

} // namespace pgrender::backends::glfw
