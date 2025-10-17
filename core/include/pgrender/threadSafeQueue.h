#pragma once
#include "types.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <functional>
#include <chrono>
#include <unordered_set>

namespace pgrender {

// Filtro de eventos
using EventFilter = std::function<bool(const Event&)>;

// Cola thread-safe genérica
template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
    
    void push(const T& item) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(item);
        }
        m_condition.notify_one();
    }
    
    void push(T&& item) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(std::move(item));
        }
        m_condition.notify_one();
    }
    
    T pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this] { return !m_queue.empty(); });
        
        T item = std::move(m_queue.front());
        m_queue.pop();
        return item;
    }
    
    template<typename Rep, typename Period>
    std::optional<T> pop_for(const std::chrono::duration<Rep, Period>& timeout) {
        std::unique_lock<std::mutex> lock(m_mutex);
        
        if (!m_condition.wait_for(lock, timeout, [this] { return !m_queue.empty(); })) {
            return std::nullopt;
        }
        
        T item = std::move(m_queue.front());
        m_queue.pop();
        return item;
    }
    
    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_queue.empty()) {
            return std::nullopt;
        }
        
        T item = std::move(m_queue.front());
        m_queue.pop();
        return item;
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::queue<T> empty;
        std::swap(m_queue, empty);
    }
    
    std::optional<T> peek() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_queue.empty()) {
            return std::nullopt;
        }
        
        return m_queue.front();
    }

private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
};

// Cola de eventos por ventana
class WindowEventQueue {
public:
    WindowEventQueue() = default;
    
    void pushEvent(const Event& event) {
        if (m_filter && !m_filter(event)) {
            return;
        }
        
        m_queue.push(event);
        
        if (m_watcher) {
            m_watcher(event);
        }
    }
    
    Event popEvent() {
        return m_queue.pop();
    }
    
    std::optional<Event> tryPopEvent() {
        return m_queue.try_pop();
    }
    
    template<typename Rep, typename Period>
    std::optional<Event> popEventFor(const std::chrono::duration<Rep, Period>& timeout) {
        return m_queue.pop_for(timeout);
    }
    
    std::optional<Event> peekEvent() const {
        return m_queue.peek();
    }
    
    void setEventFilter(EventFilter filter) {
        std::lock_guard<std::mutex> lock(m_filterMutex);
        m_filter = filter;
    }
    
    void setEventWatcher(EventFilter watcher) {
        std::lock_guard<std::mutex> lock(m_watcherMutex);
        m_watcher = watcher;
    }
    
    void clearEventFilter() {
        std::lock_guard<std::mutex> lock(m_filterMutex);
        m_filter = nullptr;
    }
    
    void clearEventWatcher() {
        std::lock_guard<std::mutex> lock(m_watcherMutex);
        m_watcher = nullptr;
    }
    
    void filterByType(EventType type, bool allow) {
        if (allow) {
            m_allowedTypes.insert(type);
        } else {
            m_allowedTypes.erase(type);
        }
        
        setEventFilter([this](const Event& event) {
            if (m_allowedTypes.empty()) {
                return true;
            }
            return m_allowedTypes.count(event.type) > 0;
        });
    }
    
    size_t size() const { return m_queue.size(); }
    bool empty() const { return m_queue.empty(); }
    void clear() { m_queue.clear(); }

private:
    ThreadSafeQueue<Event> m_queue;
    EventFilter m_filter;
    EventFilter m_watcher;
    std::mutex m_filterMutex;
    std::mutex m_watcherMutex;
    std::unordered_set<EventType> m_allowedTypes;
};

} // namespace pgrender
