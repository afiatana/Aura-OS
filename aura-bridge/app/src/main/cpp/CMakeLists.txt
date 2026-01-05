cmake_minimum_required(VERSION 3.10)

project("aura_bridge")

# more robust way to find root
get_filename_component(AURA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE)
message(STATUS "Aura Bridge Root: ${AURA_ROOT}")

# Deteksi arsitektur
if(ANDROID_ABI STREQUAL "x86_64")
    set(RUST_TARGET "x86_64-linux-android")
elseif(ANDROID_ABI STREQUAL "arm64-v8a")
    set(RUST_TARGET "aarch64-linux-android")
else()
    set(RUST_TARGET "x86_64-linux-android")
endif()

message(STATUS "Aura Bridge ABI: ${ANDROID_ABI} -> Rust Target: ${RUST_TARGET}")

# Path ke library Rust
set(KERNEL_LIB_PATH "${AURA_ROOT}/aura-kernel/target/${RUST_TARGET}/release")
message(STATUS "Aura Bridge Kernel Lib Path: ${KERNEL_LIB_PATH}")

# Path ke header graphics
include_directories("${AURA_ROOT}/aura-graphics")

add_library(aura_bridge SHARED
            aura_bridge_jni.cpp
            LiquidRenderer.cpp)

find_library(log-lib log)
find_library(vulkan-lib vulkan) # Link against Vulkan on Android

target_link_directories(aura_bridge PRIVATE "${KERNEL_LIB_PATH}")
target_link_libraries(aura_bridge
                      aura_kernel
                      ${log-lib}
                      ${vulkan-lib}
                      android
)
