# Colectar todos los headers
file(GLOB_RECURSE CORE_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Crear la librería INTERFACE
add_library(pgrender_core INTERFACE)
add_library(PGRenderCore::Core ALIAS pgrender_core)

target_include_directories(pgrender_core
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(pgrender_core
    INTERFACE
        cxx_std_20
)

target_compile_options(pgrender_core
    INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# SOLUCIÓN: Crear un target "dummy" para que aparezca en Visual Studio
if(MSVC OR CMAKE_GENERATOR MATCHES "Visual Studio")
    # Crear un custom target que depende de los headers
    add_custom_target(pgrender_core_ide
        SOURCES ${CORE_HEADERS}
    )
    
    # Organizar en carpetas de Visual Studio
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${CORE_HEADERS}
    )
    
    # Establecer como proyecto de inicio (opcional)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} 
        PROPERTY VS_STARTUP_PROJECT pgrender_core_ide
    )
    
    # Colocar en una carpeta específica en la solución
    set_target_properties(pgrender_core_ide PROPERTIES
        FOLDER "PGRenderCore/Core"
    )
endif()

# Instalar headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Instalar target
install(TARGETS pgrender_core
    EXPORT PGRenderCoreTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
