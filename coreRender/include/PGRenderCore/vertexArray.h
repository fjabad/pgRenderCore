#pragma once
#include "backendType.h"
#include "bufferObject.h"
#include "vertexLayout.h"
#include <memory>
#include <vector>
#include <cstdint>

namespace PGRenderCore {

    /**
     * @brief Vertex Array Object: encapsula layout + buffers.
     * Representa un "modelo renderizable" completo con su configuración de vértices.
     */
    class VertexArray {
    public:
        virtual ~VertexArray() = default;

        /**
         * @brief Descriptor para crear un Vertex Array Object.
         * Encapsula layout de vértices y buffers asociados.
         */
        struct Desc {
            VertexLayout layout;
            std::vector<std::shared_ptr<BufferObject>> vertexBuffers;
            std::shared_ptr<BufferObject> indexBuffer = nullptr;
        };

        /**
         * @brief Obtiene el layout de vértices de este VAO.
         */
        virtual const VertexLayout& getLayout() const = 0;

        /**
         * @brief Obtiene los buffers de vértices asociados.
         */
        virtual const std::vector<std::shared_ptr<BufferObject>>& getVertexBuffers() const = 0;

        /**
         * @brief Obtiene el buffer de índices asociado.
         */
        virtual std::shared_ptr<BufferObject> getIndexBuffer() const = 0;

        /**
         * @brief Handle nativo opaco.
         */
        virtual uint64_t nativeHandle() const = 0;

        /**
         * @brief Actualiza un buffer de vértices específico.
         * @param binding Índice del binding a actualizar.
         * @param buffer Nuevo buffer.
         */
        virtual void setVertexBuffer(uint32_t binding, std::shared_ptr<BufferObject> buffer) = 0;

        /**
         * @brief Actualiza el buffer de índices.
         */
        virtual void setIndexBuffer(std::shared_ptr<BufferObject> buffer) = 0;


        BACKEND_CHECKER
        CAST_HELPERS

    };

} // namespace PGRenderCore
