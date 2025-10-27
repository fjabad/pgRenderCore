# Función helper para crear ejemplos
function(add_pgrender_example name)
    cmake_parse_arguments(EXAMPLE "" "BACKEND" "FRONTEND;SOURCES" ${ARGN})
    
    add_executable(${name} ${EXAMPLE_SOURCES})
    
    set_target_properties(${name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples"
        FOLDER "PGRenderCore/Examples"
    )
    
	 # Enlazar con el backend apropiado
    if(EXAMPLE_BACKEND STREQUAL "SDL3")
        target_link_libraries(${name} PRIVATE PGRenderCore::SDL3)
    elseif(EXAMPLE_BACKEND STREQUAL "GLFW")
        target_link_libraries(${name} PRIVATE PGRenderCore::GLFW)
    endif()

    # Enlazar con el frontend apropiado
    if(EXAMPLE_FRONTEND STREQUAL "GL4")
		target_link_libraries(${name} PRIVATE PGRenderCore::GL4)
    endif()

	target_link_libraries(${name} PRIVATE PGRenderCore::Factory)
   
    sign_executable(${name})
endfunction()


file(GLOB example_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")


message (STATUS "Creando los ejemplos...")
foreach(example IN LISTS example_files)
	get_filename_component(base_name "${example}" NAME_WE)
	message(STATUS "Procesando archivo de ejemplo: ${base_name}")

	if(PGRENDER_BUILD_SDL3_BACKEND)
	 message(STATUS "Procesando : ${base_name}_sdl3_gl4")
	    add_pgrender_example("${base_name}_sdl3_gl4"
	        BACKEND SDL3
            FRONTEND GL4
			SOURCES ${example}
		)
	endif()
	if(PGRENDER_BUILD_GLFW_BACKEND)
	 message(STATUS "Procesando : ${base_name}_glfw_gl4")
		add_pgrender_example("${base_name}_glfw_gl4"
			BACKEND GLFW
            FRONTEND GL4
			SOURCES ${example}
		)
	endif()

endforeach()
