cmake_minimum_required(VERSION 3.1)

set(BUILD_TYPE "Debug and Release" CACHE STRING "Choose Build type")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

# set the project name
project(FocalEngineExample LANGUAGES CXX)

set(CMAKE_CXX_FLAGS_DEBUG "/MP /ZI /W3 /Od /MDd /JMC /sdl /FC /D IMGUI_IMPL_OPENGL_LOADER_GLEW /D WIN32 /D _WINDOWS /D _DEBUG /D FE_DEBUG_ENABLED")
set(CMAKE_CXX_FLAGS_RELEASE "/MP /W3 /O2 /Oi /Gy /sdl /FC /GL /OPT:REF /OPT:ICF /D IMGUI_IMPL_OPENGL_LOADER_GLEW /D NDEBUG /D WIN32 /D _WINDOWS")

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT TARGET FocalEngine)
	add_subdirectory(SubSystems/FocalEngine)
endif()

# *************** EDITOR ***************
file(GLOB Example_SRC
	"main.cpp"
)

link_directories(${GLEW_LIB_DIR})
link_directories(${GLFW_LIB_DIR})

# add the executable
add_executable(FocalEngineExample WIN32
								  ${Example_SRC}
)

target_link_libraries(FocalEngineExample FocalEngine)

target_link_libraries(FocalEngineExample glew32s.lib)
target_link_libraries(FocalEngineExample glfw3.lib)
target_link_libraries(FocalEngineExample glfw3dll.lib)
target_link_libraries(FocalEngineExample opengl32.lib)

source_group("Source Files" FILES ${Example_SRC})

# set the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT FocalEngineExample)

include_directories(
	${GLEW_INCLUDE_DIR}
	${BASICAPP_THIRDPARTY_DIR}
	${BASICAPP_DIR}
	${FOCAL_ENGINE_THIRD_PARTY_DIR}
)