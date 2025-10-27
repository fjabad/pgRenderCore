#pragma once
#include <PGRenderCore/texture.h>
#include <cstdint>
#include <PGRenderCore/backendType.h>

namespace PGRenderCore {

    // Alias opaco para el identificador de textura OpenGL
    using TextureHandle = uint32_t;

    class TextureGL : public Texture {
    public:
        explicit TextureGL(const Desc& desc);
        ~TextureGL() override;

        void update(const void* pixelData, size_t dataSize, uint32_t mipLevel = 0, uint32_t arrayLayer = 0) override;

        const Desc& getDesc() const override { return m_desc; }
        uint64_t nativeHandle() const override { return static_cast<uint64_t>(m_textureId); }

        TextureHandle nativeTextureId() const { return m_textureId; }

		BackendType getBackendType() const override { return BackendType::OpenGL; }
        unsigned int toGLTarget() const;
    private:
        Desc m_desc;
        TextureHandle m_textureId;

        // Conversión a tipos GL, implementados en .cpp
        unsigned int toGLInternalFormat() const;
        unsigned int toGLFormat() const;
        unsigned int toGLType() const;
    };

} // namespace PGRenderCore
