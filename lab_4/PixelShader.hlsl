Texture2D tex : register(t0);
SamplerState smplr : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_Target0
{
    return float4(
      tex.Sample(smplr, input.uv).xyz,
      1.0
    );
}