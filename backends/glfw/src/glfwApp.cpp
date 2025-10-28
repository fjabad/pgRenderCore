#include "glfwApp.h"
#include "glfwWindowManager.h"
#include "glfwInputSystem.h"

#include <glfwContext.h>
namespace pgrender::backends::glfw{
	// ============================================================================
	// GLFWApp::Impl
	// ============================================================================

	class GLFWApp::Impl {
	public:

		GLFWWindowManager windowManager;
		GLFWInputSystem inputSystem;
	};

	GLFWApp::GLFWApp()
		: m_impl(std::make_unique<GLFWApp::Impl>()) {
	}

	GLFWApp::~GLFWApp() = default;

	std::unique_ptr<IGraphicsContext> GLFWApp::createHeadlessContext(const IContextDescriptor& config) {
		return std::make_unique<GLFWGraphicsContext>(config);
	}

	IWindowManager& GLFWApp::getWindowManager() {
		return m_impl->windowManager;
	}

	IInputSystem& GLFWApp::getInputSystem() {
		return m_impl->inputSystem;
	}

}