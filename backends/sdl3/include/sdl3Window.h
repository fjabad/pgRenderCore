#pragma once
#include "pgrender/window.h"

// Forward declarations - NO incluir SDL3
struct SDL_Window;
typedef uint32_t SDL_WindowID;

namespace pgrender::backends::sdl3 {

	class SDL3Window : public IWindow {
	public:
		explicit SDL3Window(const WindowConfig& config);
		~SDL3Window() override;

		// IWindow interface
		void show() override;
		void hide() override;
		void setTitle(const std::string& title) override;
		void setSize(uint32_t width, uint32_t height) override;
		void getSize(uint32_t& width, uint32_t& height) const override;
		bool shouldClose() const override;
		void* getNativeHandle() const override;

		std::unique_ptr<IGraphicsContext> createContext(const ContextConfig& config) override;


		void setRelativeMouseMode(bool enabled) override;
		bool getRelativeMouseMode() const override;
		void setMouseGrab(bool grabbed) override;
		bool getMouseGrab() const override;

		SDL_WindowID getWindowID() const;
		void markForClose();

	private:
		// PIMPL: Ocultar detalles de implementación
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

} // namespace pgrender::backends::sdl3
