#pragma once
#include <memory>
#include <PGRenderCore/device.h>
#include <PGRenderCore/context.h>
#include <PGRenderCore/debugManager.h>


namespace PGRenderCore {

    /**
     * @brief Implementación de Device para OpenGL 4.
     */
    class DeviceGL : public Device {
    public:
        DeviceGL();
        ~DeviceGL() override;

        std::unique_ptr<Context> createContext(const Context::Desc& desc) override;
        DebugManager* getDebugManager() override;

    private:
		std::unique_ptr<DebugManager> m_debugManager;
    };

} // namespace PGRenderCore
