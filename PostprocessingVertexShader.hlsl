struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

PS_INPUT main(uint input : SV_VERTEXID)
{
    PS_INPUT output;

    float4 pos = float4(0, 0, 0, 0);
    switch (input)
    {
        case 0:
            pos = float4(3, 1, 0, 1);
            break;
        case 1:
            pos = float4(-1, 1, 0, 1);
            break;
        case 2:
            pos = float4(-1, -3, 0, 1);
            break;
    }

    output.pos = pos;
    output.tex = float2(pos.x * 0.5 + 0.5, 0.5 - pos.y * 0.5);

    return output;
}