#include "glfwWindowManager.h"
#include "glfwWindow.h"
#include "glfwEventSystem.h"
#include <GLFW/glfw3.h>
#include <glfwContext.h>
#include <iostream>

using NativeGLFWWindow = GLFWwindow;

namespace pgrender::backends::glfw {


	// ============================================================================
	// GLFWWindowManager::Impl
	// ============================================================================

	class GLFWWindowManager::Impl {
	public:

		mutable std::mutex mutex;
		bool glfwInitialized = false;

		Impl() {
			if (!glfwInit()) {
				throw std::runtime_error("Failed to initialize GLFW");
			}
			glfwInitialized = true;

			// Configurar callbacks de error de GLFW
			glfwSetErrorCallback([](int error, const char* description) {
				std::cerr << "GLFW Error " << error << ": " << description << std::endl;
				});
		}

		~Impl() {
			if (glfwInitialized) {
				glfwTerminate();
			}
		}
	};

	// ============================================================================
	// Constructor / Destructor
	// ============================================================================

	GLFWWindowManager::GLFWWindowManager() :
		m_impl(std::make_unique<Impl>()),
		IWindowManager(std::make_unique<GLFWEventSystem>())
	{
	}

	GLFWWindowManager::~GLFWWindowManager() {
		closeAllWindows();
	}

	// ============================================================================
	// Gestión de ventanas
	// ============================================================================

	WindowID GLFWWindowManager::createWindow(const IWindow::Desc& config) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = std::make_unique<GLFWWindow>(config);
		WindowID windowId = window->getWindowID();
		registerWindowCreation(std::move(window));

		return windowId;
	}


	// ============================================================================
	// Sistema de eventos
	// ============================================================================


	// ============================================================================
	// Gestión de displays (implementación básica)
	// ============================================================================

	int GLFWWindowManager::getDisplayCount() const {
		int count;
		glfwGetMonitors(&count);
		return count;
	}

	DisplayInfo GLFWWindowManager::getDisplayInfo(int index) const {
		DisplayInfo info{};
		info.index = index;

		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);

		if (index < 0 || index >= count || !monitors) {
			return info;
		}

		GLFWmonitor* monitor = monitors[index];

		// Nombre del monitor
		const char* name = glfwGetMonitorName(monitor);
		info.name = name ? name : "Unknown Display";

		// Resolución del monitor
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		if (mode) {
			info.bounds.width = mode->width;
			info.bounds.height = mode->height;
			info.refreshRate = mode->refreshRate;
		}

		// Posición del monitor
		int xpos, ypos;
		glfwGetMonitorPos(monitor, &xpos, &ypos);
		info.bounds.x = xpos;
		info.bounds.y = ypos;

		// Área de trabajo (GLFW no distingue usable bounds, usar bounds completos)
		info.usableBounds = info.bounds;

		// DPI (aproximado desde el tamaño físico)
		int widthMM, heightMM;
		glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);

		if (widthMM > 0 && mode) {
			// Calcular DPI: píxeles / (mm / 25.4)
			float dpi = (mode->width / (widthMM / 25.4f));
			info.ddpi = dpi;
			info.hdpi = dpi;
			info.vdpi = dpi;
		}
		else {
			// DPI por defecto
			info.ddpi = 96.0f;
			info.hdpi = 96.0f;
			info.vdpi = 96.0f;
		}

		// Content scale (GLFW 3.3+)
		float xscale, yscale;
		glfwGetMonitorContentScale(monitor, &xscale, &yscale);
		// Usar scale si está disponible
		if (xscale > 0) {
			info.ddpi *= xscale;
			info.hdpi *= xscale;
			info.vdpi *= yscale;
		}

		return info;
	}

	static NativeGLFWWindow* getGLFWWindowFromID(const GLFWWindowManager& manager, WindowID windowId) {
		const auto win = manager.getWindow(windowId);
		if (!win) {
			return nullptr;
		}
		auto window = static_cast<NativeGLFWWindow*>(win->getNativeHandle());
		return window;
	}

	int GLFWWindowManager::getWindowDisplayIndex(WindowID windowId) const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = getGLFWWindowFromID(*this, windowId);
		if (!window) {
			return -1;
		}

		GLFWmonitor* monitor = glfwGetWindowMonitor(window);
		if (!monitor) {
			// Ventana en modo no fullscreen - buscar el monitor más cercano
			int wx, wy;
			glfwGetWindowPos(window, &wx, &wy);

			int count;
			GLFWmonitor** monitors = glfwGetMonitors(&count);

			for (int i = 0; i < count; ++i) {
				int mx, my;
				glfwGetMonitorPos(monitors[i], &mx, &my);

				const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
				if (mode) {
					// Verificar si la ventana está dentro de este monitor
					if (wx >= mx && wx < mx + mode->width &&
						wy >= my && wy < my + mode->height) {
						return i;
					}
				}
			}

			return 0; // Retornar monitor primario por defecto
		}

		// Encontrar el índice del monitor
		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);

		for (int i = 0; i < count; ++i) {
			if (monitors[i] == monitor) {
				return i;
			}
		}

		return -1;
	}

	void GLFWWindowManager::centerWindowOnDisplay(WindowID windowId, int displayIndex) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);

		if (displayIndex < 0 || displayIndex >= count || !monitors) {
			return;
		}

		GLFWmonitor* monitor = monitors[displayIndex];
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		if (!mode) return;

		int mx, my;
		glfwGetMonitorPos(monitor, &mx, &my);

		auto window = getGLFWWindowFromID(*this, windowId);
		if (!window) {
			return;
		}

		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);

		int centeredX = mx + (mode->width - windowWidth) / 2;
		int centeredY = my + (mode->height - windowHeight) / 2;

		glfwSetWindowPos(window, centeredX, centeredY);
	}

	// ============================================================================
	// Posicionamiento y estados de ventana
	// ============================================================================

	void GLFWWindowManager::setWindowPosition(WindowID windowId, int x, int y) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = getGLFWWindowFromID(*this, windowId);
		if (!window) {
			return;
		}

		glfwSetWindowPos(window, x, y);
	}

	void GLFWWindowManager::getWindowPosition(WindowID windowId, int& x, int& y) const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = getGLFWWindowFromID(*this, windowId);

		if (!window) {
			x = 0;
			y = 0;
			return;
		}
		glfwGetWindowPos(window, &x, &y);
	}

	void GLFWWindowManager::maximizeWindow(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = getGLFWWindowFromID(*this, windowId);
		if (!window) {
			return;
		}
		glfwMaximizeWindow(window);
	}

	void GLFWWindowManager::minimizeWindow(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = getGLFWWindowFromID(*this, windowId);
		if (!window) {
			return;
		}
		glfwIconifyWindow(window);
	}

	void GLFWWindowManager::restoreWindow(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = getGLFWWindowFromID(*this, windowId);
		if (!window) {
			return;
		}
		glfwRestoreWindow(window);
	}

	void GLFWWindowManager::raiseWindow(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = getGLFWWindowFromID(*this, windowId);
		if (!window) {
			return;
		}
		glfwFocusWindow(window);
	}

	void GLFWWindowManager::setFullscreen(WindowID windowId, bool fullscreen) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = getGLFWWindowFromID(*this, windowId);
		if (!window) {
			return;
		}

		if (fullscreen) {
			// Obtener el monitor primario
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else {
			// Volver a modo ventana
			glfwSetWindowMonitor(window, nullptr, 100, 100, 800, 600, GLFW_DONT_CARE);
		}
	}

	void GLFWWindowManager::setFullscreenDesktop(WindowID windowId, bool fullscreen) {
		// GLFW no tiene concepto de "borderless fullscreen" directo
		// Simulamos con una ventana sin bordes del tamaño del monitor
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = getGLFWWindowFromID(*this, windowId);
		if (!window) {
			return;
		}

		if (fullscreen) {
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			int mx, my;
			glfwGetMonitorPos(monitor, &mx, &my);

			glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
			glfwSetWindowMonitor(window, nullptr, mx, my, mode->width, mode->height, GLFW_DONT_CARE);
		}
		else {
			glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
			glfwSetWindowMonitor(window, nullptr, 100, 100, 800, 600, GLFW_DONT_CARE);
		}
	}

	void GLFWWindowManager::setWindowOpacity(WindowID windowId, float opacity) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = getGLFWWindowFromID(*this, windowId);
		if (!window) {
			return;
		}
		glfwSetWindowOpacity(window, opacity);
	}

	float GLFWWindowManager::getWindowOpacity(WindowID windowId) const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = getGLFWWindowFromID(*this, windowId);
		if (!window) {
			return 1;
		}
		return glfwGetWindowOpacity(window);
	}

	void GLFWWindowManager::setBordered(WindowID windowId, bool bordered) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = getGLFWWindowFromID(*this, windowId);
		if (!window) {
			return;
		}
		glfwSetWindowAttrib(window, GLFW_DECORATED, bordered ? GLFW_TRUE : GLFW_FALSE);
	}
}
