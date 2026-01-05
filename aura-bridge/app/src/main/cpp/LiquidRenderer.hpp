#ifndef LIQUID_RENDERER_HPP
#define LIQUID_RENDERER_HPP

#ifndef VK_USE_PLATFORM_ANDROID_KHR
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#include <android/native_window.h>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>


class LiquidRenderer {
public:
  LiquidRenderer();
  ~LiquidRenderer();

  bool init(ANativeWindow *window);
  void render();
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
