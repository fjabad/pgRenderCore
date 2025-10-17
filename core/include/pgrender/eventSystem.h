#pragma once
#include "threadSafeQueue.h"
#include "types.h"
#include <functional>

namespace pgrender {

// Sistema de eventos base
class IEventSystem {
public:
    virtual ~IEventSystem() = default;
    
    virtual void pollEvents() = 0;
    virtual bool getEvent(Event& event) = 0;
    virtual void setEventCallback(std::function<void(const Event&)> callback) = 0;
    
    IEventSystem(const IEventSystem&) = delete;
    IEventSystem& operator=(const IEventSystem&) = delete;

protected:
    IEventSystem() = default;
};

// Sistema de eventos con cola por ventana
class IPerWindowEventSystem : public IEventSystem {
public:
    virtual ~IPerWindowEventSystem() = default;
    
    // Cola por ventana
    virtual WindowEventQueue* getWindowQueue(WindowID windowId) = 0;
    virtual bool getEventForWindow(WindowID windowId, Event& event) = 0;
    
    // Filtros por ventana
    virtual void setWindowEventFilter(WindowID windowId, EventFilter filter) = 0;
    virtual void setWindowEventWatcher(WindowID windowId, EventFilter watcher) = 0;
    
    // Gestión de colas
    virtual void createWindowQueue(WindowID windowId) = 0;
    virtual void destroyWindowQueue(WindowID windowId) = 0;
    
    // Estadísticas
    virtual size_t getWindowQueueSize(WindowID windowId) const = 0;
    virtual size_t getTotalQueuedEvents() const = 0;

protected:
    IPerWindowEventSystem() = default;
};

} // namespace pgrender
