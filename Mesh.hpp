#ifndef NU_MESH_HPP
#define NU_MESH_HPP

#include "Math/Vector3.hpp"

#include <vector>

namespace nu
{

struct Mesh 
{
	struct Part 
	{
		uint32_t vertexOffset;
		uint32_t vertexCount;
	};

	std::vector<float> data;
	std::vector<Part> parts;

	uint32_t size() const;

	bool loadFromFile(const char* filename, bool loadNormals, bool loadTexcoords, bool generateTangents, bool unify, uint32_t* vertexStride = nullptr);
 
	private:
		void generateTangentSpaceVectors();
		void calculateTangentAndBitangent(const float* normalData, const Vector3f& faceTangent, const Vector3f& faceBitangent, float* tangentData, float* bitangentData);
};

} // namespace nu

#endif // NU_MESH_HPP