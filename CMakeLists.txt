cmake_minimum_required(VERSION 3.20)

# ---- Options ----

option(COPY_BUILD "Copy the build output to the Fallout 4 directory." ON)

# ---- Cache build vars ----

macro(set_from_environment VARIABLE)
	if (NOT DEFINED ${VARIABLE} AND DEFINED ENV{${VARIABLE}})
		set(${VARIABLE} $ENV{${VARIABLE}})
	endif ()
endmacro()

set_from_environment(Fallout4Path)
set_from_environment(VCPKG_ROOT)


#find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
#target_include_directories(Penetration PRIVATE ${STB_INCLUDE_DIRS})



if (DEFINED VCPKG_ROOT)
	set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
	set(VCPKG_TARGET_TRIPLET "x64-windows-static-md" CACHE STRING "")
else ()
	message(
		WARNING
		"Variable VCPKG_ROOT is not set. Continuing without vcpkg."
	)
endif ()

set(Boost_USE_STATIC_RUNTIME OFF CACHE BOOL "")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE STRING "")

# ---- Project ----

project(
	FakeThroughScope
	VERSION 0.7.1
	LANGUAGES CXX
)


configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/Version.h.in
	${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
	@ONLY
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	@ONLY
)


# ---- Include guards ----

if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR
		"In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
	)
endif ()

# ---- Globals ----

if (MSVC)
	if (NOT ${CMAKE_GENERATOR} STREQUAL "Ninja")
		add_compile_options(
			/MP	# Build with Multiple Processes
		)
	endif ()
endif ()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)

set(Boost_USE_STATIC_LIBS ON)

# ---- Dependencies ----

if (NOT TARGET CommonLibF4)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../CommonLibF4" CommonLibF4)
endif ()

if (NOT TARGET ImGui186)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../imgui-1.86" ImGui186)
endif ()

#if (NOT TARGET DirectXTK12)
#	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../DirectXTK12" DirectXTK12)
#endif ()

find_package(spdlog REQUIRED CONFIG)

# ---- Add source files ----

include(cmake/sourcelist.cmake)

#source_group(
#	TREE ${CMAKE_CURRENT_SOURCE_DIR}
#	FILES ${SOURCES},
#	src/HLSL/ScopeEffect_PS.hlsl,
#	src/HLSL/ScopeEffect_VS.hlsl
#
#)

source_group(
	TREE ${CMAKE_CURRENT_BINARY_DIR}
	FILES ${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
)

# ---- Create DLL ----

add_library(
	${PROJECT_NAME}
	SHARED
	${SOURCES}
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	.clang-format
)

target_compile_definitions(
	${PROJECT_NAME}
	PRIVATE
		_UNICODE
)

target_compile_features(
	${PROJECT_NAME}
	PRIVATE
		cxx_std_20
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		#E:/CInclude
		${CMAKE_CURRENT_BINARY_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		CommonLibF4::CommonLibF4
		spdlog::spdlog
		ImGui186::ImGui186
)

link_libraries(capstone)

target_precompile_headers(
	${PROJECT_NAME}
	PRIVATE
		src/PCH.h
)

if (MSVC)
	target_compile_options(
		${PROJECT_NAME}
		PRIVATE
			/sdl	# Enable Additional Security Checks
			#/utf-8	# Set Source and Executable character sets to UTF-8
			/Zi	# Debug Information Format

			/permissive-	# Standards conformance
			/Zc:preprocessor	# Enable preprocessor conformance mode

			/W4	# Warning level
			/WX-# Warning level (warnings are errors)

			"$<$<CONFIG:DEBUG>:>"
			"$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>"
	)

	target_link_options(
		${PROJECT_NAME}
		PRIVATE
			#/WX-# Treat Linker Warnings as Errors
			"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
			"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
	)
endif ()

# ---- File copying ----

set(SCOPEEFFECT "ScopeEffect")
#if (DEFINED Fallout4Path)
if(true)
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		#COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "G:/Modding/MO2/mods/FTS/F4SE/Plugins/"
		#COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> "G:/Modding/MO2/mods/FTS/F4SE/Plugins/"
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "G:/0p/Steam/steamapps/common/Fallout 4/Data/F4SE/Plugins/"
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> "G:/0p/Steam/steamapps/common/Fallout 4/Data/F4SE/Plugins/"
	)
else ()
	message(
		WARNING
		"Variable Fallout4Path is not defined. Skipping post-build copy command."
	)
endif ()


# ---- Build artifacts ----

set(SCRIPT "scripts/archive_artifacts.py")


add_custom_target(
	create_artifact
	COMMAND
		"${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}"
		"--name=\"${PROJECT_NAME}\""
		"--dll=\"$<TARGET_FILE:${PROJECT_NAME}>\""
		"--pdb=\"$<TARGET_PDB_FILE:${PROJECT_NAME}>\""
	WORKING_DIRECTORY
		${CMAKE_CURRENT_BINARY_DIR}
	SOURCES
		${SCRIPT}
)

add_dependencies(
	create_artifact
	${PROJECT_NAME}
)

source_group(
	TREE ${CMAKE_CURRENT_SOURCE_DIR}
	FILES ${SCRIPT}
)


