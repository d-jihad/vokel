cmake_minimum_required(VERSION 3.25)
project(VoKel)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILE_FEATURES "cxx_std_20")

# project requires Vulkan SDK
find_package(Vulkan REQUIRED)

# project realted include files
include_directories(include)

# project sources
file(GLOB SRC_DIR src/*)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# create a unified output directories structure
# in windows try to use the ninja generator with clangd
set(CONFIG_LIST "Release;Debug;MinSizeRel;RelWithDebInfo")
    
foreach(CONFIG IN LISTS CONFIG_LIST)

    string(TOUPPER ${CONFIG} CONFIG_UPPERCASE)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPERCASE}
        "${CMAKE_BINARY_DIR}/${CONFIG}")

endforeach()

if (WIN32)
    include_directories(
        lib/SDL2/include/
        lib/glm/)

    link_directories(${CMAKE_SOURCE_DIR}/lib/SDL2/lib/)

endif(WIN32)

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_DIR_OUT ${SHADER_DIR}/bin) 

file(GLOB SHADERS
    ${SHADER_DIR}/*.vert
    ${SHADER_DIR}/*.frag
    ${SHADER_DIR}/*.comp 
    ${SHADER_DIR}/*.geom 
    ${SHADER_DIR}/*.tesc 
    ${SHADER_DIR}/*.tese 
    ${SHADER_DIR}/*.mesh 
    ${SHADER_DIR}/*.task 
    ${SHADER_DIR}/*.rgen 
    ${SHADER_DIR}/*.rchit 
    ${SHADER_DIR}/*.rmiss
)

foreach(SHADER IN LISTS SHADERS)
    get_filename_component(FILENAME ${SHADER} NAME)
    add_custom_command(OUTPUT ${SHADER_DIR_OUT}/${FILENAME}.spv
        COMMAND Vulkan::glslc ${SHADER} -o ${SHADER_DIR_OUT}/${FILENAME}.spv
        DEPENDS ${SHADER}
        COMMENT "Compiling ${FILENAME}")

    list(APPEND SPV_SHADERS ${SHADER_DIR_OUT}/${FILENAME}.spv)
endForeach()
add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})

link_libraries(Vulkan::Vulkan SDL2)
add_executable(${PROJECT_NAME} ${SRC_DIR} main.cpp)
add_dependencies(${PROJECT_NAME} shaders)
