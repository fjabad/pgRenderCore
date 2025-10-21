#pragma once
#include "pgrender/windowManager.h"
#include "pgrender/inputSystem.h"
#include "pgrender/renderCore.h"
#include <memory>

namespace pgrender::backends::glfw {
    class GLFWLibraryContext : public ILibraryContext {
    public:
        GLFWLibraryContext();
        ~GLFWLibraryContext();


		GLFWLibraryContext(const GLFWLibraryContext&) = delete;
		GLFWLibraryContext& operator=(const GLFWLibraryContext&) = delete;
		GLFWLibraryContext(GLFWLibraryContext&&) = delete;
		GLFWLibraryContext& operator=(GLFWLibraryContext&&) = delete;

        std::unique_ptr<IGraphicsContext> createHeadlessContext(const ContextConfig& config) override;
        IWindowManager& getWindowManager() override;
        IInputSystem& getInputSystem() override;

    private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

} // namespace pgrender::backends::glfw
