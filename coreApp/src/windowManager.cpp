#include "pgrender/windowManager.h"

namespace pgrender {

	class IWindowManager::Impl {
	public:
		Impl(std::unique_ptr<IEventSystem> es) : eventSystem(std::move(es)) {};
		~Impl() {
			windows.clear();
			eventSystem.reset();
		}

		struct WindowData {
			std::unique_ptr<IWindow> window;
			std::unique_ptr<IGraphicsContext> context;
		};

		std::unordered_map<WindowID, WindowData> windows;

		std::mutex mutex;
		std::unique_ptr<IEventSystem> eventSystem;
	};

	IWindowManager::IWindowManager(std::unique_ptr<IEventSystem> eventSystem)
		: m_impl(std::make_unique<Impl>(std::move(eventSystem))) {
	}
	IWindowManager::~IWindowManager() = default;


	void IWindowManager::registerWindowCreation(std::unique_ptr<IWindow> window) {
		Impl::WindowData data;

		auto windowPtr = window.get();
		data.window = std::move(window);
		WindowID windowId = data.window->getWindowID();
		m_impl->windows[windowId] = std::move(data);

		// Registrar ventana en el sistema de eventos
		m_impl->eventSystem->registerWindow(windowId, windowPtr);
	}

	void IWindowManager::pollEvents() {
		m_impl->eventSystem->pollEvents();
	}

	bool IWindowManager::getEventForWindow(WindowID windowId, Event& event) {
		return m_impl->eventSystem->getEventForWindow(windowId, event);
	}

	void IWindowManager::setWindowEventCallback(WindowID id, EventCallback callback) {
		m_impl->eventSystem->setWindowEventWatcher(id, callback);
	}

	void IWindowManager::setWindowEventFilter(WindowID id, EventFilter filter) {
		m_impl->eventSystem->setWindowEventFilter(id, filter);
	}

	void IWindowManager::setWindowEventWatcher(WindowID id, EventFilter watcher) {
		m_impl->eventSystem->setWindowEventWatcher(id, watcher);
	}

	size_t IWindowManager::getWindowQueueSize(WindowID id) const {
		return m_impl->eventSystem->getWindowQueueSize(id);
	}

	size_t IWindowManager::getTotalQueuedEvents() const {
		return m_impl->eventSystem->getTotalQueuedEvents();
	}

	void IWindowManager::processWindowClosures() {
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

	IWindow* IWindowManager::getWindow(WindowID id) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		auto it = m_impl->windows.find(id);
		return (it != m_impl->windows.end()) ? it->second.window.get() : nullptr;
	}

	const IWindow* IWindowManager::getWindow(WindowID id) const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		auto it = m_impl->windows.find(id);
		return (it != m_impl->windows.end()) ? it->second.window.get() : nullptr;
	}

	size_t IWindowManager::getWindowCount() const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		return m_impl->windows.size();
	}

	bool IWindowManager::hasOpenWindows() const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		return !m_impl->windows.empty();
	}

	void IWindowManager::closeAllWindows() {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		for (auto& [id, data] : m_impl->windows) {
			m_impl->eventSystem->unregisterWindow(id);
			data.context.reset();
			data.window.reset();
		}

		m_impl->windows.clear();
	}

	std::vector<WindowID> IWindowManager::getActiveWindows() const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		std::vector<WindowID> ids;
		ids.reserve(m_impl->windows.size());
		for (const auto& [id, _] : m_impl->windows) {
			ids.push_back(id);
		}
		return ids;
	}


	void IWindowManager::destroyWindow(WindowID id) {
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



	// ============================================================================
	// Gestión de contextos gráficos
	// ============================================================================

	IGraphicsContext* IWindowManager::getWindowContext(WindowID id) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		auto it = m_impl->windows.find(id);
		return (it != m_impl->windows.end()) ? it->second.context.get() : nullptr;
	}

	void IWindowManager::setWindowContext(WindowID id, std::unique_ptr<IGraphicsContext> context) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		auto it = m_impl->windows.find(id);
		if (it != m_impl->windows.end()) {
			it->second.context = std::move(context);
		}
	}

}