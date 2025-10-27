#pragma once
#include "backendType.h"
#include "shader.h"
#include "stateConstants.h"
#include <memory>
#include <cstdint>
#include <glm/vec4.hpp>

namespace PGRenderCore {


    /**
     * @brief Descriptor para modo de polígono por cara.
     */
    struct PolygonModeDesc {
        PolygonMode mode = PolygonMode::Fill; // same for front and back faces
    };

    /**
     * @brief Configuración personalizada de blending.
     */
    struct BlendStateDesc {
        bool enabled = false;
        BlendFactor srcColorFactor = BlendFactor::One;
        BlendFactor dstColorFactor = BlendFactor::Zero;
        BlendOp colorOp = BlendOp::Add;
        BlendFactor srcAlphaFactor = BlendFactor::One;
        BlendFactor dstAlphaFactor = BlendFactor::Zero;
        BlendOp alphaOp = BlendOp::Add;
		glm::vec4 constantColor = glm::vec4(0.0f);
    };

    /**
	* @brief Configuración de profundidad.
    */
    struct DepthStateDesc {
        DepthFunc depthFunc = DepthFunc::Less;
        bool depthTestEnabled = true;
        bool depthWriteEnabled = true;
    };

    /**
     * @brief Interfaz abstracta para pipeline de renderizado.
     */
    class Pipeline {
    public:
        virtual ~Pipeline() = default;

        /**
         * @brief Descriptor de pipeline.
         */
        struct Desc {
            std::shared_ptr<Program> program = nullptr;

            // Estado de rasterización
            BlendStateDesc customBlendState;

			DepthStateDesc depthState;

            CullMode cullMode = CullMode::Back;
            bool frontFaceCCW = true;  // Counter-clockwise = front face

            PolygonModeDesc polygonMode;

            float lineWidth = 1.0f;
            float pointSize = 1.0f;

            // Otros estados
            bool scissorTestEnabled = false;
            bool stencilTestEnabled = false;

            const char* debugName = nullptr;
        };


        /**
         * @brief Devuelve el descriptor del pipeline.
         */
        virtual const Pipeline::Desc& getDesc() const = 0;

        BACKEND_CHECKER
        CAST_HELPERS

    };

} // namespace PGRenderCore
