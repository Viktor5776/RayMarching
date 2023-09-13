struct Sphere
{
    float3 center;
    float radius;
    int materialIndex;
};

struct Material
{
    float3 albedo;
    float roughness;
    float metallic;
};

struct Scene
{
    float3 lightDir;
    float ambient;
    Sphere spheres[10];
    Material materials[10];
};

RWTexture2D<float4> Result : register( u0 );
Texture2D<float4> SkyboxTexture : register( t0 );
SamplerState sampler_SkyboxTexture : register( s0 );
float4x4 InverseProjectionMatrix : register( c0 );
float4x4 InverseViewProjectionMatrix : register( c4 );
float3 cameraPosition : register( c8 );
Scene scene : register( c9 );

static const float PI = 3.14159265f;

float signedDistanceSphere( float3 p, float3 center, float radius )
{
    return length( p - center ) - radius;
}


struct ObjectDistance
{
    float distance;
    int objectIndex;
};

ObjectDistance signedDistanceScene( float3 p )
{
    ObjectDistance result;
    result.distance = 10000.0f;
    result.objectIndex = -1;
    
    for ( int i = 0; i < 10; i++ )
    {
        if ( scene.spheres[i].radius > 0.0f )
        {
            float tempDistance = signedDistanceSphere(p, scene.spheres[i].center, scene.spheres[i].radius);
            
            if( tempDistance < result.distance )
            {
                result.distance = tempDistance;
                result.objectIndex = i;
            }
        }
    }
    
    return result;
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
    int ObjectIndex;
};

HitPayload MarchRay( Ray ray, int maxIterations, float surfaceDistance, float maxDistance )
{
    float3 origin = ray.origin;

    for ( int i = 0; i < maxIterations; i++ )
    {
        ObjectDistance d = signedDistanceScene( ray.origin );
        if ( d.distance < surfaceDistance )
        {
            HitPayload hit;

            hit.HitDistance = distance( origin, ray.origin );
            hit.WorldPosition = ray.origin;
            hit.ObjectIndex = d.objectIndex;
            
            //Calculate normal
            float epsilon = 0.001;
            float centerDistance = signedDistanceScene( ray.origin ).distance;
            float xDistance = signedDistanceScene( ray.origin + float3( epsilon, 0, 0 ) ).distance;
            float yDistance = signedDistanceScene( ray.origin + float3( 0, epsilon, 0 ) ).distance;
            float zDistance = signedDistanceScene( ray.origin + float3( 0, 0, epsilon ) ).distance;
            hit.WorldNormal = (float3( xDistance, yDistance, zDistance ) - centerDistance) / epsilon;

            return hit;
        }

        if ( d.distance > maxDistance )
        {
            HitPayload hit;
            hit.HitDistance = -1.0f;
            hit.WorldNormal = float3( 0, 0, 0 );
            hit.WorldPosition = float3( 0, 0, 0 );
            hit.ObjectIndex = -1;
            return hit;
        }

        ray.origin += ray.dir * d.distance;
    }
    
    HitPayload hit;
    hit.HitDistance = -1.0f;
    hit.WorldNormal = float3( 0, 0, 0 );
    hit.WorldPosition = float3( 0, 0, 0 );
    hit.ObjectIndex = -1;
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
    
    //Skybox
    if( hit.HitDistance < 0.0f )
    {
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, theta ), 0 );
        Result[id.xy] = color;
        return;
    }

    
    float lightIntensity = max(dot( hit.WorldNormal, scene.lightDir * -1.0f ), 0.0f);
    lightIntensity += scene.ambient;
    lightIntensity = lightIntensity > 1.0f ? 1.0f : lightIntensity;

    
    float3 color = scene.materials[scene.spheres[hit.ObjectIndex].materialIndex].albedo;
    color *= lightIntensity;
    
    Result[id.xy] = float4( color, 1.0f );
}