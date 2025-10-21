#include "pgrender/eventSystem.h"
#include "pgrender/window.h"
#include <memory>

namespace pgrender {

	class IEventSystem::Impl {
	public:
		Impl() = default;
		~Impl() = default;

		std::unordered_map<WindowID, std::unique_ptr<WindowEventQueue>> windowQueues;
		std::unordered_map<WindowID, IWindow*> windows;
		WindowEventQueue globalEventQueue;
		mutable std::mutex mutex;

		void createWindowQueueInternal(WindowID windowId);
		void destroyWindowQueueInternal(WindowID windowId);

		void handleWindowClose(const Event &e, WindowID windowId);
	};

	IEventSystem::IEventSystem() : m_impl(std::make_unique<IEventSystem::Impl>()) {};
	IEventSystem::~IEventSystem() = default;

	WindowEventQueue* IEventSystem::getWindowQueue(WindowID windowId)
	{
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		if (windowId == 0) {
			return &(m_impl->globalEventQueue);
		}
		auto it = m_impl->windowQueues.find(windowId);
		return (it != m_impl->windowQueues.end()) ? it->second.get() : nullptr;
	}

	bool IEventSystem::getEventForWindow(WindowID windowId, Event& event)
	{
		auto* queue = getWindowQueue(windowId);
		if (!queue) {
			return false;
		}

		auto maybeEvent = queue->tryPopEvent();
		if (maybeEvent) {
			event = *maybeEvent;
			return true;
		}

		return false;
	}

	bool IEventSystem::getGlobalEvent(Event& event)
	{
		auto e = m_impl->globalEventQueue.tryPopEvent();
		if (e) {
			event = *e;
			return true;
		}

		return false;
	}

	void IEventSystem::registerWindow(WindowID windowId, IWindow* window)
	{
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		// Usar versión interna sin lock
		m_impl->windows[windowId] = window;
		m_impl->createWindowQueueInternal(windowId);  // SIN deadlock
	}

	void IEventSystem::unregisterWindow(WindowID windowId)
	{
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		// Usar versión interna sin lock
		m_impl->destroyWindowQueueInternal(windowId);  // SIN deadlock
	}

	void IEventSystem::createWindowQueue(WindowID windowId)
	{
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		m_impl->createWindowQueueInternal(windowId);
	}

	// Versión interna sin lock
	void IEventSystem::Impl::createWindowQueueInternal(WindowID windowId) {
		// NO bloquear aquí - asume que el mutex ya está bloqueado
		if (windowQueues.find(windowId) == windowQueues.end()) {
			windowQueues[windowId] = std::make_unique<WindowEventQueue>();
		}
	}

	// Versión pública con lock
	void IEventSystem::destroyWindowQueue(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		m_impl->destroyWindowQueueInternal(windowId);
	}

	// Versión interna sin lock
	void IEventSystem::Impl::destroyWindowQueueInternal(WindowID windowId) {
		// NO bloquear aquí
		windowQueues.erase(windowId);
		windows.erase(windowId);
	}

	void IEventSystem::Impl::handleWindowClose(const Event& event, WindowID windowId) {
		if (windowId == 0) {
			for (auto& [id, window] : windows) {
				if (window) {
					window->markForClose();
				}
			}
		}
		else {
			auto it = windows.find(windowId);
			if (it != windows.end() && it->second) {
				it->second->markForClose();
			}
		}
	}


	void IEventSystem::setWindowEventFilter(WindowID windowId, EventFilter filter) {
		auto* queue = getWindowQueue(windowId);
		if (queue) {
			queue->setEventFilter(filter);
		}
	}

	void IEventSystem::setWindowEventWatcher(WindowID windowId, EventFilter watcher) {
		auto* queue = getWindowQueue(windowId);
		if (queue) {
			queue->setEventWatcher(watcher);
		}
	}

	size_t IEventSystem::getWindowQueueSize(WindowID windowId) const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windowQueues.find(windowId);
		return (it != m_impl->windowQueues.end()) ? it->second->size() : 0;
	}

	size_t IEventSystem::getTotalQueuedEvents() const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		size_t total = 0;
		for (const auto& [_, queue] : m_impl->windowQueues) {
			total += queue->size();
		}
		return total;
	}

	void IEventSystem::distributeEvent(const Event& event, WindowID targetWindow)
	{
		auto* q = getWindowQueue(targetWindow);
		if (q) {
			q->pushEvent(event);
			if (event.type == EventType::WindowClose) {
				m_impl->handleWindowClose(event, targetWindow);
			}
		}
	}
}

