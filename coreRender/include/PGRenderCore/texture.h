#pragma once
#include <cstdint>
#include "core.h"

namespace PGRenderCore {

    /**
     * @brief Clase que representa una textura en GPU.
     *
     * Soporta texturas 1D, 2D, 3D, cubemaps y buffers de textura, con formatos comunes y niveles MIP.
     */
    class Texture {
    public:
        /**
         * @brief Tipos de textura soportados.
         */
        enum class Type {
            Texture1D,
            Texture2D,
            Texture2DMultisample,
            Texture3D,
            TextureCube,
            TextureBuffer,
            // Opcionales: , TextureRectangle, etc.
        };

        /**
         * @brief Formatos de pixel soportados.
         */
        enum class Format {
            R8,
            RG8,
            RGB8,
            RGBA8,
            R16F,
            RG16F,
            RGB16F,
            RGBA16F,
            R32F,
            RG32F,
            RGB32F,
            RGBA32F,
            Depth24Stencil8,
            Depth32F,
            // Se pueden ampliar a otros formatos según soporte
        };

        /**
         * @brief Descripción para crear una textura.
         */
        struct Desc {
            Type type;                ///< Tipo de textura.
            uint32_t width;          ///< Ancho en píxeles.
            uint32_t height;         ///< Alto en píxeles (si aplica).
            uint32_t depth;          ///< Profundidad (si es 3D).
            uint16_t mipLevels;     ///< Número de niveles MIP.
            Format format;           ///< Formato interno.
            bool mipmapped = false;  ///< Si genera mipmaps automáticamente.
            bool immutable = false;  ///< Textura inmutable (no se puede modificar después).
            bool storageTexture = false; ///< Permitir uso como almacenamiento
            // Otros flags como anisotropía, muestreo, etc.
        };

        /**
         * @brief Actualiza la textura con datos desde CPU.
         * @param pixelData Datos en memoria de la textura.
         * @param dataSize Tamaño en bytes.
         * @param mipLevel Nivel MIP a actualizar.
         * @param arrayLayer Capa de array (si aplica).
         */
        virtual void update(const void* pixelData, size_t dataSize, uint32_t mipLevel = 0, uint32_t arrayLayer = 0) = 0;

        /**
         * @brief Destructor virtual.
         */
        virtual ~Texture() = default;

        /**
         * @brief Devuelve la descripción con los parámetros usados.
         */
        virtual const Desc& getDesc() const = 0;

        /**
         * @brief Devuelve el identificador interno o handle de textura.
         *        (Ej. ID de OpenGL, descriptor de Vulkan, etc.)
         */
        virtual uint64_t nativeHandle() const = 0;

        BACKEND_CHECKER
        CAST_HELPERS
    };

} // namespace PGRenderCore
