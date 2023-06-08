cbuffer CamCB : register(b1)
{
	float4x4 camViewProjM;
}

struct VSin
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

struct VSout
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
    float4 worldPosition : WorldPosition;
};

VSout main(VSin input)
{
    VSout output;

    output.uv = input.uv;

    //Calculate position of vertex in world
    output.worldPosition = float4(input.position, 1.0f);
    output.position = mul(output.worldPosition, camViewProjM);

    //Calculate the normal vector against the world matrix only.
    output.normal = input.normal;
    return output;
}