#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define VK_USE_PLATFOM_WIN32_KHR
//#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <iostream>
#include <optional>
#include "AppValidationLayersAndExtensions.h";
#include <cstdlib>
#include <set>

// Sort later, just gonna seperate all the integers over here
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete()
    {

        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};




const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation" // Contains all the standard validation provided by the khronos foundation
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Atlas {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();

    }
private:
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    // Initializes the window
    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }
    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }
    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice(); // picks the physical device
        createLogicalDevice();
    }

    /*queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
       queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
       queueCreateInfo.queueCount = 1;

       queueCreateInfo.pQueuePriorities = &queuePriority;
       VkDeviceCreateInfo createInfo{};*/

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;
       

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());

        }
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }
        return details;
    }

    void createLogicalDevice()
    {

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value()};
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
       
        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("couldnt create logical device");
        }
        
        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
        std::cout << "\n\nCreated Logical Device\n\n";
    }



   

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
    {
        
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::cout << "\n\n\nnumber of queue families is: " << queueFamilyCount << std::endl;
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport)
            {
                indices.presentFamily = i;
            }
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }
            if (indices.isComplete())// && indices.graphicsFamily > 0)
            {
                break;
            }
            i++;
        }
        return indices;
    }


    



    //bool isDeviceSuitable(VkPhysicalDevice device)
    //{
    //    // In here we will check the properties and features of the devices to score and obtain the best GPU
    //    VkPhysicalDeviceProperties properties;
    //    VkPhysicalDeviceFeatures features;
    //    vkGetPhysicalDeviceProperties(device, &properties); // acquires properties
    //    vkGetPhysicalDeviceFeatures(device, &features); // acquires features
    //    if (!(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && features.geometryShader)) // change this for DISCRETE GPU later
    //    {
    //        std::cout << "\n//Couldnt find a graphics card that was discrete gpu and geometry shader//\n";
    //        if (!(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && features.geometryShader))
    //        {
    //            std::cout << "\n//Couldnt find a graphics card that was integrated gpu and geometry shader\n//";
    //            return false;
    //        }
    //        else
    //        {
    //            std::cout << "\n//Found graphics card device with integrated gpu and geometry shader//\n";

    //        }
    //    }
    //    else
    //    {
    //        std::cout << "\n//Found graphics card device with discrete gpu and geometry shader//\n";
    //    }
    //    
    //    //QueueFamilyIndices indices = findQueueFamilies(device);
    //    //return indices.graphicsFamily.has_value(); // will return true or false since we are using <optional>
    //    //return indices.isComplete();
    //    QueueFamilyIndices indices = findQueueFamilies(device);
    //    //std::cout << indices.graphicsFamily.data() << " is the number of graphics families in this device" << std::endl;
    //    return indices.isComplete();
    //    /*if (indices.graphicsFamily > 0)
    //    {
    //        return indices.isComplete();
    //    }
    //    return false;*/
    //    //return indices.isComplete();

    //}
    bool isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    bool checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    void pickPhysicalDevice()
    {
        physicalDevice = VK_NULL_HANDLE; // initialize
        uint32_t deviceCount = 0; // will hold count
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr); // acquire count
        if (deviceCount == 0)
        {
            throw std::runtime_error("No GPU's found with Vulkan support");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount); // creates vector of devices given the size

        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()); // acquires the physical device
        
        for (const auto& device : devices) // iterate through acquired devices
        {
            if (isDeviceSuitable(device)) // this will check and score to finda  good GPU
            {
                if (device != VK_NULL_HANDLE)
                {
                    physicalDevice = device;
                    break;
                }

                
            }
        }
        if (physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Couldnt find suitable GPU");
        }
        //std::cout << "\n//Found a suitable GPU? lets see if it has integrated/discrete graphics and queueFamilies\n//";
    }
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }
    void cleanup() {
        vkDestroyDevice(device, nullptr);

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();

        
    }
    void createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo; // Use the pnext storage to store pointer to whatever this is. come back to, casted to extension pointer, intersting
        }
        else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
        std::cout << "\n\n// Created the vulkan Instance! //\n\n";
    }
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) { // This function is what links to the outputstream displaying any relevant messages when certain cases arise when setting up the engine.
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }
    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
        std::cout << "\n\n// Set up the debug messanger!//\n\n";
    }
    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }
    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
    void createSurface()
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }

    }
};









//bool isDeviceSuitable(VkPhysicalDevice device)
//{
//   /* VkPhysicalDeviceProperties deviceProperties;
//    vkGetPhysicalDeviceProperties(device, &deviceProperties);
//    VkPhysicalDeviceFeatures deviceFeatures;
//    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
//    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;*/
//    return true;
//}

//uint32_t findQueueFamilies(VkPhysicalDevice device)
//{
//    //Insert logic to find graphics queue family
//}





int main()
{
    Atlas app;
    try {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
       }
}








//int main() {
    //
    //    // Initialize window
    //    uint32_t instanceLayerCount = 0;
    //    uint32_t extensionCount = 0;
    //    uint32_t deviceCount = 0;
    //    uint32_t glfwExtensionCount = 0;
    //    glfwInit();
    //    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
    //
    //  /*  VkLayerProperties* layerProperties;
    //    vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);
    //    vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperties);*/    // Queue Families ////////////////////////////////////////////////////////////////////
    //    // Instance ////////////////////////////////////////////////////////////////////
    //
    //    VkInstance instance = VK_NULL_HANDLE;
    //    VkApplicationInfo appInfo{};
    //    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    //    appInfo.pApplicationName = "First Triangle";
    //    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    //    appInfo.pEngineName = "No Engine";
    //    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    //    appInfo.apiVersion = VK_API_VERSION_1_0;
    //
    //    VkInstanceCreateInfo instanceCInfo = {};
    //    
    //    instanceCInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    //    instanceCInfo.pApplicationInfo = &appInfo;
    //
    //    
    //    const char** glfwExtensions;
    //
    //    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    //   
    //    instanceCInfo.ppEnabledExtensionNames = glfwExtensions;
    //    instanceCInfo.enabledLayerCount = 0; //CHange later when validation layers are implemented
    //    instanceCInfo.enabledExtensionCount = glfwExtensionCount;
    //    instanceCInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    //    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    //    std::vector<VkExtensionProperties> extProperties(extensionCount);
    //    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extProperties.data());
    //
    //    std::cout << "Here are the available extensions";
    //
    //    for (const auto extension : extProperties)
    //    {
    //        std::cout << '\t' << extension.extensionName << std::endl;
    //    }
    //
    //
    //    if (vkCreateInstance(&instanceCInfo, nullptr, &instance) != VK_SUCCESS)
    //    {
    //        std::runtime_error("Couldnt create the instance");
    //    }
    //    else
    //    {
    //        std::cout << "Yooooo created the vulkan";
    //    }
    //
    //    // Physical Device ////////////////////////////////////////////////////////////////////
    //
    //
    //    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    //    // deviceCreateInfo = {};
    //    uint32_t deviceCount = 0;
    //    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    //    if (deviceCount == 0)
    //    {
    //        throw std::runtime_error("didnt find GPU with vulkan support");
    //    }
    //    std::vector<VkPhysicalDevice> devices(deviceCount);
    //    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    //    for (const auto& device : devices)
    //    {
    //        if (isDeviceSuitable(device))
    //        {
    //            physicalDevice = device;
    //            break;
    //        }
    //            
    //    }
    //   
    //    // Queue Families ////////////////////////////////////////////////////////////////////
    //
    //    
    //
    //
    //
    //    glfwDestroyWindow(window);
    //
    //    glfwTerminate();
    //
    //    return 0;
    //}