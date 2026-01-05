cmake_minimum_required(VERSION 3.10)
project(AuraGraphics)

find_package(Vulkan REQUIRED)

# Setup GLFW (Local Binary)
set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/glfw-3.4.bin.WIN64")
include_directories("${GLFW_DIR}/include")
link_directories("${GLFW_DIR}/lib-vc2022")

# Setup Rust Kernel (Static Lib)
set(KERNEL_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../aura-kernel/target/release")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
link_directories("${KERNEL_LIB_DIR}")

add_executable(AuraGraphics main.cpp)

target_include_directories(AuraGraphics PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(AuraGraphics PRIVATE 
    ${Vulkan_LIBRARIES} 
    glfw3 
    aura_kernel
    user32 gdi32 shell32 
    Ws2_32 Userenv Ntdll Bcrypt
)

if(WIN32)
    target_compile_definitions(AuraGraphics PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif()
