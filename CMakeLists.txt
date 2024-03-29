cmake_minimum_required(VERSION 3.1)

project(shadertoy-connector)

include(GNUInstallDirs)

# Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/modules)

# GL libraries
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(Boost 1.54 REQUIRED COMPONENTS filesystem date_time program_options system)
find_package(CURL REQUIRED)

find_package(GLFW3 REQUIRED)

find_package(jsoncpp REQUIRED)

find_package(Epoxy REQUIRED)

if (WIN32)
	find_package(ZeroMQ)

	if (ZeroMQ_FOUND)
		set(ZMQ_LIBRARIES libzmq-static ws2_32 iphlpapi)
	endif()
else()
	pkg_search_module(ZMQ libzmq)
	link_directories(${ZMQ_LIBRARY_DIRS})
endif()

if (ZMQ_FOUND OR ZeroMQ_FOUND)
	set(HAS_ZMQ ON)
	set(ZMQ_OPT SHADERTOY_CONNECTOR_HAS_ZMQ=1)
	message(STATUS "Building with libzmq support")
else()
	set(HAS_ZMQ OFF)
	set(ZMQ_OPT SHADERTOY_CONNECTOR_HAS_ZMQ=0)
	message(STATUS "Building without libzmq support")
endif()

# Directories
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

# Win32
if (MSVC)
	add_compile_options("/DNOMINMAX")
endif()

# libshadertoy
find_package(shadertoy 1.0.0 REQUIRED)
get_target_property(SHADERTOY_INCLUDE_DIRS shadertoy-shared INTERFACE_INCLUDE_DIRECTORIES)

# Load omw
set(OMW_CXX_STANDARD 17 CACHE STRING "OMW C++ standard")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext/omw)

cmake_dependent_option(ST_BUILD_MATHEMATICA "Build the Mathematica target" ON
	"OMW_MATHEMATICA_FOUND" OFF)
cmake_dependent_option(ST_BUILD_OCTAVE "Build the Octave target" ON
	"OMW_OCTAVE_FOUND" OFF)
option(ST_BUILD_SERVER "Build the server" ON)

# Load components
add_subdirectory(${SRC_DIR}/core)
add_subdirectory(${SRC_DIR}/gl)

if (HAS_ZMQ)
	add_subdirectory(${SRC_DIR}/net)
	add_subdirectory(${SRC_DIR}/client)

	if (ST_BUILD_SERVER)
		add_subdirectory(${SRC_DIR}/server)
	endif()

	add_subdirectory(${SRC_DIR}/cli)

	# Extra libraries for connector clients
	set(EXTRA_LIBRARIES stc_client)
else()
	set(EXTRA_LIBRARIES "")
endif()

set(ST_SOURCES
	${INCLUDE_DIR}/stc/api.hpp
	${INCLUDE_DIR}/stc/host_manager.hpp

	${SRC_DIR}/api.cpp
	${SRC_DIR}/host_manager.cpp)

if(MSVC)
	set(EXTRA_FLAGS "/W2;/wd4275;/wd4251")
else()
	set(EXTRA_FLAGS "-Wall;-Wno-missing-braces;-pedantic")
endif()

# Mathematica connector
if(ST_BUILD_MATHEMATICA)
	omw_add_mathematica(shadertoy_mathematica
		PACKAGE_NAME Shadertoy
		INSTALL st_mathematica
		MPREP_SOURCES ${SRC_DIR}/mathematica/Shadertoy.tm
		SOURCES ${ST_SOURCES}
		LINK_LIBRARIES stc_gl ${EXTRA_LIBRARIES}
		COMPILE_OPTIONS "${EXTRA_FLAGS}"
		COMPILE_DEFINITIONS ${ZMQ_OPT})
endif()

# Octave connector
if(ST_BUILD_OCTAVE)
	omw_add_octave(shadertoy_octave
		SOURCES ${ST_SOURCES}
		LINK_LIBRARIES stc_gl ${EXTRA_LIBRARIES}
		INSTALL st_octave
		COMPILE_OPTIONS "${EXTRA_ARGS}"
		COMPILE_DEFINITIONS ${ZMQ_OPT})
endif()

# Test procedures
enable_testing()
file(GLOB TEST_FILES ${CMAKE_SOURCE_DIR}/t/*.t)
foreach(TEST_FILE ${TEST_FILES})
	get_filename_component(TEST_FILE_NAME ${TEST_FILE} NAME_WE)
	add_test(NAME ${TEST_FILE_NAME} COMMAND prove -v --color ${TEST_FILE} ::
		--path ${CMAKE_CURRENT_BINARY_DIR} --octave-pkg shadertoy_octave --mathematica-pkg Shadertoy
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach()
