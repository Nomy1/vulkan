#define GLFW_INCLUDE_VULKAN

// tell GLFW header to include vulkan header
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <iostream>

#include "VulkanRenderer.h"

int main()
{
	GLFWwindow* window;

	{// create and init window
		int initSuccess = glfwInit();
		if (initSuccess == GLFW_FALSE)
		{
			std::cout << "Failure: Unable to create window" << std::endl;
			return EXIT_FAILURE;
		}

		// window hints must be set before window creation
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(800, 600, "Window Title", nullptr, nullptr);
	}

	VulkanRenderer vulkanRenderer;

	{// create and init renderer
		bool initVulkanSuccess = vulkanRenderer.init(*window, "Game Name", VK_MAKE_VERSION(1, 0, 0));
		if (!initVulkanSuccess)
		{
			return EXIT_FAILURE;
		}
	}

	// run loop until closed
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	// cleanup phase
	vulkanRenderer.cleanup();
	glfwDestroyWindow(window);

	return EXIT_SUCCESS;
}