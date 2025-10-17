# backends/glfw/CMakeLists.txt
# Colectar archivos fuente
file(GLOB_RECURSE GLFW_BACKEND_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE GLFW_BACKEND_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

# Crear librería
add_library(gfx_glfw ${GLFW_BACKEND_SOURCES} ${GLFW_BACKEND_HEADERS})
add_library(GFX::GLFW ALIAS gfx_glfw)

# Configurar propiedades
set_target_properties(gfx_glfw PROPERTIES
    OUTPUT_NAME "gfx-glfw"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Directorios de includes
target_include_directories(gfx_glfw
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependencias
target_link_libraries(gfx_glfw
    PUBLIC
        GFX::Core
        glfw
    PRIVATE
        $<$<PLATFORM_ID:Linux>:${CMAKE_DL_LIBS}>
)

# Definir macro para identificar backend
target_compile_definitions(gfx_glfw
    PUBLIC
        GFX_USE_GLFW
    PRIVATE
        GFX_GLFW_IMPLEMENTATION
)

# Opciones de compilación
target_compile_options(gfx_glfw
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Soporte para OpenGL
if(OpenGL_FOUND)
    target_link_libraries(gfx_glfw PUBLIC OpenGL::GL)
    target_compile_definitions(gfx_glfw PUBLIC GFX_OPENGL_SUPPORT)
endif()

# Soporte para Vulkan (opcional)
if(Vulkan_FOUND)
    target_link_libraries(gfx_glfw PUBLIC Vulkan::Vulkan)
    target_compile_definitions(gfx_glfw PUBLIC GFX_VULKAN_SUPPORT)
endif()

# Instalar librería y headers
install(TARGETS gfx_glfw
    EXPORT GraphicsAbstractionTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
