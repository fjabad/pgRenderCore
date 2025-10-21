#pragma once
#include "threadSafeQueue.h"
#include "types.h"
#include <functional>

namespace pgrender {

	class IWindow;

	// Sistema de eventos con cola por ventana
	class IEventSystem {
	public:
		virtual ~IEventSystem();

		virtual void pollEvents() = 0;

		// Cola por ventana, y la de eventos globales para la ventana 0
		WindowEventQueue* getWindowQueue(WindowID windowId);
		bool getEventForWindow(WindowID windowId, Event& event);
		bool getGlobalEvent(Event& event);

		virtual void registerWindow(WindowID windowId, IWindow* window);
		void unregisterWindow(WindowID windowId);

		// Filtros por ventana
		void setWindowEventFilter(WindowID windowId, EventFilter filter);
		void setWindowEventWatcher(WindowID windowId, EventFilter watcher);

		// Gestión de colas
		void createWindowQueue(WindowID windowId);
		void destroyWindowQueue(WindowID windowId);

		// Estadísticas
		size_t getWindowQueueSize(WindowID windowId) const;
		size_t getTotalQueuedEvents() const;

		// Distribución de eventos a colas
		void distributeEvent(const Event& event, WindowID targetWindow);
	protected:
		IEventSystem();
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

} // namespace pgrender
