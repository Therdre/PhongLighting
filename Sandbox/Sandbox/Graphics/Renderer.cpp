#include "Renderer.h"
#include <cmath>

using namespace DirectX;

namespace MeshGenerator
{
	MeshData GenerateSphere(float radius, unsigned sliceCount, unsigned stackCount)
	{
		MeshData data;

		data.vertices.push_back(Mesh_Vertex(XMFLOAT3(0.0f,radius,0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f,0.0f)));
		float phiStep = XM_PI / (float)stackCount;
		float thetaStep = 2.0f*XM_PI / (float)sliceCount;

		for (int i = 1;i < stackCount;++i)
		{
			float phi = (float)i*phiStep;
			for (int j = 0;j <= sliceCount;++j)
			{
				float theta = (float)j*thetaStep;

				float sinPhi = sinf(phi);
				float cosTheta = cosf(theta);
				float cosPhi = cosf(phi);
				float sinTheta = sinf(theta);
				XMFLOAT3 p(radius*sinPhi*cosTheta,radius*cosPhi,radius*sinPhi*sinTheta);
				XMVECTOR t = XMVector3Normalize(XMVectorSet(-radius * sinPhi*sinTheta, 0.0f, radius*sinPhi*cosTheta, 0.0f));
				XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&p));

				XMFLOAT2 uv(theta/(XM_PI*2.0f),phi/ XM_PI);

				Mesh_Vertex vertex;
				vertex.position = p;
				XMStoreFloat3(&vertex.normal, normal);
				XMStoreFloat3(&vertex.tangent, t);
				vertex.UV = uv;
				data.vertices.push_back(vertex);
			}
		}

		data.vertices.push_back(Mesh_Vertex(XMFLOAT3(0.0f, -radius, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)));

		for (int i = 1;i <= sliceCount;++i)
		{
			data.indices.push_back(0);
			data.indices.push_back(i+1);
			data.indices.push_back(i);
		}

		int baseIndex = 1;
		int ringVertexCount = sliceCount + 1;

		for (int i = 0; i < stackCount - 2; i++)
		{
			for (int j = 0; j < sliceCount; j++)
			{
				data.indices.push_back(baseIndex + i * ringVertexCount + j);
				data.indices.push_back(baseIndex + i * ringVertexCount + j + 1);
				data.indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

				data.indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
				data.indices.push_back(baseIndex + i * ringVertexCount + j + 1);
				data.indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
			}
		}

		int southPoleIndex = data.vertices.size() - 1;
		baseIndex = southPoleIndex - ringVertexCount;
		for (int i = 0; i < sliceCount; i++)
		{
			data.indices.push_back(southPoleIndex);
			data.indices.push_back(baseIndex + i);
			data.indices.push_back(baseIndex + i + 1);
		}

		return data;
	}
}