#pragma once
#include <memory>
#include <cstdint>
#include <string>
#include <glm/vec4.hpp>

#include "core.h"

namespace pgrender {

    class RenderTarget;


    /**
     * @brief Interfaz abstracta para RenderPass.
     * Una unidad de renderizado con control de framebuffer, clears y estado asociado.
     */
    class RenderPass {
    public:
        virtual ~RenderPass() = default;

        /**
          * @brief Descriptor para configurar un RenderPass.
          * Incluye opcionalmente el RenderTarget a usar y parámetros para limpiar buffers.
          */
        struct Desc {
            std::shared_ptr<RenderTarget> renderTarget = nullptr; ///< Render target usado, si es nullptr se usa framebuffer por defecto.

            // Bs flags de clear (pueden combinarse)
            bool clearColor = false;
            bool clearDepth = false;
            bool clearStencil = false;

            // Valores utilizados para limpiar buffers
            glm::vec4 clearColorValue { 0, 0, 0, 1 };
            float clearDepthValue = 1.0f;
            int clearStencilValue = 0;

            std::string debugName;
        };

        /**
         * @brief Devuelve un identificador nativo del render pass, si existe o 0.
         */
        virtual uint64_t nativeHandle() const = 0;

        virtual const Desc& getDesc() const = 0;

        /**
         * @brief Inicia el render pass, bindear framebuffer y limpiar buffers según descriptor.
         */
        virtual void begin() = 0;

        /**
         * @brief Finaliza el render pass, desvincula framebuffer, etc.
         */
        virtual void end() = 0;

        BACKEND_CHECKER
        CAST_HELPERS
    };

} // namespace pgrender
