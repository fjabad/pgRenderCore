#pragma once
#include "backendType.h"
#include "bufferObject.h"
#include <memory>
#include <vector>
#include <cstdint>

namespace pgrender {

    /**
     * @brief Tipo de geometría para ray tracing.
     */
    enum class RayTracingGeometryType {
        Triangles,
        AABBs,      // Axis-Aligned Bounding Boxes (para procedural)
        Instances
    };

    /**
     * @brief Flags de geometría.
     */
    enum class RayTracingGeometryFlags : uint32_t {
        None = 0,
        Opaque = 1 << 0,                    // Geometría opaca (no ejecuta any-hit shader)
        NoDuplicateAnyHitInvocation = 1 << 1 // Evita múltiples invocaciones any-hit
    };

    inline RayTracingGeometryFlags operator|(RayTracingGeometryFlags a, RayTracingGeometryFlags b) {
        return static_cast<RayTracingGeometryFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    /**
     * @brief Descriptor de geometría de triángulos.
     */
    struct RayTracingTriangleGeometry {
        std::shared_ptr<BufferObject> vertexBuffer;
        uint32_t vertexOffset = 0;
        uint32_t vertexCount = 0;
        uint32_t vertexStride = 0;          // Stride entre vértices

        std::shared_ptr<BufferObject> indexBuffer = nullptr;
        uint32_t indexOffset = 0;
        uint32_t indexCount = 0;
        bool use16BitIndices = false;       // true = uint16_t, false = uint32_t

        std::shared_ptr<BufferObject> transformBuffer = nullptr; // Matriz 3x4 opcional
        uint32_t transformOffset = 0;
    };

    /**
     * @brief Descriptor de geometría AABB (procedural).
     */
    struct RayTracingAABBGeometry {
        std::shared_ptr<BufferObject> aabbBuffer;
        uint32_t aabbOffset = 0;
        uint32_t aabbCount = 0;
        uint32_t aabbStride = 24;           // 6 floats (min.xyz, max.xyz)
    };

    /**
     * @brief Descriptor de geometría para BLAS.
     */
    struct RayTracingGeometryDesc {
        RayTracingGeometryType type = RayTracingGeometryType::Triangles;
        RayTracingGeometryFlags flags = RayTracingGeometryFlags::Opaque;

        // Union de tipos de geometría
        RayTracingTriangleGeometry triangles;
        RayTracingAABBGeometry aabbs;
    };

    /**
     * @brief Flags de construcción de acceleration structure.
     */
    enum class AccelerationStructureBuildFlags : uint32_t {
        None = 0,
        AllowUpdate = 1 << 0,               // Permite actualizaciones posteriores
        AllowCompaction = 1 << 1,           // Permite compactación
        PreferFastTrace = 1 << 2,           // Optimizar para trazado rápido
        PreferFastBuild = 1 << 3,           // Optimizar para construcción rápida
        MinimizeMemory = 1 << 4             // Minimizar uso de memoria
    };

    inline AccelerationStructureBuildFlags operator|(AccelerationStructureBuildFlags a, AccelerationStructureBuildFlags b) {
        return static_cast<AccelerationStructureBuildFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    /**
     * @brief Descriptor para Bottom-Level Acceleration Structure (BLAS).
     */
    struct BLASDesc {
        std::vector<RayTracingGeometryDesc> geometries;
        AccelerationStructureBuildFlags buildFlags = AccelerationStructureBuildFlags::PreferFastTrace;
        const char* debugName = nullptr;
    };

    /**
     * @brief Descriptor de instancia para TLAS.
     */
    struct RayTracingInstance {
        std::shared_ptr<class AccelerationStructure> blas; // BLAS referenciado
        float transform[12];                // Matriz 3x4 (row-major)
        uint32_t instanceID = 0;            // ID personalizado accesible en shaders
        uint32_t instanceMask = 0xFF;       // Máscara de visibilidad (8 bits)
        uint32_t instanceShaderBindingTableOffset = 0; // Offset en SBT
        uint32_t flags = 0;                 // Flags de instancia (ej: cull disable)
    };

    /**
     * @brief Descriptor para Top-Level Acceleration Structure (TLAS).
     */
    struct TLASDesc {
        std::vector<RayTracingInstance> instances;
        AccelerationStructureBuildFlags buildFlags = AccelerationStructureBuildFlags::PreferFastTrace;
        const char* debugName = nullptr;
    };

    /**
     * @brief Acceleration Structure (BLAS o TLAS).
     */
    class AccelerationStructure {
    public:
        virtual ~AccelerationStructure() = default;

        virtual uint64_t nativeHandle() const = 0;

        /**
         * @brief Devuelve el tamaño en bytes de la estructura.
         */
        virtual size_t getSize() const = 0;

        /**
         * @brief Actualiza la acceleration structure (si se construyó con AllowUpdate).
         */
        virtual void update() = 0;

        BACKEND_CHECKER
        CAST_HELPERS;
    };

} // namespace pgrender
