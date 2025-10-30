#include "glfwContext.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace pgrender::backends::glfw {

	class GLFWGraphicsContext::Impl {
	public:
		GLFWwindow* window = nullptr;
		RenderBackend backend = RenderBackend::Auto;
		bool ownsWindow = false;
		IGraphicsContext* shareContext = nullptr;
		void configure(GLFWwindow* win, const IContextDescriptor& config);

		void configureAttributesGL4(const pgrender::GLContextDescriptor& config) {
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.majorVersion);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.minorVersion);
			glfwWindowHint(GLFW_OPENGL_PROFILE,
				config.profile == pgrender::GLContextDescriptor::Profile::Core
				? GLFW_OPENGL_CORE_PROFILE
				: GLFW_OPENGL_COMPAT_PROFILE);

			if (config.enableDebug) {
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			}

			shareContext = static_cast<IGraphicsContext *>(config.shareContext);
		}


		void configureAttributes(const IContextDescriptor& config) {
			switch (config.getBackend()) {
			case RenderBackend::OpenGL4:
				configureAttributesGL4(static_cast<const pgrender::GLContextDescriptor&>(config));
				break;
			}
		}

		~Impl() {
			if (ownsWindow && window) {
				glfwDestroyWindow(window);
			}
		}
	};

	GLFWGraphicsContext::GLFWGraphicsContext(void* window, const IContextDescriptor& config)
		: m_impl(std::make_unique<Impl>())
	{
		if (!window) {
			throw std::runtime_error("Invalid window handle for GLFW context creation");
		}
		m_impl->configure(static_cast<GLFWwindow*>(window), config);
	}

	GLFWGraphicsContext::GLFWGraphicsContext(const IContextDescriptor& config) :
		m_impl(std::make_unique<Impl>())
	{
		m_impl->configure(nullptr, config);
	}

	void GLFWGraphicsContext::Impl::configure(GLFWwindow* win, const IContextDescriptor& config)
	{
		if (win == nullptr) {
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
			this->window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
			if (!this->window) throw std::runtime_error("Failed to create headless GLFW context");
			ownsWindow = true;
		}
		else {
			this->window = win;
		}
		backend = config.getBackend();

		configureAttributes(config);

		glfwMakeContextCurrent(this->window);
	}

	GLFWGraphicsContext::~GLFWGraphicsContext() = default;


	void GLFWGraphicsContext::makeCurrent() {
		glfwMakeContextCurrent(m_impl->window);
	}

	void GLFWGraphicsContext::swapBuffers() {
		glfwSwapBuffers(m_impl->window);
	}

	void* GLFWGraphicsContext::getNativeHandle() const {
		return static_cast<void*>(m_impl->window);
	}

	RenderBackend GLFWGraphicsContext::getBackend() const {
		return m_impl->backend;
	}

	bool GLFWGraphicsContext::isShared() const {
		return m_impl->shareContext != nullptr;
	}

	IGraphicsContext* GLFWGraphicsContext::getSharedContext() const {
		return m_impl->shareContext;
	}

	void* GLFWGraphicsContext::getWindow() const
	{
		return static_cast<void*>(m_impl->window);
	}



} // namespace pgrender::backends::glfw
