#pragma once
#include "bufferObject.h"
#include "texture.h"
#include "sampler.h"
#include "pipeline.h"
#include "renderTarget.h"
#include "backendType.h"
#include "vertexArray.h"

#include "Shader.h"
#include "RenderPass.h"
#include "RayTracingStructures.h"
//#include "RayTracingPipeline.h"
#include <memory>
#include <unordered_map>
#include <cstdint>
#include <glm/vec4.hpp>


namespace PGRenderCore {

	/**
	 * @brief Flags para especificar qué buffers limpiar.
	 */
	enum class ClearFlags : uint32_t {
		None = 0,
		Color = 1 << 0,
		Depth = 1 << 1,
		Stencil = 1 << 2,
		ColorDepth = Color | Depth,
		All = Color | Depth | Stencil
	};

	inline ClearFlags operator|(ClearFlags a, ClearFlags b) {
		return static_cast<ClearFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline ClearFlags operator&(ClearFlags a, ClearFlags b) {
		return static_cast<ClearFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	}

	inline bool operator!(ClearFlags flags) {
		return static_cast<uint32_t>(flags) == 0;
	}

	/**
	 * @brief Contexto de renderizado abstracto.
	 * No depende de ningún framework externo (SDL, GLFW, etc.).
	 * Los handles nativos son opacos y específicos de plataforma.
	 */
	class Context {
	public:

		/**
		 * @brief Descriptor para crear un contexto de renderizado.
		 */
		struct Desc {
			void* nativeWindowHandle = nullptr;         ///< Handle nativo de ventana (platform-specific)
			void* nativeDisplayHandle = nullptr;        ///< Handle nativo de display (X11, Wayland)
			std::shared_ptr<Context> sharedContext = nullptr; ///< Contexto para compartir recursos

			uint32_t width = 0;                         ///< Ancho de la superficie (opcional)
			uint32_t height = 0;                        ///< Alto de la superficie (opcional)

			bool enableDebug = false;                   ///< Habilitar validación/debug
			bool enableVSync = true;                    ///< Habilitar sincronización vertical
		};



		virtual ~Context() = default;


		/**
		 * @brief Hace current este contexto en el thread actual.
		 * Necesario antes de realizar operaciones de renderizado.
		 */
		virtual void makeCurrent() = 0;

		/**
		 * @brief Intercambia buffers (presenta frame renderizado).
		 * Solo válido si el contexto está asociado a una superficie.
		 */
		virtual void swapBuffers() = 0;

		// ===== FÁBRICAS DE RECURSOS =====

		virtual std::shared_ptr<BufferObject> createBufferObject(const BufferObject::Desc& desc) = 0;
		virtual std::shared_ptr<Texture> createTexture(const Texture::Desc& desc) = 0;
		virtual std::shared_ptr<Program> createProgram(const Program::Desc& desc) = 0;
		virtual std::shared_ptr<Sampler> createSampler(const Sampler::Desc& desc) = 0;
		virtual std::shared_ptr<Pipeline> createPipeline(const Pipeline::Desc& desc) = 0;
		virtual std::shared_ptr<RenderTarget> createRenderTarget(const RenderTarget::Desc& desc) = 0;
		virtual std::shared_ptr<RenderPass> createRenderPass(const RenderPass::Desc& desc) = 0;
		virtual std::shared_ptr<VertexArray> createVertexArray(const VertexArray::Desc& desc) = 0;

		// ===== ESTADO DE BINDING =====

		virtual void bindVertexArray(const std::shared_ptr<VertexArray>& vertexArray) = 0;
		virtual std::shared_ptr<VertexArray> getBoundVertexArray() const = 0;

		/**
		 * @brief Vincula un pipeline de renderizado.
		 * Establece shaders, estado de blending, depth test, culling, polygon mode, etc.
		 * @param pipeline Pipeline a vincular (nullptr para desvincular).
		 */
		virtual void bindPipeline(const std::shared_ptr<Pipeline>& pipeline) = 0;

		/**
		 * @brief Obtiene el pipeline actualmente vinculado.
		 * @return Pipeline vinculado o nullptr si ninguno.
		 */
		virtual std::shared_ptr<Pipeline> getBoundPipeline() const = 0;

		virtual void bindTexture(const std::shared_ptr<Texture>& texture, uint32_t slot = 0) = 0;
		virtual void bindSampler(const std::shared_ptr<Sampler>& sampler, uint32_t slot = 0) = 0;

		virtual void bindUniformBuffer(const std::shared_ptr<BufferObject>& buffer,
			uint32_t binding,
			size_t offset = 0,
			size_t size = 0) = 0;

		virtual void bindShaderStorageBuffer(const std::shared_ptr<BufferObject>& buffer,
			uint32_t binding,
			size_t offset = 0,
			size_t size = 0) = 0;

		virtual std::shared_ptr<BufferObject> getBoundUniformBuffer(uint32_t binding) const = 0;
		virtual std::shared_ptr<BufferObject> getBoundShaderStorageBuffer(uint32_t binding) const = 0;

		// ===== OPERACIONES DE LIMPIEZA =====

		/**
		 * @brief Limpia el framebuffer activo con los valores especificados.
		 * @param flags Qué buffers limpiar (color, depth, stencil).
		 * @param clearColor Color de limpieza (RGBA, valores 0.0-1.0).
		 * @param clearDepth Valor de profundidad (típicamente 1.0).
		 * @param clearStencil Valor de stencil (típicamente 0).
		 */
		virtual void clear(ClearFlags flags,
			const glm::vec4& clearColor = glm::vec4{ std::numeric_limits<float>::infinity() },
			float clearDepth = 1.0f,
			int clearStencil = 0) = 0;

		virtual void setClearColor(float r, float g, float b, float a = 1.0f) = 0;
		/**
		 * @brief Establece el valor de profundidad de limpieza por defecto.
		 * @param depth Valor de profundidad (típicamente 1.0).
		 */
		virtual void setClearDepth(float depth) = 0;

		/**
		 * @brief Establece el valor de stencil de limpieza por defecto.
		 * @param stencil Valor de stencil (típicamente 0).
		 */
		virtual void setClearStencil(int stencil) = 0;

		// ===== COMANDOS DE DIBUJO =====

		virtual void draw(uint32_t vertexCount, uint32_t firstVertex = 0) = 0;
		virtual void drawIndexed(uint32_t indexCount, uint32_t firstIndex = 0, int32_t vertexOffset = 0) = 0;
		virtual void drawInstanced(uint32_t vertexCount, uint32_t instanceCount,
			uint32_t firstVertex = 0, uint32_t firstInstance = 0) = 0;
		virtual void drawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount,
			uint32_t firstIndex = 0, int32_t vertexOffset = 0,
			uint32_t firstInstance = 0) = 0;

		// ===== RAY TRACING =====

		virtual bool isRayTracingSupported() const = 0;
		virtual std::shared_ptr<AccelerationStructure> createBLAS(const BLASDesc& desc) = 0;
		virtual std::shared_ptr<AccelerationStructure> createTLAS(const TLASDesc& desc) = 0;
		//virtual std::shared_ptr<RayTracingPipeline> createRayTracingPipeline(
		//	const RayTracingPipelineDesc& desc) = 0;
		//virtual void bindRayTracingPipeline(const std::shared_ptr<RayTracingPipeline>& pipeline) = 0;
		//virtual std::shared_ptr<RayTracingPipeline> getBoundRayTracingPipeline() const = 0;
		//virtual void bindAccelerationStructure(
		//	const std::shared_ptr<AccelerationStructure>& tlas,
		//	uint32_t binding) = 0;
		//virtual std::shared_ptr<AccelerationStructure> getBoundAccelerationStructure(
		//	uint32_t binding) const = 0;
		//virtual void traceRays(const TraceRaysDesc& desc) = 0;
		virtual void buildAccelerationStructure(
			const std::shared_ptr<AccelerationStructure>& accelerationStructure,
			bool update = false) = 0;
		virtual void rayTracingBarrier() = 0;


		// ===== VIEWPORT Y SCISSOR =====

		/**
		 * @brief Establece el viewport.
		 */
		virtual void setViewport(int x, int y, uint32_t width, uint32_t height) = 0;

		/**
		 * @brief Establece la región de scissor test.
		 */
		virtual void setScissor(int x, int y, uint32_t width, uint32_t height) = 0;

		/**
		 * @brief Establece el valor del modo de dibujado de polígonos.
		*/
		virtual void setPolygonMode(PolygonMode mode) = 0;


		BACKEND_CHECKER
		CAST_HELPERS
	};

} // namespace PGRenderCore
