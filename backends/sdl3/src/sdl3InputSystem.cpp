#include "sdl3InputSystem.h"
#include <SDL3/SDL.h>
#include <stdexcept>


namespace pgrender::backends::sdl3 {

	// ============================================================================
	// SDL3Gamepad::Impl
	// ============================================================================

	class SDL3Gamepad::Impl {
	public:
		SDL_Gamepad* gamepad = nullptr;

		explicit Impl(int instanceId) {
			gamepad = SDL_OpenGamepad(static_cast<SDL_JoystickID>(instanceId));
			if (!gamepad) {
				throw std::runtime_error(std::string("Failed to open gamepad: ") + SDL_GetError());
			}
		}

		~Impl() {
			if (gamepad) {
				SDL_CloseGamepad(gamepad);
			}
		}

		SDL_GamepadButton translateButton(GamepadButton button) const {
			switch (button) {
			case GamepadButton::A: return SDL_GAMEPAD_BUTTON_SOUTH;
			case GamepadButton::B: return SDL_GAMEPAD_BUTTON_EAST;
			case GamepadButton::X: return SDL_GAMEPAD_BUTTON_WEST;
			case GamepadButton::Y: return SDL_GAMEPAD_BUTTON_NORTH;
			case GamepadButton::Back: return SDL_GAMEPAD_BUTTON_BACK;
			case GamepadButton::Guide: return SDL_GAMEPAD_BUTTON_GUIDE;
			case GamepadButton::Start: return SDL_GAMEPAD_BUTTON_START;
			case GamepadButton::LeftStick: return SDL_GAMEPAD_BUTTON_LEFT_STICK;
			case GamepadButton::RightStick: return SDL_GAMEPAD_BUTTON_RIGHT_STICK;
			case GamepadButton::LeftShoulder: return SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
			case GamepadButton::RightShoulder: return SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;
			case GamepadButton::DPadUp: return SDL_GAMEPAD_BUTTON_DPAD_UP;
			case GamepadButton::DPadDown: return SDL_GAMEPAD_BUTTON_DPAD_DOWN;
			case GamepadButton::DPadLeft: return SDL_GAMEPAD_BUTTON_DPAD_LEFT;
			case GamepadButton::DPadRight: return SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
			default: return SDL_GAMEPAD_BUTTON_INVALID;
			}
		}

		SDL_GamepadAxis translateAxis(GamepadAxis axis) const {
			switch (axis) {
			case GamepadAxis::LeftX: return SDL_GAMEPAD_AXIS_LEFTX;
			case GamepadAxis::LeftY: return SDL_GAMEPAD_AXIS_LEFTY;
			case GamepadAxis::RightX: return SDL_GAMEPAD_AXIS_RIGHTX;
			case GamepadAxis::RightY: return SDL_GAMEPAD_AXIS_RIGHTY;
			case GamepadAxis::TriggerLeft: return SDL_GAMEPAD_AXIS_LEFT_TRIGGER;
			case GamepadAxis::TriggerRight: return SDL_GAMEPAD_AXIS_RIGHT_TRIGGER;
			default: return SDL_GAMEPAD_AXIS_INVALID;
			}
		}
	};

	SDL3Gamepad::SDL3Gamepad(int instanceId)
		: m_impl(std::make_unique<Impl>(instanceId)) {
	}

	SDL3Gamepad::~SDL3Gamepad() = default;

	bool SDL3Gamepad::isConnected() const {
		return SDL_GamepadConnected(m_impl->gamepad);
	}

	std::string SDL3Gamepad::getName() const {
		const char* name = SDL_GetGamepadName(m_impl->gamepad);
		return name ? name : "";
	}

	GamepadInfo SDL3Gamepad::getInfo() const {
		GamepadInfo info{};

		// Obtener propiedades del gamepad
		SDL_PropertiesID props = SDL_GetGamepadProperties(m_impl->gamepad);

		// Información básica
		const char* name = SDL_GetGamepadName(m_impl->gamepad);
		info.name = name ? name : "Unknown Gamepad";

		info.vendor = SDL_GetGamepadVendor(m_impl->gamepad);
		info.product = SDL_GetGamepadProduct(m_impl->gamepad);

		const char* serial = SDL_GetGamepadSerial(m_impl->gamepad);
		info.serial = serial ? serial : "";

		info.playerIndex = SDL_GetGamepadPlayerIndex(m_impl->gamepad);

		// Capacidades usando propiedades (forma correcta en SDL3)
		if (props) {
			info.hasRumble = SDL_GetBooleanProperty(props, SDL_PROP_GAMEPAD_CAP_RUMBLE_BOOLEAN, false);
			info.hasRumbleTriggers = SDL_GetBooleanProperty(props, SDL_PROP_GAMEPAD_CAP_TRIGGER_RUMBLE_BOOLEAN, false);

			// LED puede ser RGB, player indicator, o mono
			bool hasRgbLed = SDL_GetBooleanProperty(props, SDL_PROP_GAMEPAD_CAP_RGB_LED_BOOLEAN, false);
			bool hasPlayerLed = SDL_GetBooleanProperty(props, SDL_PROP_GAMEPAD_CAP_PLAYER_LED_BOOLEAN, false);
			bool hasMonoLed = SDL_GetBooleanProperty(props, SDL_PROP_GAMEPAD_CAP_MONO_LED_BOOLEAN, false);

			info.hasLED = hasRgbLed || hasPlayerLed || hasMonoLed;
		}
		else {
			// Fallback si las propiedades no están disponibles
			info.hasRumble = false;
			info.hasRumbleTriggers = false;
			info.hasLED = false;
		}

		// Sensores (estos siguen teniendo funciones dedicadas)
		info.hasSensors = SDL_GamepadHasSensor(m_impl->gamepad, SDL_SENSOR_GYRO) ||
			SDL_GamepadHasSensor(m_impl->gamepad, SDL_SENSOR_ACCEL);

		// Tipo de gamepad
		SDL_GamepadType gamepadType = SDL_GetGamepadType(m_impl->gamepad);

		switch (gamepadType) {
		case SDL_GAMEPAD_TYPE_XBOX360:
			info.type = GamepadType::Xbox360;
			info.hasTouchpad = false;
			break;

		case SDL_GAMEPAD_TYPE_XBOXONE:
			info.type = GamepadType::XboxOne;
			info.hasTouchpad = false;
			break;

		case SDL_GAMEPAD_TYPE_PS3:
			info.type = GamepadType::PS3;
			info.hasTouchpad = false;
			break;

		case SDL_GAMEPAD_TYPE_PS4:
			info.type = GamepadType::PS4;
			info.hasTouchpad = true;
			break;

		case SDL_GAMEPAD_TYPE_PS5:
			info.type = GamepadType::PS5;
			info.hasTouchpad = true;
			break;

		case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
		case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
		case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
		case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
			info.type = GamepadType::NintendoSwitch;
			info.hasTouchpad = false;
			break;
		default:
			info.type = GamepadType::Unknown;
			info.hasTouchpad = false;
			break;
		}

		return info;
	}


	bool SDL3Gamepad::getButton(GamepadButton button) const {
		return SDL_GetGamepadButton(m_impl->gamepad, m_impl->translateButton(button));
	}

	float SDL3Gamepad::getAxis(GamepadAxis axis) const {
		int16_t value = SDL_GetGamepadAxis(m_impl->gamepad, m_impl->translateAxis(axis));
		return value / 32767.0f;
	}

	bool SDL3Gamepad::rumble(float lowFreq, float highFreq, uint32_t durationMs) {
		uint16_t low = static_cast<uint16_t>(lowFreq * 0xFFFF);
		uint16_t high = static_cast<uint16_t>(highFreq * 0xFFFF);
		return SDL_RumbleGamepad(m_impl->gamepad, low, high, durationMs);
	}

	bool SDL3Gamepad::rumbleTriggers(float left, float right, uint32_t durationMs) {
		uint16_t leftVal = static_cast<uint16_t>(left * 0xFFFF);
		uint16_t rightVal = static_cast<uint16_t>(right * 0xFFFF);
		return SDL_RumbleGamepadTriggers(m_impl->gamepad, leftVal, rightVal, durationMs);
	}

	void SDL3Gamepad::stopRumble() {
		SDL_RumbleGamepad(m_impl->gamepad, 0, 0, 0);
	}

	bool SDL3Gamepad::setLED(uint8_t r, uint8_t g, uint8_t b) {
		return SDL_SetGamepadLED(m_impl->gamepad, r, g, b);
	}

	void SDL3Gamepad::setButtonMapping(const std::string& mapping) {
		SDL_AddGamepadMapping(mapping.c_str());
	}

	// ============================================================================
	// SDL3AdvancedInputSystem::Impl
	// ============================================================================

	class SDL3InputSystem::Impl {
	public:
		// No state needed for now
	};

	SDL3InputSystem::SDL3InputSystem()
		: m_impl(std::make_unique<Impl>()) {
	}

	SDL3InputSystem::~SDL3InputSystem() = default;

	int SDL3InputSystem::getGamepadCount() const {
		int count = 0;
		SDL_JoystickID* joysticks = SDL_GetGamepads(&count);
		SDL_free(joysticks);
		return count;
	}

	std::unique_ptr<IGamepad> SDL3InputSystem::openGamepad(int index) {
		int count = 0;
		SDL_JoystickID* joysticks = SDL_GetGamepads(&count);

		if (index < 0 || index >= count) {
			SDL_free(joysticks);
			return nullptr;
		}

		SDL_JoystickID id = joysticks[index];
		SDL_free(joysticks);

		return std::make_unique<SDL3Gamepad>(id);
	}

	std::vector<GamepadInfo> SDL3InputSystem::getAvailableGamepads() const {
		std::vector<GamepadInfo> gamepads;

		int count = 0;
		SDL_JoystickID* joysticks = SDL_GetGamepads(&count);

		for (int i = 0; i < count; ++i) {
			try {
				auto gamepad = std::make_unique<SDL3Gamepad>(joysticks[i]);
				gamepads.push_back(gamepad->getInfo());
			}
			catch (...) {
				// Ignorar gamepads que no se puedan abrir
			}
		}

		SDL_free(joysticks);
		return gamepads;
	}

	void SDL3InputSystem::setClipboardText(const std::string& text) {
		SDL_SetClipboardText(text.c_str());
	}

	std::string SDL3InputSystem::getClipboardText() const {
		char* text = SDL_GetClipboardText();
		std::string result = text ? text : "";
		SDL_free(text);
		return result;
	}

	bool SDL3InputSystem::hasClipboardText() const {
		return SDL_HasClipboardText();
	}

	void SDL3InputSystem::showCursor(bool show) {
		if (show) {
			SDL_ShowCursor();
		}
		else {
			SDL_HideCursor();
		}
	}

	bool SDL3InputSystem::isCursorVisible() const {
		return SDL_CursorVisible();
	}

	void SDL3InputSystem::setCursorPosition(int x, int y) {
		SDL_WarpMouseGlobal(static_cast<float>(x), static_cast<float>(y));
	}

	void SDL3InputSystem::captureMouse(bool capture) {
		SDL_CaptureMouse(capture);
	}

} // namespace pgrender::backends::sdl3