Texture2D sourceTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer PostEffectConstantBuffer : register(b0)
{
    int4 params; // x - use posteffect, y - screen width, z - screen height
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 x_color = float3(0.0, 0.0, 0.0);
    float3 y_color = float3(0.0, 0.0, 0.0);
    float3 color = float3(0.0, 0.0, 0.0);
    
    if (params.x > 0) {
        float x_kernel[3][3] =
        {
            { -1.0, 0.0, 1.0 },
            { -2.0, 0.0, 2.0 },
            { -1.0, 0.0, 1.0 }
        };
        
        float y_kernel[3][3] =
        {
            { 1.0, 2.0, 1.0 },
            { 0.0, 0.0, 0.0 },
            { -1.0, -2.0, -1.0 }
        };
        
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                float2 texOffset = normalize(float2(i, j));
                x_color += sourceTexture.Sample(Sampler, input.tex + float2(i, j) / float2(params.y, params.z)).xyz * x_kernel[i + 1][j + 1];
                y_color += sourceTexture.Sample(Sampler, input.tex + float2(i, j) / float2(params.y, params.z)).xyz * y_kernel[i + 1][j + 1];
            }
        }
        color = sqrt(x_color * x_color + y_color * y_color);
    }
    else
    {
        color = sourceTexture.Sample(Sampler, input.tex).xyz;
    }
    
    return float4(color, 1.0f);
}