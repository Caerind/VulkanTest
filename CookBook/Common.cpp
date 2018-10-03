// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and / or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The below copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// Vulkan Cookbook
// ISBN: 9781786468154
// � Packt Publishing Limited
//
// Author:   Pawel Lapinski
// LinkedIn: https://www.linkedin.com/in/pawel-lapinski-84522329
//
// Common

#include "Common.h"

#include <fstream>
#include <iostream>
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