#pragma once
#include "stdafx.h"
class FVector {
public:
	float x;
	float y;
	float z;
};
class FVector4 {
public:
	float x;
	float y;
	float z;
	float w;
};
class FTransform {
public:
	FVector4 Rotation;
	FVector Location;
	FVector Scale3D;
};
class FColor {
public:
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t A;
};

struct StaticMeshInfo
{
	std::string StaticMeshName;
	int32_t NumVertices;
	int32_t NumLOD;
	int32_t NumTriangles;
	int32_t NumIndices;
	std::vector<FVector> Vertices;
	std::vector<uint32_t> Indices;
	std::vector<FColor> ColorVertices;
	std::vector<glm::vec2> UV;
	std::vector<glm::vec4> TangentX;
};

struct ActorStruct {
	std::string ActorName;
	std::vector<FTransform> Transform;
	std::vector <std::string> StaticMeshAssetName;
};
struct Vertex
{
	glm::vec3 Pos;
	glm::vec4 Color;
	glm::vec3 Normal;
	glm::vec2 TexC;
	glm::vec4 TangentU;
};

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

};