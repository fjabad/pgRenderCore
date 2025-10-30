#pragma once
#include "pgrender/window.h"

struct SDL_Window;
struct IGraphicsDescriptor;

namespace pgrender::backends::sdl3 {

	class SDL3Window : public IWindow {
	public:
		explicit SDL3Window(const IWindow::Desc& config, const IGraphicsDescriptor *ctxConfig = nullptr);
		~SDL3Window() override;

		// IWindow interface
		void show() override;
		void hide() override;
		void setTitle(const std::string& title) override;
		void setSize(uint32_t width, uint32_t height) override;
		void getSize(uint32_t& width, uint32_t& height) const override;
		void* getNativeHandle() const override;

		std::unique_ptr<IGraphicsContext> createExtraContext(const IContextDescriptor& config) override;


		void setRelativeMouseMode(bool enabled) override;
		bool getRelativeMouseMode() const override;
		void setMouseGrab(bool grabbed) override;
		bool getMouseGrab() const override;

		WindowID getWindowID() const override;

	private:
		// PIMPL: Ocultar detalles de implementación
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

} // namespace pgrender::backends::sdl3
