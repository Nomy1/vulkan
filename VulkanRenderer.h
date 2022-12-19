#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <set>

#include "Utilities.h"

class VulkanRenderer
{
public:
	VulkanRenderer();

	bool init(GLFWwindow& window, const std::string& appName, const uint32_t& appVersion);
	void cleanup();

	~VulkanRenderer();

private:
	GLFWwindow* window;

	// components
	VkInstance instance;
	VkQueue graphicsQueue;
	VkQueue presentationQueue;
	VkSurfaceKHR surface;
	struct 
	{
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} mainDevice;

	// create functions
	bool createInstance(const std::string& appName, const uint32_t& appVersion, VkInstance* instance);
	bool createLogicalDevice(const VkPhysicalDevice& physicalDevice, VkDevice* logicalDevice);
	bool createSurface(GLFWwindow* window, const VkInstance& instance, VkSurfaceKHR* surface);

	bool getPhysicalDevice(const VkInstance& instance, VkPhysicalDevice& physicalDevice);

	bool checkInstanceExtensionSupport(std::vector<const char*>& extensions);
	bool checkDeviceExtensionSupport(const VkPhysicalDevice& physicalDevice);
	bool checkSuitableDevice(const VkPhysicalDevice& physicalDevice);
	QueueFamilyIndices getQueueFamilies(const VkPhysicalDevice& physicalDevice);
};

 