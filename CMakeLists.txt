# HOWL - Nightwave Studios

# Minimum version required for CMake
cmake_minimum_required(VERSION 3.1)

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

# Project properties
project(HOWL)
set(TARGET_VERSION_MAJOR 0)
set(TARGET_VERSION_MINOR 1)
set(TARGET_VERSION_PATCH 0)
set(TARGET_COPYRIGHT "©2017 Nightwave Studios: Vinyl Darkscratch, Light Apacha.")

set(CMAKE_BINARY_DIR ${PROJECT_BINARY_DIR}/bin)
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

include_directories(~/local/include)

add_library(HOWL SHARED
	${SRC}/Layer.cpp ${INCL}/Layer.h
	${SRC}/Project.cpp ${INCL}/Project.h
	${SRC}/TimelinePanel.cpp ${INCL}/TimelinePanel.h
)

set(CMAKE_CXX_COMPILER g++)
find_package(wxWidgets COMPONENTS html qa adv core xml net base REQUIRED)
execute_process(COMMAND "wx-config --cxxflags" OUTPUT_VARIABLE wxConfig_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${wxConfig_CXX_FLAGS}")

include(${wxWidgets_USE_FILE})
target_link_libraries(HOWL ${wxWidgets_LIBRARIES})

target_link_libraries(HOWL "-L$HOME/local/lib")
target_link_libraries(HOWL nightwave_core)

install(TARGETS HOWL DESTINATION lib/HOWL)

file(GLOB HEADERS include/*.h)
install(FILES ${HEADERS} DESTINATION include/HOWL)
