# Función helper para crear ejemplos
function(add_pgrender_example name)
    cmake_parse_arguments(EXAMPLE "" "BACKEND" "SOURCES" ${ARGN})
    
    add_executable(${name} ${EXAMPLE_SOURCES})
    
    set_target_properties(${name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples"
    )
    
    # Enlazar con el backend apropiado
    if(EXAMPLE_BACKEND STREQUAL "SDL3")
        target_link_libraries(${name} PRIVATE PGRenderCore::SDL3)
    elseif(EXAMPLE_BACKEND STREQUAL "GLFW")
        target_link_libraries(${name} PRIVATE PGRenderCore::GLFW)
    elseif(EXAMPLE_BACKEND STREQUAL "BOTH")
        target_link_libraries(${name} PRIVATE PGRenderCore::SDL3 PGRenderCore::GLFW)
    endif()
    
    # OpenGL si está disponible
    if(OpenGL_FOUND)
        target_link_libraries(${name} PRIVATE OpenGL::GL)
    endif()

    sign_executable(${name})
endfunction()

# Ejemplos con SDL3
if(PGRENDER_BUILD_SDL3_BACKEND)
    add_pgrender_example(example_basic_window_sdl3
        BACKEND SDL3
        SOURCES basic_window.cpp
    )
    
    add_pgrender_example(example_shared_contexts_sdl3
        BACKEND SDL3
        SOURCES shared_contexts.cpp
    )
    
    add_pgrender_example(example_multi_window_sdl3
        BACKEND SDL3
        SOURCES multi_window.cpp
    )
    
    add_pgrender_example(example_per_window_events_sdl3
        BACKEND SDL3
        SOURCES per_window_events.cpp
    )    
	
    add_pgrender_example(example_simple
        BACKEND SDL3
        SOURCES simple.cpp
    )
endif()

# Ejemplos con GLFW
if(PGRENDER_BUILD_GLFW_BACKEND)
    add_pgrender_example(example_basic_window_glfw
        BACKEND GLFW
        SOURCES basic_window.cpp
    )
    
    add_pgrender_example(example_shared_contexts_glfw
        BACKEND GLFW
        SOURCES shared_contexts.cpp
    )
endif()

# Ejemplo que usa ambos backends (factory)
if(PGRENDER_BUILD_SDL3_BACKEND AND PGRENDER_BUILD_GLFW_BACKEND)
    add_pgrender_example(example_backend_selection
        BACKEND BOTH
        SOURCES backend_selection.cpp
    )
endif()

add_pgrender_example(example_show_window_opts
	BACKEND SDL3
	SOURCES show_window_opts.cpp
)