#pragma once
#include "pgrender/graphicsContext.h"
#include <memory>



// Forward declarations - NO incluir SDL3
struct SDL_Window;

namespace pgrender::backends::sdl3 {

	class SDL3GraphicsContext : public IGraphicsContext {
	public:

		SDL3GraphicsContext(void* window, const IContextDescriptor& config);
		/**
		 * @brief Create a window-less context
		 * @param config 
		 */
		explicit SDL3GraphicsContext(const IContextDescriptor& config);
		~SDL3GraphicsContext() override;

		// Deshabilitar copia y movimiento
		SDL3GraphicsContext(const SDL3GraphicsContext&) = delete;
		SDL3GraphicsContext& operator=(const SDL3GraphicsContext&) = delete;
		SDL3GraphicsContext(SDL3GraphicsContext&&) = delete;
		SDL3GraphicsContext& operator=(SDL3GraphicsContext&&) = delete;

		void makeCurrent() override;
		void swapBuffers() override;
		void* getNativeHandle() const override;
		RenderBackend getBackend() const override;

		bool isShared() const override;
		IGraphicsContext* getSharedContext() const override;

		void* getWindow() const;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

} // namespace pgrender::backends::sdl3