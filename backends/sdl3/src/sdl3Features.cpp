#include "sdl3Features.h"
#include "sdl3Window.h"
#include "sdl3Context.h"
#include "sdl3EventSystem.h"
#include <SDL3/SDL.h>
#include <unordered_map>
#include <mutex>


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

	// ============================================================================
	// SDL3LibraryContext::Impl
	// ============================================================================

	class SDL3LibraryContext::Impl {
	public:
	
		SDL3WindowManager windowManager;
		SDL3AdvancedInputSystem inputSystem;

		Impl(SDL3LibraryContext& parent)
			: windowManager(parent) {
			if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD)) {
				throw std::runtime_error(std::string("Failed to initialize SDL3: ") + SDL_GetError());
			}
		}

		~Impl() {
			SDL_Quit();
		}
	};

	SDL3LibraryContext::SDL3LibraryContext()
		: m_impl(std::make_unique<Impl>(*this)) {
	}

	SDL3LibraryContext::~SDL3LibraryContext() = default;

	std::unique_ptr<IGraphicsContext> SDL3LibraryContext::createHeadlessContext(const ContextConfig& config) {
		return std::make_unique<SDL3GraphicsContext>(config);
	}

	IWindowManager& SDL3LibraryContext::getWindowManager() {
		return m_impl->windowManager;
	}

	IAdvancedInputSystem& SDL3LibraryContext::getInputSystem() {
		return m_impl->inputSystem;
	}

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

	class SDL3AdvancedInputSystem::Impl {
	public:
		// No state needed for now
	};

	SDL3AdvancedInputSystem::SDL3AdvancedInputSystem()
		: m_impl(std::make_unique<Impl>()) {
	}

	SDL3AdvancedInputSystem::~SDL3AdvancedInputSystem() = default;

	int SDL3AdvancedInputSystem::getGamepadCount() const {
		int count = 0;
		SDL_JoystickID* joysticks = SDL_GetGamepads(&count);
		SDL_free(joysticks);
		return count;
	}

	std::unique_ptr<IGamepad> SDL3AdvancedInputSystem::openGamepad(int index) {
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

	std::vector<GamepadInfo> SDL3AdvancedInputSystem::getAvailableGamepads() const {
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

	void SDL3AdvancedInputSystem::setClipboardText(const std::string& text) {
		SDL_SetClipboardText(text.c_str());
	}

	std::string SDL3AdvancedInputSystem::getClipboardText() const {
		char* text = SDL_GetClipboardText();
		std::string result = text ? text : "";
		SDL_free(text);
		return result;
	}

	bool SDL3AdvancedInputSystem::hasClipboardText() const {
		return SDL_HasClipboardText();
	}

	void SDL3AdvancedInputSystem::showCursor(bool show) {
		if (show) {
			SDL_ShowCursor();
		}
		else {
			SDL_HideCursor();
		}
	}

	bool SDL3AdvancedInputSystem::isCursorVisible() const {
		return SDL_CursorVisible();
	}

	void SDL3AdvancedInputSystem::setCursorPosition(int x, int y) {
		SDL_WarpMouseGlobal(static_cast<float>(x), static_cast<float>(y));
	}

	void SDL3AdvancedInputSystem::captureMouse(bool capture) {
		SDL_CaptureMouse(capture);
	}


} // namespace pgrender::backends::sdl3
