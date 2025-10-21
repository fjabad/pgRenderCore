#include "sdl3LibraryContext.h"
#include "sdl3Window.h"
#include "sdl3Context.h"
#include "sdl3EventSystem.h"
#include "sdl3WindowManager.h"
#include "sdl3InputSystem.h"
#include <SDL3/SDL.h>
#include <unordered_map>
#include <mutex>


namespace pgrender::backends::sdl3 {


	// ============================================================================
	// SDL3LibraryContext::Impl
	// ============================================================================

	class SDL3LibraryContext::Impl {
	public:
	
		SDL3WindowManager windowManager;
		SDL3InputSystem inputSystem;

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

	IInputSystem& SDL3LibraryContext::getInputSystem() {
		return m_impl->inputSystem;
	}


} // namespace pgrender::backends::sdl3
