# Colectar todos los headers
file(GLOB_RECURSE CORE_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)


file(GLOB_RECURSE CORE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)


# Crear la librería 
add_library(pgrender_core_app ${CORE_HEADERS} ${CORE_SOURCES})
add_library(PGRenderCore::Core::App ALIAS pgrender_core_app)

# Configurar propiedades
set_target_properties(pgrender_core_app PROPERTIES
    OUTPUT_NAME "pgrender-core"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
	FOLDER PGRenderCore/Core
)


target_include_directories(pgrender_core_app
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(pgrender_core_app
    INTERFACE
        cxx_std_20
)

target_compile_options(pgrender_core_app
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
install(TARGETS pgrender_core_app
    EXPORT PGRenderCoreTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
