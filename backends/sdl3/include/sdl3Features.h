#pragma once
#include "pgrender/windowManager.h"
#include "pgrender/inputSystem.h"
#include "pgrender/renderCore.h"
#include <memory>

namespace pgrender::backends::sdl3 {

	// Forward declarations
	class SDL3PerWindowEventSystem;

	// ============================================================================
	// Window Manager
	// ============================================================================

	class SDL3WindowManager : public IWindowManager {
	public:
		explicit SDL3WindowManager(ILibraryContext& context);
		~SDL3WindowManager() override;

		// No copiable ni movible
		SDL3WindowManager(const SDL3WindowManager&) = delete;
		SDL3WindowManager& operator=(const SDL3WindowManager&) = delete;
		SDL3WindowManager(SDL3WindowManager&&) = delete;
		SDL3WindowManager& operator=(SDL3WindowManager&&) = delete;

		// Gestión de ventanas
		WindowID createWindow(const WindowConfig& config) override;
		void destroyWindow(WindowID id) override;
		void closeAllWindows() override;

		IWindow* getWindow(WindowID id) override;
		const IWindow* getWindow(WindowID id) const override;

		std::vector<WindowID> getActiveWindows() const override;
		size_t getWindowCount() const override;
		bool hasOpenWindows() const override;

		// Contextos gráficos
		IGraphicsContext* getWindowContext(WindowID id) override;
		void setWindowContext(WindowID id, std::unique_ptr<IGraphicsContext> context) override;

		// Displays
		int getDisplayCount() const override;
		DisplayInfo getDisplayInfo(int index) const override;
		int getWindowDisplayIndex(WindowID windowId) const override;

		// Posicionamiento
		void centerWindowOnDisplay(WindowID windowId, int displayIndex) override;
		void setWindowPosition(WindowID windowId, int x, int y) override;
		void getWindowPosition(WindowID windowId, int& x, int& y) const override;

		// Estados
		void maximizeWindow(WindowID windowId) override;
		void minimizeWindow(WindowID windowId) override;
		void restoreWindow(WindowID windowId) override;
		void raiseWindow(WindowID windowId) override;

		// Fullscreen
		void setFullscreen(WindowID windowId, bool fullscreen) override;
		void setFullscreenDesktop(WindowID windowId, bool fullscreen) override;

		// Apariencia
		void setWindowOpacity(WindowID windowId, float opacity) override;
		float getWindowOpacity(WindowID windowId) const override;
		void setBordered(WindowID windowId, bool bordered) override;

		// Eventos
		void pollEvents() override;
		bool getEventForWindow(WindowID windowId, Event& event) override;
		void processWindowClosures() override;

		void setWindowEventCallback(WindowID id, WindowEventCallback callback) override;
		void setWindowEventFilter(WindowID id, EventFilter filter) override;
		void setWindowEventWatcher(WindowID id, EventFilter watcher) override;

		size_t getWindowQueueSize(WindowID id) const override;
		size_t getTotalQueuedEvents() const override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

	// ============================================================================
	// Library Context
	// ============================================================================

	class SDL3LibraryContext : public ILibraryContext {
	public:
		SDL3LibraryContext();
		~SDL3LibraryContext() override;

		SDL3LibraryContext(const SDL3LibraryContext&) = delete;
		SDL3LibraryContext& operator=(const SDL3LibraryContext&) = delete;
		SDL3LibraryContext(SDL3LibraryContext&&) = delete;
		SDL3LibraryContext& operator=(SDL3LibraryContext&&) = delete;

		std::unique_ptr<IGraphicsContext> createHeadlessContext(const ContextConfig& config) override;

		IWindowManager& getWindowManager() override;
		IAdvancedInputSystem& getInputSystem() override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

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

	class SDL3AdvancedInputSystem : public IAdvancedInputSystem {
	public:
		SDL3AdvancedInputSystem();
		~SDL3AdvancedInputSystem() override;

		// No copiable ni movible
		SDL3AdvancedInputSystem(const SDL3AdvancedInputSystem&) = delete;
		SDL3AdvancedInputSystem& operator=(const SDL3AdvancedInputSystem&) = delete;
		SDL3AdvancedInputSystem(SDL3AdvancedInputSystem&&) = delete;
		SDL3AdvancedInputSystem& operator=(SDL3AdvancedInputSystem&&) = delete;

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
