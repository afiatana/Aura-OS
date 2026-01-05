#include "LiquidRenderer.hpp"
#include "aura_kernel.h"
#include <android/log.h>
#include <string>
#include <vector>


#define LOG_TAG "LiquidRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Minimal SPIR-V for a Triangle (Vertex Shader)
const uint32_t vert_code[] = {
    0x07230203, 0x00010000, 0x00080007, 0x0000001a, 0x00000000, 0x00020011,
    0x00000001, 0x0006000b, 0x00000001, 0x4c534c47, 0x2e737464, 0x35342e30,
    0x00000000, 0x0003000e, 0x00000000, 0x00000001, 0x0005000f, 0x00000000,
    0x00000004, 0x6e69616d, 0x00000000, 0x00030003, 0x00000002, 0x000001c2,
    0x00040005, 0x00000004, 0x6e69616d, 0x00000000, 0x00050006, 0x00000009,
    0x00000000, 0x6c675f50, 0x6974696f, 0x6e000073, 0x00050006, 0x00000009,
    0x00000001, 0x6c675f50, 0x6e696f50, 0x697a6500, 0x00050006, 0x00000009,
    0x00000002, 0x6c675f43, 0x4470696c, 0x74736944, 0x636e6100, 0x00050006,
    0x00000009, 0x00000003, 0x6c675f43, 0x6c6c7543, 0x74736944, 0x636e6100,
    0x00040005, 0x00000007, 0x6c675f50, 0x65726956, 0x78000062, 0x00030005,
    0x0000000d, 0x00000000, 0x00050048, 0x00000009, 0x00000000, 0x0000000b,
    0x00000000, 0x00050048, 0x00000009, 0x00000001, 0x0000000b, 0x00000001,
    0x00050048, 0x00000009, 0x00000002, 0x0000000b, 0x00000002, 0x00050048,
    0x00000009, 0x00000003, 0x0000000b, 0x00000003, 0x00030047, 0x00000009,
    0x00000002, 0x00020013, 0x00000002, 0x00030021, 0x00000003, 0x00000002,
    0x00030016, 0x00000006, 0x00000020, 0x00040017, 0x00000007, 0x00000006,
    0x00000004, 0x00040015, 0x00000008, 0x00000020, 0x00000000, 0x00040018,
    0x00000008, 0x00000006, 0x00000001, 0x0006001c, 0x00000009, 0x00000007,
    0x00000006, 0x00000008, 0x00000008, 0x00040020, 0x0000000a, 0x00000003,
    0x00000009, 0x0004003b, 0x0000000a, 0x0000000b, 0x00000003, 0x00040015,
    0x0000000c, 0x00000020, 0x00000001, 0x0004002b, 0x0000000c, 0x0000000d,
    0x00000000, 0x00040020, 0x0000000e, 0x00000001, 0x0000000c, 0x0004003b,
    0x0000000e, 0x0000000f, 0x00000001, 0x00040020, 0x00000010, 0x00000003,
    0x00000007, 0x00040015, 0x00000012, 0x00000020, 0x00000000, 0x0004002b,
    0x00000012, 0x00000013, 0x00000003, 0x00040017, 0x00000014, 0x00000006,
    0x00000002, 0x0006001c, 0x00000015, 0x00000014, 0x00000013, 0x00000013,
    0x00000013, 0x00040020, 0x00000016, 0x00000001, 0x00000015, 0x0004003b,
    0x00000016, 0x00000017, 0x00000001, 0x0004002b, 0x00000006, 0x00000018,
    0x00000000, 0x0004002b, 0x00000006, 0x00000019, 0x3f800000, 0x00050036,
    0x00000002, 0x00000004, 0x00000000, 0x00000003, 0x000200f8, 0x00000005,
    0x0004003d, 0x0000000c, 0x00000011, 0x0000000f, 0x00050041, 0x00000010,
    0x00000017, 0x00000011, 0x00000013, 0x0004003d, 0x00000014, 0x00000018,
    0x00000017, 0x00050051, 0x00000006, 0x00000019, 0x00000018, 0x00000000,
    0x00050051, 0x00000006, 0x0000001a, 0x00000018, 0x00000001, 0x00070050,
    0x00000007, 0x0000001b, 0x00000019, 0x0000001a, 0x00000018, 0x00000019,
    0x00050041, 0x00000010, 0x0000001c, 0x0000000b, 0x0000000d, 0x0003003e,
    0x0000001c, 0x0000001b, 0x000100fd, 0x00010038};

LiquidRenderer::LiquidRenderer() {}
LiquidRenderer::~LiquidRenderer() { cleanup(); }

bool LiquidRenderer::init(ANativeWindow *win) {
  window = win;
  if (!createInstance())
    return false;
  if (!createSurface())
    return false;
  if (!pickPhysicalDevice())
    return false;
  if (!createLogicalDevice())
    return false;
  if (!createSwapChain())
    return false;
  if (!createImageViews())
    return false;
  if (!createRenderPass())
    return false;
  if (!createGraphicsPipeline())
    return false;
  if (!createFramebuffers())
    return false;
  if (!createCommandPool())
    return false;
  if (!createCommandBuffers())
    return false;
  if (!createSyncObjects())
    return false;

  LOGI("Aura Android Graphics: Vulkan Initialized!");
  return true;
}

bool LiquidRenderer::createInstance() {
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Aura Android";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  std::vector<const char *> extensions = {
      VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME};

  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  return vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS;
}

bool LiquidRenderer::createSurface() {
  VkAndroidSurfaceCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
  createInfo.window = window;
  return vkCreateAndroidSurfaceKHR(instance, &createInfo, nullptr, &surface) ==
         VK_SUCCESS;
}

bool LiquidRenderer::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (deviceCount == 0)
    return false;
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
  physicalDevice = devices[0];
  return true;
}

bool LiquidRenderer::createLogicalDevice() {
  float queuePriority = 1.0f;
  VkDeviceQueueCreateInfo queueCreateInfo = {};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = 0;
  queueCreateInfo.queueCount = 1;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
      VK_SUCCESS)
    return false;
  vkGetDeviceQueue(device, 0, 0, &graphicsQueue);
  presentQueue = graphicsQueue;
  return true;
}

bool LiquidRenderer::createSwapChain() {
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
                                            &capabilities);
  swapChainExtent = capabilities.currentExtent;
  swapChainImageFormat = VK_FORMAT_R8G8B8A8_UNORM;

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = 2;
  createInfo.imageFormat = swapChainImageFormat;
  createInfo.imageExtent = swapChainExtent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  createInfo.preTransform = capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;

  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) !=
      VK_SUCCESS)
    return false;
  uint32_t imageCount;
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
                          swapChainImages.data());
  return true;
}

bool LiquidRenderer::createImageViews() {
  swapChainImageViews.resize(swapChainImages.size());
  for (size_t i = 0; i < swapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = swapChainImageFormat;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.layerCount = 1;
    vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]);
  }
  return true;
}

bool LiquidRenderer::createRenderPass() {
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = swapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorRef = {0,
                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorRef;

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  return vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) ==
         VK_SUCCESS;
}

VkShaderModule
LiquidRenderer::createShaderModule(const std::vector<uint32_t> &code) {
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size() * sizeof(uint32_t);
  createInfo.pCode = code.data();
  VkShaderModule shaderModule;
  vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
  return shaderModule;
}

bool LiquidRenderer::createGraphicsPipeline() {
  std::vector<uint32_t> vert_vec(std::begin(vert_code), std::end(vert_code));
  VkShaderModule vertModule = createShaderModule(vert_vec);

  // In actual app, we'd also load frag shader. For now, empty pipeline is
  // better than null.
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);

  vkDestroyShaderModule(device, vertModule, nullptr);
  return true;
}

bool LiquidRenderer::createFramebuffers() {
  swapChainFramebuffers.resize(swapChainImageViews.size());
  for (size_t i = 0; i < swapChainImageViews.size(); i++) {
    VkImageView attachments[] = {swapChainImageViews[i]};
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = swapChainExtent.width;
    framebufferInfo.height = swapChainExtent.height;
    framebufferInfo.layers = 1;
    vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                        &swapChainFramebuffers[i]);
  }
  return true;
}

bool LiquidRenderer::createCommandPool() {
  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = 0;
  return vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) ==
         VK_SUCCESS;
}

bool LiquidRenderer::createCommandBuffers() {
  commandBuffers.resize(swapChainFramebuffers.size());
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();
  return vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) ==
         VK_SUCCESS;
}

bool LiquidRenderer::createSyncObjects() {
  imageAvailableSemaphores.resize(2);
  renderFinishedSemaphores.resize(2);
  inFlightFences.resize(2);
  VkSemaphoreCreateInfo semInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr,
                                 VK_FENCE_CREATE_SIGNALED_BIT};
  for (int i = 0; i < 2; i++) {
    vkCreateSemaphore(device, &semInfo, nullptr, &imageAvailableSemaphores[i]);
    vkCreateSemaphore(device, &semInfo, nullptr, &renderFinishedSemaphores[i]);
    vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]);
  }
  return true;
}

void LiquidRenderer::render() {
  vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE,
                  UINT64_MAX);
  vkResetFences(device, 1, &inFlightFences[currentFrame]);
  uint32_t imageIndex;
  vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
                        imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE,
                        &imageIndex);

  VkCommandBufferBeginInfo beginInfo = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo);
  VkRenderPassBeginInfo rpBegin = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
  rpBegin.renderPass = renderPass;
  rpBegin.framebuffer = swapChainFramebuffers[imageIndex];
  rpBegin.renderArea.extent = swapChainExtent;
  VkClearValue clearColor = {{{0.05f, 0.05f, 0.15f, 1.0f}}};
  rpBegin.clearValueCount = 1;
  rpBegin.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffers[imageIndex], &rpBegin,
                       VK_SUBPASS_CONTENTS_INLINE);
  vkCmdEndRenderPass(commandBuffers[imageIndex]);
  vkEndCommandBuffer(commandBuffers[imageIndex]);

  VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];
  vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);

  VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapChain;
  presentInfo.pImageIndices = &imageIndex;
  vkQueuePresentKHR(graphicsQueue, &presentInfo);

  currentFrame = (currentFrame + 1) % 2;
}

uint32_t LiquidRenderer::findMemoryType(uint32_t typeFilter,
                                        VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }
  return 0;
}

void LiquidRenderer::cleanup() {
  if (device != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(device);
    for (auto f : inFlightFences)
      vkDestroyFence(device, f, nullptr);
    for (auto s : renderFinishedSemaphores)
      vkDestroySemaphore(device, s, nullptr);
    for (auto s : imageAvailableSemaphores)
      vkDestroySemaphore(device, s, nullptr);
    vkDestroyCommandPool(device, commandPool, nullptr);
    for (auto fb : swapChainFramebuffers)
      vkDestroyFramebuffer(device, fb, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    for (auto iv : swapChainImageViews)
      vkDestroyImageView(device, iv, nullptr);
    vkDestroySwapchainKHR(device, swapChain, nullptr);
    vkDestroyDevice(device, nullptr);
  }
  if (instance != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
  }
}
