struct Sphere
{
    float3 center;
    float radius;
};

struct Scene
{
    float3 lightDir;
    float ambient;
    Sphere spheres[10];
};

RWTexture2D<float4> Result : register( u0 );
float4x4 InverseProjectionMatrix : register( c0 );
float4x4 InverseViewProjectionMatrix : register( c4 );
float3 cameraPosition : register( c8 );
Scene scene : register( c9 );


float signedDistanceSphere( float3 p, float3 center, float radius )
{
    return length( p - center ) - radius;
}

float signedDistanceScene( float3 p )
{
    float d = 10000.0f;
    for ( int i = 0; i < 10; i++ )
    {
        if ( scene.spheres[i].radius > 0.0f )
            d = min( d, signedDistanceSphere( p, scene.spheres[i].center, scene.spheres[i].radius ) );
    }
    return d;
}

struct Ray
{
    float3 dir;
    float3 origin;
};

struct HitPayload
{
    float HitDistance;
    float3 WorldPosition;
    float3 WorldNormal;
};

HitPayload MarchRay( Ray ray, int maxIterations, float surfaceDistance, float maxDistance )
{
    float3 origin = ray.origin;

    for ( int i = 0; i < maxIterations; i++ )
    {
        float d = signedDistanceScene( ray.origin );
        if ( d < surfaceDistance )
        {
            HitPayload hit;

            hit.HitDistance = distance( origin, ray.origin );
            hit.WorldPosition = ray.origin;
            hit.WorldNormal = normalize( ray.origin - ray.dir * d );

            return hit;
        }

        if ( d > maxDistance )
        {
            HitPayload hit;
            hit.HitDistance = -1.0f;
            hit.WorldNormal = float3( 0, 0, 0 );
            hit.WorldPosition = float3( 0, 0, 0 );
            return hit;
        }

        ray.origin += ray.dir * d;
    }
    
    HitPayload hit;
    hit.HitDistance = -1.0f;
    hit.WorldNormal = float3( 0, 0, 0 );
    hit.WorldPosition = float3( 0, 0, 0 );
    return hit;
}

[numthreads(8, 8, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
    uint width, height;
    Result.GetDimensions( width, height );
    
    float4x4 inverseProjectionMatrix = transpose( InverseProjectionMatrix );
    float4x4 inverseViewProjectionMatrix = transpose( InverseViewProjectionMatrix );
    
    float2 uv = float2( id.xy ) / float2( width, height );
    
    
    uint maxIterations = 100;
    float minDistance = 0.01f;
    float maxDistance = 100.0f;
    
    Ray ray;
    ray.origin = cameraPosition;
    
    float2 coord = uv * 2.0f - 1.0f;
    
    float4 target = mul( inverseProjectionMatrix, float4( coord.x, coord.y, 1.0f, 1.0f ) );
    float4 rayDir4D = mul( inverseViewProjectionMatrix, float4( normalize( float3( target.x, target.y, target.z ) / target.w ), 0.0f ) );
    ray.dir = float3( rayDir4D.xyz );
    
    HitPayload hit = MarchRay( ray, maxIterations, minDistance, maxDistance );
    
    if( hit.HitDistance < 0.0f )
    {
        //Sky color
        Result[id.xy] = float4( 0.2f, 0.4f, 1.0f, 1.0f );
        return;
    }

    float lightIntensity = max(dot( hit.WorldNormal, scene.lightDir * -1.0f ), 0.0f);
    lightIntensity += scene.ambient;
    lightIntensity = lightIntensity > 1.0f ? 1.0f : lightIntensity;

    float3 color = hit.WorldNormal * 0.5f + 0.5f;
    color *= lightIntensity;
    
    Result[id.xy] = float4( color, 1.0f );
}