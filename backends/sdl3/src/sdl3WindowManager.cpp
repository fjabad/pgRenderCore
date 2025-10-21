#include "sdl3WindowManager.h"
#include "sdl3Window.h"
#include <sdl3EventSystem.h>

#include <SDL3/SDL.h>

namespace pgrender::backends::sdl3 {
	// ============================================================================
	// SDL3WindowManager::Impl
	// ============================================================================

	class SDL3WindowManager::Impl {
	public:
		struct WindowData {
			std::unique_ptr<IWindow> window;
			std::unique_ptr<IGraphicsContext> context;
			WindowEventCallback eventCallback;
		};

		SDL3PerWindowEventSystem eventSystem;
		ILibraryContext& context;
		std::unordered_map<WindowID, WindowData> windows;
		mutable std::mutex mutex;

		explicit Impl(ILibraryContext& ctx) : context(ctx) {}

		SDL_Window* getNativeSDL3Window(WindowID id) {
			auto it = windows.find(id);
			if (it != windows.end()) {
				return static_cast<SDL_Window*>(it->second.window->getNativeHandle());
			}
			return nullptr;
		}
	};

	SDL3WindowManager::SDL3WindowManager(ILibraryContext& context)
		: m_impl(std::make_unique<Impl>(context)) {
	}

	SDL3WindowManager::~SDL3WindowManager() = default;

	WindowID SDL3WindowManager::createWindow(const WindowConfig& config) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto window = std::make_unique<SDL3Window>(config);
		auto* sdlWindow = static_cast<SDL3Window*>(window.get());
		WindowID windowId = sdlWindow->getWindowID();

		Impl::WindowData data;
		data.window = std::move(window);

		m_impl->windows[windowId] = std::move(data);

		m_impl->eventSystem.registerWindow(windowId, sdlWindow);

		return windowId;
	}

	void SDL3WindowManager::destroyWindow(WindowID id) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		auto it = m_impl->windows.find(id);
		if (it != m_impl->windows.end()) {
			m_impl->eventSystem.unregisterWindow(id);

			it->second.context.reset();
			it->second.window.reset();

			m_impl->windows.erase(it);
		}
	}

	void SDL3WindowManager::closeAllWindows() {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		for (auto& [id, data] : m_impl->windows) {
			m_impl->eventSystem.unregisterWindow(id);
			data.context.reset();
			data.window.reset();
		}

		m_impl->windows.clear();
	}

	IWindow* SDL3WindowManager::getWindow(WindowID id) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		auto it = m_impl->windows.find(id);
		return (it != m_impl->windows.end()) ? it->second.window.get() : nullptr;
	}

	const IWindow* SDL3WindowManager::getWindow(WindowID id) const {
		auto it = m_impl->windows.find(id);
		return (it != m_impl->windows.end()) ? it->second.window.get() : nullptr;
	}

	std::vector<WindowID> SDL3WindowManager::getActiveWindows() const {
		std::vector<WindowID> ids;
		ids.reserve(m_impl->windows.size());
		for (const auto& [id, _] : m_impl->windows) {
			ids.push_back(id);
		}
		return ids;
	}

	size_t SDL3WindowManager::getWindowCount() const {
		return m_impl->windows.size();
	}

	bool SDL3WindowManager::hasOpenWindows() const {
		return !m_impl->windows.empty();
	}

	IGraphicsContext* SDL3WindowManager::getWindowContext(WindowID id) {
		auto it = m_impl->windows.find(id);
		return (it != m_impl->windows.end()) ? it->second.context.get() : nullptr;
	}

	void SDL3WindowManager::setWindowContext(WindowID id, std::unique_ptr<IGraphicsContext> context) {
		auto it = m_impl->windows.find(id);
		if (it != m_impl->windows.end()) {
			it->second.context = std::move(context);
		}
	}

	int SDL3WindowManager::getDisplayCount() const {
		int count = 0;
		SDL_DisplayID* displays = SDL_GetDisplays(&count);
		SDL_free(displays);
		return count;
	}

	DisplayInfo SDL3WindowManager::getDisplayInfo(int index) const {
		DisplayInfo info{};
		info.index = index;

		int displayCount = 0;
		SDL_DisplayID* displays = SDL_GetDisplays(&displayCount);

		if (!displays || index < 0 || index >= displayCount) {
			SDL_free(displays);
			return info;
		}

		SDL_DisplayID displayId = displays[index];
		SDL_free(displays);

		const char* name = SDL_GetDisplayName(displayId);
		info.name = name ? name : "Unknown Display";

		SDL_Rect bounds;
		if (SDL_GetDisplayBounds(displayId, &bounds)) {
			info.bounds.x = bounds.x;
			info.bounds.y = bounds.y;
			info.bounds.width = bounds.w;
			info.bounds.height = bounds.h;
		}

		SDL_Rect usableBounds;
		if (SDL_GetDisplayUsableBounds(displayId, &usableBounds)) {
			info.usableBounds.x = usableBounds.x;
			info.usableBounds.y = usableBounds.y;
			info.usableBounds.width = usableBounds.w;
			info.usableBounds.height = usableBounds.h;
		}
		else {
			info.usableBounds = info.bounds;
		}

		const SDL_DisplayMode* currentMode = SDL_GetCurrentDisplayMode(displayId);
		if (currentMode) {
			info.refreshRate = static_cast<int>(currentMode->refresh_rate);
		}

		float contentScale = SDL_GetDisplayContentScale(displayId);
		const float BASE_DPI = 96.0f;
		info.ddpi = BASE_DPI * contentScale;
		info.hdpi = BASE_DPI * contentScale;
		info.vdpi = BASE_DPI * contentScale;

		return info;
	}

	int SDL3WindowManager::getWindowDisplayIndex(WindowID windowId) const {
		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (!sdlWindow) {
			return -1;
		}
		SDL_DisplayID displayId = SDL_GetDisplayForWindow(sdlWindow);

		int count = 0;
		SDL_DisplayID* displays = SDL_GetDisplays(&count);

		int index = -1;
		for (int i = 0; i < count; ++i) {
			if (displays[i] == displayId) {
				index = i;
				break;
			}
		}

		SDL_free(displays);
		return index;
	}

	void SDL3WindowManager::centerWindowOnDisplay(WindowID windowId, int displayIndex) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);

		int count = 0;
		SDL_DisplayID* displays = SDL_GetDisplays(&count);

		if (!displays || displayIndex < 0 || displayIndex >= count) {
			SDL_free(displays);
			return;
		}

		SDL_DisplayID displayId = displays[displayIndex];
		SDL_free(displays);

		SDL_Rect displayBounds;
		if (!SDL_GetDisplayBounds(displayId, &displayBounds)) {
			return;
		}

		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (sdlWindow == nullptr)
			return;


		int windowWidth, windowHeight;
		SDL_GetWindowSize(sdlWindow, &windowWidth, &windowHeight);

		int centeredX = displayBounds.x + (displayBounds.w - windowWidth) / 2;
		int centeredY = displayBounds.y + (displayBounds.h - windowHeight) / 2;

		SDL_SetWindowPosition(sdlWindow, centeredX, centeredY);
	}

	void SDL3WindowManager::setWindowPosition(WindowID windowId, int x, int y) {
		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (sdlWindow == nullptr)
			return;
		SDL_SetWindowPosition(sdlWindow, x, y);
	}

	void SDL3WindowManager::getWindowPosition(WindowID windowId, int& x, int& y) const {
		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (sdlWindow == nullptr) {
			x = 0;
			y = 0;
		}
		else {
			SDL_GetWindowPosition(sdlWindow, &x, &y);
		}
	}

	void SDL3WindowManager::maximizeWindow(WindowID windowId) {
		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (sdlWindow != nullptr) {
			SDL_MaximizeWindow(sdlWindow);
		}
	}

	void SDL3WindowManager::minimizeWindow(WindowID windowId) {
		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (sdlWindow != nullptr) {
			SDL_MinimizeWindow(sdlWindow);
		}
	}

	void SDL3WindowManager::restoreWindow(WindowID windowId) {
		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (sdlWindow != nullptr) {
			SDL_RestoreWindow(sdlWindow);
		}
	}

	void SDL3WindowManager::raiseWindow(WindowID windowId) {
		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (sdlWindow != nullptr) {
			SDL_RaiseWindow(sdlWindow);
		}
	}

	void SDL3WindowManager::setFullscreen(WindowID windowId, bool fullscreen) {
		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (sdlWindow != nullptr) {
			SDL_SetWindowFullscreen(sdlWindow, fullscreen);
		}
	}

	void SDL3WindowManager::setFullscreenDesktop(WindowID windowId, bool fullscreen) {
		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (sdlWindow != nullptr) {
			SDL_SetWindowFullscreen(sdlWindow, fullscreen);
		}
	}

	void SDL3WindowManager::setWindowOpacity(WindowID windowId, float opacity) {
		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (sdlWindow != nullptr) {
			SDL_SetWindowOpacity(sdlWindow, opacity);
		}
	}

	float SDL3WindowManager::getWindowOpacity(WindowID windowId) const {
		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (sdlWindow != nullptr) {
			return SDL_GetWindowOpacity(sdlWindow);
		}

		return 1.0f;
	}

	void SDL3WindowManager::setBordered(WindowID windowId, bool bordered) {
		auto* sdlWindow = m_impl->getNativeSDL3Window(windowId);
		if (sdlWindow != nullptr) {
			SDL_SetWindowBordered(sdlWindow, bordered);
		}
	}


	// Eventos
	void SDL3WindowManager::pollEvents() {
		m_impl->eventSystem.pollEvents();
	}

	bool SDL3WindowManager::getEventForWindow(WindowID windowId, Event& event) {
		return m_impl->eventSystem.getEventForWindow(windowId, event);
	}

	void SDL3WindowManager::processWindowClosures() {
		std::vector<WindowID> toClose;

		{
			std::lock_guard<std::mutex> lock(m_impl->mutex);
			for (const auto& [id, data] : m_impl->windows) {
				if (data.window && data.window->shouldClose()) {
					toClose.push_back(id);
				}
			}
		}

		for (WindowID id : toClose) {
			destroyWindow(id);
		}
	}

	void SDL3WindowManager::setWindowEventCallback(WindowID id, WindowEventCallback callback) {
		std::lock_guard<std::mutex> lock(m_impl->mutex);
		auto it = m_impl->windows.find(id);
		if (it != m_impl->windows.end()) {
			it->second.eventCallback = callback;
		}
	}

	void SDL3WindowManager::setWindowEventFilter(WindowID id, EventFilter filter) {
		m_impl->eventSystem.setWindowEventFilter(id, filter);
	}

	void SDL3WindowManager::setWindowEventWatcher(WindowID id, EventFilter watcher) {
		m_impl->eventSystem.setWindowEventWatcher(id, watcher);
	}

	size_t SDL3WindowManager::getWindowQueueSize(WindowID id) const {
		return m_impl->eventSystem.getWindowQueueSize(id);
	}

	size_t SDL3WindowManager::getTotalQueuedEvents() const {
		return m_impl->eventSystem.getTotalQueuedEvents();
	}


}