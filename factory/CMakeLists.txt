# core/CMakeLists.txt
add_library(pgrender_factory INTERFACE)
add_library(PGRenderCore::Factory ALIAS pgrender_factory)

# Headers públicos
target_include_directories(pgrender_factory
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Requerimientos de C++
target_compile_features(pgrender_factory
    INTERFACE
        cxx_std_20
)

# Opciones de compilación recomendadas
target_compile_options(pgrender_factory
    INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Instalar headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Instalar target
install(TARGETS pgrender_factory
    EXPORT GraphicsAbstractionTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
