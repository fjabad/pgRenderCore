#include "sdl3App.h"
#include <sdl3WindowManager.h>
#include <sdl3InputSystem.h>
#include <SDL3/SDL_init.h>
#include <sdl3Context.h>



namespace pgrender::backends::sdl3 {


	// ============================================================================
	// SDL3App::Impl
	// ============================================================================

	class SDL3App::Impl {
	public:
	
		SDL3WindowManager windowManager;
		SDL3InputSystem inputSystem;

		Impl(SDL3App& parent)
			: windowManager(parent) {
			if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD)) {
				throw std::runtime_error(std::string("Failed to initialize SDL3: ") + SDL_GetError());
			}
		}

		~Impl() {
			SDL_Quit();
		}
	};

	SDL3App::SDL3App()
		: m_impl(std::make_unique<Impl>(*this)) {
	}

	SDL3App::~SDL3App() = default;

	std::unique_ptr<IGraphicsContext> SDL3App::createHeadlessContext(const ContextConfig& config) {
		return std::make_unique<SDL3GraphicsContext>(config);
	}

	IWindowManager& SDL3App::getWindowManager() {
		return m_impl->windowManager;
	}

	IInputSystem& SDL3App::getInputSystem() {
		return m_impl->inputSystem;
	}


} // namespace pgrender::backends::sdl3
