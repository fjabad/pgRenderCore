#pragma once
#include <PGRenderCore/bufferObject.h>
#include <cstdint>

namespace PGRenderCore {

    using BufferHandle = uint32_t;

    class BufferObjectGL : public BufferObject {
    public:
        explicit BufferObjectGL(const BufferObject::Desc& desc);
        ~BufferObjectGL() override;

        BackendType getBackendType() const override { return BackendType::OpenGL; }
        const BufferObject::Desc& getDesc() const override { return m_desc; }
        uint64_t nativeHandle() const override { return static_cast<uint64_t>(m_bufferId); }
        size_t getSize() const override { return m_size; }

        void update(const void* data, size_t size, size_t offset = 0) override;
        void* map(BufferAccessFlags access = BufferAccessFlags::Write,
            size_t offset = 0,
            size_t size = 0) override;
        void unmap() override;
        void copyFrom(const std::shared_ptr<BufferObject>& src,
            size_t srcOffset,
            size_t dstOffset,
            size_t size) override;
        void resize(size_t newSize, const void* data = nullptr) override;

        BufferHandle nativeBufferId() const { return m_bufferId; }

    private:
        BufferObject::Desc m_desc;
        BufferHandle m_bufferId;
        size_t m_size;
        bool m_isMapped;

        unsigned int toGLTarget() const;
        unsigned int toGLUsage() const;
        unsigned int toGLAccessFlags(BufferAccessFlags flags) const;
    };

} // namespace PGRenderCore
