#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

const char* validation_layers[] = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const int enable_validation_layers = 0;
#else
const int enable_validation_layers = 1;
#endif

VkInstance instance;
VkPhysicalDevice physical_device = VK_NULL_HANDLE;
VkDevice device;
VkQueue graphics_queue;

int check_validation_layer_support()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties available_layers[layer_count];
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

    for (int i = 0; i < sizeof(validation_layers) / sizeof(validation_layers[0]); i++) {
        int layer_found = 0;
        for (int j = 0; j < layer_count; j++) {
            if (strcmp(validation_layers[i], available_layers[j].layerName) == 0) {
                layer_found = 1;
                break;
            }
        }
        if (!layer_found) return 0;
    }
    return 1;
}

void create_instance()
{
    if (enable_validation_layers && !check_validation_layer_support()) {
        fprintf(stderr, "Validation layers requested but not available.\n");
        exit(1);
    }

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "vulkan_triangle";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "no_engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    if (enable_validation_layers) {
        create_info.enabledLayerCount = sizeof(validation_layers) / sizeof(validation_layers[0]);
        create_info.ppEnabledLayerNames = validation_layers;
    } else {
        create_info.enabledLayerCount = 0;
    }

    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;

    if (vkCreateInstance(&create_info, NULL, &instance) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create Vulkan instance.\n");
        exit(1);
    }
}

void pick_physical_device()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);

    if (device_count == 0) {
        fprintf(stderr, "Failed to find GPUs with Vulkan support.\n");
        exit(1);
    }

    VkPhysicalDevice devices[device_count];
    vkEnumeratePhysicalDevices(instance, &device_count, devices);

    for (uint32_t i = 0; i < device_count; i++) {
        physical_device = devices[i];
        break; // Just pick the first GPU
    }

    if (physical_device == VK_NULL_HANDLE) {
        fprintf(stderr, "Failed to find a suitable GPU.\n");
        exit(1);
    }
}

void create_logical_device() {
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = 0; // Assume the first queue family supports graphics
    queue_create_info.queueCount = 1;
    float queue_priority = 1.0f;
    queue_create_info.pQueuePriorities = &queue_priority;

    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = &queue_create_info;
    create_info.queueCreateInfoCount = 1;

    if (vkCreateDevice(physical_device, &create_info, NULL, &device) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create logical device.\n");
        exit(1);
    }

    vkGetDeviceQueue(device, 0, 0, &graphics_queue);
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        return -1;
    }

    create_instance();
    pick_physical_device();
    create_logical_device();

    printf("Vulkan protocol initialized successfully.\n");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Magi", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create window.\n");
        vkDestroyDevice(device, NULL);
        vkDestroyInstance(instance, NULL);
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);

    glfwTerminate();
    return 0;
}
