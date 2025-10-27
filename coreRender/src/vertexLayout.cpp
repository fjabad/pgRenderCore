#include "PGRenderCore/vertexLayout.h"
#include <stdexcept>

namespace PGRenderCore {

    void VertexLayout::addAttribute(const VertexAttribute& attribute) {
        m_attributes.push_back(attribute);
    }

    void VertexLayout::addBufferBinding(const VertexBufferBinding& bufferBinding) {
        m_bufferBindings.push_back(bufferBinding);
    }

    size_t VertexLayout::getSizeOfAttributeType(VertexAttributeType type) {
        switch (type) {
        case VertexAttributeType::Float: return 4;
        case VertexAttributeType::Float2: return 8;
        case VertexAttributeType::Float3: return 12;
        case VertexAttributeType::Float4: return 16;
        case VertexAttributeType::Int: return 4;
        case VertexAttributeType::Int2: return 8;
        case VertexAttributeType::Int3: return 12;
        case VertexAttributeType::Int4: return 16;
        case VertexAttributeType::UInt: return 4;
        case VertexAttributeType::UInt2: return 8;
        case VertexAttributeType::UInt3: return 12;
        case VertexAttributeType::UInt4: return 16;
        case VertexAttributeType::Byte4: return 4;
        case VertexAttributeType::UByte4: return 4;
        case VertexAttributeType::Short2: return 4;
        case VertexAttributeType::Short4: return 8;
        case VertexAttributeType::UShort2: return 4;
        case VertexAttributeType::UShort4: return 8;
        case VertexAttributeType::Half2: return 4;
        case VertexAttributeType::Half4: return 8;
        default:
            throw std::runtime_error("Unknown vertex attribute type");
        }
    }

    uint32_t VertexLayout::getComponentCount(VertexAttributeType type) {
        switch (type) {
        case VertexAttributeType::Float:
        case VertexAttributeType::Int:
        case VertexAttributeType::UInt:
            return 1;
        case VertexAttributeType::Float2:
        case VertexAttributeType::Int2:
        case VertexAttributeType::UInt2:
        case VertexAttributeType::Short2:
        case VertexAttributeType::UShort2:
        case VertexAttributeType::Half2:
            return 2;
        case VertexAttributeType::Float3:
        case VertexAttributeType::Int3:
        case VertexAttributeType::UInt3:
            return 3;
        case VertexAttributeType::Float4:
        case VertexAttributeType::Int4:
        case VertexAttributeType::UInt4:
        case VertexAttributeType::Byte4:
        case VertexAttributeType::UByte4:
        case VertexAttributeType::Short4:
        case VertexAttributeType::UShort4:
        case VertexAttributeType::Half4:
            return 4;
        default:
            throw std::runtime_error("Unknown vertex attribute type");
        }
    }

    void VertexLayout::clear() {
        m_attributes.clear();
        m_bufferBindings.clear();
    }

    // ===== BUILDER =====

    VertexLayoutBuilder& VertexLayoutBuilder::addAttribute(uint32_t location,
        VertexAttributeType type,
        uint32_t binding,
        uint32_t offset,
        bool normalized) {
        VertexAttribute attr;
        attr.location = location;
        attr.type = type;
        attr.binding = binding;
        attr.offset = offset;
        attr.normalized = normalized;
        m_layout.addAttribute(attr);
        return *this;
    }

    VertexLayoutBuilder& VertexLayoutBuilder::addBufferBinding(uint32_t binding,
        uint32_t stride,
        bool instanceRate,
        uint32_t divisor) {
        VertexBufferBinding bufferBinding;
        bufferBinding.binding = binding;
        bufferBinding.stride = stride;
        bufferBinding.instanceRate = instanceRate;
        bufferBinding.divisor = divisor;
        m_layout.addBufferBinding(bufferBinding);
        return *this;
    }

    VertexLayout VertexLayoutBuilder::build() {
        return m_layout;
    }

} // namespace PGRenderCore
