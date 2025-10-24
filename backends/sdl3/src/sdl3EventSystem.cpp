#include "sdl3EventSystem.h"
#include <SDL3/SDL.h>

namespace pgrender::backends::sdl3 {

	static Event translateEvent(const SDL_Event& sdlEvent);
	static WindowID getWindowIdFromMouse(const SDL_Event& sdlEvent);
	static KeyCode translateKeyCode(SDL_Keycode key);
	static MouseButton translateMouseButton(uint8_t button);

	SDL3EventSystem::~SDL3EventSystem() = default;

	void SDL3EventSystem::pollEvents() {
		SDL_Event sdlEvent;

		while (SDL_PollEvent(&sdlEvent)) {
			Event event = translateEvent(sdlEvent);
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
				targetWindow = getWindowIdFromMouse(sdlEvent);
			}
			else if (sdlEvent.type == SDL_EVENT_QUIT) {
				targetWindow = 0;
			}

			distributeEvent(event, targetWindow);
		}
	}


	WindowID getWindowIdFromMouse(const SDL_Event& sdlEvent) {
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


	Event translateEvent(const SDL_Event& sdlEvent) {
		Event event{};
		event.timestamp = sdlEvent.common.timestamp;

		switch (sdlEvent.type) {
		case SDL_EVENT_QUIT:
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
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

	KeyCode translateKeyCode(SDL_Keycode key) {
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

	MouseButton translateMouseButton(uint8_t button) {
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
