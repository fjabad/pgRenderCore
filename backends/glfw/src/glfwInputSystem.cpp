#include "glfwInputSystem.h"
#include <GLFW/glfw3.h>
#include <cstring>
#include <stdexcept>

namespace pgrender::backends::glfw {

	// ============================================================================
	// GLFWGamepad::Impl
	// ============================================================================

	class GLFWGamepad::Impl {
	public:
		int joystickId;
		bool connected = false;

		explicit Impl(int jid) : joystickId(jid) {
			connected = glfwJoystickPresent(joystickId) && glfwJoystickIsGamepad(joystickId);
			if (!connected) {
				throw std::runtime_error("Gamepad not present or not supported");
			}
		}

		int translateButton(GamepadButton button) const {
			switch (button) {
			case GamepadButton::A: return GLFW_GAMEPAD_BUTTON_A;
			case GamepadButton::B: return GLFW_GAMEPAD_BUTTON_B;
			case GamepadButton::X: return GLFW_GAMEPAD_BUTTON_X;
			case GamepadButton::Y: return GLFW_GAMEPAD_BUTTON_Y;
			case GamepadButton::Back: return GLFW_GAMEPAD_BUTTON_BACK;
			case GamepadButton::Guide: return GLFW_GAMEPAD_BUTTON_GUIDE;
			case GamepadButton::Start: return GLFW_GAMEPAD_BUTTON_START;
			case GamepadButton::LeftStick: return GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
			case GamepadButton::RightStick: return GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
			case GamepadButton::LeftShoulder: return GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
			case GamepadButton::RightShoulder: return GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
			case GamepadButton::DPadUp: return GLFW_GAMEPAD_BUTTON_DPAD_UP;
			case GamepadButton::DPadDown: return GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
			case GamepadButton::DPadLeft: return GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
			case GamepadButton::DPadRight: return GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
			default: return -1;
			}
		}

		int translateAxis(GamepadAxis axis) const {
			switch (axis) {
			case GamepadAxis::LeftX: return GLFW_GAMEPAD_AXIS_LEFT_X;
			case GamepadAxis::LeftY: return GLFW_GAMEPAD_AXIS_LEFT_Y;
			case GamepadAxis::RightX: return GLFW_GAMEPAD_AXIS_RIGHT_X;
			case GamepadAxis::RightY: return GLFW_GAMEPAD_AXIS_RIGHT_Y;
			case GamepadAxis::TriggerLeft: return GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
			case GamepadAxis::TriggerRight: return GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
			default: return -1;
			}
		}
	};

	// ============================================================================
	// GLFWGamepad
	// ============================================================================

	GLFWGamepad::GLFWGamepad(int joystickId)
		: m_impl(std::make_unique<Impl>(joystickId)) {
	}

	GLFWGamepad::~GLFWGamepad() = default;

	bool GLFWGamepad::isConnected() const {
		return glfwJoystickPresent(m_impl->joystickId) && glfwJoystickIsGamepad(m_impl->joystickId);
	}

	std::string GLFWGamepad::getName() const {
		const char* name = glfwGetGamepadName(m_impl->joystickId);
		return name ? name : "Unknown Gamepad";
	}

	GamepadInfo GLFWGamepad::getInfo() const {
		GamepadInfo info{};

		info.deviceIndex = m_impl->joystickId;
		info.name = getName();

		// GLFW no proporciona vendor/product ID directamente
		// Usar valores por defecto
		info.vendor = 0;
		info.product = 0;
		info.serial = "";
		info.playerIndex = -1;

		// GLFW no tiene API para detectar estas capacidades
		// Asumir capacidades básicas
		info.hasRumble = false; // GLFW no soporta rumble directamente
		info.hasRumbleTriggers = false;
		info.hasLED = false;
		info.hasTouchpad = false;
		info.hasSensors = false;

		// Intentar detectar tipo por nombre
		std::string nameStr = info.name;
		if (nameStr.find("Xbox") != std::string::npos || nameStr.find("360") != std::string::npos) {
			info.type = GamepadType::Xbox360;
		}
		else if (nameStr.find("Xbox One") != std::string::npos || nameStr.find("Series") != std::string::npos) {
			info.type = GamepadType::XboxOne;
		}
		else if (nameStr.find("PS3") != std::string::npos || nameStr.find("PLAYSTATION(R)3") != std::string::npos) {
			info.type = GamepadType::PS3;
		}
		else if (nameStr.find("PS4") != std::string::npos || nameStr.find("DualShock 4") != std::string::npos) {
			info.type = GamepadType::PS4;
			info.hasTouchpad = true;
		}
		else if (nameStr.find("PS5") != std::string::npos || nameStr.find("DualSense") != std::string::npos) {
			info.type = GamepadType::PS5;
			info.hasTouchpad = true;
		}
		else if (nameStr.find("Switch") != std::string::npos || nameStr.find("Joy-Con") != std::string::npos) {
			info.type = GamepadType::NintendoSwitch;
		}
		else {
			info.type = GamepadType::Unknown;
		}

		return info;
	}

	bool GLFWGamepad::getButton(GamepadButton button) const {
		GLFWgamepadstate state;
		if (!glfwGetGamepadState(m_impl->joystickId, &state)) {
			return false;
		}

		int glfwButton = m_impl->translateButton(button);
		if (glfwButton < 0 || glfwButton >= GLFW_GAMEPAD_BUTTON_LAST) {
			return false;
		}

		return state.buttons[glfwButton] == GLFW_PRESS;
	}

	float GLFWGamepad::getAxis(GamepadAxis axis) const {
		GLFWgamepadstate state;
		if (!glfwGetGamepadState(m_impl->joystickId, &state)) {
			return 0.0f;
		}

		int glfwAxis = m_impl->translateAxis(axis);
		if (glfwAxis < 0 || glfwAxis >= GLFW_GAMEPAD_AXIS_LAST) {
			return 0.0f;
		}

		return state.axes[glfwAxis];
	}

	bool GLFWGamepad::rumble(float lowFreq, float highFreq, uint32_t durationMs) {
		// GLFW no soporta rumble directamente
		// Retornar false para indicar que no está soportado
		(void)lowFreq;
		(void)highFreq;
		(void)durationMs;
		return false;
	}

	bool GLFWGamepad::rumbleTriggers(float left, float right, uint32_t durationMs) {
		// GLFW no soporta rumble en gatillos
		(void)left;
		(void)right;
		(void)durationMs;
		return false;
	}

	void GLFWGamepad::stopRumble() {
		// GLFW no soporta rumble
	}

	bool GLFWGamepad::setLED(uint8_t r, uint8_t g, uint8_t b) {
		// GLFW no soporta control de LEDs
		(void)r;
		(void)g;
		(void)b;
		return false;
	}

	void GLFWGamepad::setButtonMapping(const std::string& mapping) {
		// GLFW tiene su propio sistema de mapeo de gamepads
		// Intentar actualizar el mapeo
		if (!mapping.empty()) {
			glfwUpdateGamepadMappings(mapping.c_str());
		}
	}

	// ============================================================================
	// GLFWInputSystem::Impl
	// ============================================================================

	class GLFWInputSystem::Impl {
	public:
		GLFWwindow* currentWindow = nullptr;
		bool cursorVisible = true;

		Impl() {
			// No requiere inicialización especial
		}

		~Impl() {
			// No requiere limpieza especial
		}
	};

	// ============================================================================
	// GLFWInputSystem
	// ============================================================================

	GLFWInputSystem::GLFWInputSystem()
		: m_impl(std::make_unique<Impl>()) {
	}

	GLFWInputSystem::~GLFWInputSystem() = default;

	int GLFWInputSystem::getGamepadCount() const {
		int count = 0;
		for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
			if (glfwJoystickPresent(jid) && glfwJoystickIsGamepad(jid)) {
				count++;
			}
		}
		return count;
	}

	std::unique_ptr<IGamepad> GLFWInputSystem::openGamepad(int index) {
		int currentIndex = 0;
		for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
			if (glfwJoystickPresent(jid) && glfwJoystickIsGamepad(jid)) {
				if (currentIndex == index) {
					try {
						return std::make_unique<GLFWGamepad>(jid);
					}
					catch (const std::exception&) {
						return nullptr;
					}
				}
				currentIndex++;
			}
		}
		return nullptr;
	}

	std::vector<GamepadInfo> GLFWInputSystem::getAvailableGamepads() const {
		std::vector<GamepadInfo> gamepads;

		for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
			if (glfwJoystickPresent(jid) && glfwJoystickIsGamepad(jid)) {
				try {
					auto gamepad = std::make_unique<GLFWGamepad>(jid);
					gamepads.push_back(gamepad->getInfo());
				}
				catch (const std::exception&) {
					// Ignorar gamepads que no se puedan abrir
				}
			}
		}

		return gamepads;
	}

	void GLFWInputSystem::setClipboardText(const std::string& text) {
		// GLFW requiere una ventana para clipboard
		// Usar la ventana actual del contexto si está disponible
		GLFWwindow* window = glfwGetCurrentContext();
		if (window) {
			glfwSetClipboardString(window, text.c_str());
		}
	}

	std::string GLFWInputSystem::getClipboardText() const {
		GLFWwindow* window = glfwGetCurrentContext();
		if (window) {
			const char* text = glfwGetClipboardString(window);
			return text ? text : "";
		}
		return "";
	}

	bool GLFWInputSystem::hasClipboardText() const {
		std::string text = getClipboardText();
		return !text.empty();
	}

	void GLFWInputSystem::showCursor(bool show) {
		m_impl->cursorVisible = show;

		// GLFW requiere una ventana específica
		// Aplicar a la ventana actual si está disponible
		GLFWwindow* window = glfwGetCurrentContext();
		if (window) {
			glfwSetInputMode(window, GLFW_CURSOR,
				show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
		}
	}

	bool GLFWInputSystem::isCursorVisible() const {
		return m_impl->cursorVisible;
	}

	void GLFWInputSystem::setCursorPosition(int x, int y) {
		GLFWwindow* window = glfwGetCurrentContext();
		if (window) {
			glfwSetCursorPos(window, static_cast<double>(x), static_cast<double>(y));
		}
	}

	void GLFWInputSystem::captureMouse(bool capture) {
		GLFWwindow* window = glfwGetCurrentContext();
		if (window) {
			glfwSetInputMode(window, GLFW_CURSOR,
				capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		}
	}

} // namespace pgrender::backends::glfw
