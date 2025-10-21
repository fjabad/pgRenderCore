#pragma once
#include "pgrender/graphicsContext.h"
#include <GLFW/glfw3.h>
#include <memory>


// Forward declarations - NO incluir GLFW
struct GLFWwindow;

namespace pgrender::backends::glfw {

    class GLFWGraphicsContext : public IGraphicsContext {
    public:
        explicit GLFWGraphicsContext(void* window, const ContextConfig& config);
        explicit GLFWGraphicsContext(const ContextConfig& config);
        ~GLFWGraphicsContext() override;

		// Deshabilitar copia y movimiento
		GLFWGraphicsContext(const GLFWGraphicsContext&) = delete;
		GLFWGraphicsContext& operator=(const GLFWGraphicsContext&) = delete;
		GLFWGraphicsContext(GLFWGraphicsContext&&) = delete;
		GLFWGraphicsContext& operator=(GLFWGraphicsContext&&) = delete;


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

} // namespace pgrender::backends::glfw
