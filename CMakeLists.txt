# HOWL - Gooborg Studios

# Minimum version required for CMake
cmake_minimum_required(VERSION 3.1)

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10" CACHE STRING "Minimum macOS deployment version")

# Project properties
project(HOWL)
set(TARGET_VERSION_MAJOR 0)
set(TARGET_VERSION_MINOR 1)
set(TARGET_VERSION_PATCH 0)
set(TARGET_COPYRIGHT "©2018 Gooborg Studios: Vinyl Darkscratch, Light Apacha.")

set(CMAKE_BINARY_DIR ${PROJECT_BINARY_DIR})
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
aux_source_directory(src source_file_list)

set(ROOT  ${CMAKE_SOURCE_DIR})
set(SRC   ${PROJECT_SOURCE_DIR})
set(BIN   ${CMAKE_BINARY_DIR})
set(INCL  ${ROOT}/include)
set(LIB   ${ROOT}/lib)
set(BUILD ${PROJECT_BINARY_DIR})

include_directories(${ROOT})
include_directories(${INCL})

add_library(HOWL SHARED
	${SRC}/Layer.cpp ${INCL}/Layer.h
	${SRC}/Project.cpp ${INCL}/Project.h
	${SRC}/TimelinePanel.cpp ${INCL}/TimelinePanel.h
	${SRC}/Selection.cpp ${INCL}/Selection.h
)

IF(APPLE)
	add_definitions(-DOSX_BUNDLE)
	target_link_libraries(HOWL "-framework CoreFoundation")
	set_target_properties(HOWL PROPERTIES MACOSX_FRAMEWORK_IDENTIFIER co.nightwave.howl)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions(-D__NO_INLINE__)
ENDIF()

find_package(wxWidgets COMPONENTS html qa adv core xml net base)
IF(wxWidgets_FOUND)
	add_definitions(-DUSE_WXWIDGETS)
	execute_process(COMMAND "wx-config --cxxflags" OUTPUT_VARIABLE wxConfig_CXX_FLAGS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${wxConfig_CXX_FLAGS}")
	include(${wxWidgets_USE_FILE})
	target_link_libraries(HOWL ${wxWidgets_LIBRARIES})
ENDIF(wxWidgets_FOUND)

find_package(GooCore REQUIRED)
include_directories(${GooCore_INCLUDE_DIR})
target_link_libraries(HOWL ${GooCore_LIBRARIES})

find_package(Doxygen)
IF(DOXYGEN_FOUND)
	configure_file(${ROOT}/doxygen.conf.in ${BUILD}/doxygen.conf @ONLY)
	add_custom_target(docs ALL DEPENDS ${BUILD}/doxygen/)
	add_custom_command(OUTPUT ${BUILD}/doxygen/ COMMAND ${DOXYGEN_EXECUTABLE} ${BUILD}/doxygen.conf DEPENDS ${SRC}/* ${INCL}/* ${ROOT}/README.md ${ROOT}/doxygen.conf.in VERBATIM) # COMMENT "Generating API documentation with Doxygen"
ENDIF(DOXYGEN_FOUND)

install(TARGETS HOWL DESTINATION lib)

file(GLOB HEADERS include/*.h)
install(FILES ${HEADERS} DESTINATION include/HOWL)
