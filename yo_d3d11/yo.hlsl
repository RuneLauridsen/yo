struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer vs_uniform : register(b0)
{
    float2 resolution;
};

VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
    VOut output;

    output.color = color;
    output.position = position;
    output.position.xy = (output.position.xy / resolution) * 2.0 - 1.0;

    return output;
}


float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}