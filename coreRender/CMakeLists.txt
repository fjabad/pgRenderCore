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
add_library(pgrender_core_render ${CORE_HEADERS} ${CORE_SOURCES})
add_library(PGRenderCore::Core::Render ALIAS pgrender_core_render)

# Configurar propiedades
set_target_properties(pgrender_core_render PROPERTIES
    OUTPUT_NAME "pgrender-core"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
	FOLDER PGRenderCore/Core
)


target_include_directories(pgrender_core_render
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(pgrender_core_render
    INTERFACE
        cxx_std_20
)

target_compile_options(pgrender_core_render
    INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)


include(FetchContent)

# Declarar la dependencia GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.2    # o la versión más estable que prefieras
)

# Descargar y hacer disponible la dependencia
FetchContent_MakeAvailable(glm)

target_link_libraries(pgrender_core_render
	PUBLIC
		glm::glm
)

# Instalar headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Instalar target
install(TARGETS pgrender_core_render glm glm-header-only
    EXPORT PGRenderCoreTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
