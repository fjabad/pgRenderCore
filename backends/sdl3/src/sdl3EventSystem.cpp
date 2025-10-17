#include "sdl3EventSystem.h"
#include "sdl3Window.h"
#include <SDL3/SDL.h>
#include <unordered_map>
#include <mutex>

namespace pgrender::backends::sdl3 {

	class SDL3PerWindowEventSystem::Impl {
	public:
		std::unordered_map<WindowID, std::unique_ptr<WindowEventQueue>> windowQueues;
		std::unordered_map<WindowID, SDL3Window*> windows;
		std::function<void(const Event&)> globalCallback;
		mutable std::mutex mutex;
		mutable std::mutex callbackMutex;

		// Versiones internas sin lock (asumen que el mutex ya está bloqueado)
		void createWindowQueueInternal(WindowID windowId);
		void destroyWindowQueueInternal(WindowID windowId);

		void distributeEvent(const Event& event, WindowID targetWindow);
		WindowID getWindowIdFromMouse(const SDL_Event& sdlEvent);
		void handleWindowClose(const SDL_Event& sdlEvent, WindowID windowId);

		Event translateEvent(const SDL_Event& sdlEvent);
		KeyCode translateKeyCode(SDL_Keycode key);
		MouseButton translateMouseButton(uint8_t button);
	};

	SDL3PerWindowEventSystem::SDL3PerWindowEventSystem()
		: m_impl(std::make_unique<Impl>()) {
	}

	SDL3PerWindowEventSystem::~SDL3PerWindowEventSystem() = default;


	// Versión pública con lock
	void SDL3PerWindowEventSystem::createWindowQueue(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		m_impl->createWindowQueueInternal(windowId);
	}

	// Versión interna sin lock
	void SDL3PerWindowEventSystem::Impl::createWindowQueueInternal(WindowID windowId) {
		// NO bloquear aquí - asume que el mutex ya está bloqueado
		if (windowQueues.find(windowId) == windowQueues.end()) {
			windowQueues[windowId] = std::make_unique<WindowEventQueue>();
		}
	}


	// Versión pública con lock
	void SDL3PerWindowEventSystem::destroyWindowQueue(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		m_impl->destroyWindowQueueInternal(windowId);
	}

	// Versión interna sin lock
	void SDL3PerWindowEventSystem::Impl::destroyWindowQueueInternal(WindowID windowId) {
		// NO bloquear aquí
		windowQueues.erase(windowId);
		windows.erase(windowId);
	}

	WindowEventQueue* SDL3PerWindowEventSystem::getWindowQueue(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windowQueues.find(windowId);
		return (it != m_impl->windowQueues.end()) ? it->second.get() : nullptr;
	}

	void SDL3PerWindowEventSystem::registerWindow(WindowID windowId, SDL3Window* window) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		// Usar versión interna sin lock
		m_impl->windows[windowId] = window;
		m_impl->createWindowQueueInternal(windowId);  // SIN deadlock
	}

	void SDL3PerWindowEventSystem::unregisterWindow(WindowID windowId) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		// Usar versión interna sin lock
		m_impl->destroyWindowQueueInternal(windowId);  // SIN deadlock
	}


	void SDL3PerWindowEventSystem::pollEvents() {
		SDL_Event sdlEvent;

		while (SDL_PollEvent(&sdlEvent)) {
			Event event = m_impl->translateEvent(sdlEvent);
			WindowID targetWindow = 0;

			if (sdlEvent.type >= SDL_EVENT_WINDOW_FIRST &&
				sdlEvent.type <= SDL_EVENT_WINDOW_LAST) {
				targetWindow = sdlEvent.window.windowID;
			}
			else if (sdlEvent.type >= SDL_EVENT_KEY_DOWN &&
				sdlEvent.type <= SDL_EVENT_KEY_UP) {
				targetWindow = sdlEvent.key.windowID;
			}
			else if (sdlEvent.type >= SDL_EVENT_MOUSE_MOTION &&
				sdlEvent.type <= SDL_EVENT_MOUSE_WHEEL) {
				targetWindow = m_impl->getWindowIdFromMouse(sdlEvent);
			}
			else if (sdlEvent.type == SDL_EVENT_QUIT) {
				targetWindow = 0;
			}

			m_impl->distributeEvent(event, targetWindow);

			// Callback global (lock separado)
			{
				std::lock_guard<std::mutex> lock(m_impl->callbackMutex);
				if (m_impl->globalCallback) {
					m_impl->globalCallback(event);
				}
			}

			m_impl->handleWindowClose(sdlEvent, targetWindow);
		}
	}

	bool SDL3PerWindowEventSystem::getEvent(Event& event) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		for (auto& [windowId, queue] : m_impl->windowQueues) {
			auto maybeEvent = queue->tryPopEvent();
			if (maybeEvent) {
				event = *maybeEvent;
				return true;
			}
		}

		return false;
	}

	bool SDL3PerWindowEventSystem::getEventForWindow(WindowID windowId, Event& event) {
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

	void SDL3PerWindowEventSystem::setEventCallback(std::function<void(const Event&)> callback) {
		std::lock_guard<std::mutex> lock(m_impl->callbackMutex);
		m_impl->globalCallback = callback;
	}

	void SDL3PerWindowEventSystem::setWindowEventFilter(WindowID windowId, EventFilter filter) {
		auto* queue = getWindowQueue(windowId);
		if (queue) {
			queue->setEventFilter(filter);
		}
	}

	void SDL3PerWindowEventSystem::setWindowEventWatcher(WindowID windowId, EventFilter watcher) {
		auto* queue = getWindowQueue(windowId);
		if (queue) {
			queue->setEventWatcher(watcher);
		}
	}

	size_t SDL3PerWindowEventSystem::getWindowQueueSize(WindowID windowId) const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windowQueues.find(windowId);
		return (it != m_impl->windowQueues.end()) ? it->second->size() : 0;
	}

	size_t SDL3PerWindowEventSystem::getTotalQueuedEvents() const {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		size_t total = 0;
		for (const auto& [_, queue] : m_impl->windowQueues) {
			total += queue->size();
		}
		return total;
	}

	// Implementaciones privadas de Impl
	void SDL3PerWindowEventSystem::Impl::distributeEvent(const Event& event, WindowID targetWindow) {
		std::lock_guard<std::mutex> lock(mutex);

		if (targetWindow == 0) {
			for (auto& [windowId, queue] : windowQueues) {
				queue->pushEvent(event);
			}
		}
		else {
			auto it = windowQueues.find(targetWindow);
			if (it != windowQueues.end()) {
				it->second->pushEvent(event);
			}
		}
	}

	WindowID SDL3PerWindowEventSystem::Impl::getWindowIdFromMouse(const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_EVENT_MOUSE_MOTION:
			return sdlEvent.motion.windowID;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
			return sdlEvent.button.windowID;
		case SDL_EVENT_MOUSE_WHEEL:
			return sdlEvent.wheel.windowID;
		default:
			return 0;
		}
	}

	void SDL3PerWindowEventSystem::Impl::handleWindowClose(const SDL_Event& sdlEvent, WindowID windowId) {
		if (sdlEvent.type == SDL_EVENT_QUIT) {
			for (auto& [id, window] : windows) {
				if (window) {
					window->markForClose();
				}
			}
		}
		else if (sdlEvent.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
			auto it = windows.find(windowId);
			if (it != windows.end() && it->second) {
				it->second->markForClose();
			}
		}
	}

	Event SDL3PerWindowEventSystem::Impl::translateEvent(const SDL_Event& sdlEvent) {
		Event event{};
		event.timestamp = sdlEvent.common.timestamp;

		switch (sdlEvent.type) {
		case SDL_EVENT_QUIT:
			event.type = EventType::WindowClose;
			break;

		case SDL_EVENT_WINDOW_RESIZED:
			event.type = EventType::WindowResize;
			event.windowResize.width = sdlEvent.window.data1;
			event.windowResize.height = sdlEvent.window.data2;
			break;

		case SDL_EVENT_WINDOW_MOVED:
			event.type = EventType::WindowMoved;
			event.windowMoved.x = sdlEvent.window.data1;
			event.windowMoved.y = sdlEvent.window.data2;
			break;

		case SDL_EVENT_WINDOW_FOCUS_GAINED:
			event.type = EventType::WindowFocus;
			break;

		case SDL_EVENT_WINDOW_FOCUS_LOST:
			event.type = EventType::WindowLostFocus;
			break;

		case SDL_EVENT_WINDOW_MINIMIZED:
			event.type = EventType::WindowMinimized;
			break;

		case SDL_EVENT_WINDOW_MAXIMIZED:
			event.type = EventType::WindowMaximized;
			break;

		case SDL_EVENT_WINDOW_RESTORED:
			event.type = EventType::WindowRestored;
			break;

		case SDL_EVENT_KEY_DOWN:
			event.type = EventType::KeyPress;
			event.key.key = translateKeyCode(sdlEvent.key.key);
			event.key.repeat = sdlEvent.key.repeat;
			event.key.mod = sdlEvent.key.mod;
			break;

		case SDL_EVENT_KEY_UP:
			event.type = EventType::KeyRelease;
			event.key.key = translateKeyCode(sdlEvent.key.key);
			event.key.mod = sdlEvent.key.mod;
			break;

		case SDL_EVENT_MOUSE_MOTION:
			event.type = EventType::MouseMove;
			event.mouseMove.x = sdlEvent.motion.x;
			event.mouseMove.y = sdlEvent.motion.y;
			event.mouseMove.deltaX = sdlEvent.motion.xrel;
			event.mouseMove.deltaY = sdlEvent.motion.yrel;
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			event.type = EventType::MouseButtonPress;
			event.mouseButton.button = translateMouseButton(sdlEvent.button.button);
			event.mouseButton.x = sdlEvent.button.x;
			event.mouseButton.y = sdlEvent.button.y;
			event.mouseButton.clicks = sdlEvent.button.clicks;
			break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
			event.type = EventType::MouseButtonRelease;
			event.mouseButton.button = translateMouseButton(sdlEvent.button.button);
			event.mouseButton.x = sdlEvent.button.x;
			event.mouseButton.y = sdlEvent.button.y;
			event.mouseButton.clicks = sdlEvent.button.clicks;
			break;

		case SDL_EVENT_MOUSE_WHEEL:
			event.type = EventType::MouseScroll;
			event.mouseScroll.deltaX = sdlEvent.wheel.x;
			event.mouseScroll.deltaY = sdlEvent.wheel.y;
			event.mouseScroll.flipped = (sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED);
			break;

		case SDL_EVENT_GAMEPAD_ADDED:
			event.type = EventType::GamepadConnected;
			break;

		case SDL_EVENT_GAMEPAD_REMOVED:
			event.type = EventType::GamepadDisconnected;
			break;
		}

		return event;
	}

	KeyCode SDL3PerWindowEventSystem::Impl::translateKeyCode(SDL_Keycode key) {
		switch (key) {
		case SDLK_A: return KeyCode::A;
		case SDLK_B: return KeyCode::B;
		case SDLK_C: return KeyCode::C;
		case SDLK_D: return KeyCode::D;
		case SDLK_E: return KeyCode::E;
		case SDLK_F: return KeyCode::F;
		case SDLK_G: return KeyCode::G;
		case SDLK_H: return KeyCode::H;
		case SDLK_I: return KeyCode::I;
		case SDLK_J: return KeyCode::J;
		case SDLK_K: return KeyCode::K;
		case SDLK_L: return KeyCode::L;
		case SDLK_M: return KeyCode::M;
		case SDLK_N: return KeyCode::N;
		case SDLK_O: return KeyCode::O;
		case SDLK_P: return KeyCode::P;
		case SDLK_Q: return KeyCode::Q;
		case SDLK_R: return KeyCode::R;
		case SDLK_S: return KeyCode::S;
		case SDLK_T: return KeyCode::T;
		case SDLK_U: return KeyCode::U;
		case SDLK_V: return KeyCode::V;
		case SDLK_W: return KeyCode::W;
		case SDLK_X: return KeyCode::X;
		case SDLK_Y: return KeyCode::Y;
		case SDLK_Z: return KeyCode::Z;
		case SDLK_0: return KeyCode::Num0;
		case SDLK_1: return KeyCode::Num1;
		case SDLK_2: return KeyCode::Num2;
		case SDLK_3: return KeyCode::Num3;
		case SDLK_4: return KeyCode::Num4;
		case SDLK_5: return KeyCode::Num5;
		case SDLK_6: return KeyCode::Num6;
		case SDLK_7: return KeyCode::Num7;
		case SDLK_8: return KeyCode::Num8;
		case SDLK_9: return KeyCode::Num9;
		case SDLK_ESCAPE: return KeyCode::Escape;
		case SDLK_SPACE: return KeyCode::Space;
		case SDLK_RETURN: return KeyCode::Enter;
		case SDLK_TAB: return KeyCode::Tab;
		case SDLK_BACKSPACE: return KeyCode::Backspace;
		case SDLK_LEFT: return KeyCode::Left;
		case SDLK_RIGHT: return KeyCode::Right;
		case SDLK_UP: return KeyCode::Up;
		case SDLK_DOWN: return KeyCode::Down;
		case SDLK_F1: return KeyCode::F1;
		case SDLK_F2: return KeyCode::F2;
		case SDLK_F3: return KeyCode::F3;
		case SDLK_F4: return KeyCode::F4;
		case SDLK_F5: return KeyCode::F5;
		case SDLK_F6: return KeyCode::F6;
		case SDLK_F7: return KeyCode::F7;
		case SDLK_F8: return KeyCode::F8;
		case SDLK_F9: return KeyCode::F9;
		case SDLK_F10: return KeyCode::F10;
		case SDLK_F11: return KeyCode::F11;
		case SDLK_F12: return KeyCode::F12;
		default: return KeyCode::Unknown;
		}
	}

	MouseButton SDL3PerWindowEventSystem::Impl::translateMouseButton(uint8_t button) {
		switch (button) {
		case SDL_BUTTON_LEFT: return MouseButton::Left;
		case SDL_BUTTON_RIGHT: return MouseButton::Right;
		case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
		case SDL_BUTTON_X1: return MouseButton::X1;
		case SDL_BUTTON_X2: return MouseButton::X2;
		default: return MouseButton::Left;
		}
	}

} // namespace pgrender::backends::sdl3
