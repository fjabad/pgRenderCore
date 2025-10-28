#pragma once
#include "pgrender/windowManager.h"
#include "pgrender/inputSystem.h"
#include "pgrender/app.h"
#include <memory>

namespace pgrender::backends::glfw {
	class GLFWApp : public App {
	public:
		GLFWApp();
		~GLFWApp();


		GLFWApp(const GLFWApp&) = delete;
		GLFWApp& operator=(const GLFWApp&) = delete;
		GLFWApp(GLFWApp&&) = delete;
		GLFWApp& operator=(GLFWApp&&) = delete;

		std::unique_ptr<IGraphicsContext> createHeadlessContext(const IContextDescriptor& config) override;
		IWindowManager& getWindowManager() override;
		IInputSystem& getInputSystem() override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

} // namespace pgrender::backends::glfw
