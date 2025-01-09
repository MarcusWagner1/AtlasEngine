#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

/*** 1. Enumerate Instance Layer properties ***/
// // Get number of instance layers
//uint32_t instanceLayerCount;
//// Use second parameter as NULL to return the layer count
//
//vkEnumerateLayerInstanceProperties(&instanceLayerCount, NULL);
//VkLayerProperties* layerProperty = NULL;
//vkEnumerateInstanceLayerProperties(&instanceLayerCount,
//	layerProperty);
//// Get the extensions for each available instance layer
//foreach layerProperty{
//VkExtensionProperties * instanceExtensions;
//res = vkEnumerateInstanceExtensionProperties(layer_name,
//&instanceExtensionCount, instanceExtensions);