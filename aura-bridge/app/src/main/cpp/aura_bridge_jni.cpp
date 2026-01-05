#include "LiquidRenderer.hpp"
#include "aura_kernel.h"
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <iostream>
#include <jni.h>
#include <string>

#define LOG_TAG "AuraBridge"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static LiquidRenderer *g_renderer = nullptr;

extern "C" {

JNIEXPORT jint JNICALL
Java_com_aura_bridge_AuraBridge_initializeBridge(JNIEnv *env, jobject thiz) {
  LOGI("[Aura Bridge] Initializing JNI Connection...");
  if (g_renderer == nullptr) {
    g_renderer = new LiquidRenderer();
  }
  return (jint)aura_kernel_init();
}

JNIEXPORT jstring JNICALL
Java_com_aura_bridge_AuraBridge_getKernelVersion(JNIEnv *env, jobject thiz) {
  char *version = aura_kernel_get_version();
  jstring result = env->NewStringUTF(version);
  aura_kernel_free_string(version);
  return result;
}

JNIEXPORT void JNICALL Java_com_aura_bridge_AuraBridge_startLiquidIsland(
    JNIEnv *env, jobject thiz, jobject surface) {
  ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
  if (window != nullptr && g_renderer != nullptr) {
    LOGI("[Aura Bridge] Initializing Vulkan Renderer on Native Window...");
    if (g_renderer->init(window)) {
      LOGI("[Aura Bridge] Vulkan Renderer Ready!");
      // In a real app, you'd start a render thread here.
      // For now, we just do one frame to verify.
      g_renderer->render();
    }
  } else {
    LOGI("[Aura Bridge] Failed to capture Native Window or Renderer not "
         "initialized.");
  }
}

JNIEXPORT void JNICALL Java_com_aura_bridge_AuraBridge_updateIslandState(
    JNIEnv *env, jobject thiz, jfloat width, jfloat height, jfloat x, jfloat y,
    jfloat cornerRadius, jfloat deltaTime) {
  if (g_renderer != nullptr) {
    IslandState target = {width, height, x, y, cornerRadius};
    g_renderer->updateState(target, deltaTime);
  }
}
}
