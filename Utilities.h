#pragma once

#define FAILURE 0
#define SUCCESS 1

typedef enum AppResult : uint8_t
{
	Failure = 0,
	Success = 1,
};

// indices of locations of queue families if they exist
struct QueueFamilyIndices {
	int graphicsFamily = -1;	// locations of graphics
	
	bool isValid()
	{
		return graphicsFamily >= 0;
	}
};