
cmake_minimum_required(VERSION 2.8)

project(Trans)

IF (UNIX)
        set(CMAKE_C_COMPILER "/usr/bin/clang")
        set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -I/usr/include/libcxxabi")
        set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -stdlib=libc++ -lc++abi -rpath=./")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
ENDIF()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include) 
include_directories(${PROJECT_SOURCE_DIR}/include/asio)
include_directories(${PROJECT_SOURCE_DIR}/include/coord_conver)
include_directories(${PROJECT_SOURCE_DIR}/src)

set(Trans "Trans")
file(GLOB TransFiles "src/*.cpp")
add_executable(${Trans} ${TransFiles})

if(MSVC)
    if(CMAKE_CONFIGURATION_TYPES STREQUAL "Release")
        set_target_properties( ${Trans} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT" )
    else()
        set_target_properties( ${Trans} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMTD" )
    endif()
endif()

