cbuffer cbPerObject: register(b0)
{
	float4x4 worldViewProjection;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VOut VShader(float3 position : POSITION, float4 color : COLOR)
{
    VOut output;

    output.position = mul(float4(position,1.0f), worldViewProjection);
	//output.position = float4(position, 1.0f);
	//color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	output.color = color;

    return output;
}