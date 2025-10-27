#pragma once
#include <PGRenderCore/Context.h>
#include <unordered_map>
#include <cstdint>

namespace PGRenderCore {

    /**
     * @brief Implementación OpenGL del contexto de renderizado.
     * Depende de OpenGL (GLEW) pero NO de SDL, GLFW u otros frameworks.
     */
    class ContextGL : public Context {
    public:
        /**
         * @brief Constructor.
         * @param desc Descriptor con handles nativos de ventana/display.
         * @param sharedContext Contexto OpenGL para compartir recursos (opcional).
         */
        explicit ContextGL(const Context::Desc& desc);
        ~ContextGL() override;

        BackendType getBackendType() const override { return BackendType::OpenGL; }

        void makeCurrent() override;
        void swapBuffers() override;

        // Fábricas
        std::shared_ptr<BufferObject> createBufferObject(const BufferObject::Desc& desc) override;
        std::shared_ptr<Texture> createTexture(const Texture::Desc& desc) override;
        std::shared_ptr<Program> createProgram(const Program::Desc& desc) override;
        std::shared_ptr<Pipeline> createPipeline(const Pipeline::Desc& desc) override;
        std::shared_ptr<RenderTarget> createRenderTarget(const RenderTarget::Desc& desc) override;
        std::shared_ptr<RenderPass> createRenderPass(const RenderPass::Desc& desc) override;
        std::shared_ptr<Sampler> createSampler(const Sampler::Desc& desc) override;
        std::shared_ptr<VertexArray> createVertexArray(const VertexArray::Desc& desc) override;

        // Binding
        void bindVertexArray(const std::shared_ptr<VertexArray>& vertexArray) override;
        std::shared_ptr<VertexArray> getBoundVertexArray() const override;

        void bindPipeline(const std::shared_ptr<Pipeline>& pipeline) override;
        std::shared_ptr<Pipeline> getBoundPipeline() const override;

        void bindTexture(const std::shared_ptr<Texture>& texture, uint32_t slot = 0) override;
        void bindSampler(const std::shared_ptr<Sampler>& sampler, uint32_t slot = 0) override;

        void bindUniformBuffer(const std::shared_ptr<BufferObject>& buffer,
            uint32_t binding,
            size_t offset = 0,
            size_t size = 0) override;

        void bindShaderStorageBuffer(const std::shared_ptr<BufferObject>& buffer,
            uint32_t binding,
            size_t offset = 0,
            size_t size = 0) override;

        std::shared_ptr<BufferObject> getBoundUniformBuffer(uint32_t binding) const override;
        std::shared_ptr<BufferObject> getBoundShaderStorageBuffer(uint32_t binding) const override;

        // Limpieza
        void clear(ClearFlags flags,
            const glm::vec4& clearColor = glm::vec4{ std::numeric_limits<float>::infinity() },
            float clearDepth = 1.0f,
            int clearStencil = 0) override;

        void setClearColor(float r, float g, float b, float a = 1.0f) override;
        void setClearDepth(float depth) override;
        void setClearStencil(int stencil) override;

        // Dibujo
        void draw(uint32_t vertexCount, uint32_t firstVertex = 0) override;
        void drawIndexed(uint32_t indexCount, uint32_t firstIndex = 0, int32_t vertexOffset = 0) override;
        void drawInstanced(uint32_t vertexCount, uint32_t instanceCount,
            uint32_t firstVertex = 0, uint32_t firstInstance = 0) override;
        void drawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount,
            uint32_t firstIndex = 0, int32_t vertexOffset = 0,
            uint32_t firstInstance = 0) override;

        // Ray Tracing
        bool isRayTracingSupported() const override;
        std::shared_ptr<AccelerationStructure> createBLAS(const BLASDesc& desc) override;
        std::shared_ptr<AccelerationStructure> createTLAS(const TLASDesc& desc) override;
        //std::shared_ptr<RayTracingPipeline> createRayTracingPipeline(
        //    const RayTracingPipelineDesc& desc) override;
        //void bindRayTracingPipeline(const std::shared_ptr<RayTracingPipeline>& pipeline) override;
        //std::shared_ptr<RayTracingPipeline> getBoundRayTracingPipeline() const override;
        //void bindAccelerationStructure(
        //    const std::shared_ptr<AccelerationStructure>& tlas,
        //    uint32_t binding) override;
        //std::shared_ptr<AccelerationStructure> getBoundAccelerationStructure(
        //    uint32_t binding) const override;
        //void traceRays(const TraceRaysDesc& desc) override;
        void buildAccelerationStructure(
            const std::shared_ptr<AccelerationStructure>& accelerationStructure,
            bool update = false) override;
        void rayTracingBarrier() override;

        // Viewport y Scissor
        void setViewport(int x, int y, uint32_t width, uint32_t height) override;
        void setScissor(int x, int y, uint32_t width, uint32_t height) override;
        void setPolygonMode(PolygonMode mode) override;

        /**
         * @brief Obtiene el contexto OpenGL nativo.
         * El tipo exacto depende de la plataforma (HGLRC, GLXContext, etc.).
         */
        void* getNativeContext() const { return m_glContext; }

    private:
        void* m_nativeWindowHandle;     // Platform-specific window handle
        void* m_nativeDisplayHandle;    // Platform-specific display handle (X11, Wayland)
        void* m_glContext;              // OpenGL context (HGLRC, GLXContext, EGLContext, etc.)
        uint32_t m_vao;

        // Estado de binding
        std::shared_ptr<VertexArray> m_boundVertexArray;
        std::shared_ptr<Pipeline> m_boundPipeline;
        std::unordered_map<uint32_t, std::shared_ptr<BufferObject>> m_boundUniformBuffers;
        std::unordered_map<uint32_t, std::shared_ptr<BufferObject>> m_boundShaderStorageBuffers;

        // Ray tracing
        bool m_rayTracingSupported;
//        std::shared_ptr<RayTracingPipeline> m_boundRayTracingPipeline;
        std::unordered_map<uint32_t, std::shared_ptr<AccelerationStructure>> m_boundAccelerationStructures;

        // Clear values
        float m_clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float m_clearDepth = 1.0f;
        int m_clearStencil = 0;

        // Platform-specific initialization
        void initializeGLContext(const Context::Desc& desc);
        void cleanupGLContext();
    };

} // namespace PGRenderCore
