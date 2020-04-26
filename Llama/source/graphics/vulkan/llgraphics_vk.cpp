#include "llcore.h"

#include "llgraphics_vk.h"

#include "math/llmath.h"

#include <GLFW/glfw3.h>


llama::GraphicsDevice_IVulkan::GraphicsDevice_IVulkan()
{
    std::vector<std::string_view> extensions = { "VK_EXT_debug_utils" };

    

    uint32_t glfwExtensionCount;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; ++i)
        extensions.push_back(glfwExtensions[i]);

    createVulkanInstance({ "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_monitor" }, std::initializer_list<std::string_view>(extensions.data(), extensions.data() + extensions.size()));
    createVulkanDebugUtilsMessenger();
    createVulkanPhysicalDevice();
    createVulkanLogicalDevice({ "VK_LAYER_KHRONOS_validation" }, { "VK_KHR_swapchain" });
    createMemoryAllocator();
}

llama::GraphicsDevice_IVulkan::~GraphicsDevice_IVulkan()
{
    
}

bool llama::GraphicsDevice_IVulkan::createVulkanInstance(std::initializer_list<std::string_view> instanceLayers,
                                                         std::initializer_list<std::string_view> instanceExtensions)
{
    Timestamp start;

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_vulkanLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    std::vector<const char*> enabledLayers;
    std::vector<const char*> enabledExtensions;

    std::vector<vk::LayerProperties> layers;
    std::vector<vk::ExtensionProperties> extensions;

    if (!assert_vulkan(vk::enumerateInstanceLayerProperties(), layers, LLAMA_DEBUG_INFO, "vk::enumerateInstanceLayerProperties() failed! No Layers will be loaded!", Colors::YELLOW))
        goto create_instance;

    for (const auto& a : instanceLayers)
    {
        bool found = false;

        for (const auto& b : layers)
            if (a == b.layerName)
            {
                enabledLayers.push_back(b.layerName);
                logfile()->print(Colors::GREY, "Enabled Vulkan Instance Layer %s (%s)", b.layerName, b.description);
                found = true;
                break;
            }

        if (!found)
            logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Requested Instance Layer %*s is unavailable and thus not loaded", a.size(), a.data());
    }

    if (!assert_vulkan(vk::enumerateInstanceExtensionProperties(nullptr), extensions, LLAMA_DEBUG_INFO, "vk::enumerateInstanceExtensionProperties() failed! No Extensions will be loaded!", Colors::YELLOW))
        goto create_instance;

    for (const auto& a : instanceExtensions)
    {
        bool found = false;

        for (const auto& b : extensions)
            if (a == b.extensionName)
            {
                enabledExtensions.push_back(b.extensionName);
                logfile()->print(Colors::GREY, "Enabled Vulkan Instance Extension %s", b.extensionName);
                found = true;
                break;
            }

        if (!found)
            logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Requested Instance Extension %*s is unavailable and thus not loaded", a.size(), a.data());
    }

    create_instance:

    vk::ApplicationInfo application_info("Llama Application", VK_MAKE_VERSION(1, 0, 0),
                                         "Llama Game Library", VK_MAKE_VERSION(1, 0, 0),
                                         VK_VERSION_1_0);

    vk::InstanceCreateInfo instance_ci({ }, // Flags
                                       &application_info,
                                       static_cast<uint32_t>(enabledLayers.size()), enabledLayers.data(), // Layers
                                       static_cast<uint32_t>(enabledExtensions.size()), enabledExtensions.data() /* Extensions*/);

    if (!assert_vulkan(vk::createInstanceUnique(instance_ci), m_vulkanInstance, LLAMA_DEBUG_INFO, "vk::createInstance() failed! This device may not support Vulkan!"))
        return false;

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vulkanInstance.get());

    logfile()->print(Colors::GREEN, "Created Vulkan Instance! (%s)", duration(start, Timestamp()).c_str());

    return true;
}

bool llama::GraphicsDevice_IVulkan::createVulkanDebugUtilsMessenger()
{
    vk::DebugUtilsMessengerCreateInfoEXT debug_utils_messenger_ci({ }, // Flags
                                                                  vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                                                                  vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | 
                                                                  vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
                                                                  vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                                  vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                                  vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                                                                  &GraphicsDevice_IVulkan::debugCallback, // Callback
                                                                  nullptr /* User Data*/);

    

    if(!assert_vulkan(m_vulkanInstance->createDebugUtilsMessengerEXTUnique(debug_utils_messenger_ci, nullptr), m_debugMessenger,
                      LLAMA_DEBUG_INFO, "vk::Instance::createDebugUtilsMessengerEXTUnique() failed!"))
        return false;

    return true;
}

bool llama::GraphicsDevice_IVulkan::createVulkanPhysicalDevice()
{
    std::vector<vk::PhysicalDevice> devices;

    if (!assert_vulkan(m_vulkanInstance->enumeratePhysicalDevices(), devices, LLAMA_DEBUG_INFO, "vk::Instance::enumeratePhyiscalDevices() failed!"))
        return false;

    if (devices.empty())
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "No Graphics Card found that supports Vulkan!");
        return false;
    }

    std::pair bestScore(devices.end(), 0u);

    Table t("Graphics Devices", Colors::GREY, { "Name", "VRAM (MB)", "Total RAM (MB)" });

    for (auto i = devices.begin(); i != devices.end(); ++i)
    {
        uint32_t score = 0;

        auto properties = i->getProperties();
        auto memoryProperties = i->getMemoryProperties();

        // Only Graphics Cards
        if (properties.deviceType == vk::PhysicalDeviceType::eCpu)
            continue;

        // Higher score if dedicated GPU
        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            score += 10000;

        size_t vram = 0;
        size_t ram = 0;

        
        for (uint32_t j = 0; j < memoryProperties.memoryHeapCount; ++j)
        {
            ram += memoryProperties.memoryHeaps[j].size / 1048576;
            if (memoryProperties.memoryHeaps[j].flags & vk::MemoryHeapFlagBits::eDeviceLocal)
            {
                vram += memoryProperties.memoryHeaps[j].size / 1048576;
                // Higher score if more VRAM
                score += static_cast<uint32_t>(memoryProperties.memoryHeaps[j].size / 1048576);
            }
        }
                

        if (score > bestScore.second)
            bestScore = std::make_pair(i, score);

        t.addRow(Colors::GREY, { properties.deviceName, std::to_string(vram), std::to_string(ram) });
    }

    m_physicalDevice = *bestScore.first;

    logfile()->print(t, true);
    logfile()->print(Colors::ORANGE, "Selecting Device %s", m_physicalDevice.getProperties().deviceName);
    return true;
}

bool llama::GraphicsDevice_IVulkan::createVulkanLogicalDevice(std::initializer_list<std::string_view> deviceLayers,
                                                              std::initializer_list<std::string_view> deviceExtensions)
{
    Timestamp start;

    std::vector<const char*> enabledLayers;
    std::vector<const char*> enabledExtensions;

    std::vector<vk::LayerProperties> layers;
    std::vector<vk::ExtensionProperties> extensions;

    if (!assert_vulkan(m_physicalDevice.enumerateDeviceLayerProperties(), layers, LLAMA_DEBUG_INFO, "vk::PhysicalDevice::enumerateInstanceLayerProperties() failed! No Layers will be loaded!", Colors::YELLOW))
        goto create_device;

    for (const auto& a : deviceLayers)
    {
        bool found = false;

        for (const auto& b : layers)
            if (a == b.layerName)
            {
                enabledLayers.push_back(b.layerName);
                logfile()->print(Colors::GREY, "Enabled Vulkan Device Layer %s (%s)", b.layerName, b.description);
                found = true;
                break;
            }

        if (!found)
            logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Requested Instance Layer %*s is unavailable and thus not loaded", a.size(), a.data());
    }

    if (!assert_vulkan(m_physicalDevice.enumerateDeviceExtensionProperties(nullptr), extensions, LLAMA_DEBUG_INFO, 
                       "vk::PhysicalDevice::enumerateInstanceExtensionProperties() failed! No Extensions will be loaded!", Colors::YELLOW))
        goto create_device;

    for (const auto& a : deviceExtensions)
    {
        bool found = false;

        for (const auto& b : extensions)
            if (a == b.extensionName)
            {
                enabledExtensions.push_back(b.extensionName);
                logfile()->print(Colors::GREY, "Enabled Vulkan Device Extension %s", b.extensionName);
                found = true;
                break;
            }

        if (!found)
            logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Requested Device Extension %*s is unavailable and thus not loaded", a.size(), a.data());
    }

    create_device:
    
    vk::PhysicalDeviceFeatures features = { };

    QueueManager manager(m_physicalDevice, m_vulkanInstance.get());

    manager.requestQueue(vk::QueueFlagBits::eGraphics, true, &m_graphicsQueue);
    manager.requestQueue(vk::QueueFlagBits::eTransfer, false, &m_transferQueue);

    auto queueInfo = manager.getQueueInformation();

    vk::DeviceCreateInfo device_ci({ }, // Flags
                                   static_cast<uint32_t>(queueInfo.size()), queueInfo.data(), // Queues
                                   static_cast<uint32_t>(enabledLayers.size()), enabledLayers.data(), // Layers
                                   static_cast<uint32_t>(enabledExtensions.size()), enabledExtensions.data(), // Extensions
                                   &features);

    if (!assert_vulkan(m_physicalDevice.createDeviceUnique(device_ci), m_logicalDevice, LLAMA_DEBUG_INFO, "vk::createDevice() failed!"))
        return false;

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_logicalDevice.get());

    manager.writeQueues(m_logicalDevice.get());

    return false;
}

bool llama::GraphicsDevice_IVulkan::createMemoryAllocator()
{
    m_functions = vma::VulkanFunctions(VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceProperties,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceMemoryProperties,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkAllocateMemory,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkFreeMemory,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkMapMemory,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkUnmapMemory,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkFlushMappedMemoryRanges,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkInvalidateMappedMemoryRanges,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkBindBufferMemory,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkBindImageMemory,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkGetBufferMemoryRequirements,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkGetImageMemoryRequirements,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateBuffer,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyBuffer,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateImage,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyImage,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkCmdCopyBuffer,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkGetBufferMemoryRequirements2KHR,
                                       VULKAN_HPP_DEFAULT_DISPATCHER.vkGetImageMemoryRequirements2KHR);


    if (!assert_vulkan(vma::createAllocator(vma::AllocatorCreateInfo(vma::AllocatorCreateFlags(), // Flags
                                                                     m_physicalDevice,
                                                                     m_logicalDevice.get(),
                                                                     0, nullptr, nullptr, 0, nullptr, 
                                                                     &m_functions)), m_memoryAllocator, LLAMA_DEBUG_INFO, "vma::createAllocator() failed!"))
        return false;

    return false;
}

VkBool32 llama::GraphicsDevice_IVulkan::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        logfile()->print(Colors::BLUE, "%s (%d): %s", pCallbackData->pMessageIdName, pCallbackData->messageIdNumber, pCallbackData->pMessage); break;

    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        logfile()->print(Colors::YELLOW, "%s (%d): %s", pCallbackData->pMessageIdName, pCallbackData->messageIdNumber, pCallbackData->pMessage); break;

    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        logfile()->print(Colors::RED, "%s (%d): %s", pCallbackData->pMessageIdName, pCallbackData->messageIdNumber, pCallbackData->pMessage); break;

    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        logfile()->print(Colors::ORANGE, "%s (%d): %s", pCallbackData->pMessageIdName, pCallbackData->messageIdNumber, pCallbackData->pMessage); break;

    }

    return VK_FALSE;
}

llama::GraphicsDevice_IVulkan::QueueManager::QueueManager(vk::PhysicalDevice physicalDevice, vk::Instance instance) :
    m_properties(physicalDevice.getQueueFamilyProperties()),
    m_vulkanInstance(instance),
    m_phyiscalDevice(physicalDevice)
{
    m_queueCounts.resize(m_properties.size());
}

bool llama::GraphicsDevice_IVulkan::QueueManager::requestQueue(vk::QueueFlags flags, bool needsPresent, VulkanQueue* future, float priority)
{
    std::pair bestScore(0xffffffff, uint8_t(32));

    for (uint32_t i = 0; i < m_properties.size(); ++i)
    {
        // If all Queues are already in use skip family
        if (m_queueCounts[i].first >= m_properties[i].queueCount)
            continue;

        // If family doesnt have all flags, skip
        if ((flags & ~m_properties[i].queueFlags) != vk::QueueFlags{ })
            continue;

        // If queue needs to present but can't present
        if (needsPresent && glfwGetPhysicalDevicePresentationSupport(m_vulkanInstance, m_phyiscalDevice, i) == VK_FALSE)
            continue;

        uint8_t score = hammingWeight(static_cast<uint32_t>(~flags & m_properties[i].queueFlags));

        if (score < bestScore.second)
            bestScore = std::make_pair(i, score);
    }

    if (bestScore.first != 0xffffffff)
    {
        future->queueFamily = bestScore.first;
        m_outputQueues.push_back(std::make_pair(future, m_queueCounts[bestScore.first].first++));
        m_queueCounts[bestScore.first].second.push_back(priority);
        return true;
    }

    return false;
}

std::vector<vk::DeviceQueueCreateInfo> llama::GraphicsDevice_IVulkan::QueueManager::getQueueInformation() const
{
    std::vector<vk::DeviceQueueCreateInfo> result;

    for (uint32_t i = 0; i < m_queueCounts.size(); ++i)
        if (m_queueCounts[i].first > 0)
            result.push_back(vk::DeviceQueueCreateInfo({ }, i, m_queueCounts[i].first, m_queueCounts[i].second.data()));

    return std::move(result);
}

void llama::GraphicsDevice_IVulkan::QueueManager::writeQueues(vk::Device device)
{
    for (const auto& a : m_outputQueues)
        a.first->queueHandle = device.getQueue(a.first->queueFamily, a.second);
}
