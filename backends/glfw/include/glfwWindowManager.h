#pragma once

#include "pgrender/windowManager.h"


namespace pgrender::backends::glfw {
	class GLFWWindowManager : public IWindowManager {
	public:
		explicit GLFWWindowManager();
		~GLFWWindowManager() override;

		// Ventanas
		WindowID createWindow(const WindowConfig& config) override;

		int getDisplayCount() const override;
		DisplayInfo getDisplayInfo(int index) const override;
		int getWindowDisplayIndex(WindowID windowId) const override;
		void centerWindowOnDisplay(WindowID windowId, int displayIndex) override;
		void setWindowPosition(WindowID, int, int) override;
		void getWindowPosition(WindowID, int&, int&) const override;
		void maximizeWindow(WindowID) override;
		void minimizeWindow(WindowID) override;
		void restoreWindow(WindowID) override;
		void raiseWindow(WindowID) override;
		void setFullscreen(WindowID, bool) override;
		void setFullscreenDesktop(WindowID, bool) override;
		void setWindowOpacity(WindowID, float) override;
		float getWindowOpacity(WindowID) const override;
		void setBordered(WindowID, bool) override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};
}