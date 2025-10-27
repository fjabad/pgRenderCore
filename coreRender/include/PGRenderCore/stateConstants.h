#pragma once

namespace PGRenderCore {


    /**
     * @brief Factores de blending personalizados.
     */
    enum class BlendFactor {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
		OneMinusConstantAlpha
    };

    /**
     * @brief Operaciones de blending.
     */
    enum class BlendOp {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };


    /**
     * @brief Funciones de comparaci�n usadas en test de profundidad (depth test).
     */
    enum class DepthFunc {
        Never,          // Nunca pasa (descarta todo)
        Less,           // Pasa si nuevo valor menor que almacenado (default t�pico)
        LessEqual,      // Pasa si nuevo valor menor o igual
        Greater,        // Pasa si nuevo valor mayor que almacenado
        GreaterEqual,   // Pasa si nuevo valor mayor o igual
        Equal,          // Igualdad exacta
        NotEqual,       // Diferente
        Always          // Siempre pasa (sin test)
    };

    /**
     * @brief Modos de culling para descartar pol�gonos seg�n orientaci�n.
     */
    enum class CullMode {
        None,           // No se descarta nada
        Front,          // Culling caras frontales
        Back,           // Culling caras traseras (default t�pico)
		FrontAndBack    // Culling ambas caras
    };

    /**
     * @brief Referencia a las caras de un pol�gono.
     */
    enum class PolygonFace {
        Front,          // Cara frontal
        Back,           // Cara traseras
        FrontAndBack    // Ambas caras
    };

    /**
     * @brief Modos de dibujado de los pol�gonos.
     */
    enum class PolygonMode {
        Fill,           // Relleno
        Line,           // S�lo aristas
        Point           // S�lo v�rtices
    };

} // namespace PGRenderCore
