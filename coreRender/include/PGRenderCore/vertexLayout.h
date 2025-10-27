#pragma once
#include <vector>
#include <cstdint>
#include <string>

namespace PGRenderCore {

    /**
     * @brief Tipos de datos de atributos de vértice.
     */
    enum class VertexAttributeType {
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Int2,
        Int3,
        Int4,
        UInt,
        UInt2,
        UInt3,
        UInt4,
        Byte4,
        UByte4,
        Short2,
        Short4,
        UShort2,
        UShort4,
        Half2,
        Half4
    };

    /**
     * @brief Describe un atributo individual de vértice.
     */
    struct VertexAttribute {
        uint32_t location;                  ///< Location en el shader (layout(location = X))
        VertexAttributeType type;           ///< Tipo de dato del atributo
        uint32_t offset;                    ///< Offset en bytes desde el inicio del vértice
        uint32_t binding;                   ///< Binding point del buffer que contiene este atributo
        bool normalized = false;            ///< Si debe normalizarse (ej: [0,255] -> [0,1])

        std::string semanticName;           ///< Nombre semántico (opcional, para DX12)
        uint32_t semanticIndex = 0;         ///< Índice semántico (opcional, para DX12)
    };

    /**
     * @brief Describe cómo leer datos de un buffer de vértices específico.
     */
    struct VertexBufferBinding {
        uint32_t binding;                   ///< Binding point del buffer
        uint32_t stride;                    ///< Stride en bytes entre vértices consecutivos
        bool instanceRate = false;          ///< true = per-instance, false = per-vertex
        uint32_t divisor = 1;               ///< Para instancing: incremento cada N instancias
    };

    /**
     * @brief Layout completo de vértices con atributos y bindings.
     */
    class VertexLayout {
    public:
        VertexLayout() = default;

        /**
         * @brief Añade un atributo de vértice.
         */
        void addAttribute(const VertexAttribute& attribute);

        /**
         * @brief Añade un buffer binding.
         */
        void addBufferBinding(const VertexBufferBinding& bufferBinding);

        /**
         * @brief Obtiene todos los atributos.
         */
        const std::vector<VertexAttribute>& getAttributes() const { return m_attributes; }

        /**
         * @brief Obtiene todos los buffer bindings.
         */
        const std::vector<VertexBufferBinding>& getBufferBindings() const { return m_bufferBindings; }

        /**
         * @brief Calcula el tamaño en bytes de un tipo de atributo.
         */
        static size_t getSizeOfAttributeType(VertexAttributeType type);

        /**
         * @brief Devuelve el número de componentes de un tipo de atributo.
         */
        static uint32_t getComponentCount(VertexAttributeType type);

        /**
         * @brief Limpia todos los atributos y bindings.
         */
        void clear();
    private:
        std::vector<VertexAttribute> m_attributes;
        std::vector<VertexBufferBinding> m_bufferBindings;
    };

    /**
     * @brief Builder helper para construir layouts de forma fluida.
     */
    class VertexLayoutBuilder {
    public:
        VertexLayoutBuilder() = default;

        /**
         * @brief Añade un atributo.
         */
        VertexLayoutBuilder& addAttribute(uint32_t location,
            VertexAttributeType type,
            uint32_t binding = 0,
            uint32_t offset = 0,
            bool normalized = false);

        /**
         * @brief Añade un buffer binding.
         */
        VertexLayoutBuilder& addBufferBinding(uint32_t binding,
            uint32_t stride,
            bool instanceRate = false,
            uint32_t divisor = 1);

        /**
         * @brief Construye y devuelve el layout.
         */
        VertexLayout build();

    private:
        VertexLayout m_layout;
    };

} // namespace PGRenderCore
