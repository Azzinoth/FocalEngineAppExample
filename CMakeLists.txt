cmake_minimum_required(VERSION 3.10)

set(BUILD_TYPE "Debug and Release" CACHE STRING "Choose Build type")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

# set the project name
project(FocalEngineExample)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags based on USE_STATIC_RUNTIME
if(MSVC)
    # Disable C4251 warning
    add_compile_options(/wd4251)

    if(USE_STATIC_RUNTIME)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    else()
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    endif()

    # Always add /MP for multi-processor compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT TARGET FocalEngine)
	add_subdirectory(SubSystems/FocalEngine)
endif()

# Extract the relative path of the engine folder
file(RELATIVE_PATH RELATIVE_PATH_TO_ENGINE_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}" "${ENGINE_FOLDER}")
# We need to re-configure the Config.h file for the engine
# This is necessary for any project that will include FocalEngine
# to make sure that the engine can find its dependencies
set(ENGINE_FOLDER "${RELATIVE_PATH_TO_ENGINE_FOLDER}")
configure_file(${ENGINE_FOLDER}/ResourceManager/Config.h.in ${ENGINE_FOLDER}/ResourceManager/Config.h @ONLY)

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

target_link_libraries(FocalEngineExample
    PRIVATE
        FEBasicApplication
        FocalEngine
        ${GLFW_LIBRARY}
        ${GLEW_LIBRARY}
        opengl32.lib
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(FocalEngineExample PRIVATE FOCALENGINE_SHARED)
endif()

source_group("Source Files" FILES ${Example_SRC})

# set the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT FocalEngineExample)

include_directories(
	${GLEW_INCLUDE_DIR}
	${BASICAPP_THIRDPARTY_DIR}
	${BASICAPP_DIR}
	${FOCAL_ENGINE_INCLUDES}
)

# If building as DLL, copy necessary DLLs to output directory
if(BUILD_SHARED_LIBS)
    add_custom_command(TARGET FocalEngineExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE_DIR:FocalEngine>/glfw3.dll"
            "$<TARGET_FILE_DIR:FocalEngineExample>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE_DIR:FocalEngine>/FEBasicApplication.dll"
            "$<TARGET_FILE_DIR:FocalEngineExample>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE_DIR:FocalEngine>/FocalEngine.dll"
            "$<TARGET_FILE_DIR:FocalEngineExample>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OpenXRLoader_DLL}
            "$<TARGET_FILE_DIR:FocalEngineExample>"
    )
endif()

# If using dynamic runtime, copy necessary DLLs to output directory
if(NOT USE_STATIC_RUNTIME)
    add_custom_command(TARGET FocalEngineExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLEW_LIB_DIR}/glew32.dll"
            "$<TARGET_FILE_DIR:FocalEngineExample>"
    )
endif()