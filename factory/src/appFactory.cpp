#include "appFactory.h"
#include <sdl3App.h>
#include <glfwApp.h>

namespace pgrender {

	std::unique_ptr<App> AppFactory::createApp(WindowBackend backend) {
		if (backend == WindowBackend::Auto) {
#ifdef PGRENDER_USE_SDL3
			backend = WindowBackend::SDL3;
#elif defined(PGRENDER_USE_GLFW)
			backend = WindowBackend::GLFW;
#else
			throw std::runtime_error("No window backend available");
#endif
		}

		if (backend == WindowBackend::SDL3) {
#ifdef PGRENDER_USE_SDL3
			return std::make_unique<backends::sdl3::SDL3App>();
#else
			throw std::runtime_error("SDL3 backend not available");
#endif
		}
		else if (backend == WindowBackend::GLFW) {
#ifdef PGRENDER_USE_GLFW
			return std::make_unique<backends::glfw::GLFWApp>();
#else
			throw std::runtime_error("GLFW backend not available");
#endif
		}

		throw std::runtime_error("Invalid window backend");
	}

} // namespace pgrender	