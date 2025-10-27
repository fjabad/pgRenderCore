#pragma once
#include "core.h"
#include "PGRenderCore/context.h"
#include <memory>

namespace PGRenderCore {
    class DebugManager;
    class Device {
    public:
        virtual std::unique_ptr<class Context> createContext(const struct Context::Desc&) = 0;
        virtual class DebugManager* getDebugManager() = 0;
        virtual ~Device() = default;

        CAST_HELPERS
    };
}
