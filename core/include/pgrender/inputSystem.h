#pragma once
#include "types.h"
#include <memory>
#include <vector>
#include <string>

namespace pgrender {

// Gamepad
class IGamepad {
public:
    virtual ~IGamepad() = default;
    
    virtual bool isConnected() const = 0;
    virtual std::string getName() const = 0;
    virtual GamepadInfo getInfo() const = 0;
    
    virtual bool getButton(GamepadButton button) const = 0;
    virtual float getAxis(GamepadAxis axis) const = 0;
    
    virtual bool rumble(float lowFreq, float highFreq, uint32_t durationMs) = 0;
    virtual bool rumbleTriggers(float left, float right, uint32_t durationMs) = 0;
    virtual void stopRumble() = 0;
    
    virtual bool setLED(uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual void setButtonMapping(const std::string& mapping) = 0;
};

// Sistema de entrada avanzado
class IAdvancedInputSystem {
public:
	virtual ~IAdvancedInputSystem() = default;

	// Gamepads
	virtual int getGamepadCount() const = 0;
	virtual std::unique_ptr<IGamepad> openGamepad(int index) = 0;
	virtual std::vector<GamepadInfo> getAvailableGamepads() const = 0;

	// Portapapeles
	virtual void setClipboardText(const std::string& text) = 0;
	virtual std::string getClipboardText() const = 0;
	virtual bool hasClipboardText() const = 0;

	// Cursor (funciones globales)
	virtual void showCursor(bool show) = 0;
	virtual bool isCursorVisible() const = 0;
	virtual void setCursorPosition(int x, int y) = 0;
	virtual void captureMouse(bool capture) = 0;

protected:
	IAdvancedInputSystem() = default;
};


} // namespace pgrender
