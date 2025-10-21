#include "sdl3Window.h"
#include "sdl3Context.h"
#include <SDL3/SDL.h>
#include <stdexcept>

namespace pgrender::backends::sdl3 {

	// Implementación privada que SÍ conoce SDL3
	class SDL3Window::Impl {
	public:
		SDL_Window* window = nullptr;
		SDL_WindowID windowId = 0;
		WindowConfig config;
		bool shouldClose = false;

		explicit Impl(const WindowConfig& cfg) : config(cfg) {
			SDL_WindowFlags flags = 0;

			if (cfg.renderBackend == RenderBackend::OpenGL4) {
				flags |= SDL_WINDOW_OPENGL;
			}
			else if (cfg.renderBackend == RenderBackend::Vulkan) {
				flags |= SDL_WINDOW_VULKAN;
			}
			else if (cfg.renderBackend == RenderBackend::Metal) {
				flags |= SDL_WINDOW_METAL;
			}

			if (cfg.resizable) flags |= SDL_WINDOW_RESIZABLE;
			if (cfg.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
			if (cfg.highDPI) flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
			if (cfg.transparent) flags |= SDL_WINDOW_TRANSPARENT;
			if (cfg.alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
			if (cfg.borderless) flags |= SDL_WINDOW_BORDERLESS;

			window = SDL_CreateWindow(
				cfg.title.c_str(),
				cfg.width,
				cfg.height,
				flags
			);

			if (!window) {
				throw std::runtime_error(std::string("Failed to create window: ") + SDL_GetError());
			}

			if (cfg.vsync) {
				SDL_GL_SetSwapInterval(1);
			}

			windowId = SDL_GetWindowID(window);
		}

		~Impl() {
			if (window) {
				SDL_DestroyWindow(window);
			}
		}
	};

	// Implementación de SDL3Window usando PIMPL
	SDL3Window::SDL3Window(const WindowConfig& config)
		: m_impl(std::make_unique<Impl>(config)) {
	}

	SDL3Window::~SDL3Window() = default;

	void SDL3Window::show() {
		SDL_ShowWindow(m_impl->window);
	}

	void SDL3Window::hide() {
		SDL_HideWindow(m_impl->window);
	}

	void SDL3Window::setTitle(const std::string& title) {
		SDL_SetWindowTitle(m_impl->window, title.c_str());
	}

	void SDL3Window::setSize(uint32_t width, uint32_t height) {
		SDL_SetWindowSize(m_impl->window, width, height);
	}

	void SDL3Window::getSize(uint32_t& width, uint32_t& height) const {
		int w, h;
		SDL_GetWindowSize(m_impl->window, &w, &h);
		width = static_cast<uint32_t>(w);
		height = static_cast<uint32_t>(h);
	}

	void* SDL3Window::getNativeHandle() const {
		return m_impl->window;
	}

	std::unique_ptr<IGraphicsContext> SDL3Window::createContext(const ContextConfig& config) {
		return std::make_unique<SDL3GraphicsContext>(m_impl->window, config);
	}



	// SDL3: Modo relativo de mouse (por ventana)
	void SDL3Window::setRelativeMouseMode(bool enabled) {
		SDL_SetWindowRelativeMouseMode(m_impl->window, enabled);
	}

	bool SDL3Window::getRelativeMouseMode() const {
		return SDL_GetWindowRelativeMouseMode(m_impl->window);
	}

	// SDL3: Mouse grab (confinamiento)
	void SDL3Window::setMouseGrab(bool grabbed) {
		SDL_SetWindowMouseGrab(m_impl->window, grabbed);
	}

	bool SDL3Window::getMouseGrab() const {
		return SDL_GetWindowMouseGrab(m_impl->window);
	}


	WindowID SDL3Window::getWindowID() const {
		return m_impl->windowId;
	}

} // namespace pgrender::backends::sdl3
