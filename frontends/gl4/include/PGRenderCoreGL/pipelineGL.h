#pragma once
#include <PGRenderCore/Pipeline.h>
#include <PGRenderCore/stateConstants.h>

#include <cstdint>

namespace PGRenderCore {
    class PipelineGL : public Pipeline {
    public:
        explicit PipelineGL(const Pipeline::Desc& desc);
        ~PipelineGL() override;

        BackendType getBackendType() const override { return BackendType::OpenGL; }
        const Pipeline::Desc& getDesc() const override { return m_desc; }

    private:
        friend class ContextGL;  // Para que ContextGL pueda llamar a apply()

        Pipeline::Desc m_desc;

        void apply() const;
        void applyBlendMode() const;
        void applyDepthState() const;
        void applyCullMode() const;
        void applyPolygonMode() const;
        void applyOtherStates() const;

        unsigned int toGLBlendFactor(BlendFactor factor) const;
        unsigned int toGLBlendOp(BlendOp op) const;
    };

} // namespace PGRenderCore
