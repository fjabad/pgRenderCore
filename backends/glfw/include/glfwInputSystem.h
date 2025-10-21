#pragma once
#include "pgrender/inputSystem.h"
#include <memory>

namespace pgrender::backends::glfw {

	// ============================================================================
	// GLFWGamepad
	// ============================================================================

	class GLFWGamepad : public IGamepad {
	public:
		explicit GLFWGamepad(int joystickId);
		~GLFWGamepad() override;

		// No copiable ni movible
		GLFWGamepad(const GLFWGamepad&) = delete;
		GLFWGamepad& operator=(const GLFWGamepad&) = delete;
		GLFWGamepad(GLFWGamepad&&) = delete;
		GLFWGamepad& operator=(GLFWGamepad&&) = delete;

		bool isConnected() const override;
		std::string getName() const override;
		GamepadInfo getInfo() const override;

		bool getButton(GamepadButton button) const override;
		float getAxis(GamepadAxis axis) const override;

		bool rumble(float lowFreq, float highFreq, uint32_t durationMs) override;
		bool rumbleTriggers(float left, float right, uint32_t durationMs) override;
		void stopRumble() override;

		bool setLED(uint8_t r, uint8_t g, uint8_t b) override;
		void setButtonMapping(const std::string& mapping) override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

	// ============================================================================
	// GLFWInputSystem
	// ============================================================================

	class GLFWInputSystem : public IInputSystem {
	public:
		GLFWInputSystem();
		~GLFWInputSystem() override;

		// No copiable ni movible
		GLFWInputSystem(const GLFWInputSystem&) = delete;
		GLFWInputSystem& operator=(const GLFWInputSystem&) = delete;
		GLFWInputSystem(GLFWInputSystem&&) = delete;
		GLFWInputSystem& operator=(GLFWInputSystem&&) = delete;

		// Gamepads
		int getGamepadCount() const override;
		std::unique_ptr<IGamepad> openGamepad(int index) override;
		std::vector<GamepadInfo> getAvailableGamepads() const override;

		// Portapapeles
		void setClipboardText(const std::string& text) override;
		std::string getClipboardText() const override;
		bool hasClipboardText() const override;

		// Cursor
		void showCursor(bool show) override;
		bool isCursorVisible() const override;
		void setCursorPosition(int x, int y) override;
		void captureMouse(bool capture) override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

} // namespace pgrender::backends::glfw
