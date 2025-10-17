#pragma once
#include "pgrender/eventSystem.h"
#include <memory>

namespace pgrender::backends::sdl3 {

	// Forward declaration
	class SDL3Window;

	class SDL3PerWindowEventSystem : public IPerWindowEventSystem {
	public:
		SDL3PerWindowEventSystem();
		~SDL3PerWindowEventSystem() override;

		// No copiable ni movible
		SDL3PerWindowEventSystem(const SDL3PerWindowEventSystem&) = delete;
		SDL3PerWindowEventSystem& operator=(const SDL3PerWindowEventSystem&) = delete;
		SDL3PerWindowEventSystem(SDL3PerWindowEventSystem&&) = delete;
		SDL3PerWindowEventSystem& operator=(SDL3PerWindowEventSystem&&) = delete;

		void createWindowQueue(WindowID windowId) override;
		void destroyWindowQueue(WindowID windowId) override;
		WindowEventQueue* getWindowQueue(WindowID windowId) override;

		void registerWindow(WindowID windowId, SDL3Window* window);
		void unregisterWindow(WindowID windowId);

		void pollEvents() override;
		bool getEvent(Event& event) override;
		bool getEventForWindow(WindowID windowId, Event& event) override;

		void setEventCallback(std::function<void(const Event&)> callback) override;
		void setWindowEventFilter(WindowID windowId, EventFilter filter) override;
		void setWindowEventWatcher(WindowID windowId, EventFilter watcher) override;

		size_t getWindowQueueSize(WindowID windowId) const override;
		size_t getTotalQueuedEvents() const override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

} // namespace pgrender::backends::sdl3
