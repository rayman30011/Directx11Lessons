#include "GeometryGenerator.h"

void GeometryGenerator::createGrid(float width, float depth, uint32_t m, uint32_t n, MeshData& meshData)
{
	int vertexCount = m * n;
	int faceCount = (m - 1) * (n - 1) * 2;

	float halfWidth = width / 2.f;
	float halfDepth = depth / 2.f;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.f / (n - 1);
	float dv = 1.f / (m - 1);

	meshData.vertices.resize(vertexCount);
	
	for (size_t i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (size_t j = 0; j < n; ++j)
		{
			float x = -halfDepth + j * dx;
			float y = getHeight(x, z);

			XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			if (y < -10.f)
				color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
			else if (y < 5.f)
				color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
			else if (y < 12.f)
				color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
			else if (y < 20.f)
				color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);

			meshData.vertices[i * n + j].position = XMFLOAT3(x, y, z);
			//meshData.vertices[i * n + j].color = color;

			meshData.vertices[i * n + j].normal = getHillNormal(x, y);
			//meshData.Vertices[i * n + j].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);
			// Ignore for now, used for texturing. 
			meshData.vertices[i*n+j].textureCoords.x = j*du; 
			meshData.vertices[i*n+j].textureCoords.y = i*dv;

		}
	}
	
	meshData.indices.resize(faceCount * 3);

	int k = 0;
	for (size_t i = 0; i < m - 1; ++i)
	{
		for (size_t j = 0; j < n - 1; ++j)
		{
			meshData.indices[k] = i * n + j; 
			meshData.indices[k + 1] = i * n + j + 1;
			meshData.indices[k + 2] = (i + 1) * n + j;
			meshData.indices[k + 3] = (i + 1) * n + j;
			meshData.indices[k + 4] = i * n + j + 1;
			meshData.indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}
}

void GeometryGenerator::createPlane(float width, float depth, uint32_t m, uint32_t n, MeshData& meshData, const XMFLOAT4& color)
{
	int vertexCount = m * n;
	int faceCount = (m - 1) * (n - 1) * 2;

	float halfWidth = width / 2.f;
	float halfDepth = depth / 2.f;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.f / (n - 1);
	float dv = 1.f / (m - 1);

	meshData.vertices.resize(vertexCount);

	for (size_t i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (size_t j = 0; j < n; ++j)
		{
			float x = -halfDepth + j * dx;
			float y = 0;

			meshData.vertices[i * n + j].position = XMFLOAT3(x, y, z);
			//meshData.vertices[i * n + j].color = color;

			// Ignore for now, used for lighting.
			meshData.vertices[i * n + j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			/*meshData.Vertices[i * n + j].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f); */
			// Ignore for now, used for texturing. 
			meshData.vertices[i*n+j].textureCoords.x = j * du;
			meshData.vertices[i*n+j].textureCoords.y = i * dv;
		}
	}

	meshData.indices.resize(faceCount * 3);

	int k = 0;
	for (size_t i = 0; i < m - 1; ++i)
	{
		for (size_t j = 0; j < n - 1; ++j)
		{
			meshData.indices[k] = i * n + j;
			meshData.indices[k + 1] = i * n + j + 1;
			meshData.indices[k + 2] = (i + 1) * n + j;
			meshData.indices[k + 3] = (i + 1) * n + j;
			meshData.indices[k + 4] = i * n + j + 1;
			meshData.indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}
}

void GeometryGenerator::createBox(float width, float height, float depth, const XMFLOAT4& color, MeshData& meshData)
{
	Vertex v[24];

	float w2 = 0.5f*width;
	float h2 = 0.5f*height;
	float d2 = 0.5f*depth;
    
	// Fill in the front face vertex data.
	v[0] = { XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0, 1) };
	v[1] = { XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0, 0) };
	v[2] = { XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1, 0) };
	v[3] = { XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1, 1) };

	// Fill in the back face vertex data.
	v[4] = { XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1, 1) };
	v[5] = { XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0, 1) };
	v[6] = { XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0, 0) };
	v[7] = { XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1, 0) };

	// Fill in the top face vertex data.
	v[8]  = { XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0, 1) };
	v[9]  = { XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0, 0) };
	v[10] = { XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1, 0) };
	v[11] = { XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1, 1) };

	// Fill in the bottom face vertex data.
	v[12] = { XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0, 1) };
	v[13] = { XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0, 0) };
	v[14] = { XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1, 0) };
	v[15] = { XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1, 1) };

	// Fill in the left face vertex data.
	v[16] = { XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0, 1) };
	v[17] = { XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0, 0) };
	v[18] = { XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1, 0) };
	v[19] = { XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1, 1) };

	// Fill in the right face vertex data.
	v[20] = { XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0, 1) };
	v[21] = { XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1, 0) };
	v[22] = { XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0, 0) };
	v[23] = { XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1, 1) };

	meshData.vertices.assign(&v[0], &v[24]);

	UINT i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	meshData.indices.assign(&i[0], &i[36]);
}

void GeometryGenerator::createCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData, const XMFLOAT4& color)
{
	float stackHeight = height / stackCount;
	uint32_t ringCount = stackCount + 1;
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	for (size_t i = 0; i < ringCount; i++)
	{
		float y = -0.5f * height + i * stackHeight;
		float ringRadius = bottomRadius + i * radiusStep;
		float theta = 2.0f * XM_PI / sliceCount;

		for (size_t j = 0; j <= sliceCount; j++)
		{
			Vertex vertex;
			float c = cosf(j * theta);
			float s = sinf(j * theta);

			float dr = bottomRadius - topRadius;
			XMFLOAT3 bitangent(dr * c, -height, dr * s);
			auto tangentU = XMFLOAT3(-s, 0.0f, c);
			XMVECTOR T = XMLoadFloat3(&tangentU);
			XMVECTOR B = XMLoadFloat3(&bitangent);
			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
			XMStoreFloat3(&vertex.normal, N);

			vertex.position = XMFLOAT3(ringRadius * c, y, ringRadius * s);
			meshData.vertices.push_back(vertex);

			vertex.textureCoords.x = (float)j / sliceCount; 
			vertex.textureCoords.y = 1.0f - (float)i / stackCount;
		}
	}

	uint32_t ringVertexCount = sliceCount + 1;
	for (size_t i = 0; i < stackCount; i++)
	{
		for (size_t j = 0; j < sliceCount; j++)
		{
			meshData.indices.push_back(i * ringVertexCount + j); 
			meshData.indices.push_back((i + 1) * ringVertexCount + j);
			meshData.indices.push_back((i + 1) * ringVertexCount + j + 1);
			meshData.indices.push_back(i * ringVertexCount + j);
			meshData.indices.push_back((i + 1) * ringVertexCount + j + 1);
			meshData.indices.push_back(i * ringVertexCount + j + 1);
		}
	}

	buildCylinderTopCap(topRadius, height, sliceCount, stackCount, meshData, color);
	buildCylinderBottomCap(bottomRadius, height, sliceCount, stackCount, meshData, color);
}

void GeometryGenerator::createSphere(float radius, uint32_t subvisions, MeshData& meshData, const XMFLOAT4& color)
{
	const float x = 0.525731f; 
	const float z = 0.850651f;

	XMFLOAT3 pos[12] = 
	{ 
		XMFLOAT3(-x, 0.0f, z), XMFLOAT3(x, 0.0f, z), 
		XMFLOAT3(-x, 0.0f, -z), XMFLOAT3(x, 0.0f, -z), 
		XMFLOAT3(0.0f, z, x), XMFLOAT3(0.0f, z, -x), 
		XMFLOAT3(0.0f, -z, x), XMFLOAT3(0.0f, -z, -x), 
		XMFLOAT3(z, x, 0.0f), XMFLOAT3(-z, x, 0.0f), 
		XMFLOAT3(z, -x, 0.0f), XMFLOAT3(-z, -x, 0.0f) 
	};

	DWORD k[60] = 
	{ 
		1,4,0, 4,9,0, 4,5,9, 
		8,5,4, 1,8,4, 1,10,8, 
		10,3,8, 8,3,5, 3,2,5, 
		3,7,2, 3,10,7, 10,6,7, 
		6,11,7, 6,0,11, 6,1,0, 
		10,1,6, 11,0,9, 2,11,9, 
		5,2,9, 11,2,7 
	};

	meshData.vertices.resize(12); 
	meshData.indices.resize(60);

	for (size_t i = 0; i < 12; i++)
		meshData.vertices[i].position = pos[i];

	for (size_t i = 0; i < 60; i++)
		meshData.indices[i] = k[i];

	for (size_t i = 0; i < subvisions; ++i) 
		subdivide(meshData);

	for (size_t i = 0; i < meshData.vertices.size(); i++)
	{
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&meshData.vertices[i].position));
		XMVECTOR p = radius * n;

		XMStoreFloat3(&meshData.vertices[i].position, p);
		XMStoreFloat3(&meshData.vertices[i].normal, n);

		float theta = Math::angleFromXY(meshData.vertices[i].position.x, meshData.vertices[i].position.z);
		float phi = acos(meshData.vertices[i].position.y / radius);

		meshData.vertices[i].textureCoords.x = theta / XM_2PI; 
		meshData.vertices[i].textureCoords.y = phi / XM_PI; 
		//// Partial derivative of P with respect to theta 
		//meshData.Vertices[i].TangentU.x = -radius*sinf(phi)*sinf(theta);
		//meshData.Vertices[i].TangentU.y = 0.0f; 
		//meshData.Vertices[i].TangentU.z = +radius*sinf(phi)*cosf(theta); 
		//XMVECTOR T = XMLoadFloat3(&meshData.Vertices[i].TangentU); 
		//XMStoreFloat3(&meshData.Vertices[i].TangentU, XMVector3Normalize(T));
	}
}

float GeometryGenerator::getHeight(float x, float z)
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

float GeometryGenerator::updateWawes(float x, float z, float dt)
{
	static float t = 0;
	t += dt;

	return 0.0f;
}

XMFLOAT3 GeometryGenerator::getHillNormal(float x, float z) const
{
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));
	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n)); 
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void GeometryGenerator::buildCylinderTopCap(float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData, const XMFLOAT4& color)
{
	uint32_t baseIndex = meshData.vertices.size();
	float y = 0.5f * height;
	float theta = 2.f * XM_PI / sliceCount;

	for (size_t i = 0; i < sliceCount; i++)
	{
		float x = topRadius * cosf(i * theta);
		float z = topRadius * sinf(i * theta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.vertices.push_back({ XMFLOAT3(x, y, z), XMFLOAT3() });
	}

	uint32_t centerIndex = meshData.vertices.size() - 1;
	for (size_t i = 0; i < sliceCount; i++)
	{ 
		meshData.indices.push_back(centerIndex); 
		meshData.indices.push_back(baseIndex + i + 1); 
		meshData.indices.push_back(baseIndex + i); 
	}
}

void GeometryGenerator::buildCylinderBottomCap(float bottomRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData, const XMFLOAT4& color)
{
	uint32_t baseIndex = meshData.vertices.size();
	float y = 0.5f * height;
	float theta = 2.f * XM_PI / sliceCount;

	for (size_t i = 0; i < sliceCount; i++)
	{
		float x = bottomRadius * cosf(i * theta);
		float z = bottomRadius * sinf(i * theta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.vertices.push_back({ XMFLOAT3(x, -y, z), XMFLOAT3() });
	}

	uint32_t centerIndex = meshData.vertices.size() - 1;
	for (size_t i = 0; i < sliceCount; i++)
	{
		meshData.indices.push_back(centerIndex);
		meshData.indices.push_back(baseIndex + i);
		meshData.indices.push_back(baseIndex + i + 1);
	}
}

void GeometryGenerator::subdivide(MeshData& meshData)
{
	// Save a copy of the input geometry.
	MeshData inputCopy = meshData;

	meshData.vertices.resize(0);
	meshData.indices.resize(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	UINT numTris = inputCopy.indices.size() / 3;
	for (UINT i = 0; i < numTris; ++i)
	{
		Vertex v0 = inputCopy.vertices[inputCopy.indices[i * 3 + 0]];
		Vertex v1 = inputCopy.vertices[inputCopy.indices[i * 3 + 1]];
		Vertex v2 = inputCopy.vertices[inputCopy.indices[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		Vertex m0, m1, m2;

		// For subdivision, we just care about the position component.  We derive the other
		// vertex components in CreateGeosphere.

		m0.position = XMFLOAT3(
			0.5f * (v0.position.x + v1.position.x),
			0.5f * (v0.position.y + v1.position.y),
			0.5f * (v0.position.z + v1.position.z));

		m1.position = XMFLOAT3(
			0.5f * (v1.position.x + v2.position.x),
			0.5f * (v1.position.y + v2.position.y),
			0.5f * (v1.position.z + v2.position.z));

		m2.position = XMFLOAT3(
			0.5f * (v0.position.x + v2.position.x),
			0.5f * (v0.position.y + v2.position.y),
			0.5f * (v0.position.z + v2.position.z));

		//
		// Add new geometry.
		//

		meshData.vertices.push_back(v0); // 0
		meshData.vertices.push_back(v1); // 1
		meshData.vertices.push_back(v2); // 2
		meshData.vertices.push_back(m0); // 3
		meshData.vertices.push_back(m1); // 4
		meshData.vertices.push_back(m2); // 5

		meshData.indices.push_back(i * 6 + 0);
		meshData.indices.push_back(i * 6 + 3);
		meshData.indices.push_back(i * 6 + 5);

		meshData.indices.push_back(i * 6 + 3);
		meshData.indices.push_back(i * 6 + 4);
		meshData.indices.push_back(i * 6 + 5);
 
		meshData.indices.push_back(i * 6 + 5);
		meshData.indices.push_back(i * 6 + 4);
		meshData.indices.push_back(i * 6 + 2);

		meshData.indices.push_back(i * 6 + 3);
		meshData.indices.push_back(i * 6 + 1);
		meshData.indices.push_back(i * 6 + 4);
	}
}
