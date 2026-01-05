#include "LiquidIslandRenderer.hpp"
#include <algorithm>

LiquidIslandRenderer::LiquidIslandRenderer(vk::Instance instance,
                                           vk::PhysicalDevice physicalDevice,
                                           vk::Device device,
                                           vk::SurfaceKHR surface)
    : instance(instance), physicalDevice(physicalDevice), device(device),
      surface(surface) {

  // Inisialisasi state awal (Pusat layar, bentuk kecil)
  currentState = {200.0f, 40.0f, 400.0f, 50.0f, 20.0f};
  currentVelocity = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
}

LiquidIslandRenderer::~LiquidIslandRenderer() {
  if (swapChain) {
    device.destroySwapchainKHR(swapChain);
  }
  if (renderPass) {
    device.destroyRenderPass(renderPass);
  }
}

void LiquidIslandRenderer::updateState(const IslandState &targetState,
                                       float deltaTime) {
  // Logika Spring Physics: a = -k*(x - target) - d*v
  // Diterapkan pada setiap atribut untuk animasi "Gooey" yang sinkron

  auto calculateSpring = [&](float current, float target,
                             float &velocity) -> float {
    float force = -stiffness * (current - target) - damping * velocity;
    velocity += force * deltaTime;
    return current + velocity * deltaTime;
  };

  currentState.width = calculateSpring(currentState.width, targetState.width,
                                       currentVelocity.width);
  currentState.height = calculateSpring(currentState.height, targetState.height,
                                        currentVelocity.height);
  currentState.x =
      calculateSpring(currentState.x, targetState.x, currentVelocity.x);
  currentState.y =
      calculateSpring(currentState.y, targetState.y, currentVelocity.y);
  currentState.cornerRadius =
      calculateSpring(currentState.cornerRadius, targetState.cornerRadius,
                      currentVelocity.cornerRadius);

  // Logging performa (Opsional untuk debug 120fps)
  // std::cout << "[LiquidIsland] Morphing: Width=" << currentState.width <<
  // std::endl;
}

void LiquidIslandRenderer::createRenderPass() {
  // Penyiapan Render Pass Transparan (Overlay Mode)
  vk::AttachmentDescription colorAttachment(
      {},
      vk::Format::eB8G8R8A8Unorm, // Mendukung transparansi
      vk::SampleCountFlagBits::e1,
      vk::AttachmentLoadOp::eClear, // Clear color setiap frame
      vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
      vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
      vk::ImageLayout::ePresentSrcKHR);

  vk::AttachmentReference colorAttachmentRef(
      0, vk::ImageLayout::eColorAttachmentOptimal);

  vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, 0,
                                 nullptr, 1, &colorAttachmentRef);

  vk::RenderPassCreateInfo renderPassInfo({}, 1, &colorAttachment, 1, &subpass);

  renderPass = device.createRenderPass(renderPassInfo);
}

void LiquidIslandRenderer::createSwapChain(uint32_t width, uint32_t height) {
  // Implementasi sederhana swapchain (Abstraksi dari main.cpp)
  vk::SwapchainCreateInfoKHR createInfo(
      {}, surface,
      3, // Triple buffering untuk 120fps yang mulus
      vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear,
      vk::Extent2D{width, height}, 1, vk::ImageUsageFlagBits::eColorAttachment);

  // Logic sharing mode disederhanakan untuk contoh
  createInfo.imageSharingMode = vk::SharingMode::eExclusive;

  swapChain = device.createSwapchainKHR(createInfo);
  swapChainImages = device.getSwapchainImagesKHR(swapChain);
}

void LiquidIslandRenderer::initGraphicsPipeline(uint32_t width,
                                                uint32_t height) {
  createSwapChain(width, height);
  createRenderPass();
  std::cout << "[LiquidIsland] Pipeline Grafis Transparan Siap." << std::endl;
}

void LiquidIslandRenderer::drawFrame() {
  // Slot untuk pengiriman command buffer ke GPU
  // Di sinilah Compute Shaders akan diintegrasikan nanti
}
