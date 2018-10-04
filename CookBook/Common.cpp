#include "Common.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdParty/stb_image.h"

#include <fstream>
#include <cmath>

bool getBinaryFileContents(const std::string& filename, std::vector<unsigned char>& contents)
{
	contents.clear();

	std::ifstream file(filename, std::ios::binary);
	if (file.fail())
	{
		// TODO : Use Numea Log System
		printf("Could not open '%s' file\n", filename.c_str());
		return false;
	}

	std::streampos begin;
	std::streampos end;
	begin = file.tellg();
	file.seekg(0, std::ios::end);
	end = file.tellg();

	if ((end - begin) == 0)
	{
		// TODO : Use Numea Log System
		printf("The '%s' file is empty\n", filename.c_str());
		return false;
	}

	contents.resize(static_cast<size_t>(end - begin));
	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(contents.data()), end - begin);
	file.close();

	return true;
}

bool loadTextureDataFromFile(const std::string& filename, int numRequestedComponents, std::vector<unsigned char>& data, int* width, int* height, int* numComponents, int* dataSize)
{
	int imgWidth = 0;
	int imgHeight = 0;
	int imgNumComponents = 0;
	std::unique_ptr<unsigned char, void(*)(void*)> stbi_data(stbi_load(filename.c_str(), &imgWidth, &imgHeight, &imgNumComponents, numRequestedComponents), stbi_image_free);

	if (!stbi_data || imgWidth <= 0 || imgHeight <= 0 || imgNumComponents <= 0) 
	{
		// TODO : Use Numea Log System
		printf("Could not read image '%s'\nReason : %s\n", filename.c_str(), stbi_failure_reason());
		return false;
	}

	int imgDataSize = imgWidth * imgHeight * (numRequestedComponents > 0 ? numRequestedComponents : imgNumComponents);
	if (dataSize) 
	{
		*dataSize = imgDataSize;
	}
	if (width) 
	{
		*width = imgWidth;
	}
	if (height) 
	{
		*height = imgHeight;
	}
	if (numComponents)
	{
		*numComponents = imgNumComponents;
	}

	data.resize(imgDataSize);
	std::memcpy(data.data(), stbi_data.get(), imgDataSize);

	return true;
}
