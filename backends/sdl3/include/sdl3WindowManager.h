#pragma once
#include "pgrender/windowManager.h"
#include "pgrender/renderCore.h"

#include <memory>


namespace pgrender::backends::sdl3 {

	// ============================================================================
	// Window Manager
	// ============================================================================

	class SDL3WindowManager : public IWindowManager {
	public:
		explicit SDL3WindowManager(ILibraryContext& context);
		~SDL3WindowManager();

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
}