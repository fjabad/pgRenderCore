#pragma once
#include <string>
#include <vector>

#include "core.h"

namespace pgrender {

    /**
     * @brief Enum que representa las etapas (stages) de shader soportadas,
     * incluyendo etapas para trazado de rayos por hardware.
     */
    enum class ShaderStage {
        Vertex,
        Fragment,
        Geometry,
        TessControl,
        TessEvaluation,
        Compute,
        // Etapas específicas para trazado de rayos por hardware:
        RayGeneration,
        ClosestHit,
        AnyHit,
        Miss,
        Intersection,
        Callable
    };

    /**
     * @brief Descriptor que define los shaders en sus distintas etapas.
     */
    struct ShaderSource {
        ShaderStage stage;
        std::string source;  ///< Código fuente GLSL u otro lenguaje compatible.
    };

    /**
     * @brief Interfaz abstracta para shaders que soportan múltiples etapas,
     * tanto gráficos tradicionales como trazado de rayos.
     */
    class Program {
    public:
        virtual ~Program() = default;

        struct Desc {
            /**
             * @brief Lista de etapas del shader con su código fuente.
             */
            std::vector<ShaderSource> stages;
            const char* debugName = nullptr; ///< Nombre para depuración (opcional).
        };

        /**
         * @brief Compila y enlaza el shader con las etapas indicadas en el descriptor.
         * @return true si la compilación y enlace fueron exitosos, false en caso contrario.
         */
        virtual bool compile() = 0;

        /**
         * @brief Libera los recursos internos del shader.
         */
        virtual void release() = 0;

        /**
         * @brief Devuelve el descriptor con las etapas y fuentes usadas.
         */
        virtual const Desc& getDesc() const = 0;

        /**
         * @brief Devuelve un identificador nativo o handle específico del backend.
         */
        virtual unsigned long nativeHandle() const = 0;

        BACKEND_CHECKER
        CAST_HELPERS

    };

} // namespace pgrender
