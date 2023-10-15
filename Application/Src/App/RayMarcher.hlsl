//Random
namespace Random
{

    uint PCG_Hash( uint input )
    {
        uint state = input * 747796405u + 2891336453u;
        uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }

    float RandomFloat( inout uint seed )
    {
        seed = PCG_Hash( seed );
    // [0, 1)
        return ( float ) seed / ( float ) 0xffffffffu;
    }

    float RandomFloat( inout uint seed, float min, float max )
    {
        return RandomFloat( seed ) * (max - min) + min;
    }

    float3 randomVector3( inout uint seed )
    {
        return float3( RandomFloat( seed ), RandomFloat( seed ), RandomFloat( seed ) );
    }

    float3 randomVector3( inout uint seed, float min, float max )
    {
        return float3( RandomFloat( seed, min, max ), RandomFloat( seed, min, max ), RandomFloat( seed, min, max ) );
    }

    float3 random_in_unit_sphere( inout uint seed )
    {
        while ( true )
        {
            float3 p = randomVector3( seed, -1.0f, 1.0f );
            if ( length( p ) < 1.0f ) 
                return p;
        }
        
        return float3( 0.0f, 0.0f, 0.0f );
    }

    float3 random_unit_vector( inout uint seed )
    {
        return normalize( random_in_unit_sphere( seed ) );
    }

    float3 random_on_hemisphere( inout uint seed, float3 normal )
    {
        float3 on_unit_sphere = random_unit_vector( seed );
        if ( dot( on_unit_sphere, normal ) > 0.0f )
            return on_unit_sphere;
        else
            return -on_unit_sphere;
    }

}

//Utility
bool near_zero( float3 v )
{
    const float s = 1e-8;
    return ( abs( v.x ) < s ) && ( abs( v.y ) < s ) && ( abs( v.z ) < s );
}

float3 reflect( float3 v, float3 n )
{
    return v - 2.0f * dot( v, n ) * n;
}

//Ray marcher
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

struct Sphere
{
    float3 center;
    float radius;
    int materialIndex;
    int3 padding;
};

struct Material
{
    float3 albedo;
    
    //Options
    float metalRoughness;
    int isMetal;
    int3 padding;
    
    bool scatter( inout Ray ray_in, inout HitPayload hit, inout float3 attenuation, inout Ray scattered, inout uint seed )
    {
        if( isMetal == 0)
        {
            float3 scatter_direction = hit.WorldNormal + Random::random_unit_vector( seed );
            
            if ( near_zero( scatter_direction ) )
                scatter_direction = hit.WorldNormal;
            
            scattered.origin = hit.WorldPosition + scatter_direction * 0.001f;
            scattered.dir = scatter_direction;
            
            attenuation = albedo;
        
            return true;
        }
        
        if( isMetal == 1)
        {
            float3 reflected = reflect( normalize( ray_in.dir ), hit.WorldNormal );
            scattered.origin = hit.WorldPosition + reflected * 0.001f;
            scattered.dir = reflected + metalRoughness * Random::random_unit_vector( seed );
            
            attenuation = albedo;
            
            return (dot( scattered.dir, hit.WorldNormal ) > 0.0f);
        }
        
        return false;
    }
};

static const int MAX_OBJECTS = 128;

struct Scene
{
    float3 lightDir;
    float ambient;
    int objectCount;
    int materialCount;
    Sphere spheres[MAX_OBJECTS];
    Material materials[MAX_OBJECTS];
};

RWTexture2D<float4> Result : register( u0 );
Texture2D<float4> SkyboxTexture : register( t0 );
SamplerState sampler_SkyboxTexture : register( s0 );
float4x4 InverseProjectionMatrix : register( c0 );
float4x4 InverseViewProjectionMatrix : register( c4 );
float3 cameraPosition : register( c8 );
int renderInterations : register( c9 );
uint seedStart : register( c10 );
Scene scene : register( c11 );

static const float PI = 3.14159265f;



float linear_to_gamma( float linear_component )
{
    return sqrt( linear_component );
}

float3 linear_to_gamma( float3 linear_component )
{
    return sqrt( linear_component );
}

struct ObjectDistance
{
    float distance;
    int objectIndex;
};

float signedDistanceSphere( float3 p, float3 center, float radius )
{
    return length( p - center ) - radius;
}

ObjectDistance signedDistanceScene( float3 p )
{
    ObjectDistance result;
    result.distance = 10000.0f;
    result.objectIndex = -1;
    
    for ( int i = 0; i < scene.objectCount; i++ )
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

namespace RayColor
{

    float3 RayColor1( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance )
    {
        return float3( 0, 0, 0 );
    }

    float3 RayColor2( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance )
    {
        uint width, height;
        Result.GetDimensions( width, height );
    
        //Generate small diffrence in ray direction between samples
        float2 delta = float2( Random::RandomFloat( seed ), Random::RandomFloat( seed ) ) / float2( width, height );
        ray.dir += float3( delta, 0.0f );
        
        HitPayload hit = MarchRay( ray, maxIterations, minDistance, maxDistance );
    
        if ( hit.HitDistance > 0.0f )
        {
            Ray scattered;
            float3 attenuation;
            if ( scene.materials[scene.spheres[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                return attenuation * RayColor1( seed, scattered, maxIterations, minDistance, maxDistance );
            }
            else
            {
                return float3( 0.0f, 0.0f, 0.0f );
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }

    float3 RayColor3( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance )
    {
        uint width, height;
        Result.GetDimensions( width, height );
    
        //Generate small diffrence in ray direction between samples
        float2 delta = float2( Random::RandomFloat( seed ), Random::RandomFloat( seed ) ) / float2( width, height );
        ray.dir += float3( delta, 0.0f );
        
        HitPayload hit = MarchRay( ray, maxIterations, minDistance, maxDistance );
    
        if ( hit.HitDistance > 0.0f )
        {
            Ray scattered;
            float3 attenuation;
            if ( scene.materials[scene.spheres[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                return attenuation * RayColor2( seed, scattered, maxIterations, minDistance, maxDistance );
            }
            else
            {
                return float3( 0.0f, 0.0f, 0.0f );
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor4( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance )
    {
        uint width, height;
        Result.GetDimensions( width, height );
    
        //Generate small diffrence in ray direction between samples
        float2 delta = float2( Random::RandomFloat( seed ), Random::RandomFloat( seed ) ) / float2( width, height );
        ray.dir += float3( delta, 0.0f );
        
        HitPayload hit = MarchRay( ray, maxIterations, minDistance, maxDistance );
    
        if ( hit.HitDistance > 0.0f )
        {
            Ray scattered;
            float3 attenuation;
            if ( scene.materials[scene.spheres[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                return attenuation * RayColor3( seed, scattered, maxIterations, minDistance, maxDistance );
            }
            else
            {
                return float3( 0.0f, 0.0f, 0.0f );
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor5( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance )
    {
        uint width, height;
        Result.GetDimensions( width, height );
    
        //Generate small diffrence in ray direction between samples
        float2 delta = float2( Random::RandomFloat( seed ), Random::RandomFloat( seed ) ) / float2( width, height );
        ray.dir += float3( delta, 0.0f );
        
        HitPayload hit = MarchRay( ray, maxIterations, minDistance, maxDistance );
    
        if ( hit.HitDistance > 0.0f )
        {
            Ray scattered;
            float3 attenuation;
            if ( scene.materials[scene.spheres[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                return attenuation * RayColor4( seed, scattered, maxIterations, minDistance, maxDistance );
            }
            else
            {
                return float3( 0.0f, 0.0f, 0.0f );
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor6( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance )
    {
        uint width, height;
        Result.GetDimensions( width, height );
    
        //Generate small diffrence in ray direction between samples
        float2 delta = float2( Random::RandomFloat( seed ), Random::RandomFloat( seed ) ) / float2( width, height );
        ray.dir += float3( delta, 0.0f );
        
        HitPayload hit = MarchRay( ray, maxIterations, minDistance, maxDistance );
    
        if ( hit.HitDistance > 0.0f )
        {
            Ray scattered;
            float3 attenuation;
            if ( scene.materials[scene.spheres[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                return attenuation * RayColor5( seed, scattered, maxIterations, minDistance, maxDistance );
            }
            else
            {
                return float3( 0.0f, 0.0f, 0.0f );
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor7( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance )
    {
        uint width, height;
        Result.GetDimensions( width, height );
    
        //Generate small diffrence in ray direction between samples
        float2 delta = float2( Random::RandomFloat( seed ), Random::RandomFloat( seed ) ) / float2( width, height );
        ray.dir += float3( delta, 0.0f );
        
        HitPayload hit = MarchRay( ray, maxIterations, minDistance, maxDistance );
    
        if ( hit.HitDistance > 0.0f )
        {
            Ray scattered;
            float3 attenuation;
            if ( scene.materials[scene.spheres[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                return attenuation * RayColor6( seed, scattered, maxIterations, minDistance, maxDistance );
            }
            else
            {
                return float3( 0.0f, 0.0f, 0.0f );
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor8( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance )
    {
        uint width, height;
        Result.GetDimensions( width, height );
    
        //Generate small diffrence in ray direction between samples
        float2 delta = float2( Random::RandomFloat( seed ), Random::RandomFloat( seed ) ) / float2( width, height );
        ray.dir += float3( delta, 0.0f );
        
        HitPayload hit = MarchRay( ray, maxIterations, minDistance, maxDistance );
    
        if ( hit.HitDistance > 0.0f )
        {
            Ray scattered;
            float3 attenuation;
            if ( scene.materials[scene.spheres[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                return attenuation * RayColor7( seed, scattered, maxIterations, minDistance, maxDistance );
            }
            else
            {
                return float3( 0.0f, 0.0f, 0.0f );
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor9( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance )
    {
        uint width, height;
        Result.GetDimensions( width, height );
    
        //Generate small diffrence in ray direction between samples
        float2 delta = float2( Random::RandomFloat( seed ), Random::RandomFloat( seed ) ) / float2( width, height );
        ray.dir += float3( delta, 0.0f );
        
        HitPayload hit = MarchRay( ray, maxIterations, minDistance, maxDistance );
    
        if ( hit.HitDistance > 0.0f )
        {
            Ray scattered;
            float3 attenuation;
            if ( scene.materials[scene.spheres[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                return attenuation * RayColor8( seed, scattered, maxIterations, minDistance, maxDistance );
            }
            else
            {
                return float3( 0.0f, 0.0f, 0.0f );
            }
        }
    
        //Skybox        
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance )
    {
        uint width, height;
        Result.GetDimensions( width, height );
    
        //Generate small diffrence in ray direction between samples
        float2 delta = float2( Random::RandomFloat( seed ), Random::RandomFloat( seed ) ) / float2( width, height );
        ray.dir += float3( delta, 0.0f );
        
        HitPayload hit = MarchRay( ray, maxIterations, minDistance, maxDistance );
    
        if ( hit.HitDistance > 0.0f )
        {
            Ray scattered;
            float3 attenuation;
            if( scene.materials[scene.spheres[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                return attenuation * RayColor9( seed, scattered, maxIterations, minDistance, maxDistance );
            }
            else
            {
                return float3( 0.0f, 0.0f, 0.0f );
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
}

[numthreads(8, 8, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
    uint width, height;
    Result.GetDimensions( width, height );
    
    float4x4 inverseProjectionMatrix = transpose( InverseProjectionMatrix );
    float4x4 inverseViewProjectionMatrix = transpose( InverseViewProjectionMatrix );
    
    float2 uv = float2( id.xy ) / float2( width, height );

    uint maxIterations = 1000;
    float minDistance = 0.0001f;
    float maxDistance = 100.0f;
    
    Ray originalRay;
    originalRay.origin = cameraPosition;
    
    float2 coord = uv * 2.0f - 1.0f;
    
    float4 target = mul( inverseProjectionMatrix, float4( coord.x, coord.y, 1.0f, 1.0f ) );
    float4 rayDir4D = mul( inverseViewProjectionMatrix, float4( normalize( float3( target.x, target.y, target.z ) / target.w ), 0.0f ) );
    originalRay.dir = float3( rayDir4D.xyz );
    
    uint seed = id.x + id.y * width + seedStart;
    
    //Accumalate color
    float3 accumelatedColor = float3( 0, 0, 0 );
    
    for ( int i = 0; i < renderInterations; i++ )
    {
        accumelatedColor += RayColor::RayColor( seed, originalRay, maxIterations, minDistance, maxDistance );
    }
    
    accumelatedColor /= renderInterations;
    
    //Maybe use this?
    accumelatedColor = linear_to_gamma( accumelatedColor );
    
    Result[id.xy] = float4( accumelatedColor, 1.0f );
}