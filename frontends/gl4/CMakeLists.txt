# Opciones para detectar OpenGL e incluir directorios
find_package(OpenGL REQUIRED)

# Añadir directorios de código fuente
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Archivos fuente y cabeceras de PGRenderCoreGL
file(GLOB_RECURSE PGRENDERCOREGL_SOURCES
    ${SRC_DIR}/*.cpp
)

file(GLOB_RECURSE PGRENDERCOREGL_HEADERS
    ${INC_DIR}/PGRenderCoreGL/*.h
)

add_library(pgrender_gl4 ${PGRENDERCOREGL_SOURCES} ${PGRENDERCOREGL_HEADERS})
add_library(PGRenderCore::GL4 ALIAS pgrender_gl4)

# Configurar propiedades
set_target_properties(pgrender_gl4 PROPERTIES
    OUTPUT_NAME "pgrender-gl4"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
	FOLDER PGRenderCore/Frontends
)

# Incluir carpetas públicas (API y backend)
target_include_directories(pgrender_gl4
    PUBLIC
        $<BUILD_INTERFACE:${INC_DIR}>
        $<INSTALL_INTERFACE:include>
)

include(FetchContent)

# Forzar la política para respetar las variables normales
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

# Forzar el valor desde el cache
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "Build shared variant of GLEW" FORCE)
set (ONLY_LIBS ON CACHE BOOL "Do no build executables" FORCE)

FetchContent_Declare(
    GLEW
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG glew-cmake-2.2.0
)
FetchContent_MakeAvailable(GLEW)

# Vincular con OpenGL
target_link_libraries(pgrender_gl4
    PUBLIC
        PGRenderCore::Core::App  PGRenderCore::Core::Render
    PRIVATE
        libglew_static OpenGL::GL
)

