cmake_minimum_required(VERSION 3.10)
project(AuraOS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force look for Vulkan
if(DEFINED ENV{VULKAN_SDK})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{VULKAN_SDK}")
endif()

add_subdirectory(aura-graphics)
