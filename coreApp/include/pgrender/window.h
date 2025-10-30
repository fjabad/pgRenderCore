#pragma once
#include "graphicsContext.h"
#include "types.h"
#include <memory>
#include <string>

namespace pgrender {

// Interfaz de ventana
class IWindow {
public:
    virtual ~IWindow() = default;
    
    // Configuración de ventana
    struct Desc {
        std::string title = "PGRenderCore Window";
        uint32_t width = 800;
        uint32_t height = 600;
        bool resizable = true;
        bool fullscreen = false;
        bool vsync = true;
        bool highDPI = false;
        bool transparent = false;
        bool alwaysOnTop = false;
        bool borderless = false;
        RenderBackend renderBackend = RenderBackend::Auto;
    };

    // Operaciones de ventana
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void setTitle(const std::string& title) = 0;
    virtual void setSize(uint32_t width, uint32_t height) = 0;
    virtual void getSize(uint32_t& width, uint32_t& height) const = 0;
    bool shouldClose() const { return m_shouldClose; }
	void markForClose() { m_shouldClose = true; }
    virtual void* getNativeHandle() const = 0;
	virtual WindowID getWindowID() const = 0;
    
    // Crear contexto gráfico
    virtual std::unique_ptr<IGraphicsContext> createExtraContext(const IContextDescriptor& config) = 0;
    
	// Modo relativo de mouse (por ventana en SDL3)
	virtual void setRelativeMouseMode(bool enabled) = 0;
	virtual bool getRelativeMouseMode() const = 0;

	// Captura de mouse (confinamiento a la ventana)
	virtual void setMouseGrab(bool grabbed) = 0;
	virtual bool getMouseGrab() const = 0;

    
    // No copiable ni movible
    IWindow(const IWindow&) = delete;
    IWindow& operator=(const IWindow&) = delete;
    IWindow(IWindow&&) = delete;
    IWindow& operator=(IWindow&&) = delete;

protected:
    IWindow() = default;
	bool m_shouldClose = false;
};

} // namespace pgrender
