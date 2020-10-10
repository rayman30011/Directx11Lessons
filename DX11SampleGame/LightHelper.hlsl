struct DirectionLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float pad;
};

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Position;
    float Range;
    float3 Att;
    float pad;
};

struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Position;
    float Range;
    float3 Direction;
    float Spot;
    float3 Att;
    float pad;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
};


float toonDiffuse(float val)
{
    if (val <= 0.0)
        return 0.4;
    else if (val <= 0.5)
        return 0.6;
    return 1.f;
}

float toonSpecular(float val)
{
    if (val <= 0.1)
        return 0.0;
    else if (val < 0.8)
        return 0.5;
    return 0.8f;
}


void ComputeDirectionLight(Material material, DirectionLight light, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float3 lightVec = -light.Direction;
    ambient = material.Ambient * light.Ambient;
	
    float diffuseFactor = dot(lightVec, normal);

	[flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), material.Specular.w);
        diffuse = diffuseFactor * material.Diffuse * light.Diffuse;
        spec = specFactor * material.Specular * light.Specular;
    }
}
	

void ComputePointLight(Material material, PointLight light, float3 position, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float3 lightVec = light.Position - position;
    float d = length(lightVec);
    if (d > light.Range)
        return;
		
    lightVec /= d;

    ambient = material.Ambient * light.Ambient;
    float difuseFactor = dot(lightVec, normal);
	
	[flatten]
    if (difuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), material.Specular.w);
		
        diffuse = difuseFactor * material.Diffuse * light.Diffuse;
        spec = specFactor * material.Specular * light.Specular;
    }
	
    float att = 1.0f / dot(light.Att, float3(1.0f, d, d * d));
    diffuse *= att;
    spec *= att;
}

void ComputeSpotLight(Material material, SpotLight light, float3 position, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
    float3 lightVec = light.Position - position;
    float d = length(lightVec);
    if (d > light.Range)
        return;
	
    lightVec /= d;
    ambient = material.Ambient * light.Ambient;
    float difuseFactor = dot(lightVec, normal);
	[flatten]
    if (difuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), material.Specular.w);
		
        diffuse = difuseFactor * material.Diffuse * light.Diffuse;
        spec = specFactor * material.Specular * light.Specular;
    }
	
    float spot = pow(max(dot(-lightVec, light.Direction), 0.0f), light.Spot);
    float att = spot / dot(light.Att, float3(1.0f, d, d * d));
    ambient *= spot;
    diffuse *= att;
    spec *= att;
}