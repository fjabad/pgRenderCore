#pragma once
#include <PGRenderCore/renderTarget.h>
#include <vector>
#include <memory>
#include <cstdint>

namespace pgrender {

    class TextureGL;

    using FramebufferHandle = uint32_t;

    class RenderTargetGL : public RenderTarget {
    public:
        explicit RenderTargetGL(const Desc& desc);
        ~RenderTargetGL() override;

        std::shared_ptr<Texture> getColorAttachment(uint32_t index) const override;
        std::shared_ptr<Texture> getDepthStencilAttachment() const override;

        uint32_t getWidth() const override { return m_width; }
        uint32_t getHeight() const override { return m_height; }

        uint64_t nativeHandle() const override { return static_cast<uint64_t>(m_fboId); }

		BackendType getBackendType() const override { return BackendType::OpenGL; }

    private:
        Desc m_desc;
        FramebufferHandle m_fboId = 0;
        uint32_t m_width = 0;
        uint32_t m_height = 0;

        void checkFramebufferStatus() const;

        // Mantener copias para retornar en getters
        std::vector<std::shared_ptr<Texture>> m_colorAttachments;
        std::shared_ptr<Texture> m_depthStencilAttachment;
    };

} // namespace pgrender

