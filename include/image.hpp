#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include <memory>
#include <vector>

struct StImage
{
	std::shared_ptr<std::vector<float>> data;
	std::vector<int> dims;

	// Flag to indicate the data in the data field has changed since the last
	// rendering
	bool changed;

	// Rendering duration of the main buffer
	unsigned long long frameTiming;

	StImage();
};

#endif /* _IMAGE_HPP_ */
