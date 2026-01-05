#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include "aura_kernel.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>


// Aura OS Liquid Island - Graphics Engine Prototype v6
// Fixed: Explicitly enabling Swapchain Extension

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

static std::vector<char> readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  if (!file.is_open())
    throw std::runtime_error("failed to open file: " + filename);
  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();
  return buffer;
}

class LiquidIslandApp {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  GLFWwindow *window;
  vk::Instance instance;
  vk::SurfaceKHR surface;
  vk::PhysicalDevice physicalDevice;
  vk::Device device;
  vk::Queue graphicsQueue;
  vk::Queue presentQueue;

  vk::SwapchainKHR swapChain;
  std::vector<vk::Image> swapChainImages;
  vk::Format swapChainImageFormat;
  vk::Extent2D swapChainExtent;
  std::vector<vk::ImageView> swapChainImageViews;
  std::vector<vk::Framebuffer> swapChainFramebuffers;

  vk::RenderPass renderPass;
  vk::PipelineLayout pipelineLayout;
  vk::Pipeline graphicsPipeline;

  vk::CommandPool commandPool;
  std::vector<vk::CommandBuffer> commandBuffers;

  std::vector<vk::Semaphore> imageAvailableSemaphores;
  std::vector<vk::Semaphore> renderFinishedSemaphores;
  std::vector<vk::Fence> inFlightFences;
  uint32_t currentFrame = 0;

  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Aura OS - Liquid Island", nullptr,
                              nullptr);
  }

  void initVulkan() {
    // Initialize Rust Kernel first
    if (aura_kernel_init()) {
      char *version = aura_kernel_get_version();
      std::cout << "Aura Kernel FFI Linked! Version: " << version << std::endl;
      aura_kernel_free_string(version);
    }

    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
    std::cout << "Aura Graphics Engine: Ready to Render!" << std::endl;
  }

  void createInstance() {
    vk::ApplicationInfo appInfo("Aura Graphics", VK_MAKE_VERSION(1, 0, 0),
                                "Aura Engine", VK_MAKE_VERSION(1, 0, 0),
                                VK_API_VERSION_1_3);
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);
    vk::InstanceCreateInfo createInfo({}, &appInfo, 0, nullptr,
                                      (uint32_t)extensions.size(),
                                      extensions.data());
    instance = vk::createInstance(createInfo);
  }

  void createSurface() {
    VkSurfaceKHR rawSurface;
    if (glfwCreateWindowSurface((VkInstance)instance, window, nullptr,
                                &rawSurface) != VK_SUCCESS)
      throw std::runtime_error("failed to create surface!");
    surface = rawSurface;
  }

  void pickPhysicalDevice() {
    auto devices = instance.enumeratePhysicalDevices();
    for (const auto &d : devices) {
      if (checkDeviceExtensionSupport(d) && findQueueFamilies(d).isComplete()) {
        physicalDevice = d;
        break;
      }
    }
    if (!physicalDevice)
      throw std::runtime_error("failed to find suitable GPU!");
    std::cout << "Using GPU: " << physicalDevice.getProperties().deviceName
              << std::endl;
  }

  bool checkDeviceExtensionSupport(vk::PhysicalDevice d) {
    auto availableExtensions = d.enumerateDeviceExtensionProperties();
    std::set<std::string> required(deviceExtensions.begin(),
                                   deviceExtensions.end());
    for (const auto &ext : availableExtensions) {
      required.erase(ext.extensionName);
    }
    return required.empty();
  }

  QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice d) {
    QueueFamilyIndices indices;
    auto families = d.getQueueFamilyProperties();
    int i = 0;
    for (const auto &f : families) {
      if (f.queueFlags & vk::QueueFlagBits::eGraphics)
        indices.graphicsFamily = i;
      if (d.getSurfaceSupportKHR(i, surface))
        indices.presentFamily = i;
      if (indices.isComplete())
        break;
      i++;
    }
    return indices;
  }

  void createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    float priority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> queues = {
        {{}, indices.graphicsFamily.value(), 1, &priority}};
    if (indices.graphicsFamily != indices.presentFamily)
      queues.push_back({{}, indices.presentFamily.value(), 1, &priority});

    vk::PhysicalDeviceFeatures features;
    vk::DeviceCreateInfo createInfo(
        {}, (uint32_t)queues.size(), queues.data(), 0, nullptr,
        (uint32_t)deviceExtensions.size(), deviceExtensions.data(), &features);
    device = physicalDevice.createDevice(createInfo);
    graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
    presentQueue = device.getQueue(indices.presentFamily.value(), 0);
  }

  void createSwapChain() {
    swapChainImageFormat = vk::Format::eB8G8R8A8Unorm;
    swapChainExtent = vk::Extent2D{WIDTH, HEIGHT};
    vk::SwapchainCreateInfoKHR createInfo(
        {}, surface, 3, swapChainImageFormat, vk::ColorSpaceKHR::eSrgbNonlinear,
        swapChainExtent, 1, vk::ImageUsageFlagBits::eColorAttachment);

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueIndices[] = {indices.graphicsFamily.value(),
                               indices.presentFamily.value()};
    if (indices.graphicsFamily != indices.presentFamily) {
      createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueIndices;
    } else {
      createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    swapChain = device.createSwapchainKHR(createInfo);
    swapChainImages = device.getSwapchainImagesKHR(swapChain);
  }

  void createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++) {
      vk::ImageViewCreateInfo createInfo(
          {}, swapChainImages[i], vk::ImageViewType::e2D, swapChainImageFormat,
          {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
      swapChainImageViews[i] = device.createImageView(createInfo);
    }
  }

  void createRenderPass() {
    vk::AttachmentDescription colorAttachment(
        {}, swapChainImageFormat, vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
    vk::AttachmentReference colorRef(0,
                                     vk::ImageLayout::eColorAttachmentOptimal);
    vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, 0,
                                   nullptr, 1, &colorRef);
    vk::RenderPassCreateInfo createInfo({}, 1, &colorAttachment, 1, &subpass);
    renderPass = device.createRenderPass(createInfo);
  }

  void createGraphicsPipeline() {
    auto vertCode = readFile("shaders/vert.spv");
    auto fragCode = readFile("shaders/frag.spv");
    vk::ShaderModule vertModule = device.createShaderModule(
        {{},
         vertCode.size(),
         reinterpret_cast<const uint32_t *>(vertCode.data())});
    vk::ShaderModule fragModule = device.createShaderModule(
        {{},
         fragCode.size(),
         reinterpret_cast<const uint32_t *>(fragCode.data())});

    vk::PipelineShaderStageCreateInfo stages[] = {
        {{}, vk::ShaderStageFlagBits::eVertex, vertModule, "main"},
        {{}, vk::ShaderStageFlagBits::eFragment, fragModule, "main"}};

    vk::PipelineVertexInputStateCreateInfo vertexInput({}, 0, nullptr, 0,
                                                       nullptr);
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
        {}, vk::PrimitiveTopology::eTriangleList, VK_FALSE);
    vk::Viewport viewport(0.0f, 0.0f, (float)swapChainExtent.width,
                          (float)swapChainExtent.height, 0.0f, 1.0f);
    vk::Rect2D scissor({0, 0}, swapChainExtent);
    vk::PipelineViewportStateCreateInfo viewportState({}, 1, &viewport, 1,
                                                      &scissor);
    vk::PipelineRasterizationStateCreateInfo rasterizer(
        {}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise, VK_FALSE, 0.0f,
        0.0f, 0.0f, 1.0f);
    vk::PipelineMultisampleStateCreateInfo multisampling(
        {}, vk::SampleCountFlagBits::e1, VK_FALSE);
    vk::PipelineColorBlendAttachmentState colorBlendAttachment(
        VK_FALSE, vk::BlendFactor::eOne, vk::BlendFactor::eZero,
        vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero,
        vk::BlendOp::eAdd,
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    vk::PipelineColorBlendStateCreateInfo colorBlending(
        {}, VK_FALSE, vk::LogicOp::eCopy, 1, &colorBlendAttachment);

    vk::PushConstantRange pushConstantRange(vk::ShaderStageFlagBits::eFragment,
                                            0, sizeof(float));
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 0, nullptr, 1,
                                                    &pushConstantRange);
    pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

    vk::GraphicsPipelineCreateInfo pipelineInfo(
        {}, 2, stages, &vertexInput, &inputAssembly, nullptr, &viewportState,
        &rasterizer, &multisampling, nullptr, &colorBlending, nullptr,
        pipelineLayout, renderPass, 0);
    auto result = device.createGraphicsPipeline(nullptr, pipelineInfo);
    if (result.result != vk::Result::eSuccess)
      throw std::runtime_error("failed to create pipeline!");
    graphicsPipeline = result.value;

    device.destroyShaderModule(fragModule);
    device.destroyShaderModule(vertModule);
  }

  void createFramebuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());
    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
      vk::ImageView attachments[] = {swapChainImageViews[i]};
      vk::FramebufferCreateInfo framebufferInfo({}, renderPass, 1, attachments,
                                                swapChainExtent.width,
                                                swapChainExtent.height, 1);
      swapChainFramebuffers[i] = device.createFramebuffer(framebufferInfo);
    }
  }

  void createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
    vk::CommandPoolCreateInfo poolInfo(
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        queueFamilyIndices.graphicsFamily.value());
    commandPool = device.createCommandPool(poolInfo);
  }

  void createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    vk::CommandBufferAllocateInfo allocInfo(commandPool,
                                            vk::CommandBufferLevel::ePrimary,
                                            (uint32_t)commandBuffers.size());
    commandBuffers = device.allocateCommandBuffers(allocInfo);
  }

  void recordCommandBuffer(vk::CommandBuffer commandBuffer,
                           uint32_t imageIndex) {
    vk::CommandBufferBeginInfo beginInfo;
    commandBuffer.begin(beginInfo);

    vk::ClearValue clearColor(
        vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}));
    vk::RenderPassBeginInfo renderPassInfo(
        renderPass, swapChainFramebuffers[imageIndex],
        {{0, 0}, swapChainExtent}, 1, &clearColor);

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                               graphicsPipeline);

    float time = (float)glfwGetTime();
    // Use intensity derived from Rust Kernel logic
    float intensity = aura_kernel_calculate_fluid_intensity(time);
    commandBuffer.pushConstants(pipelineLayout,
                                vk::ShaderStageFlagBits::eFragment, 0,
                                sizeof(float), &intensity);

    commandBuffer.draw(3, 1, 0, 0);
    commandBuffer.endRenderPass();
    commandBuffer.end();
  }

  void createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    vk::SemaphoreCreateInfo semaphoreInfo;
    vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      imageAvailableSemaphores[i] = device.createSemaphore(semaphoreInfo);
      renderFinishedSemaphores[i] = device.createSemaphore(semaphoreInfo);
      inFlightFences[i] = device.createFence(fenceInfo);
    }
  }

  void drawFrame() {
    if (device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE,
                             UINT64_MAX) != vk::Result::eSuccess)
      return;
    device.resetFences(1, &inFlightFences[currentFrame]);

    auto result = device.acquireNextImageKHR(
        swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr);
    uint32_t imageIndex = result.value;

    commandBuffers[currentFrame].reset();
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    vk::Semaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    vk::PipelineStageFlags waitStages[] = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::Semaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};

    vk::SubmitInfo submitInfo(1, waitSemaphores, waitStages, 1,
                              &commandBuffers[currentFrame], 1,
                              signalSemaphores);
    graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);

    vk::SwapchainKHR swapChains[] = {swapChain};
    vk::PresentInfoKHR presentInfo(1, signalSemaphores, 1, swapChains,
                                   &imageIndex);
    if (presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess)
      return;

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      drawFrame();
    }
    device.waitIdle();
  }

  void cleanup() {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      device.destroySemaphore(renderFinishedSemaphores[i]);
      device.destroySemaphore(imageAvailableSemaphores[i]);
      device.destroyFence(inFlightFences[i]);
    }
    device.destroyCommandPool(commandPool);
    for (auto framebuffer : swapChainFramebuffers)
      device.destroyFramebuffer(framebuffer);
    device.destroyPipeline(graphicsPipeline);
    device.destroyPipelineLayout(pipelineLayout);
    device.destroyRenderPass(renderPass);
    for (auto imageView : swapChainImageViews)
      device.destroyImageView(imageView);
    device.destroySwapchainKHR(swapChain);
    device.destroy();
    instance.destroySurfaceKHR(surface);
    instance.destroy();
    glfwDestroyWindow(window);
    glfwTerminate();
  }
};

int main() {
  LiquidIslandApp app;
  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << "Aura Graphics Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
