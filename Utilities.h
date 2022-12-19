#pragma once

const std::vector<const char*> requiredDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

// indices of locations of queue families if they exist
struct QueueFamilyIndices {
	int graphicsFamily = -1;		// locations of graphics
	int presentationFamily = -1;	// location of presentation queue family

	bool isValid()
	{
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};