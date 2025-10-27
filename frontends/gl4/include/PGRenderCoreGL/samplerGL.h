#pragma once
#include <PGRenderCore/sampler.h>
#include <cstdint>

namespace PGRenderCore {

    // Alias opaco para identificador OpenGL de sampler
    using SamplerHandle = uint32_t;

    class SamplerGL : public Sampler {
    public:
        explicit SamplerGL(const Desc& desc);
        ~SamplerGL() override;

        const Desc& getDesc() const override { return m_desc; }
        uint64_t nativeHandle() const override { return static_cast<uint64_t>(m_samplerId); }

        SamplerHandle nativeSamplerId() const { return m_samplerId; }
		BackendType getBackendType() const override { return BackendType::OpenGL; }
    private:
        Desc m_desc;
        SamplerHandle m_samplerId;

        // Helpers para conversión a enums OpenGL. Implementados en .cpp
        unsigned int toGLFilterMode(FilterMode mode) const;
        unsigned int toGLAddressMode(AddressMode mode) const;
    };
} // namespace PGRenderCore

