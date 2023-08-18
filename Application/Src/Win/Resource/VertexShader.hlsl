struct VertexInput
{
    float3 pos : POS;
    float2 texCoord : TEXCOORD0;
};

struct VertexOut
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

VertexOut main( VertexInput input )
{
    VertexOut output;
    output.pos = float4( input.pos, 1.0f );
    output.texCoord = input.texCoord;
    
    return output;
}