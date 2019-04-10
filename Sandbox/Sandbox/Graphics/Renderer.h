#pragma once

#include <DirectXMath.h>
#include <vector>

struct Mesh_Vertex
{
	Mesh_Vertex() {}
	Mesh_Vertex(const DirectX::XMFLOAT3& p,
		const DirectX::XMFLOAT3& n,
		const DirectX::XMFLOAT3& t,
		const DirectX::XMFLOAT2& uv) : position(p), normal(n),UV(uv)
	{

	}

   DirectX::XMFLOAT3 position;
   DirectX::XMFLOAT3 normal;
   DirectX::XMFLOAT3 tangent;
   DirectX::XMFLOAT2 UV;
};

struct MeshData
{
	std::vector<Mesh_Vertex> vertices;
	std::vector<unsigned> indices;
};

namespace MeshGenerator
{
	
	MeshData GenerateSphere(float radius, unsigned sliceCount, unsigned stackCount);
		
}