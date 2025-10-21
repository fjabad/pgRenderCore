#include "glfwEventSystem.h"
#include "glfwWindow.h"
#include <GLFW/glfw3.h>

#include <unordered_map>
#include <mutex>
#include <iostream>

namespace pgrender::backends::glfw {

	// ============================================================================
	// Impl - Implementación interna
	// ============================================================================

	class GLFWEventSystem::Impl {
	public:
		mutable std::mutex mutex;


		static void distribute(const Event& e, GLFWwindow* window);

		// Callbacks estáticas de GLFW
		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
		static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void windowCloseCallback(GLFWwindow* window);
		static void windowSizeCallback(GLFWwindow* window, int width, int height);
		static void windowPosCallback(GLFWwindow* window, int xpos, int ypos);
		static void windowFocusCallback(GLFWwindow* window, int focused);
		static void windowIconifyCallback(GLFWwindow* window, int iconified);
		static void windowMaximizeCallback(GLFWwindow* window, int maximized);

		// Helpers de traducción
		static KeyCode translateKeyCode(int glfwKey);
		static MouseButton translateMouseButton(int glfwButton);
	};

	// ============================================================================
	// Constructor / Destructor
	// ============================================================================

	GLFWEventSystem::GLFWEventSystem()
		: m_impl(std::make_unique<Impl>()) {
	}

	GLFWEventSystem::~GLFWEventSystem() = default;

	// ============================================================================
	// Registro de ventanas
	// ============================================================================

	void GLFWEventSystem::registerWindow(WindowID windowId, IWindow* window) {

		IEventSystem::registerWindow(windowId, window);

		// Obtener handle de GLFW
		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->getNativeHandle());

		// Establecer puntero de usuario para callbacks
		glfwSetWindowUserPointer(glfwWindow, this);

		// Registrar callbacks
		glfwSetKeyCallback(glfwWindow, Impl::keyCallback);
		glfwSetMouseButtonCallback(glfwWindow, Impl::mouseButtonCallback);
		glfwSetCursorPosCallback(glfwWindow, Impl::cursorPosCallback);
		glfwSetScrollCallback(glfwWindow, Impl::scrollCallback);
		glfwSetWindowCloseCallback(glfwWindow, Impl::windowCloseCallback);
		glfwSetWindowSizeCallback(glfwWindow, Impl::windowSizeCallback);
		glfwSetWindowPosCallback(glfwWindow, Impl::windowPosCallback);
		glfwSetWindowFocusCallback(glfwWindow, Impl::windowFocusCallback);
		glfwSetWindowIconifyCallback(glfwWindow, Impl::windowIconifyCallback);
		glfwSetWindowMaximizeCallback(glfwWindow, Impl::windowMaximizeCallback);
	}

	// ============================================================================
	// Procesamiento de eventos
	// ============================================================================

	void GLFWEventSystem::pollEvents() {
		// GLFW usa un sistema de callbacks, solo necesitamos llamar a glfwPollEvents
		glfwPollEvents();
	}

	// ============================================================================
	// Callbacks de GLFW
	// ============================================================================

	void GLFWEventSystem::Impl::distribute(const Event& e, GLFWwindow* window)
	{
		WindowID windowId = reinterpret_cast<WindowID>(window);
		auto* es = static_cast<GLFWEventSystem*>(glfwGetWindowUserPointer(window));
		if (!es) {
			std::cerr << "Warning: GLFWEventSystem::Impl::distribute: No event system associated with window";
			return;
		}
		es->distributeEvent(e, windowId);
	}

	void GLFWEventSystem::Impl::keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int mods) {
		auto* impl = static_cast<Impl*>(glfwGetWindowUserPointer(window));
		if (!impl) return;

		Event event{};
		event.timestamp = static_cast<uint64_t>(glfwGetTime() * 1000.0);
		event.type = (action == GLFW_PRESS || action == GLFW_REPEAT) ? EventType::KeyPress : EventType::KeyRelease;
		event.key.key = translateKeyCode(key);
		event.key.repeat = (action == GLFW_REPEAT);
		event.key.mod = static_cast<uint16_t>(mods);

		distribute(event, window);
	}

	void GLFWEventSystem::Impl::mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/) {
		auto* impl = static_cast<Impl*>(glfwGetWindowUserPointer(window));
		if (!impl) return;

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		Event event{};
		event.timestamp = static_cast<uint64_t>(glfwGetTime() * 1000.0);
		event.type = (action == GLFW_PRESS) ? EventType::MouseButtonPress : EventType::MouseButtonRelease;
		event.mouseButton.button = translateMouseButton(button);
		event.mouseButton.x = static_cast<float>(xpos);
		event.mouseButton.y = static_cast<float>(ypos);
		event.mouseButton.clicks = 1;

		distribute(event, window);
	}

	void GLFWEventSystem::Impl::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
		auto* impl = static_cast<Impl*>(glfwGetWindowUserPointer(window));
		if (!impl) return;

		static std::unordered_map<GLFWwindow*, std::pair<double, double>> lastPositions;

		auto& lastPos = lastPositions[window];

		Event event{};
		event.timestamp = static_cast<uint64_t>(glfwGetTime() * 1000.0);
		event.type = EventType::MouseMove;
		event.mouseMove.x = static_cast<float>(xpos);
		event.mouseMove.y = static_cast<float>(ypos);
		event.mouseMove.deltaX = static_cast<float>(xpos - lastPos.first);
		event.mouseMove.deltaY = static_cast<float>(ypos - lastPos.second);

		lastPos = { xpos, ypos };

		distribute(event, window);
	}

	void GLFWEventSystem::Impl::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		auto* impl = static_cast<Impl*>(glfwGetWindowUserPointer(window));
		if (!impl) return;

		Event event{};
		event.timestamp = static_cast<uint64_t>(glfwGetTime() * 1000.0);
		event.type = EventType::MouseScroll;
		event.mouseScroll.deltaX = static_cast<float>(xoffset);
		event.mouseScroll.deltaY = static_cast<float>(yoffset);
		event.mouseScroll.flipped = false;

		distribute(event, window);
	}

	void GLFWEventSystem::Impl::windowCloseCallback(GLFWwindow* window) {
		Event event{};
		event.timestamp = static_cast<uint64_t>(glfwGetTime() * 1000.0);
		event.type = EventType::WindowClose;

		distribute(event, window);
	}

	void GLFWEventSystem::Impl::windowSizeCallback(GLFWwindow* window, int width, int height) {
		auto* impl = static_cast<Impl*>(glfwGetWindowUserPointer(window));
		if (!impl) return;

		Event event{};
		event.timestamp = static_cast<uint64_t>(glfwGetTime() * 1000.0);
		event.type = EventType::WindowResize;
		event.windowResize.width = width;
		event.windowResize.height = height;

		distribute(event, window);
	}

	void GLFWEventSystem::Impl::windowPosCallback(GLFWwindow* window, int xpos, int ypos) {
		auto* impl = static_cast<Impl*>(glfwGetWindowUserPointer(window));
		if (!impl) return;

		Event event{};
		event.timestamp = static_cast<uint64_t>(glfwGetTime() * 1000.0);
		event.type = EventType::WindowMoved;
		event.windowMoved.x = xpos;
		event.windowMoved.y = ypos;

		distribute(event, window);
	}

	void GLFWEventSystem::Impl::windowFocusCallback(GLFWwindow* window, int focused) {
		auto* impl = static_cast<Impl*>(glfwGetWindowUserPointer(window));
		if (!impl) return;

		Event event{};
		event.timestamp = static_cast<uint64_t>(glfwGetTime() * 1000.0);
		event.type = focused ? EventType::WindowFocus : EventType::WindowLostFocus;

		distribute(event, window);
	}

	void GLFWEventSystem::Impl::windowIconifyCallback(GLFWwindow* window, int iconified) {
		auto* impl = static_cast<Impl*>(glfwGetWindowUserPointer(window));
		if (!impl) return;

		Event event{};
		event.timestamp = static_cast<uint64_t>(glfwGetTime() * 1000.0);
		event.type = iconified ? EventType::WindowMinimized : EventType::WindowRestored;

		distribute(event, window);
	}

	void GLFWEventSystem::Impl::windowMaximizeCallback(GLFWwindow* window, int maximized) {
		auto* impl = static_cast<Impl*>(glfwGetWindowUserPointer(window));
		if (!impl) return;

		Event event{};
		event.timestamp = static_cast<uint64_t>(glfwGetTime() * 1000.0);
		event.type = maximized ? EventType::WindowMaximized : EventType::WindowRestored;

		distribute(event, window);
	}

	// ============================================================================
	// Traducción de códigos
	// ============================================================================

	KeyCode GLFWEventSystem::Impl::translateKeyCode(int glfwKey) {
		switch (glfwKey) {
		case GLFW_KEY_A: return KeyCode::A;
		case GLFW_KEY_B: return KeyCode::B;
		case GLFW_KEY_C: return KeyCode::C;
		case GLFW_KEY_D: return KeyCode::D;
		case GLFW_KEY_E: return KeyCode::E;
		case GLFW_KEY_F: return KeyCode::F;
		case GLFW_KEY_G: return KeyCode::G;
		case GLFW_KEY_H: return KeyCode::H;
		case GLFW_KEY_I: return KeyCode::I;
		case GLFW_KEY_J: return KeyCode::J;
		case GLFW_KEY_K: return KeyCode::K;
		case GLFW_KEY_L: return KeyCode::L;
		case GLFW_KEY_M: return KeyCode::M;
		case GLFW_KEY_N: return KeyCode::N;
		case GLFW_KEY_O: return KeyCode::O;
		case GLFW_KEY_P: return KeyCode::P;
		case GLFW_KEY_Q: return KeyCode::Q;
		case GLFW_KEY_R: return KeyCode::R;
		case GLFW_KEY_S: return KeyCode::S;
		case GLFW_KEY_T: return KeyCode::T;
		case GLFW_KEY_U: return KeyCode::U;
		case GLFW_KEY_V: return KeyCode::V;
		case GLFW_KEY_W: return KeyCode::W;
		case GLFW_KEY_X: return KeyCode::X;
		case GLFW_KEY_Y: return KeyCode::Y;
		case GLFW_KEY_Z: return KeyCode::Z;
		case GLFW_KEY_0: return KeyCode::Num0;
		case GLFW_KEY_1: return KeyCode::Num1;
		case GLFW_KEY_2: return KeyCode::Num2;
		case GLFW_KEY_3: return KeyCode::Num3;
		case GLFW_KEY_4: return KeyCode::Num4;
		case GLFW_KEY_5: return KeyCode::Num5;
		case GLFW_KEY_6: return KeyCode::Num6;
		case GLFW_KEY_7: return KeyCode::Num7;
		case GLFW_KEY_8: return KeyCode::Num8;
		case GLFW_KEY_9: return KeyCode::Num9;
		case GLFW_KEY_ESCAPE: return KeyCode::Escape;
		case GLFW_KEY_SPACE: return KeyCode::Space;
		case GLFW_KEY_ENTER: return KeyCode::Enter;
		case GLFW_KEY_TAB: return KeyCode::Tab;
		case GLFW_KEY_BACKSPACE: return KeyCode::Backspace;
		case GLFW_KEY_LEFT: return KeyCode::Left;
		case GLFW_KEY_RIGHT: return KeyCode::Right;
		case GLFW_KEY_UP: return KeyCode::Up;
		case GLFW_KEY_DOWN: return KeyCode::Down;
		case GLFW_KEY_F1: return KeyCode::F1;
		case GLFW_KEY_F2: return KeyCode::F2;
		case GLFW_KEY_F3: return KeyCode::F3;
		case GLFW_KEY_F4: return KeyCode::F4;
		case GLFW_KEY_F5: return KeyCode::F5;
		case GLFW_KEY_F6: return KeyCode::F6;
		case GLFW_KEY_F7: return KeyCode::F7;
		case GLFW_KEY_F8: return KeyCode::F8;
		case GLFW_KEY_F9: return KeyCode::F9;
		case GLFW_KEY_F10: return KeyCode::F10;
		case GLFW_KEY_F11: return KeyCode::F11;
		case GLFW_KEY_F12: return KeyCode::F12;
		default: return KeyCode::Unknown;
		}
	}

	MouseButton GLFWEventSystem::Impl::translateMouseButton(int glfwButton) {
		switch (glfwButton) {
		case GLFW_MOUSE_BUTTON_LEFT: return MouseButton::Left;
		case GLFW_MOUSE_BUTTON_RIGHT: return MouseButton::Right;
		case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
		case GLFW_MOUSE_BUTTON_4: return MouseButton::X1;
		case GLFW_MOUSE_BUTTON_5: return MouseButton::X2;
		default: return MouseButton::Left;
		}
	}

} // namespace pgrender::backends::glfw
