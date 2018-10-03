#include "Mesh.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "ThirdParty/tiny_obj_loader.h"

#include <array>

namespace nu 
{

uint32_t Mesh::size() const
{
	return sizeof(float) * data.size();
}

bool Mesh::loadFromFile(const char* filename, bool loadNormals, bool loadTexcoords, bool generateTangents, bool unify, uint32_t* vertexStride)
{
	// Load model
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string error;

	bool result = tinyobj::LoadObj(&attribs, &shapes, &materials, &error, filename);
	if (!result) 
	{
		// TODO : Use Numea Log System
		printf("Could not open the '%s' file\n");
		if (error.size() > 0)
		{
			// TODO : Use Numea Log System
			printf("%s\n", error.c_str());
		}
		return false;
	}

	// Normal vectors and texture coordinates are required to generate tangent and bitangent vectors
	if (!loadNormals || !loadTexcoords) 
	{
		generateTangents = false;
	}

	// Load model data and unify (normalize) its size and position
	float minX = attribs.vertices[0];
	float maxX = attribs.vertices[0];
	float minY = attribs.vertices[1];
	float maxY = attribs.vertices[1];
	float minZ = attribs.vertices[2];
	float maxZ = attribs.vertices[2];

	data.clear();
	parts.clear();
		
	uint32_t offset = 0;
	for (auto& shape : shapes)
	{
		uint32_t partOffset = offset;

		for (auto& index : shape.mesh.indices) 
		{
			data.emplace_back(attribs.vertices[3 * index.vertex_index + 0]);
			data.emplace_back(attribs.vertices[3 * index.vertex_index + 1]);
			data.emplace_back(attribs.vertices[3 * index.vertex_index + 2]);
			++offset;

			if (loadNormals) 
			{
				if (attribs.normals.size() == 0) 
				{
					// TODO : Use Numea Log System
					printf("Could not load normal vectors data in the '%s' file\n", filename);
					return false;
				}
				else 
				{
					data.emplace_back(attribs.normals[3 * index.normal_index + 0]);
					data.emplace_back(attribs.normals[3 * index.normal_index + 1]);
					data.emplace_back(attribs.normals[3 * index.normal_index + 2]);
				}
			}

			if (loadTexcoords) 
			{
				if (attribs.texcoords.size() == 0) 
				{
					// TODO : Use Numea Log System
					printf("Could not load texture coordinates data in the '%s' file\n", filename);
					return false;
				}
				else 
				{
					data.emplace_back(attribs.texcoords[2 * index.texcoord_index + 0]);
					data.emplace_back(attribs.texcoords[2 * index.texcoord_index + 1]);
				}
			}

			if (generateTangents) 
			{
				// Insert temporary tangent space vectors data
				for (int i = 0; i < 6; ++i) 
				{
					data.emplace_back(0.0f);
				}
			}

			if (unify) 
			{
				if (attribs.vertices[3 * index.vertex_index + 0] < minX) 
				{
					minX = attribs.vertices[3 * index.vertex_index + 0];
				}
				if (attribs.vertices[3 * index.vertex_index + 0] > maxX) 
				{
					maxX = attribs.vertices[3 * index.vertex_index + 0];
				}
				if (attribs.vertices[3 * index.vertex_index + 1] < minY) 
				{
					minY = attribs.vertices[3 * index.vertex_index + 1];
				}
				if (attribs.vertices[3 * index.vertex_index + 1] > maxY) 
				{
					maxY = attribs.vertices[3 * index.vertex_index + 1];
				}
				if (attribs.vertices[3 * index.vertex_index + 2] < minZ) 
				{
					minZ = attribs.vertices[3 * index.vertex_index + 2];
				}
				if (attribs.vertices[3 * index.vertex_index + 2] > maxZ) 
				{
					maxZ = attribs.vertices[3 * index.vertex_index + 2];
				}
			}
		}

		uint32_t partVertexCount = offset - partOffset;
		if (partVertexCount > 0) 
		{
			parts.push_back({ partOffset, partVertexCount });
		}
	}

	uint32_t stride = 3 + (loadNormals ? 3 : 0) + (loadTexcoords ? 2 : 0) + (generateTangents ? 6 : 0);
	if (vertexStride) 
	{
		*vertexStride = stride * sizeof(float);
	}

	if (generateTangents) 
	{
		generateTangentSpaceVectors();
	}

	if (unify) 
	{
		float offsetX = 0.5f * (minX + maxX);
		float offsetY = 0.5f * (minY + maxY);
		float offsetZ = 0.5f * (minZ + maxZ);
		float scaleX = abs(minX - offsetX) > abs(maxX - offsetX) ? abs(minX - offsetX) : abs(maxX - offsetX);
		float scaleY = abs(minY - offsetY) > abs(maxY - offsetY) ? abs(minY - offsetY) : abs(maxY - offsetY);
		float scaleZ = abs(minZ - offsetZ) > abs(maxZ - offsetZ) ? abs(minZ - offsetZ) : abs(maxZ - offsetZ);
		float scale = scaleX > scaleY ? scaleX : scaleY;
		scale = scaleZ > scale ? 1.0f / scaleZ : 1.0f / scale;

		for (size_t i = 0; i < data.size() - 2; i += stride) 
		{
			data[i + 0] = scale * (data[i + 0] - offsetX);
			data[i + 1] = scale * (data[i + 1] - offsetY);
			data[i + 2] = scale * (data[i + 2] - offsetZ);
		}
	}

	return true;
}

void Mesh::generateTangentSpaceVectors()
{
	const size_t normalOffset = 3;
	const size_t texcoordOffset = 6;
	const size_t tangentOffset = 8;
	const size_t bitangentOffset = 11;
	const size_t stride = bitangentOffset + 3;

	for (auto& part : parts) 
	{
		for (size_t i = 0; i < data.size(); i += stride * 3) 
		{
			size_t i1 = i;
			size_t i2 = i1 + stride;
			size_t i3 = i2 + stride;
			const Vector3f v1 = { data[i1], data[i1 + 1], data[i1 + 2] };
			const Vector3f v2 = { data[i2], data[i2 + 1], data[i2 + 2] };
			const Vector3f v3 = { data[i3], data[i3 + 1], data[i3 + 2] };

			const std::array<float, 2> w1 = { data[i1 + texcoordOffset], data[i1 + texcoordOffset + 1] };
			const std::array<float, 2> w2 = { data[i2 + texcoordOffset], data[i2 + texcoordOffset + 1] };
			const std::array<float, 2> w3 = { data[i3 + texcoordOffset], data[i3 + texcoordOffset + 1] };

			float x1 = v2[0] - v1[0];
			float x2 = v3[0] - v1[0];
			float y1 = v2[1] - v1[1];
			float y2 = v3[1] - v1[1];
			float z1 = v2[2] - v1[2];
			float z2 = v3[2] - v1[2];

			float s1 = w2[0] - w1[0];
			float s2 = w3[0] - w1[0];
			float t1 = w2[1] - w1[1];
			float t2 = w3[1] - w1[1];

			float r = 1.0f / (s1 * t2 - s2 * t1);
			const Vector3f faceTangent = { (t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r };
			const Vector3f faceBitangent = { (s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r };

			calculateTangentAndBitangent(&data[i1 + normalOffset], faceTangent, faceBitangent, &data[i1 + tangentOffset], &data[i1 + bitangentOffset]);
			calculateTangentAndBitangent(&data[i2 + normalOffset], faceTangent, faceBitangent, &data[i2 + tangentOffset], &data[i2 + bitangentOffset]);
			calculateTangentAndBitangent(&data[i3 + normalOffset], faceTangent, faceBitangent, &data[i3 + tangentOffset], &data[i3 + bitangentOffset]);
		}
	}
}

void Mesh::calculateTangentAndBitangent(const float* normalData, const Vector3f& faceTangent, const Vector3f& faceBitangent, float* tangentData, float* bitangentData)
{
	// Gram-Schmidt orthogonalize
	const Vector3f normal = { normalData[0], normalData[1], normalData[2] };
	const Vector3f tangent = (faceTangent - normal * normal.dotProduct(faceTangent)).normalized();

	// Calculate handedness
	float handedness = (normal.crossProduct(tangent).dotProduct(faceBitangent) < 0.0f) ? -1.0f : 1.0f;

	const Vector3f bitangent = handedness * normal.crossProduct(tangent);

	tangentData[0] = tangent[0];
	tangentData[1] = tangent[1];
	tangentData[2] = tangent[2];

	bitangentData[0] = bitangent[0];
	bitangentData[1] = bitangent[1];
	bitangentData[2] = bitangent[2];
}

} // namespace VulkanCookbook
