#pragma once
#include <PGRenderCore/renderPass.h>
#include <cstdint>
#include <PGRenderCore/backendType.h>

namespace pgrender {

    class RenderPassGL : public RenderPass {
    public:
        explicit RenderPassGL(const RenderPass::Desc& desc);
        ~RenderPassGL() override;

        const RenderPass::Desc& getDesc() const override;
        uint64_t nativeHandle() const override;

        void begin() override;
        void end() override;

		BackendType getBackendType() const override { return BackendType::OpenGL; }

    private:
        RenderPass::Desc m_desc;
        bool m_isActive = false;
    };

} // namespace pgrender
