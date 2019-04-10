//put in a fx file

struct DirectionalLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 direction;
	float pad;
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 reflection;
};

cbuffer cbPerFrame: register(b0)
{
	float3 gEyePosW;
	float pad;
	Material mat;
	DirectionalLight gDirLight;
	
};

//cbuffer cbPerObject
//{
//	Material mat;
//};

void ComputeDirectionalLight(Material mat, DirectionalLight l, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = -l.direction;
	ambient = mat.ambient*l.ambient;

	float diffuseFactor = dot(lightVec, normal);
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

		diffuse = diffuseFactor * mat.diffuse*l.diffuse;
		spec = specFactor * mat.specular*l.specular;
	}

}

float4 PShader(float4 position : SV_POSITION, float3 posW : POSITION, float3 normal:NORMAL) : SV_TARGET
{
	normal = normalize(normal);

	float3 toEyeW = normalize(gEyePosW - posW);

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 A, D, S;
	ComputeDirectionalLight(mat, gDirLight, normal, toEyeW, A, D, S);

	float4 finalColor = A + D + S;
	finalColor.a = mat.diffuse.a;
	return finalColor;
	//return gDirLight.ambient;
}