#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include "core.h"

namespace pgrender {

    class Texture;

    /**
     * @brief Abstracción de un render target (framebuffer) para renderizado.
     *
     * Agrupa uno o varios attachments (color, profundidad, stencil),
     * y permite gestionar configuración y acceso a texturas de salida.
     */
    class RenderTarget {
    public:
        /**
         * @brief Descriptor para crear un RenderTarget.
         */
        struct Desc {
            /**
             * @brief Texturas usadas como attachments de color del render target.
             * Puede contener múltiples niveles para MRT (Multiple Render Targets).
             */
            std::vector<std::shared_ptr<Texture>> colorAttachments;

            /**
             * @brief Textura usada como attachment de profundidad (y stencil opcional).
             */
            std::shared_ptr<Texture> depthStencilAttachment;

            /**
             * @brief Ancho y alto del render target (deben coincidir con las texturas).
             */
            uint32_t width = 0;
            uint32_t height = 0;

            /**
             * @brief Más flags adicionales si se requiere (multisample, array layers, etc.)
             */
        };

        virtual ~RenderTarget() = default;

        /**
         * @brief Devuelve la textura attachment de color en el slot indicado.
         * @param index Índice del attachment de color.
         * @return Shared pointer a la textura si existe, nullptr si índice inválido.
         */
        virtual std::shared_ptr<Texture> getColorAttachment(uint32_t index) const = 0;

        /**
         * @brief Devuelve la textura attachment de profundidad y stencil.
         * @return Shared pointer a la textura de profundidad/stencil, nullptr si no existe.
         */
        virtual std::shared_ptr<Texture> getDepthStencilAttachment() const = 0;

        /**
         * @brief Devuelve el ancho del render target.
         */
        virtual uint32_t getWidth() const = 0;

        /**
         * @brief Devuelve la altura del render target.
         */
        virtual uint32_t getHeight() const = 0;

        /**
         * @brief Devuelve el identificador nativo o handle interno del render target.
         *        (Ej. framebuffer OpenGL o Vulkan).
         */
        virtual uint64_t nativeHandle() const = 0;

        BACKEND_CHECKER
        CAST_HELPERS
    };

} // namespace pgrender
