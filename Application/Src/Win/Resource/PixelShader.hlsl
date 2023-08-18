Texture2D tex : register( t0 );
SamplerState sam : register( s0 );

struct VertexOut
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

float4 main( VertexOut input ) : SV_TARGET
{
    return tex.Sample( sam, input.texCoord );
}