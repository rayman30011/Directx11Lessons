#pragma once
#include "framework.h"

struct DirectionLight
{
	DirectionLight()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT3 direction;
	float pad;
};

struct PointLight
{
	PointLight()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT3 position;
	float range;
	XMFLOAT3 att;
	float pad;
};

struct SpotLight
{
	SpotLight()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;

	XMFLOAT3 position;
	float range;

	XMFLOAT3 direction;
	float spot;

	XMFLOAT3 att;
	float pad;
};
