#pragma once
#include "types.h"
#include <memory>

namespace pgrender {

// Contexto gráfico abstracto
class IGraphicsContext {
public:
    virtual ~IGraphicsContext() = default;
    
    // Operaciones básicas
    virtual void makeCurrent() = 0;
    virtual void swapBuffers() = 0;
    virtual void* getNativeHandle() const = 0;
    virtual RenderBackend getBackend() const = 0;
    
    // Información sobre compartición
    virtual bool isShared() const = 0;
    virtual IGraphicsContext* getSharedContext() const = 0;
    
    // No copiable
    IGraphicsContext(const IGraphicsContext&) = delete;
    IGraphicsContext& operator=(const IGraphicsContext&) = delete;
    
    // Movible
    IGraphicsContext(IGraphicsContext&&) = default;
    IGraphicsContext& operator=(IGraphicsContext&&) = default;

protected:
    IGraphicsContext() = default;
};

} // namespace pgrender
