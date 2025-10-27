#pragma once
#include <cstdint>
#include "core.h"
#pragma once

namespace PGRenderCore {

    /**
     * @brief Clase que representa un sampler, que define cómo se muestrea
     * una textura en shaders.
     */
    class Sampler {
    public:
        /**
         * @brief Modo de filtrado para la textura muestreada.
         */
        enum class FilterMode {
            Nearest,      ///< Selección del texel más cercano (pixelado).
            Linear,       ///< Interpolación lineal (suavizado).
            Anisotropic   ///< Filtrado anisotrópico para mejora de calidad en ángulos oblicuos.
        };

        /**
         * @brief Modo de dirección para coordenadas texture.
         */
        enum class AddressMode {
            Repeat,        ///< Coordenadas repetidas (tiled).
            ClampToEdge,   ///< Clamp a borde (último texel se extiende).
            MirroredRepeat,///< Repetición espejada.
            ClampToBorder  ///< Clamp a color borde (fuera de rango).
        };

        /**
         * @brief Descriptor para crear un Sampler.
         */
        struct Desc {
            FilterMode minFilter = FilterMode::Linear; ///< Filtrado para minificación.
            FilterMode magFilter = FilterMode::Linear; ///< Filtrado para ampliación.
            FilterMode mipFilter = FilterMode::Linear; ///< Filtrado entre niveles mip.
            AddressMode addressU = AddressMode::Repeat; ///< Dirección en eje U.
            AddressMode addressV = AddressMode::Repeat; ///< Dirección en eje V.
            AddressMode addressW = AddressMode::Repeat; ///< Dirección en eje W (3D).
            float maxAnisotropy = 1.0f;                 ///< Máximo grado de anisotropía (>=1).
            bool compareEnable = false;                  ///< Habilitar comparación para shadow maps.
            int compareFunc = 0;                         ///< Función comparación si está habilitada.
        };

        virtual ~Sampler() = default;

        /**
         * @brief Devuelve la descripción usada para crear el sampler.
         */
        virtual const Desc& getDesc() const = 0;

        /**
         * @brief Devuelve el identificador o handle nativo del sampler.
         *        (Ej. ID de OpenGL, descriptor Vulkan, etc.)
         */
        virtual uint64_t nativeHandle() const = 0;

		BACKEND_CHECKER
        CAST_HELPERS

    };

} // namespace PGRenderCore
