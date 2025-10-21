cmake_minimum_required(VERSION 3.25)

project(PGRenderCore
    VERSION 1.0.0
    DESCRIPTION "Modern C++ Graphics Rendering Abstraction Layer"
    LANGUAGES CXX
)

# Configuración C++ moderna
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opciones del proyecto
option(PGRENDER_BUILD_SDL3_BACKEND "Build SDL3 backend" ON)
option(PGRENDER_BUILD_GLFW_BACKEND "Build GLFW backend" ON)
option(PGRENDER_BUILD_EXAMPLES "Build example programs" ON)
option(PGRENDER_BUILD_TESTS "Build tests" OFF)
option(PGRENDER_BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Configuración de tipo de librería
if(PGRENDER_BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
else()
    set(BUILD_SHARED_LIBS OFF)
endif()

# Incluir módulo de dependencias
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Dependencies)

# Configuración de instalación
include(GNUInstallDirs)

# Librería core 
add_subdirectory(core)

# Función para firmar un ejecutable en macOS
function(sign_executable target_name)
    if(APPLE)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND /usr/bin/codesign --force --deep --sign - $<TARGET_FILE:${target_name}>
            COMMENT "Signing ${target_name} for macOS"
        )
    endif()
endfunction()

# Backends
if(PGRENDER_BUILD_SDL3_BACKEND)
    add_subdirectory(backends/sdl3)
endif()

if(PGRENDER_BUILD_GLFW_BACKEND)
    add_subdirectory(backends/glfw)
endif()

add_subdirectory(factory)

# Ejemplos
if(PGRENDER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
if(PGRENDER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Configuración de export para otros proyectos
install(EXPORT PGRenderCoreTargets
    FILE PGRenderCoreTargets.cmake
    NAMESPACE PGRenderCore::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PGRenderCore
)

# Generar archivo de configuración
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/PGRenderCoreConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/PGRenderCoreConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PGRenderCore
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/PGRenderCoreConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/PGRenderCoreConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/PGRenderCoreConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PGRenderCore
)
