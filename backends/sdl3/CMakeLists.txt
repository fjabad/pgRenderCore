# backends/sdl3/CMakeLists.txt

# Verificar que SDL3 esté disponible
if(NOT DEFINED SDL3_LIBRARY_TARGET)
    message(FATAL_ERROR "SDL3 not configured. Please enable PGRENDER_BUILD_SDL3_BACKEND")
endif()

if(NOT DEFINED SDL3_INCLUDE_DIRECTORIES)
    message(FATAL_ERROR "SDL3_INCLUDE_DIRECTORIES not set")
endif()

# Colectar archivos fuente
file(GLOB_RECURSE SDL3_BACKEND_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE SDL3_BACKEND_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Crear librería
add_library(pgrender_sdl3 ${SDL3_BACKEND_SOURCES} ${SDL3_BACKEND_HEADERS})
add_library(PGRenderCore::SDL3 ALIAS pgrender_sdl3)

# Configurar propiedades
set_target_properties(pgrender_sdl3 PROPERTIES
    OUTPUT_NAME "pgrender-sdl3"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Filtrar directorios que no existen
set(SDL3_VALID_INCLUDES)
foreach(dir ${SDL3_INCLUDE_DIRECTORIES})
    if(EXISTS ${dir})
        list(APPEND SDL3_VALID_INCLUDES ${dir})
    endif()
endforeach()

# Mostrar directorios válidos
message(STATUS "SDL3 valid include directories for pgrender_sdl3:")
foreach(dir ${SDL3_VALID_INCLUDES})
    message(STATUS "  - ${dir}")
endforeach()

# Directorios de includes
target_include_directories(pgrender_sdl3
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${SDL3_VALID_INCLUDES}
)

# Vincular con SDL3 usando generator expressions
target_link_libraries(pgrender_sdl3
    PUBLIC
        PGRenderCore::Core
    PRIVATE
        $<BUILD_INTERFACE:${SDL3_LIBRARY_TARGET}>
        $<$<PLATFORM_ID:Linux>:${CMAKE_DL_LIBS}>
        $<$<PLATFORM_ID:Linux>:m>
        $<$<PLATFORM_ID:Windows>:winmm>
        $<$<PLATFORM_ID:Windows>:imm32>
        $<$<PLATFORM_ID:Windows>:version>
        $<$<PLATFORM_ID:Windows>:setupapi>
)

# Definir macros
target_compile_definitions(pgrender_sdl3
    PUBLIC
        PGRENDER_USE_SDL3
    PRIVATE
        PGRENDER_SDL3_IMPLEMENTATION
)

# Opciones de compilación
target_compile_options(pgrender_sdl3
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Soporte para Vulkan (opcional)
if(Vulkan_FOUND)
    target_link_libraries(pgrender_sdl3 PRIVATE Vulkan::Vulkan)
    target_compile_definitions(pgrender_sdl3 PUBLIC PGRENDER_VULKAN_SUPPORT)
endif()

# Instalar librería y headers
install(TARGETS pgrender_sdl3
    EXPORT PGRenderCoreTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Instalar SDL3 junto con pgrender_sdl3
if(TARGET ${SDL3_LIBRARY_TARGET})
    # Copiar la librería SDL3
    install(TARGETS ${SDL3_LIBRARY_TARGET}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    
    # Copiar headers de SDL3
    foreach(include_dir ${SDL3_VALID_INCLUDES})
        if(EXISTS ${include_dir}/SDL3)
            install(DIRECTORY ${include_dir}/SDL3
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
                FILES_MATCHING PATTERN "*.h"
            )
        endif()
    endforeach()
endif()
