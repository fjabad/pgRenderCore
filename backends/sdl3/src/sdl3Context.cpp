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

		void preConfigureAttributesGL4(const pgrender::GLContextDescriptor& config) {
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config.majorVersion);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config.minorVersion);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
				config.profile == pgrender::GLContextDescriptor::Profile::Compatibility ?
				SDL_GL_CONTEXT_PROFILE_COMPATIBILITY :
				SDL_GL_CONTEXT_PROFILE_CORE);

			if (config.debugContext) {
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
			}
		}

		void postConfigureAttributesGL4(const pgrender::GLContextDescriptor& config) {
			shareContext = static_cast<IGraphicsContext*>(config.shareContext);
			if (shareContext) {
				auto* sharedCtx = static_cast<SDL3GraphicsContext*>(config.shareContext);
				SDL_GLContext shareHandle = static_cast<SDL_GLContext>(sharedCtx->getNativeHandle());
				SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
				SDL_GL_MakeCurrent(window, shareHandle);
			}

		}

		void configureAttributes(const IContextDescriptor& config) {
			switch (config.getBackend())
			{
			case RenderBackend::OpenGL4:
				preConfigureAttributesGL4(static_cast<const pgrender::GLContextDescriptor&>(config));
				postConfigureAttributesGL4(static_cast<const pgrender::GLContextDescriptor&>(config));
				break;
			}
		}

		void createDummyWindowGL4(const pgrender::GLContextDescriptor& config) {

			preConfigureAttributesGL4(config);

			SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN;

			window = SDL_CreateWindow("", 1, 1, flags);
			if (!window) {
				throw std::runtime_error(std::string("Failed to create headless window: ") + SDL_GetError());
			}
			ownsWindow = true;

			if (config.shareContext) {
				auto* sharedCtx = static_cast<SDL3GraphicsContext*>(config.shareContext);
				SDL_GLContext shareHandle = static_cast<SDL_GLContext>(sharedCtx->getNativeHandle());
				SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

				SDL_Window* shareWindow = static_cast<SDL_Window*>(sharedCtx->getWindow());
				SDL_GL_MakeCurrent(shareWindow, shareHandle);
			}

		}


		void createDummyWindow(const IContextDescriptor& config) {
			switch (config.getBackend()) {
			case RenderBackend::OpenGL4:
				createDummyWindowGL4(static_cast<const pgrender::GLContextDescriptor&>(config));
				break;
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

	SDL3GraphicsContext::SDL3GraphicsContext(void* window, const IContextDescriptor& config)
		: m_impl(std::make_unique<Impl>()) {

		m_impl->window = static_cast<SDL_Window*>(window);
		m_impl->backend = config.getBackend();


		m_impl->configureAttributes(config);

		m_impl->context = SDL_GL_CreateContext(m_impl->window);
		if (!m_impl->context) {
			throw std::runtime_error(std::string("Failed to create GL context: ") + SDL_GetError());
		}
	}

	SDL3GraphicsContext::SDL3GraphicsContext(const IContextDescriptor& config)
		: m_impl(std::make_unique<Impl>()) {

		m_impl->backend = config.getBackend();
		switch (config.getBackend()) {
		case RenderBackend::OpenGL4:
		{
			auto glConfig = static_cast<const pgrender::GLContextDescriptor&>(config);
			if (glConfig.shareContext) {
				// if the new context is going to share another context, use the other context window
				m_impl->window = static_cast<SDL3GraphicsContext*>(glConfig.shareContext)->m_impl->window;
				m_impl->configureAttributes(config);
			}
			else {
				// create a dummy window
				m_impl->preConfigureAttributesGL4(glConfig);
				m_impl->createDummyWindow(config);
				m_impl->postConfigureAttributesGL4(glConfig);
			}
		}
		break;
		default:
			m_impl->configureAttributes(config);
		}

		m_impl->context = SDL_GL_CreateContext(m_impl->window);

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
