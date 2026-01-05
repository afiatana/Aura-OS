#ifndef LIQUID_RENDERER_HPP
#define LIQUID_RENDERER_HPP

#ifndef VK_USE_PLATFORM_ANDROID_KHR
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#include <android/log.h>
#include <android/native_window.h>
#include <cstdint>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

/**
 * @brief Status geometri pulau untuk animasi spring physics (Standard Master
 * Context).
 */
struct IslandState {
  float width;
  float height;
  float x;
  float y;
  float cornerRadius;
};

class LiquidRenderer {
public:
  LiquidRenderer();
  ~LiquidRenderer();

  bool init(ANativeWindow *window);
  void render();
  void updateState(IslandState target, float deltaTime);
  void cleanup();

private:
  ANativeWindow *window = nullptr;
  VkInstance instance = VK_NULL_HANDLE;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device = VK_NULL_HANDLE;
  VkQueue graphicsQueue = VK_NULL_HANDLE;
  VkQueue presentQueue = VK_NULL_HANDLE;
  VkSwapchainKHR swapChain = VK_NULL_HANDLE;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  VkRenderPass renderPass = VK_NULL_HANDLE;
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkPipeline graphicsPipeline = VK_NULL_HANDLE;

  VkCommandPool commandPool = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> commandBuffers;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  uint32_t currentFrame = 0;

  // Spring Physics State
  IslandState currentState;
  IslandState currentVelocity;
  const float stiffness = 150.0f;
  const float damping = 20.0f;

  bool createInstance();
  bool setupDebugMessenger();
  bool createSurface();
  bool pickPhysicalDevice();
  bool createLogicalDevice();
  bool createSwapChain();
  bool createImageViews();
  bool createRenderPass();
  bool createGraphicsPipeline();
  bool createFramebuffers();
  bool createCommandPool();
  bool createCommandBuffers();
  bool createSyncObjects();

  VkShaderModule createShaderModule(const std::vector<uint32_t> &code);
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
};

#endif
