#include "sdl3Context.h"
#include <SDL3/SDL.h>
#include <stdexcept>

namespace pgrender::backends::sdl3 {

	class SDL3GraphicsContext::Impl {
	public:
		SDL_Window* window = nullptr;
		SDL_GLContext context = nullptr;
		RenderBackend backend = RenderBackend::Auto;
		bool ownsWindow = false;
		IGraphicsContext* shareContext = nullptr;

		void configureAttributes(const ContextConfig& config) {
			if (config.backend == RenderBackend::OpenGL4) {
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config.majorVersion);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config.minorVersion);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

				if (config.debugContext) {
					SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
				}
			}
		}

		~Impl() {
			if (context) {
				SDL_GL_DestroyContext(context);
			}
			if (ownsWindow && window) {
				SDL_DestroyWindow(window);
			}
		}
	};

	SDL3GraphicsContext::SDL3GraphicsContext(void* window, const ContextConfig& config)
		: m_impl(std::make_unique<Impl>()) {

		m_impl->window = static_cast<SDL_Window*>(window);
		m_impl->backend = config.backend;
		m_impl->shareContext = static_cast<IGraphicsContext*>(config.shareContext);

		m_impl->configureAttributes(config);

		if (config.shareContext) {
			auto* sharedCtx = static_cast<SDL3GraphicsContext*>(config.shareContext);
			SDL_GLContext shareHandle = static_cast<SDL_GLContext>(sharedCtx->getNativeHandle());
			SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
			SDL_GL_MakeCurrent(m_impl->window, shareHandle);
		}

		m_impl->context = SDL_GL_CreateContext(m_impl->window);
		if (!m_impl->context) {
			throw std::runtime_error(std::string("Failed to create GL context: ") + SDL_GetError());
		}
	}

	SDL3GraphicsContext::SDL3GraphicsContext(const ContextConfig& config)
		: m_impl(std::make_unique<Impl>()) {

		m_impl->backend = config.backend;
		m_impl->shareContext = static_cast<IGraphicsContext*>(config.shareContext);

		m_impl->configureAttributes(config);

		SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN;

		m_impl->window = SDL_CreateWindow("", 1, 1, flags);
		if (!m_impl->window) {
			throw std::runtime_error(std::string("Failed to create headless window: ") + SDL_GetError());
		}

		if (config.shareContext) {
			auto* sharedCtx = static_cast<SDL3GraphicsContext*>(config.shareContext);
			SDL_GLContext shareHandle = static_cast<SDL_GLContext>(sharedCtx->getNativeHandle());
			SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

			SDL_Window* shareWindow = static_cast<SDL_Window*>(sharedCtx->getWindow());
			SDL_GL_MakeCurrent(shareWindow, shareHandle);
		}

		m_impl->context = SDL_GL_CreateContext(m_impl->window);
		m_impl->ownsWindow = true;

		if (!m_impl->context) {
			throw std::runtime_error(std::string("Failed to create headless context: ") + SDL_GetError());
		}
	}

	SDL3GraphicsContext::~SDL3GraphicsContext() = default;

	void SDL3GraphicsContext::makeCurrent() {
		if (!SDL_GL_MakeCurrent(m_impl->window, m_impl->context)) {
			throw std::runtime_error(std::string("Failed to make context current: ") + SDL_GetError());
		}
	}

	void SDL3GraphicsContext::swapBuffers() {
		if (m_impl->window && !SDL_GL_SwapWindow(m_impl->window)) {
			throw std::runtime_error(std::string("Failed to swap buffers: ") + SDL_GetError());
		}
	}

	void* SDL3GraphicsContext::getNativeHandle() const {
		return m_impl->context;
	}

	void* SDL3GraphicsContext::getWindow() const {
		return m_impl->window;
	}

	RenderBackend SDL3GraphicsContext::getBackend() const {
		return m_impl->backend;
	}

	bool SDL3GraphicsContext::isShared() const {
		return m_impl->shareContext != nullptr;
	}

	IGraphicsContext* SDL3GraphicsContext::getSharedContext() const {
		return m_impl->shareContext;
	}

} // namespace pgrender::backends::sdl3
