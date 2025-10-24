#pragma once
#include "pgrender/windowManager.h"
#include "pgrender/app.h"

#include <memory>


namespace pgrender::backends::sdl3 {

	// ============================================================================
	// Window Manager
	// ============================================================================

	class SDL3WindowManager : public IWindowManager {
	public:
		explicit SDL3WindowManager(App& context);
		~SDL3WindowManager();

		// No copiable ni movible
		SDL3WindowManager(const SDL3WindowManager&) = delete;
		SDL3WindowManager& operator=(const SDL3WindowManager&) = delete;
		SDL3WindowManager(SDL3WindowManager&&) = delete;
		SDL3WindowManager& operator=(SDL3WindowManager&&) = delete;

		// Gestión de ventanas
		WindowID createWindow(const WindowConfig& config) override;

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

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};
}