#pragma once
#include "core.h"

#include <cstdint>
#include <memory>

namespace pgrender {

    /**
     * @brief Tipo de buffer.
     */
    enum class BufferType {
        Vertex,           ///< Buffer de vértices
        Index,            ///< Buffer de índices
        Uniform,          ///< Uniform Buffer Object (UBO)
        ShaderStorage,    ///< Shader Storage Buffer Object (SSBO)
        TransferSrc,      ///< Buffer fuente para transferencias
        TransferDst       ///< Buffer destino para transferencias
    };

    /**
     * @brief Patrón de uso del buffer.
     */
    enum class BufferUsage {
        Static,      ///< Datos escritos una vez, leídos muchas veces (GPU-only optimal)
        Dynamic,     ///< Datos actualizados frecuentemente desde CPU
        Stream       ///< Datos escritos una vez por frame, leídos pocas veces
    };

    /**
     * @brief Flags de acceso para mapeo de memoria.
     */
    enum class BufferAccessFlags : uint32_t {
        Read = 1 << 0,         ///< Acceso de lectura
        Write = 1 << 1,        ///< Acceso de escritura
        ReadWrite = Read | Write
    };

    inline BufferAccessFlags operator|(BufferAccessFlags a, BufferAccessFlags b) {
        return static_cast<BufferAccessFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline BufferAccessFlags operator&(BufferAccessFlags a, BufferAccessFlags b) {
        return static_cast<BufferAccessFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    /**
     * @brief Interfaz abstracta para buffers de GPU.
     * El buffer NO almacena información de binding, es responsabilidad del contexto/VAO.
     */
    class BufferObject {
    public:

        /**
         * @brief Descriptor de buffer.
         */
        struct Desc {
            BufferType type = BufferType::Vertex;
            BufferUsage usage = BufferUsage::Static;
            size_t size = 0;                ///< Tamaño en bytes
            const void* data = nullptr;     ///< Datos iniciales (opcional)
            const char* debugName = nullptr; ///< Nombre para debugging (opcional)
        };

        virtual ~BufferObject() = default;

        /**
         * @brief Devuelve el descriptor del buffer.
         */
        virtual const Desc& getDesc() const = 0;

        /**
         * @brief Handle nativo opaco (platform-specific).
         */
        virtual uint64_t nativeHandle() const = 0;

        /**
         * @brief Devuelve el tamaño del buffer en bytes.
         */
        virtual size_t getSize() const = 0;

        /**
         * @brief Actualiza datos del buffer.
         * @param data Puntero a los datos.
         * @param size Tamaño de los datos en bytes.
         * @param offset Offset en el buffer donde escribir.
         */
        virtual void update(const void* data, size_t size, size_t offset = 0) = 0;

        /**
         * @brief Mapea el buffer en memoria del host para lectura/escritura directa.
         * @param access Flags de acceso (lectura, escritura o ambos).
         * @param offset Offset en bytes desde el inicio del buffer.
         * @param size Tamaño a mapear (0 = todo el buffer desde offset).
         * @return Puntero a la memoria mapeada.
         */
        virtual void* map(BufferAccessFlags access = BufferAccessFlags::Write,
            size_t offset = 0,
            size_t size = 0) = 0;

        /**
         * @brief Desmapea el buffer previamente mapeado.
         * Los cambios realizados en la memoria mapeada se hacen visibles a la GPU.
         */
        virtual void unmap() = 0;

        /**
         * @brief Copia datos desde otro buffer.
         * @param src Buffer fuente.
         * @param srcOffset Offset en el buffer fuente.
         * @param dstOffset Offset en este buffer (destino).
         * @param size Tamaño a copiar en bytes.
         */
        virtual void copyFrom(const std::shared_ptr<BufferObject>& src,
            size_t srcOffset,
            size_t dstOffset,
            size_t size) = 0;

        /**
         * @brief Redimensiona el buffer (destruye contenido anterior).
         * @param newSize Nuevo tamaño en bytes.
         * @param data Datos iniciales opcionales.
         */
        virtual void resize(size_t newSize, const void* data = nullptr) = 0;

        BACKEND_CHECKER
        CAST_HELPERS
    };

} // namespace pgrender
