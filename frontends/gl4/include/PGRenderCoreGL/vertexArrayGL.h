#pragma once
#include <PGRenderCore/vertexArray.h>
#include <cstdint>

namespace PGRenderCore {

    class VertexArrayGL : public VertexArray {
    public:
        explicit VertexArrayGL(const VertexArray::Desc& desc);
        ~VertexArrayGL() override;

        BackendType getBackendType() const override { return BackendType::OpenGL; }

        const VertexLayout& getLayout() const override { return m_layout; }
        const std::vector<std::shared_ptr<BufferObject>>& getVertexBuffers() const override { return m_vertexBuffers; }
        std::shared_ptr<BufferObject> getIndexBuffer() const override { return m_indexBuffer; }

        uint64_t nativeHandle() const override { return static_cast<uint64_t>(m_vao); }

        void setVertexBuffer(uint32_t binding, std::shared_ptr<BufferObject> buffer) override;
        void setIndexBuffer(std::shared_ptr<BufferObject> buffer) override;

        uint32_t nativeVAO() const { return m_vao; }

    private:
        uint32_t m_vao;
        VertexLayout m_layout;
        std::vector<std::shared_ptr<BufferObject>> m_vertexBuffers;
        std::shared_ptr<BufferObject> m_indexBuffer;

        void setupVertexAttributes();
        unsigned int toGLAttributeType(VertexAttributeType type) const;
    };

} // namespace PGRenderCore
