#include "VulkanRenderer.h"
#include "Utilities.h"

VulkanRenderer::VulkanRenderer()
{
}

bool VulkanRenderer::init(GLFWwindow& newWindow, const std::string& appName, const uint32_t& appVersion)
{
	window = &newWindow;

	if (!createInstance(appName, appVersion, &instance))
	{
		printf("error. unable to create vulkan instance\n");
		return false;
	}

	if (!createSurface(window, instance, &surface))
	{
		printf("error. unable to create vulkan surface\n");
		return false;
	}

	if (!getPhysicalDevice(instance, mainDevice.physicalDevice))
	{
		printf("error. unable to get vulkan physical device\n");
		return false;
	}

	if (!createLogicalDevice(mainDevice.physicalDevice, &mainDevice.logicalDevice))
	{
		printf("error. unable to create vulkan logical device\n");
		return false;
	}

	return true;
}

void VulkanRenderer::cleanup()
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);
}

VulkanRenderer::~VulkanRenderer()
{
}

bool VulkanRenderer::createInstance(const std::string& appName, const uint32_t& appVersion, VkInstance* instance)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName.c_str();
	appInfo.applicationVersion = appVersion;
	appInfo.pEngineName = "Custom Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// create list of holding instance extensions
	auto instanceExtensions = std::vector<const char*>();

	// setup extensions instance will use
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (size_t i = 0; i < glfwExtensionCount; ++i)
	{
		instanceExtensions.push_back(glfwExtensions[i]);
	}

	if (!checkInstanceExtensionSupport(instanceExtensions))
	{
		printf("Error: vkInstance does not support required extensions");
		return false;
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	// TODO: setup validation layers
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;

	// create instance
	return vkCreateInstance(&createInfo, nullptr, instance) == VkResult::VK_SUCCESS;
}

bool VulkanRenderer::createLogicalDevice(const VkPhysicalDevice& physicalDevice, VkDevice* logicalDevice)
{
	QueueFamilyIndices queues = getQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> queueFamilyIndices = { queues.graphicsFamily, queues.presentationFamily };

	for (const int queueFamilyIndex : queueFamilyIndices)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		float priority = 1.0f; // highest priority
		queueCreateInfo.pQueuePriorities = &priority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

	VkPhysicalDeviceFeatures features = {};
	deviceCreateInfo.pEnabledFeatures = &features;

	VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, logicalDevice);
	
	if (result != VkResult::VK_SUCCESS)
	{
		return false;
	}

	// queues are created at the same time as device
	vkGetDeviceQueue(*logicalDevice, queues.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(*logicalDevice, queues.presentationFamily, 0, &presentationQueue);
	
	return true;
}

bool VulkanRenderer::createSurface(GLFWwindow* window, const VkInstance& instance, VkSurfaceKHR* surface)
{
	VkResult result = glfwCreateWindowSurface(instance, window, nullptr, surface);

	return result == VK_SUCCESS;
}

bool VulkanRenderer::getPhysicalDevice(const VkInstance& instance, VkPhysicalDevice& physicalDevice)
{
	// get list of physical devices
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		printf("Failure. no physical device present");
		return false;
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (auto& device : devices)
	{
		if (checkSuitableDevice(device))
		{
			physicalDevice = device;
			return true;
		}
	}

	return false;
}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>& checkExtensions)
{
	// get number of extension to create array of correct size of extensions
	uint32_t extensionCount = 0;
	VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	if (result != VkResult::VK_SUCCESS)
	{
		return false;
	}

	// create list of VkExtensionProperties
	std::vector<VkExtensionProperties> extensions(extensionCount);
	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	if (result != VkResult::VK_SUCCESS)
	{
		return false;
	}

	for (const auto& checkExtension : checkExtensions)
	{
		bool hasExtension = false;

		for (const auto& extension : extensions)
		{
			if (strcmp(checkExtension, extension.extensionName))
			{
				// extension found
				hasExtension = true;
				break;
			}
		}

		// missing an extension
		if (!hasExtension)
		{
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::checkDeviceExtensionSupport(const VkPhysicalDevice& physicalDevice)
{
	// get device extension count
	uint32_t deviceExtCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtCount, nullptr);
	
	if (deviceExtCount == 0)
	{
		printf("No property extensions exist");
		return false;
	}

	// get device extensions
	std::vector<VkExtensionProperties> deviceExtensions(deviceExtCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtCount, deviceExtensions.data());

	// check for required extensions
	for (const auto & requiredExt : requiredDeviceExtensions)
	{
		bool hasExtension = false;

		for (const auto& ext : deviceExtensions)
		{
			if (strcmp(ext.extensionName, requiredExt) == 0)
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension)
		{
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::checkSuitableDevice(const VkPhysicalDevice& physicalDevice)
{
	/*
	// information about device itself
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);

	// information about what device can do
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(device, &features);
	*/

	QueueFamilyIndices indices = getQueueFamilies(physicalDevice);
	bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice);

	return indices.isValid() && extensionsSupported;
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(const VkPhysicalDevice& physicalDevice)
{
	QueueFamilyIndices indices;

	// get family queues from physical device
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyList.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilyList)
	{
		// find first family queue with graphics queue
		if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		// check if queue family supports presentation
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentationSupport);

		if (queueFamily.queueCount > 0 && presentationSupport)
		{
			indices.presentationFamily = i;
		}

		if (indices.isValid())
		{
			break;
		}

		++i;
	}

	return indices;
}
