#include "LightHelper.hlsl"

cbuffer cbPerObject
{
    float4x4 gWorld;
    float4x4 gWorldInvTranspose;
	matrix worldViewProj;
    float4x4 gTexTransform;
    Material gMaterial;
};

cbuffer cbPerFrame
{
	DirectionLight gDirLight; 
	PointLight gPointLight;
	SpotLight gSpotLight;
	float3 gEyePosW;
};

struct VertexIn 
{
	float3 Position: POSITION;
	float3 Normal: NORMAL;
    float2 Tex : TEXCOORD;
};

struct VertexOut 
{
	float4 PositionH: SV_POSITION;
	float3 PositionW: POSITIONT;
    float3 NormalW : NORMAL;
    float2 Tex : TEXCOORD;
};

Texture2D gDiffuseMap;

SamplerState samAnisontropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 1;

};

VertexOut VS(VertexIn vertex)
{
	VertexOut result;
	result.PositionW = mul(float4(vertex.Position, 1.0f), gWorld).xyz;
    result.NormalW = mul(vertex.Normal, (float3x3)gWorldInvTranspose);
    result.PositionH = mul(float4(vertex.Position, 1.0f), worldViewProj);
    result.Tex = mul(float4(vertex.Tex, 0.0f, 1.0f), gTexTransform).xy;
    
	return result;
}

float4 PS(VertexOut vertex) : SV_Target
{
    float4 texColor = gDiffuseMap.Sample(samAnisontropic, vertex.Tex);
    
    vertex.NormalW = normalize(vertex.NormalW);
    float3 toEyeW = normalize(gEyePosW - vertex.PositionW);
	
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
    float4 A, D, S;
	
    ComputeDirectionLight(gMaterial, gDirLight, vertex.NormalW, toEyeW, A, D, S);
    ambient += A;
    diffuse += D;
    spec += S;
	
    ComputePointLight(gMaterial, gPointLight, vertex.PositionW, vertex.NormalW, toEyeW, A, D, S);
    ambient += A;
    diffuse += D;
    spec += S;
	
    ComputeSpotLight(gMaterial, gSpotLight, vertex.PositionW, vertex.NormalW, toEyeW, A, D, S);
    ambient += A;
    diffuse += D;
    spec += S;
    
    float4 litColor = texColor * (ambient + diffuse) + spec;
    return litColor;
}

technique11 ColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}