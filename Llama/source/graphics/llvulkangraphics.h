#pragma once

#include "graphics/llgraphics.h"

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

namespace llama
{
    inline const char* vkResultToString(vk::Result result)
    {
        switch (result)
        {
        case vk::Result::eSuccess                                       : return "VK_SUCCESS";
        case vk::Result::eNotReady                                      : return "VK_NOT_READY";
        case vk::Result::eTimeout                                       : return "VK_TIMEOUT";
        case vk::Result::eEventSet                                      : return "VK_EVENT_SET";
        case vk::Result::eEventReset                                    : return "VK_EVENT_RESET";
        case vk::Result::eIncomplete                                    : return "VK_INCOMPLETE";
        case vk::Result::eErrorOutOfHostMemory                          : return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case vk::Result::eErrorOutOfDeviceMemory                        : return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case vk::Result::eErrorInitializationFailed                     : return "VK_ERROR_INITIALIZATION_FAILED";
        case vk::Result::eErrorDeviceLost                               : return "VK_ERROR_DEVICE_LOST";
        case vk::Result::eErrorMemoryMapFailed                          : return "VK_ERROR_MEMORY_MAP_FAILED";
        case vk::Result::eErrorLayerNotPresent                          : return "VK_ERROR_LAYER_NOT_PRESENT";
        case vk::Result::eErrorExtensionNotPresent                      : return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case vk::Result::eErrorFeatureNotPresent                        : return "VK_ERROR_FEATURE_NOT_PRESENT";
        case vk::Result::eErrorIncompatibleDriver                       : return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case vk::Result::eErrorTooManyObjects                           : return "VK_ERROR_TOO_MANY_OBJECTS";
        case vk::Result::eErrorFormatNotSupported                       : return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case vk::Result::eErrorFragmentedPool                           : return "VK_ERROR_FRAGMENTED_POOL";
        case vk::Result::eErrorUnknown                                  : return "VK_ERROR_UNKNOWN";
        case vk::Result::eErrorOutOfPoolMemory                          : return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case vk::Result::eErrorInvalidExternalHandle                    : return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case vk::Result::eErrorFragmentation                            : return "VK_ERROR_FRAGMENTATION";
        case vk::Result::eErrorInvalidOpaqueCaptureAddress              : return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case vk::Result::eErrorSurfaceLostKHR                           : return "VK_ERROR_SURFACE_LOST_KHR";
        case vk::Result::eErrorNativeWindowInUseKHR                     : return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case vk::Result::eSuboptimalKHR                                 : return "VK_SUBOPTIMAL_KHR";
        case vk::Result::eErrorOutOfDateKHR                             : return "VK_ERROR_OUT_OF_DATE_KHR";
        case vk::Result::eErrorIncompatibleDisplayKHR                   : return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case vk::Result::eErrorValidationFailedEXT                      : return "VK_ERROR_VALIDATION_FAILED_EXT";
        case vk::Result::eErrorInvalidShaderNV                          : return "VK_ERROR_INVALID_SHADER_NV";
        case vk::Result::eErrorIncompatibleVersionKHR                   : return "VK_ERROR_INCOMPATIBLE_VERSION_KHR";
        case vk::Result::eErrorInvalidDrmFormatModifierPlaneLayoutEXT   : return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case vk::Result::eErrorNotPermittedEXT                          : return "VK_ERROR_NOT_PERMITTED_EXT";
        case vk::Result::eErrorFullScreenExclusiveModeLostEXT           : return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case vk::Result::eThreadIdleKHR                                 : return "VK_THREAD_IDLE_KHR";
        case vk::Result::eThreadDoneKHR                                 : return "VK_THREAD_DONE_KHR";
        case vk::Result::eOperationDeferredKHR                          : return "VK_OPERATION_DEFERRED_KHR";
        case vk::Result::eOperationNotDeferredKHR                       : return "VK_OPERATION_NOT_DEFERRED_KHR";
        case vk::Result::eErrorPipelineCompileRequiredEXT               : return "VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT";
        }

        return "UNKNOWN ERROR";
    }

    template <typename ReturnType>
    inline bool assert_vulkan(vk::ResultValue<ReturnType>&& result, ReturnType& output, llama::DebugInfo debugInfo, std::string_view message, llama::Color color = Colors::RED)
    {
        if (result.result == vk::Result::eSuccess)
        {
            output = std::move(result.value);
            return true;
        }

        logfile()->print(color, debugInfo, "%*s (%s)", message.size(), message.data(), vkResultToString(result.result));
        return false;
    }

    struct VulkanQueue
    {
        vk::Queue queueHandle;
        uint32_t queueFamily;
    };

    class GraphicsDevice_IVulkan : public GraphicsDevice_T
    {
    public:

        GraphicsDevice_IVulkan();

        ~GraphicsDevice_IVulkan() override;

        bool createVulkanInstance(std::initializer_list<std::string_view> instanceLayers,
                                  std::initializer_list<std::string_view> instanceExtensions);

        bool createVulkanDebugUtilsMessenger();

        bool createVulkanPhysicalDevice();

        bool createVulkanLogicalDevice(std::initializer_list<std::string_view> deviceLayers,
                                       std::initializer_list<std::string_view> deviceExtensions);

    private:

        class QueueManager
        {
        public:

            QueueManager(vk::PhysicalDevice physicalDevice);

            bool requestQueue(vk::QueueFlags flags, bool needsPresent, VulkanQueue* future);

            std::vector<vk::DeviceQueueCreateInfo> getQueueInformation() const;

        private:

            std::vector<vk::QueueFamilyProperties> m_properties;
            std::vector<uint8_t> m_queueCounts;
            std::vector<std::pair<VulkanQueue*, uint32_t>> m_outputQueues;
        };

        static VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                      VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                      void* pUserData);

        static constexpr std::array<float, 16> s_vulkanQueuePriorities{1.0f};

        vk::DynamicLoader m_vulkanLoader;
        vk::UniqueInstance m_vulkanInstance;
        vk::DispatchLoaderDynamic m_dynamicLoader;
        vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> m_debugMessenger;
        vk::PhysicalDevice m_physicalDevice;
        vk::UniqueDevice m_logicalDevice;

        VulkanQueue m_graphicsQueue;
        VulkanQueue m_transferQueue;
    };
}