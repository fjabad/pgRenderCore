#pragma once
#include <cstdint>
#include "core.h"
#pragma once

namespace PGRenderCore {

    /**
     * @brief Clase que representa un sampler, que define c�mo se muestrea
     * una textura en shaders.
     */
    class Sampler {
    public:
        /**
         * @brief Modo de filtrado para la textura muestreada.
         */
        enum class FilterMode {
            Nearest,      ///< Selecci�n del texel m�s cercano (pixelado).
            Linear,       ///< Interpolaci�n lineal (suavizado).
            Anisotropic   ///< Filtrado anisotr�pico para mejora de calidad en �ngulos oblicuos.
        };

        /**
         * @brief Modo de direcci�n para coordenadas texture.
         */
        enum class AddressMode {
            Repeat,        ///< Coordenadas repetidas (tiled).
            ClampToEdge,   ///< Clamp a borde (�ltimo texel se extiende).
            MirroredRepeat,///< Repetici�n espejada.
            ClampToBorder  ///< Clamp a color borde (fuera de rango).
        };

        /**
         * @brief Descriptor para crear un Sampler.
         */
        struct Desc {
            FilterMode minFilter = FilterMode::Linear; ///< Filtrado para minificaci�n.
            FilterMode magFilter = FilterMode::Linear; ///< Filtrado para ampliaci�n.
            FilterMode mipFilter = FilterMode::Linear; ///< Filtrado entre niveles mip.
            AddressMode addressU = AddressMode::Repeat; ///< Direcci�n en eje U.
            AddressMode addressV = AddressMode::Repeat; ///< Direcci�n en eje V.
            AddressMode addressW = AddressMode::Repeat; ///< Direcci�n en eje W (3D).
            float maxAnisotropy = 1.0f;                 ///< M�ximo grado de anisotrop�a (>=1).
            bool compareEnable = false;                  ///< Habilitar comparaci�n para shadow maps.
            int compareFunc = 0;                         ///< Funci�n comparaci�n si est� habilitada.
        };

        virtual ~Sampler() = default;

        /**
         * @brief Devuelve la descripci�n usada para crear el sampler.
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
