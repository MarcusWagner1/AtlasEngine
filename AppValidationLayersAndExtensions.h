//#pragma once
//#include<vulkan\vulkan.h>
//#include<vector>
//#include<iostream>
//#define GLFW_INCLUDE_VULKAN
//#include<GLFW\glfw3.h>
//class AppValidationLayersAndExtensions{
//public:
//	AppValidationLayersAndExtensions();
//	~AppValidationLayersAndExtensions();
//	const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_LUNARG_standard_validation"};
//	bool checkValidationLayerSupport();
//	std::vector<const char*>getRequiredExtensions(bool isValidationLayersEnabled);
//	// Debug Callback
//	VkDebugReportCallbackEXT callback;
//	void setupDebugCallback(bool isValidationLayersEnabled,
//VkInstance vkInstance);
// void destroy(VkInstance instance, bool isValidationLayersEnabled);
// // Callback
//*pCreateInfo, VkResultcreateDebugReportCallbackEXT(
// VkInstance instance,
// const VkDebugReportCallbackCreateInfoEXT
//const VkAllocationCallbacks* pAllocator,
// VkDebugReportCallbackEXT* pCallback) {
// auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance,
//"vkCreateDebugReportCallbackEXT");
// if (func != nullptr) {
// return func(instance, pCreateInfo, pAllocator, pCallback);
// }
// else {
// returnVK_ERROR_EXTENSION_NOT_PRESENT;
// }
// }
// void DestroyDebugReportCallbackEXT(
// VkInstanceinstance,
// VkDebugReportCallbackEXTcallback,
// constVkAllocationCallbacks* pAllocator) {
// auto func =
//(PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance,
//"vkDestroyDebugReportCallbackEXT");
// if (func != nullptr) {
// func(instance, callback, pAllocator);
// }
// }
//};

//const uint32_t WIDTH = 800;
//const uint32_t HEIGHT = 600;
//
//const std::vector<const char*> valiationLayers = {
//	"VK_LAYER_KHRONOS_validation" // Contains all the standard validation provided by the khronos foundation
//}
//
//bool checkValidationLayerSupport() // check for validation layers support
//{
//	uint32_t layerCount;
//	vkEnumerateInstanceLayerProperties(&layerCount, nullptr); // obtain number of available layers
//	std::vector<VkLayerProperties> availableLayers(layerCount);
//	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()); // using the obtained count, e
//	for (const char* layerName : validationLayers)
//	{
//		bool layerFound = false;
//		for (const auto& layerProperties : availableLayers)
//		{
//			if (strcmp(layerName, layerProperties.layerName) == 0)
//			{
//				layerFound = true;
//				break;
//			}
//		}//Iterates through the propreties for every layer
//	}// Iterates through each layer within the validation layers
//	if (!layerFound)
//	{
//		return false;
//	}
//	return true;
//}
//
//#ifdef NDEBUG
//const bool enableValidationLayers = false;
//#else
//const bool enableValidationLayers = true;
//
//#endif