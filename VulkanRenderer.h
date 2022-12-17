#pragma once

// tell GLFW header to include vulkan header
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include "Utilities.h"

class VulkanRenderer
{
public:
	VulkanRenderer();

	int init(GLFWwindow* window, std::string appName, uint32_t appVersion);
	void cleanup();

	~VulkanRenderer();

private:
	GLFWwindow* window;

	// components
	VkInstance instance;
	VkQueue graphicsQueue;
	struct 
	{
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} mainDevice;

	// create functions
	void createInstance(std::string* appName, uint32_t appVersion);
	void createLogicalDevice();

	void getPhysicalDevice();

	bool checkInstanceExtensionSupport(std::vector<const char*>* extensions);
	bool checkSuitableDevice(VkPhysicalDevice device);
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
};

