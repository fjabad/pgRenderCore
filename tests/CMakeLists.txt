# Descargar y configurar Google Test
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    GIT_SHALLOW TRUE
)

# Para Windows: Prevenir sobrescritura de flags del compilador padre
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Habilitar testing
enable_testing()

# Colectar TODOS los archivos de test
file(GLOB_RECURSE TEST_SOURCES_UNIT
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/*.cpp"
)

file(GLOB_RECURSE TEST_SOURCES_INTEGR
    "${CMAKE_CURRENT_SOURCE_DIR}/integration/*.cpp"
)

source_group("tests\\unit" 
    FILES ${TEST_SOURCES_UNIT}
)

source_group("tests\\integration" 
    FILES ${TEST_SOURCES_INTEGR}
)

# Vincular con backends si están disponibles
if(PGRENDER_BUILD_SDL3_BACKEND)
	add_executable(pgrender_tests_sdl3
		${TEST_SOURCES_UNIT} ${TEST_SOURCES_INTEGR}
		main.cpp
	)

	set_target_properties(pgrender_tests_sdl3 PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
		FOLDER "PGRenderCore/Tests"
	)

	# Vincular con todas las dependencias
	target_link_libraries(pgrender_tests_sdl3
		PRIVATE
			PGRenderCore::Core::App
			PGRenderCore::Factory
			PGRenderCore::SDL3
			GTest::gtest
			GTest::gtest_main
			GTest::gmock
	)
    target_compile_definitions(pgrender_tests_sdl3 PRIVATE PGRENDER_HAS_SDL3)
	# OpenGL si está disponible
	if(OpenGL_FOUND)
		target_link_libraries(pgrender_tests_sdl3 PRIVATE OpenGL::GL)
	endif()
	# Incluir directorios
	target_include_directories(pgrender_tests_sdl3
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}
	)

	sign_executable(pgrender_tests_sdl3)
endif()

if(PGRENDER_BUILD_GLFW_BACKEND)
	add_executable(pgrender_tests_glfw
		${TEST_SOURCES_UNIT} ${TEST_SOURCES_INTEGR}
		main.cpp
	)

	set_target_properties(pgrender_tests_glfw PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
		FOLDER "PGRenderCore/Tests"
	)

	# Vincular con todas las dependencias
	target_link_libraries(pgrender_tests_glfw
		PRIVATE
			PGRenderCore::Core::App
			PGRenderCore::Factory
			PGRenderCore::GLFW
			GTest::gtest
			GTest::gtest_main
			GTest::gmock
	)
    target_compile_definitions(pgrender_tests_glfw PRIVATE PGRENDER_HAS_GLFW)
	# OpenGL si está disponible
	if(OpenGL_FOUND)
		target_link_libraries(pgrender_tests_glfw PRIVATE OpenGL::GL)
	endif()
	# Incluir directorios
	target_include_directories(pgrender_tests_glfw
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}
	)

	sign_executable(pgrender_tests_glfw)

endif()



# Registrar con CTest (descubrimiento automático de tests)
include(GoogleTest)
gtest_discover_tests(pgrender_tests_sdl3
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
    PROPERTIES
        LABELS "PGRenderCore"
)

gtest_discover_tests(pgrender_tests_glfw
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
    PROPERTIES
        LABELS "PGRenderCore"
)


# También añadir como test simple de CTest
#add_test(NAME pgrender_all_tests 
#    COMMAND pgrender_tests
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
#)
