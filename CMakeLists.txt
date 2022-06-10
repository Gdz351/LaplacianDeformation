cmake_minimum_required(VERSION 3.0)

project(LaplacianDeformation)

file(GLOB header inc/*.h)
file(GLOB source src/*.cpp)

set(CMAKE_CXX_STANDARD 11)
set(YRender "D:/repos/YRender")

add_subdirectory(${YRender} YRender_binary_dir)
find_package(OpenMesh)
	
add_executable(${PROJECT_NAME} ${header} ${source})

if(MSVC)
   add_definitions(-D_USE_MATH_DEFINES)
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
		inc
		${YRender}/inc
		$ENV{INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
		YRender
		OpenMeshCore
)
