# cmake/Dependencies.cmake
include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# =============================================================================
# SDL3
# =============================================================================
if(PGRENDER_BUILD_SDL3_BACKEND)
    message(STATUS "Fetching SDL3 from official repository...")
    
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG main
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    
    FetchContent_MakeAvailable(SDL3)
    
    # Obtener el directorio source de SDL3
    FetchContent_GetProperties(SDL3 SOURCE_DIR SDL3_SOURCE_DIR)
    
    # Determinar qué target está disponible
    if(TARGET SDL3-static)
        set(SDL3_LIBRARY_TARGET SDL3-static CACHE INTERNAL "SDL3 library target")
        message(STATUS "SDL3 configured successfully (static library)")
    elseif(TARGET SDL3-shared)
        set(SDL3_LIBRARY_TARGET SDL3-shared CACHE INTERNAL "SDL3 library target")
        message(STATUS "SDL3 configured successfully (shared library)")
    else()
        message(FATAL_ERROR "SDL3 target not found")
    endif()
    
    # SDL3 usa include/SDL3/ para sus headers
    set(SDL3_INCLUDE_DIRECTORIES 
        "${SDL3_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/_deps/sdl3-build/include"
        "${CMAKE_BINARY_DIR}/_deps/sdl3-build/include-config-release"
        "${CMAKE_BINARY_DIR}/_deps/sdl3-build/include-config-debug"
        CACHE INTERNAL "SDL3 include directories"
    )
    
    message(STATUS "SDL3 source dir: ${SDL3_SOURCE_DIR}")
    message(STATUS "SDL3 includes: ${SDL3_INCLUDE_DIRECTORIES}")
endif()

# =============================================================================
# GLFW
# =============================================================================
if(PGRENDER_BUILD_GLFW_BACKEND)
    message(STATUS "Fetching GLFW from official repository...")
    
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    set(GLFW_VULKAN_STATIC OFF CACHE BOOL "" FORCE)
    
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    
    FetchContent_MakeAvailable(glfw)
    
    FetchContent_GetProperties(glfw SOURCE_DIR GLFW_SOURCE_DIR)
    
    if(TARGET glfw)
        set(GLFW_LIBRARY_TARGET glfw CACHE INTERNAL "GLFW library target")
        set(GLFW_INCLUDE_DIRECTORIES 
            "${GLFW_SOURCE_DIR}/include"
            CACHE INTERNAL "GLFW include directories"
        )
        
        message(STATUS "GLFW configured successfully")
        message(STATUS "GLFW source dir: ${GLFW_SOURCE_DIR}")
        message(STATUS "GLFW includes: ${GLFW_INCLUDE_DIRECTORIES}")
    endif()
endif()

# =============================================================================
# OpenGL (opcional, para ejemplos)
# =============================================================================
if(PGRENDER_BUILD_EXAMPLES OR PGRENDER_BUILD_TESTS)
    find_package(OpenGL)
    if(OpenGL_FOUND)
        message(STATUS "OpenGL found: ${OPENGL_LIBRARIES}")
    endif()
endif()

# =============================================================================
# Vulkan (opcional)
# =============================================================================
find_package(Vulkan QUIET)
if(Vulkan_FOUND)
    message(STATUS "Vulkan SDK found: ${Vulkan_VERSION}")
else()
    message(STATUS "Vulkan SDK not found (optional)")
endif()
