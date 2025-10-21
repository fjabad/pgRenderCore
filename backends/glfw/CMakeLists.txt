# backends/glfw/CMakeLists.txt

# Verificar que GLFW esté disponible
if(NOT DEFINED GLFW_LIBRARY_TARGET)
    message(FATAL_ERROR "GLFW not configured. Please enable PGRENDER_BUILD_GLFW_BACKEND")
endif()

if(NOT DEFINED GLFW_INCLUDE_DIRECTORIES)
    message(FATAL_ERROR "GLFW_INCLUDE_DIRECTORIES not set")
endif()

# Colectar archivos fuente
file(GLOB_RECURSE GLFW_BACKEND_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE GLFW_BACKEND_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Crear librería
add_library(pgrender_glfw ${GLFW_BACKEND_SOURCES} ${GLFW_BACKEND_HEADERS})
add_library(PGRenderCore::GLFW ALIAS pgrender_glfw)

# Configurar propiedades
set_target_properties(pgrender_glfw PROPERTIES
    OUTPUT_NAME "pgrender-glfw"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
	FOLDER PGRenderCore/Backends
)

# Directorios de includes
target_include_directories(pgrender_glfw
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependencias
target_link_libraries(pgrender_glfw
    PUBLIC
        PGRenderCore::Core
        glfw
    PRIVATE
        $<$<PLATFORM_ID:Linux>:${CMAKE_DL_LIBS}>
)

# Definir macro para identificar backend
target_compile_definitions(pgrender_glfw
    PUBLIC
        PGRENDER_USE_GLFW
    PRIVATE
        PGRENDER_GLFW_IMPLEMENTATION
)

# Opciones de compilación
target_compile_options(pgrender_glfw
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Soporte para OpenGL
if(OpenGL_FOUND)
    target_link_libraries(pgrender_glfw PUBLIC OpenGL::GL)
    target_compile_definitions(pgrender_glfw PUBLIC GFX_OPENGL_SUPPORT)
endif()

# Soporte para Vulkan (opcional)
if(Vulkan_FOUND)
    target_link_libraries(pgrender_glfw PUBLIC Vulkan::Vulkan)
    target_compile_definitions(pgrender_glfw PUBLIC GFX_VULKAN_SUPPORT)
endif()

# Instalar librería y headers
install(TARGETS pgrender_glfw
    EXPORT GraphicsAbstractionTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)
