# Colectar todos los headers
file(GLOB_RECURSE FACTORY_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)


file(GLOB_RECURSE FACTORY_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)


# Crear la librería 
add_library(pgrender_factory ${FACTORY_HEADERS} ${FACTORY_SOURCES})
add_library(PGRenderCore::Factory ALIAS pgrender_factory)

# Configurar propiedades
set_target_properties(pgrender_factory PROPERTIES
    OUTPUT_NAME "pgrender-factory"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
	FOLDER PGRenderCore/Factory
)


target_link_libraries(pgrender_factory PRIVATE PGRenderCore::Core PGRenderCore::GLFW PGRenderCore::SDL3)

target_include_directories(pgrender_factory
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(pgrender_factory
    INTERFACE
        cxx_std_20
)

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
    EXPORT PGRenderCoreTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)