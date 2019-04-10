cbuffer cbPerObject: register(b0)
{
	float4x4 world;
	float4x4 worldInvTranspose;
	float4x4 worldViewProjection;
};

struct VOut
{
    float4 position : SV_POSITION;
    //float4 color : COLOR;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
};




VOut VShader(float3 position : POSITION, float3 normal : NORMAL)
{
    VOut output;

	output.posW= mul(float4(position, 1.0f), world).xyz;
	output.normalW = mul(normal, (float3x3)worldInvTranspose);
    output.position = mul(float4(position,1.0f), worldViewProjection);
	//output.position = float4(position, 1.0f);
	//color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	//output.color = color;

    return output;
}