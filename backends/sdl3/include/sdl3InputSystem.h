#pragma once


#include "pgrender/inputSystem.h"
#include <memory>


namespace pgrender::backends::sdl3 {
// ============================================================================
// Gamepad
// ============================================================================

class SDL3Gamepad : public IGamepad {
public:
	explicit SDL3Gamepad(int instanceId);
	~SDL3Gamepad() override;

	// No copiable ni movible
	SDL3Gamepad(const SDL3Gamepad&) = delete;
	SDL3Gamepad& operator=(const SDL3Gamepad&) = delete;
	SDL3Gamepad(SDL3Gamepad&&) = delete;
	SDL3Gamepad& operator=(SDL3Gamepad&&) = delete;

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
// Input System
// ============================================================================

class SDL3InputSystem : public IInputSystem {
public:
	SDL3InputSystem();
	~SDL3InputSystem() override;

	// No copiable ni movible
	SDL3InputSystem(const SDL3InputSystem&) = delete;
	SDL3InputSystem& operator=(const SDL3InputSystem&) = delete;
	SDL3InputSystem(SDL3InputSystem&&) = delete;
	SDL3InputSystem& operator=(SDL3InputSystem&&) = delete;

	int getGamepadCount() const override;
	std::unique_ptr<IGamepad> openGamepad(int index) override;
	std::vector<GamepadInfo> getAvailableGamepads() const override;

	void setClipboardText(const std::string& text) override;
	std::string getClipboardText() const override;
	bool hasClipboardText() const override;

	void showCursor(bool show) override;
	bool isCursorVisible() const override;
	void setCursorPosition(int x, int y) override;
	void captureMouse(bool capture) override;

private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};

} // namespace pgrender::backends::sdl3