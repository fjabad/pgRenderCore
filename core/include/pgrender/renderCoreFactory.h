#pragma once
#include "renderCore.h"

#ifdef PGRENDER_USE_SDL3
#include "sdl3Features.h"
#endif

#ifdef PGRENDER_USE_GLFW
#include "pgrender/backends/glfw/GLFWFeatures.hpp"
#endif


namespace pgrender {

	inline std::unique_ptr<ILibraryContext> RenderCoreFactory::createContext(WindowBackend backend) {
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
			return std::make_unique<backends::sdl3::SDL3LibraryContext>();
#else
			throw std::runtime_error("SDL3 backend not available");
#endif
		}
		else if (backend == WindowBackend::GLFW) {
#ifdef PGRENDER_USE_GLFW
			return std::make_unique<backends::glfw::GLFWLibraryContext>();
#else
			throw std::runtime_error("GLFW backend not available");
#endif
		}

		throw std::runtime_error("Invalid window backend");
	}

} // namespace pgrender
