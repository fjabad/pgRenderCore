#include "PGRenderCoreGL/contextGL.h"
#include "PGRenderCoreGL/bufferObjectGL.h"
#include "PGRenderCoreGL/textureGL.h"
#include "PGRenderCoreGL/shaderGL.h"
#include "PGRenderCoreGL/pipelineGL.h"
#include "PGRenderCoreGL/renderTargetGL.h"
#include "PGRenderCoreGL/renderPassGL.h"
#include "PGRenderCoreGL/samplerGL.h"
#include "PGRenderCoreGL/vertexArrayGL.h"
#include <GL/glew.h>
#include <stdexcept>
#include <iostream>
#include <cstring>

// Platform-specific includes
#ifdef _WIN32
#include <windows.h>
#include <GL/wglew.h>
#elif defined(__linux__)
#include <GL/glx.h>
#include <X11/Xlib.h>
#elif defined(__APPLE__)
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#endif

namespace PGRenderCore {

	// ===== CONSTRUCTOR Y DESTRUCTOR =====

	ContextGL::ContextGL(const Context::Desc& desc)
		: m_nativeWindowHandle(desc.nativeWindowHandle),
		m_nativeDisplayHandle(desc.nativeDisplayHandle),
		m_glContext(nullptr),
		m_vao(0),
		m_rayTracingSupported(false)
	{
		if (!m_nativeWindowHandle) {
			throw std::runtime_error("Native window handle is null");
		}

		// Inicializar contexto OpenGL específico de plataforma
		initializeGLContext(desc);

		// Hacer current el contexto
		makeCurrent();

		// Inicializar GLEW
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			cleanupGLContext();
			throw std::runtime_error(
				std::string("GLEW initialization failed: ") +
				reinterpret_cast<const char*>(glewGetErrorString(err))
			);
		}

		// Información de la implementación OpenGL
		std::cout << "=== OpenGL Context Information ===" << std::endl;
		std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
		std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

		// Crear VAO por defecto (requerido en OpenGL Core Profile)
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Verificar soporte de ray tracing (extensión NVIDIA)
#ifdef GL_NV_ray_tracing
		m_rayTracingSupported = GLEW_NV_ray_tracing != 0;
#else
		m_rayTracingSupported = false;
#endif

		if (m_rayTracingSupported) {
			std::cout << "Ray Tracing: Supported (GL_NV_ray_tracing)" << std::endl;
		}
		else {
			std::cout << "Ray Tracing: Not Supported" << std::endl;
		}

		std::cout << "===================================" << std::endl;
	}

	ContextGL::~ContextGL() {
		if (m_vao) {
			glDeleteVertexArrays(1, &m_vao);
			m_vao = 0;
		}
		cleanupGLContext();
	}

	// ===== INICIALIZACIÓN ESPECÍFICA DE PLATAFORMA =====

	void ContextGL::initializeGLContext(const Context::Desc& desc) {
#ifdef _WIN32
		// ===== Windows - WGL =====
		HWND hwnd = static_cast<HWND>(m_nativeWindowHandle);
		HDC hdc = GetDC(hwnd);
		if (!hdc) {
			throw std::runtime_error("Failed to get device context (GetDC)");
		}

		// Configurar pixel format
		PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int pixelFormat = ChoosePixelFormat(hdc, &pfd);
		if (!pixelFormat) {
			throw std::runtime_error("Failed to choose pixel format");
		}

		if (!SetPixelFormat(hdc, pixelFormat, &pfd)) {
			throw std::runtime_error("Failed to set pixel format");
		}

		// Crear contexto temporal para obtener wglCreateContextAttribsARB
		HGLRC tempContext = wglCreateContext(hdc);
		if (!tempContext) {
			throw std::runtime_error("Failed to create temporary OpenGL context");
		}

		wglMakeCurrent(hdc, tempContext);

		// Inicializar GLEW para cargar wglCreateContextAttribsARB
		glewExperimental = GL_TRUE;
		glewInit();

		// Atributos para contexto moderno OpenGL 4.6 Core
		const int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			WGL_CONTEXT_FLAGS_ARB, desc.enableDebug ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
			0
		};

		HGLRC sharedContext = nullptr;
		if (desc.sharedContext) {
			sharedContext = static_cast<HGLRC>(desc.sharedContext->as<ContextGL>()->getNativeContext());
		}

		HGLRC glContext = wglCreateContextAttribsARB(hdc, sharedContext, attribs);

		// Eliminar contexto temporal
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(tempContext);

		if (!glContext) {
			throw std::runtime_error("Failed to create OpenGL 4.6 Core context");
		}

		m_glContext = glContext;

#elif defined(__linux__)
		// ===== Linux - GLX (X11) =====
		Display* display = static_cast<Display*>(m_nativeDisplayHandle);
		if (!display) {
			display = XOpenDisplay(nullptr);
			if (!display) {
				throw std::runtime_error("Failed to open X11 display");
			}
			m_nativeDisplayHandle = display;
		}

		Window window = static_cast<Window>(reinterpret_cast<uintptr_t>(m_nativeWindowHandle));

		// Configurar atributos del framebuffer
		static int visualAttribs[] = {
			GLX_X_RENDERABLE, True,
			GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
			GLX_RENDER_TYPE, GLX_RGBA_BIT,
			GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
			GLX_RED_SIZE, 8,
			GLX_GREEN_SIZE, 8,
			GLX_BLUE_SIZE, 8,
			GLX_ALPHA_SIZE, 8,
			GLX_DEPTH_SIZE, 24,
			GLX_STENCIL_SIZE, 8,
			GLX_DOUBLEBUFFER, True,
			None
		};

		int fbcount;
		GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visualAttribs, &fbcount);
		if (!fbc || fbcount == 0) {
			throw std::runtime_error("Failed to retrieve framebuffer config");
		}

		GLXFBConfig bestFbc = fbc[0];
		XFree(fbc);

		// Inicializar extensiones GLX
		glXQueryExtension(display, nullptr, nullptr);

		// Atributos para contexto OpenGL 4.6 Core
		static int contextAttribs[] = {
			GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
			GLX_CONTEXT_MINOR_VERSION_ARB, 6,
			GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
			GLX_CONTEXT_FLAGS_ARB, desc.enableDebug ? GLX_CONTEXT_DEBUG_BIT_ARB : 0,
			None
		};

		GLXContext sharedContext = nullptr;
		if (desc.sharedContext) {
			sharedContext = static_cast<GLXContext>(desc.sharedContext->as<ContextGL>()->getNativeContext());
		}

		GLXContext glContext = glXCreateContextAttribsARB(display, bestFbc, sharedContext, True, contextAttribs);

		if (!glContext) {
			throw std::runtime_error("Failed to create OpenGL 4.6 Core context");
		}

		m_glContext = glContext;

#elif defined(__APPLE__)
		// ===== macOS - CGL/NSOpenGL =====
		throw std::runtime_error("macOS OpenGL context creation not yet implemented");

		// TODO: Implementar con CGLCreateContext o NSOpenGLContext

#else
		throw std::runtime_error("Unsupported platform for OpenGL context creation");
#endif
	}

	void ContextGL::cleanupGLContext() {
		if (!m_glContext) return;

#ifdef _WIN32
		HGLRC glContext = static_cast<HGLRC>(m_glContext);
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(glContext);

#elif defined(__linux__)
		Display* display = static_cast<Display*>(m_nativeDisplayHandle);
		GLXContext glContext = static_cast<GLXContext>(m_glContext);
		glXMakeCurrent(display, None, nullptr);
		glXDestroyContext(display, glContext);

#elif defined(__APPLE__)
		// macOS cleanup

#endif

		m_glContext = nullptr;
	}

	// ===== OPERACIONES DE CONTEXTO =====

	void ContextGL::makeCurrent() {
		if (!m_glContext) {
			throw std::runtime_error("OpenGL context is null");
		}

#ifdef _WIN32
		HDC hdc = GetDC(static_cast<HWND>(m_nativeWindowHandle));
		HGLRC glContext = static_cast<HGLRC>(m_glContext);
		if (!wglMakeCurrent(hdc, glContext)) {
			throw std::runtime_error("wglMakeCurrent failed");
		}

#elif defined(__linux__)
		Display* display = static_cast<Display*>(m_nativeDisplayHandle);
		Window window = static_cast<Window>(reinterpret_cast<uintptr_t>(m_nativeWindowHandle));
		GLXContext glContext = static_cast<GLXContext>(m_glContext);
		if (!glXMakeCurrent(display, window, glContext)) {
			throw std::runtime_error("glXMakeCurrent failed");
		}

#elif defined(__APPLE__)
		// macOS make current

#endif
	}

	void ContextGL::swapBuffers() {
#ifdef _WIN32
		HDC hdc = GetDC(static_cast<HWND>(m_nativeWindowHandle));
		SwapBuffers(hdc);

#elif defined(__linux__)
		Display* display = static_cast<Display*>(m_nativeDisplayHandle);
		Window window = static_cast<Window>(reinterpret_cast<uintptr_t>(m_nativeWindowHandle));
		glXSwapBuffers(display, window);

#elif defined(__APPLE__)
		// macOS swap

#endif
	}

	// ===== FÁBRICAS DE RECURSOS =====

	std::shared_ptr<BufferObject> ContextGL::createBufferObject(const BufferObject::Desc& desc) {
		return std::make_shared<BufferObjectGL>(desc);
	}

	std::shared_ptr<Texture> ContextGL::createTexture(const Texture::Desc& desc) {
		return std::make_shared<TextureGL>(desc);
	}

	std::shared_ptr<Program> ContextGL::createProgram(const Program::Desc& desc) {
		return std::make_shared<ShaderGL>(desc);
	}

	std::shared_ptr<Pipeline> ContextGL::createPipeline(const Pipeline::Desc& desc) {
		return std::make_shared<PipelineGL>(desc);
	}

	std::shared_ptr<RenderTarget> ContextGL::createRenderTarget(const RenderTarget::Desc& desc) {
		return std::make_shared<RenderTargetGL>(desc);
	}

	std::shared_ptr<RenderPass> ContextGL::createRenderPass(const RenderPass::Desc& desc) {
		return std::make_shared<RenderPassGL>(desc);
	}

	std::shared_ptr<Sampler> ContextGL::createSampler(const Sampler::Desc& desc) {
		return std::make_shared<SamplerGL>(desc);
	}

	std::shared_ptr<VertexArray> ContextGL::createVertexArray(const VertexArray::Desc& desc) {
		return std::make_shared<VertexArrayGL>(desc);
	}

	// ===== BINDING DE RECURSOS =====

	void ContextGL::bindVertexArray(const std::shared_ptr<VertexArray>& vertexArray) {
		if (!vertexArray) {
			glBindVertexArray(0);
			m_boundVertexArray = nullptr;
			return;
		}

		if (vertexArray->getBackendType() != BackendType::OpenGL) {
			throw std::runtime_error("Cannot bind non-OpenGL vertex array to OpenGL context");
		}

		// Evitar re-binding innecesario
		if (m_boundVertexArray == vertexArray) {
			return;
		}

		auto* vaoGL = vertexArray->as<VertexArrayGL>();
		glBindVertexArray(vaoGL->nativeVAO());
		m_boundVertexArray = vertexArray;
	}

	std::shared_ptr<VertexArray> ContextGL::getBoundVertexArray() const {
		return m_boundVertexArray;
	}

	void ContextGL::bindPipeline(const std::shared_ptr<Pipeline>& pipeline) {
		if (!pipeline) {
			glUseProgram(0);
			m_boundPipeline = nullptr;
			return;
		}

		if (pipeline->getBackendType() != BackendType::OpenGL) {
			throw std::runtime_error("Cannot bind non-OpenGL pipeline to OpenGL context");
		}

		// Evitar re-binding del mismo pipeline
		if (m_boundPipeline == pipeline) {
			return;
		}

		auto* pipelineGL = pipeline->as<PipelineGL>();
		pipelineGL->apply();

		m_boundPipeline = pipeline;
	}

	std::shared_ptr<Pipeline> ContextGL::getBoundPipeline() const {
		return m_boundPipeline;
	}

	void ContextGL::bindTexture(const std::shared_ptr<Texture>& texture, uint32_t slot) {
		glActiveTexture(GL_TEXTURE0 + slot);

		if (!texture) {
			glBindTexture(GL_TEXTURE_2D, 0);
			return;
		}

		if (texture->getBackendType() != BackendType::OpenGL) {
			throw std::runtime_error("Cannot bind non-OpenGL texture to OpenGL context");
		}

		auto* texGL = texture->as<TextureGL>();
		glBindTexture(texGL->toGLTarget(), texGL->nativeTextureId());
	}

	void ContextGL::bindSampler(const std::shared_ptr<Sampler>& sampler, uint32_t slot) {
		if (!sampler) {
			glBindSampler(slot, 0);
			return;
		}

		if (sampler->getBackendType() != BackendType::OpenGL) {
			throw std::runtime_error("Cannot bind non-OpenGL sampler to OpenGL context");
		}

		auto* samplerGL = sampler->as<SamplerGL>();
		glBindSampler(slot, samplerGL->nativeSamplerId());
	}

	void ContextGL::bindUniformBuffer(const std::shared_ptr<BufferObject>& buffer,
		uint32_t binding,
		size_t offset,
		size_t size) {
		if (!buffer) {
			glBindBufferBase(GL_UNIFORM_BUFFER, binding, 0);
			m_boundUniformBuffers.erase(binding);
			return;
		}

		if (buffer->getBackendType() != BackendType::OpenGL) {
			throw std::runtime_error("Cannot bind non-OpenGL buffer to OpenGL context");
		}

		auto* bufferGL = buffer->as<BufferObjectGL>();

		if (size > 0) {
			glBindBufferRange(GL_UNIFORM_BUFFER, binding, bufferGL->nativeBufferId(), offset, size);
		}
		else {
			glBindBufferBase(GL_UNIFORM_BUFFER, binding, bufferGL->nativeBufferId());
		}

		m_boundUniformBuffers[binding] = buffer;
	}

	void ContextGL::bindShaderStorageBuffer(const std::shared_ptr<BufferObject>& buffer,
		uint32_t binding,
		size_t offset,
		size_t size) {
		if (!buffer) {
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, 0);
			m_boundShaderStorageBuffers.erase(binding);
			return;
		}

		if (buffer->getBackendType() != BackendType::OpenGL) {
			throw std::runtime_error("Cannot bind non-OpenGL buffer to OpenGL context");
		}

		auto* bufferGL = buffer->as<BufferObjectGL>();

		if (size > 0) {
			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, binding, bufferGL->nativeBufferId(), offset, size);
		}
		else {
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, bufferGL->nativeBufferId());
		}

		m_boundShaderStorageBuffers[binding] = buffer;
	}

	std::shared_ptr<BufferObject> ContextGL::getBoundUniformBuffer(uint32_t binding) const {
		auto it = m_boundUniformBuffers.find(binding);
		return (it != m_boundUniformBuffers.end()) ? it->second : nullptr;
	}

	std::shared_ptr<BufferObject> ContextGL::getBoundShaderStorageBuffer(uint32_t binding) const {
		auto it = m_boundShaderStorageBuffers.find(binding);
		return (it != m_boundShaderStorageBuffers.end()) ? it->second : nullptr;
	}

	// ===== LIMPIEZA =====

	void ContextGL::clear(ClearFlags flags,
		const glm::vec4 &clearColor,
		float clearDepth,
		int clearStencil) {
		GLbitfield glClearMask = 0;

		if (static_cast<uint32_t>(flags & ClearFlags::Color)) {
			if (!std::isinf(clearColor.x)) {
				glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
			}
			else {
				glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
			}
			glClearMask |= GL_COLOR_BUFFER_BIT;
		}

		if (static_cast<uint32_t>(flags & ClearFlags::Depth)) {
			glClearDepth(clearDepth);
			glClearMask |= GL_DEPTH_BUFFER_BIT;
		}

		if (static_cast<uint32_t>(flags & ClearFlags::Stencil)) {
			glClearStencil(clearStencil);
			glClearMask |= GL_STENCIL_BUFFER_BIT;
		}

		if (glClearMask != 0) {
			glClear(glClearMask);
		}
	}

	void ContextGL::setClearColor(float r, float g, float b, float a) {
		m_clearColor[0] = r;
		m_clearColor[1] = g;
		m_clearColor[2] = b;
		m_clearColor[3] = a;
		glClearColor(r, g, b, a);
	}

	void ContextGL::setClearDepth(float depth) {
		m_clearDepth = depth;
		glClearDepth(depth);
	}

	void ContextGL::setClearStencil(int stencil) {
		m_clearStencil = stencil;
		glClearStencil(stencil);
	}

	// ===== COMANDOS DE DIBUJO =====

	void ContextGL::draw(uint32_t vertexCount, uint32_t firstVertex) {
		glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
	}

	void ContextGL::drawIndexed(uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset) {
		const void* offset = reinterpret_cast<const void*>(
			static_cast<uintptr_t>(firstIndex * sizeof(uint32_t))
			);

		if (vertexOffset != 0) {
			glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, offset, vertexOffset);
		}
		else {
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, offset);
		}
	}

	void ContextGL::drawInstanced(uint32_t vertexCount,
		uint32_t instanceCount,
		uint32_t firstVertex,
		uint32_t firstInstance) {
		if (firstInstance > 0) {
			glDrawArraysInstancedBaseInstance(GL_TRIANGLES, firstVertex, vertexCount,
				instanceCount, firstInstance);
		}
		else {
			glDrawArraysInstanced(GL_TRIANGLES, firstVertex, vertexCount, instanceCount);
		}
	}

	void ContextGL::drawIndexedInstanced(uint32_t indexCount,
		uint32_t instanceCount,
		uint32_t firstIndex,
		int32_t vertexOffset,
		uint32_t firstInstance) {
		const void* offset = reinterpret_cast<const void*>(
			static_cast<uintptr_t>(firstIndex * sizeof(uint32_t))
			);

		if (firstInstance > 0) {
			glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, indexCount,
				GL_UNSIGNED_INT, offset, instanceCount, vertexOffset, firstInstance);
		}
		else if (vertexOffset != 0) {
			glDrawElementsInstancedBaseVertex(GL_TRIANGLES, indexCount,
				GL_UNSIGNED_INT, offset, instanceCount, vertexOffset);
		}
		else {
			glDrawElementsInstanced(GL_TRIANGLES, indexCount,
				GL_UNSIGNED_INT, offset, instanceCount);
		}
	}

	// ===== RAY TRACING =====

	bool ContextGL::isRayTracingSupported() const {
		return m_rayTracingSupported;
	}

	std::shared_ptr<AccelerationStructure> ContextGL::createBLAS(const BLASDesc& desc) {
		if (!m_rayTracingSupported) {
			throw std::runtime_error(
				"Ray tracing is not supported on this device/driver. "
				"OpenGL ray tracing requires GL_NV_ray_tracing extension."
			);
		}

		throw std::runtime_error(
			"Ray tracing BLAS creation not fully implemented for OpenGL backend. "
			"Consider using Vulkan or DirectX 12 for production ray tracing."
		);
	}

	std::shared_ptr<AccelerationStructure> ContextGL::createTLAS(const TLASDesc& desc) {
		if (!m_rayTracingSupported) {
			throw std::runtime_error("Ray tracing is not supported on this device/driver");
		}

		throw std::runtime_error(
			"Ray tracing TLAS creation not fully implemented for OpenGL backend. "
			"Consider using Vulkan or DirectX 12 for production ray tracing."
		);
	}

	//std::shared_ptr<RayTracingPipeline> ContextGL::createRayTracingPipeline(
	//    const RayTracingPipelineDesc& desc) {
	//    if (!m_rayTracingSupported) {
	//        throw std::runtime_error("Ray tracing is not supported on this device/driver");
	//    }

	//    throw std::runtime_error(
	//        "Ray tracing pipeline creation not fully implemented for OpenGL backend. "
	//        "Consider using Vulkan or DirectX 12 for production ray tracing."
	//    );
	//}

	/*void ContextGL::bindRayTracingPipeline(const std::shared_ptr<RayTracingPipeline>& pipeline) {
		if (!m_rayTracingSupported) {
			throw std::runtime_error("Ray tracing is not supported");
		}

		if (pipeline && pipeline->getBackendType() != BackendType::OpenGL) {
			throw std::runtime_error("Cannot bind non-OpenGL ray tracing pipeline");
		}

		m_boundRayTracingPipeline = pipeline;
	}

	std::shared_ptr<RayTracingPipeline> ContextGL::getBoundRayTracingPipeline() const {
		return m_boundRayTracingPipeline;
	}

	void ContextGL::bindAccelerationStructure(const std::shared_ptr<AccelerationStructure>& tlas,
		uint32_t binding) {
		if (!m_rayTracingSupported) {
			throw std::runtime_error("Ray tracing is not supported");
		}

		if (!tlas) {
			m_boundAccelerationStructures.erase(binding);
			return;
		}

		if (tlas->getBackendType() != BackendType::OpenGL) {
			throw std::runtime_error("Cannot bind non-OpenGL acceleration structure");
		}

		m_boundAccelerationStructures[binding] = tlas;
	}

	std::shared_ptr<AccelerationStructure> ContextGL::getBoundAccelerationStructure(uint32_t binding) const {
		auto it = m_boundAccelerationStructures.find(binding);
		return (it != m_boundAccelerationStructures.end()) ? it->second : nullptr;
	}

	void ContextGL::traceRays(const TraceRaysDesc& desc) {
		if (!m_rayTracingSupported) {
			throw std::runtime_error("Ray tracing is not supported");
		}

		if (!m_boundRayTracingPipeline) {
			throw std::runtime_error("No ray tracing pipeline bound");
		}

		throw std::runtime_error(
			"Ray tracing dispatch not fully implemented for OpenGL backend. "
			"Consider using Vulkan or DirectX 12 for production ray tracing."
		);
	}*/

	void ContextGL::buildAccelerationStructure(
		const std::shared_ptr<AccelerationStructure>& accelerationStructure,
		bool update) {
		throw std::runtime_error("Ray tracing is not supported");
	}

	void ContextGL::rayTracingBarrier() {
		throw std::runtime_error("Ray tracing is not supported");
	}

	// ===== VIEWPORT Y SCISSOR =====

	void ContextGL::setViewport(int x, int y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void ContextGL::setScissor(int x, int y, uint32_t width, uint32_t height) {
		glScissor(x, y, width, height);
	}

	void ContextGL::setPolygonMode(PolygonMode mode)
	{
		auto toGLMode = [](PolygonMode mode) -> GLenum {
			switch (mode) {
			case PolygonMode::Fill: return GL_FILL;
			case PolygonMode::Line: return GL_LINE;
			case PolygonMode::Point: return GL_POINT;
			default: return GL_FILL;
			}
			};
		glPolygonMode(GL_FRONT_AND_BACK, toGLMode(mode));
	}

} // namespace PGRenderCore
