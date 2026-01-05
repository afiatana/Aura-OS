#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <iostream>

/**
 * @brief Struct untuk menyimpan status geometri dari Liquid Island.
 */
struct IslandState {
    float width;
    float height;
    float x;
    float y;
    float cornerRadius;
};

/**
 * @brief LiquidIslandRenderer mengelola siklus hidup grafis Vulkan untuk UI Aura OS.
 * Target: 120fps dengan animasi berbasis spring physics.
 */
class LiquidIslandRenderer {
public:
    LiquidIslandRenderer(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface);
    ~LiquidIslandRenderer();

    /**
     * @brief Memperbarui state pulau menggunakan logika spring physics.
     * @param targetState State tujuan yang diinginkan.
     * @param deltaTime Waktu yang berlalu sejak frame terakhir.
     */
    void updateState(const IslandState& targetState, float deltaTime);

    /**
     * @brief Menyiapkan swapchain dan render pass transparan.
     */
    void initGraphicsPipeline(uint32_t width, uint32_t height);
    
    /**
     * @brief Merender frame ke swapchain.
     */
    void drawFrame();

private:
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::SurfaceKHR surface;

    vk::SwapchainKHR swapChain;
    vk::RenderPass renderPass;
    std::vector<vk::Image> swapChainImages;
    vk::Format swapChainImageFormat;
    
    // Status Arsitektur Spring Physics
    IslandState currentState;
    IslandState currentVelocity;
    
    // Konstanta Pegas (Dapat dituning untuk feel organik)
    const float stiffness = 150.0f;
    const float damping = 20.0f;

    void createSwapChain(uint32_t width, uint32_t height);
    void createRenderPass();
};
