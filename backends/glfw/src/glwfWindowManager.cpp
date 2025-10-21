#include "glfwWindowManager.h"
#include "glfwWindow.h"
#include "glfwEventSystem.h"
#include <GLFW/glfw3.h>
#include <glfwContext.h>
#include <iostream>

namespace pgrender::backends::glfw {


	// ============================================================================
	// GLFWWindowManager::Impl
	// ============================================================================

	class GLFWWindowManager::Impl {
	public:
		struct WindowData {
			std::unique_ptr<IWindow> window;
			std::unique_ptr<IGraphicsContext> context;
			WindowEventCallback eventCallback;
		};

		std::unordered_map<WindowID, WindowData> windows;
		std::unique_ptr<GLFWEventSystem> eventSystem;
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

			eventSystem = std::make_unique<GLFWEventSystem>();
		}

		~Impl() {
			windows.clear();
			eventSystem.reset();

			if (glfwInitialized) {
				glfwTerminate();
			}
		}
	};

	// ============================================================================
	// Constructor / Destructor
	// ============================================================================

	GLFWWindowManager::GLFWWindowManager()
		: m_impl(std::make_unique<Impl>()) {
	}

	GLFWWindowManager::~GLFWWindowManager() = default;

	// ============================================================================
	// Gestión de ventanas
	// ============================================================================

	WindowID GLFWWindowManager::createWindow(const WindowConfig& config) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = std::make_unique<GLFWWindow>(config);
		auto* glfwWindow = static_cast<GLFWWindow*>(window.get());
		WindowID windowId = glfwWindow->getWindowID();

		Impl::WindowData data;
		data.window = std::move(window);

		m_impl->windows[windowId] = std::move(data);

		// Registrar ventana en el sistema de eventos
		m_impl->eventSystem->registerWindow(windowId, glfwWindow);

		return windowId;
	}

	void GLFWWindowManager::destroyWindow(WindowID id) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windows.find(id);
		if (it != m_impl->windows.end()) {
			// Desregistrar del sistema de eventos
			m_impl->eventSystem->unregisterWindow(id);

			// Destruir contexto y ventana
			it->second.context.reset();
			it->second.window.reset();

			m_impl->windows.erase(it);
		}
	}

	void GLFWWindowManager::closeAllWindows() {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		for (auto& [id, data] : m_impl->windows) {
			m_impl->eventSystem->unregisterWindow(id);
			data.context.reset();
			data.window.reset();
		}

		m_impl->windows.clear();
	}

	std::vector<WindowID> GLFWWindowManager::getActiveWindows() const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		std::vector<WindowID> ids;
		ids.reserve(m_impl->windows.size());
		for (const auto& [id, _] : m_impl->windows) {
			ids.push_back(id);
		}
		return ids;
	}

	size_t GLFWWindowManager::getWindowCount() const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		return m_impl->windows.size();
	}

	bool GLFWWindowManager::hasOpenWindows() const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		return !m_impl->windows.empty();
	}

	// ============================================================================
	// Gestión de contextos gráficos
	// ============================================================================

	IGraphicsContext* GLFWWindowManager::getWindowContext(WindowID id) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		auto it = m_impl->windows.find(id);
		return (it != m_impl->windows.end()) ? it->second.context.get() : nullptr;
	}

	void GLFWWindowManager::setWindowContext(WindowID id, std::unique_ptr<IGraphicsContext> context) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		auto it = m_impl->windows.find(id);
		if (it != m_impl->windows.end()) {
			it->second.context = std::move(context);
		}
	}

	// ============================================================================
	// Sistema de eventos
	// ============================================================================

	void GLFWWindowManager::pollEvents() {
		// Procesar eventos capturados por el sistema de eventos
		m_impl->eventSystem->pollEvents();
	}

	bool GLFWWindowManager::getEventForWindow(WindowID windowId, Event& event) {
		return m_impl->eventSystem->getEventForWindow(windowId, event);
	}

	void GLFWWindowManager::processWindowClosures() {
		std::vector<WindowID> toClose;

		{
			std::lock_guard<std::mutex> lock(m_impl->mutex);
			for (const auto& [id, data] : m_impl->windows) {
				if (data.window && data.window->shouldClose()) {
					toClose.push_back(id);
				}
			}
		}

		for (WindowID id : toClose) {
			destroyWindow(id);
		}
	}

	void GLFWWindowManager::setWindowEventCallback(WindowID id, WindowEventCallback callback) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		auto it = m_impl->windows.find(id);
		if (it != m_impl->windows.end()) {
			it->second.eventCallback = callback;
		}
	}

	void GLFWWindowManager::setWindowEventFilter(WindowID id, EventFilter filter) {
		m_impl->eventSystem->setWindowEventFilter(id, filter);
	}

	void GLFWWindowManager::setWindowEventWatcher(WindowID id, EventFilter watcher) {
		m_impl->eventSystem->setWindowEventWatcher(id, watcher);
	}

	size_t GLFWWindowManager::getWindowQueueSize(WindowID id) const {
		return m_impl->eventSystem->getWindowQueueSize(id);
	}

	size_t GLFWWindowManager::getTotalQueuedEvents() const {
		return m_impl->eventSystem->getTotalQueuedEvents();
	}

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

	int GLFWWindowManager::getWindowDisplayIndex(WindowID windowId) const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windows.find(windowId);
		if (it == m_impl->windows.end() || !it->second.window) {
			return -1;
		}

		auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
		GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());

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

		auto it = m_impl->windows.find(windowId);
		if (it == m_impl->windows.end() || !it->second.window) {
			return;
		}

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

		auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
		GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());

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

		auto it = m_impl->windows.find(windowId);
		if (it != m_impl->windows.end() && it->second.window) {
			auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
			GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());
			glfwSetWindowPos(window, x, y);
		}
	}

	void GLFWWindowManager::getWindowPosition(WindowID windowId, int& x, int& y) const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windows.find(windowId);
		if (it != m_impl->windows.end() && it->second.window) {
			auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
			GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());
			glfwGetWindowPos(window, &x, &y);
		}
		else {
			x = 0;
			y = 0;
		}
	}

	void GLFWWindowManager::maximizeWindow(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windows.find(windowId);
		if (it != m_impl->windows.end() && it->second.window) {
			auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
			GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());
			glfwMaximizeWindow(window);
		}
	}

	void GLFWWindowManager::minimizeWindow(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windows.find(windowId);
		if (it != m_impl->windows.end() && it->second.window) {
			auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
			GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());
			glfwIconifyWindow(window);
		}
	}

	void GLFWWindowManager::restoreWindow(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windows.find(windowId);
		if (it != m_impl->windows.end() && it->second.window) {
			auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
			GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());
			glfwRestoreWindow(window);
		}
	}

	void GLFWWindowManager::raiseWindow(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windows.find(windowId);
		if (it != m_impl->windows.end() && it->second.window) {
			auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
			GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());
			glfwFocusWindow(window);
		}
	}

	void GLFWWindowManager::setFullscreen(WindowID windowId, bool fullscreen) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windows.find(windowId);
		if (it == m_impl->windows.end() || !it->second.window) {
			return;
		}

		auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
		GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());

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

		auto it = m_impl->windows.find(windowId);
		if (it == m_impl->windows.end() || !it->second.window) {
			return;
		}

		auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
		GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());

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

		auto it = m_impl->windows.find(windowId);
		if (it != m_impl->windows.end() && it->second.window) {
			auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
			GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());
			glfwSetWindowOpacity(window, opacity);
		}
	}

	float GLFWWindowManager::getWindowOpacity(WindowID windowId) const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windows.find(windowId);
		if (it != m_impl->windows.end() && it->second.window) {
			auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
			GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());
			return glfwGetWindowOpacity(window);
		}

		return 1.0f;
	}

	void GLFWWindowManager::setBordered(WindowID windowId, bool bordered) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windows.find(windowId);
		if (it != m_impl->windows.end() && it->second.window) {
			auto* glfwWindow = static_cast<GLFWWindow*>(it->second.window.get());
			GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow->getNativeHandle());
			glfwSetWindowAttrib(window, GLFW_DECORATED, bordered ? GLFW_TRUE : GLFW_FALSE);
		}
	}

	IWindow* GLFWWindowManager::getWindow(WindowID id) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		auto it = m_impl->windows.find(id);
		return (it != m_impl->windows.end()) ? it->second.window.get() : nullptr;
	}

	const IWindow* GLFWWindowManager::getWindow(WindowID id) const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		auto it = m_impl->windows.find(id);
		return (it != m_impl->windows.end()) ? it->second.window.get() : nullptr;
	}


}