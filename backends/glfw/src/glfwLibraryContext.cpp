#include "glfwLibraryContext.h"
#include "glfwWindowManager.h"
#include "glfwInputSystem.h"

#include <glfwContext.h>
namespace pgrender::backends::glfw{
	// ============================================================================
	// GLFWLibraryContext::Impl
	// ============================================================================

	class GLFWLibraryContext::Impl {
	public:

		GLFWWindowManager windowManager;
		GLFWInputSystem inputSystem;
	};

	GLFWLibraryContext::GLFWLibraryContext()
		: m_impl(std::make_unique<GLFWLibraryContext::Impl>()) {
	}

	GLFWLibraryContext::~GLFWLibraryContext() = default;

	std::unique_ptr<IGraphicsContext> GLFWLibraryContext::createHeadlessContext(const ContextConfig& config) {
		return std::make_unique<GLFWGraphicsContext>(config);
	}

	IWindowManager& GLFWLibraryContext::getWindowManager() {
		return m_impl->windowManager;
	}

	IInputSystem& GLFWLibraryContext::getInputSystem() {
		return m_impl->inputSystem;
	}

}