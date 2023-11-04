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

float3 refract( float3 uv, float3 n, float etai_over_etat)
{
    float cos_theta = min(dot(-uv, n), 1.0);
    float3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    float3 r_out_parallel = -sqrt(abs(1.0 - length(r_out_perp) * length(r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
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

struct Object
{
    int id;
    int materialIndex;
    int active;
    int pading;
    
    float4 data[4];
};

struct Sphere
{
    float3 center;
    float radius;
    int materialIndex;
};

Sphere SphereFromObject( Object obj )
{
    Sphere sphere;
        
    sphere.materialIndex = obj.materialIndex;
    sphere.center = obj.data[0].xyz;
    sphere.radius = obj.data[0].w;
        
    return sphere;
}

struct Cube
{
    float3 center;
    float3 size;
    int materialIndex;
};

Cube CubeFromObject( Object obj )
{
    Cube cube;
        
    cube.materialIndex = obj.materialIndex;
    cube.center = obj.data[0].xyz;
    cube.size = float3(obj.data[0].w, obj.data[1].xy);
        
    return cube;
}

struct Torus
{
    float3 posision;
    float2 size;
    int materialIndex;
};

Torus TorusFromObject( Object obj )
{
    Torus torus;
        
    torus.materialIndex = obj.materialIndex;
    torus.posision = obj.data[0].xyz;
    torus.size = float2(obj.data[0].w, obj.data[1].x);
        
    return torus;
}

struct Material
{
    int id;
    float3 emited_light;
    
    //Options
    float4 data[4];
    
    bool scatter( inout Ray ray_in, inout HitPayload hit, inout float3 attenuation, inout Ray scattered, inout uint seed )
    {
        if( id == 0)
        {
            float3 scatter_direction = hit.WorldNormal + Random::random_unit_vector( seed );
            
            if ( near_zero( scatter_direction ) )
                scatter_direction = hit.WorldNormal;
            
            scattered.origin = hit.WorldPosition + scatter_direction * 0.001f;
            scattered.dir = scatter_direction;
            
            attenuation = data[0].xyz;
        
            return true;
        }
        
        if( id == 1)
        {
            float3 reflected = reflect( normalize( ray_in.dir ), hit.WorldNormal );
            scattered.origin = hit.WorldPosition + reflected * 0.001f;
            scattered.dir = reflected + data[0].w * Random::random_unit_vector( seed );
            
            attenuation = data[0].xyz;
            
            return (dot( scattered.dir, hit.WorldNormal ) > 0.0f);
        }
        
        if( id == 2 )
        {
            attenuation = float3(1.0, 1.0, 1.0);
            double refraction_ratio = hit.HitDistance > 0.0f ? (1.0 / data[0].x) : data[0].x;

            float3 unit_direction = normalize(ray_in.dir);
            
            float cos_theta = min( dot( -unit_direction, hit.WorldNormal ), 1.0 );
            float sin_theta = sqrt( 1.0 - cos_theta * cos_theta );

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            float3 direction;

            float r0 = (1 - refraction_ratio) / (1 + refraction_ratio);
            r0 = r0 * r0;
            float r1 = r0 + (1 - r0) * pow( (1 - cos_theta), 5 );
            
            if ( cannot_refract || r1 > Random::RandomFloat( seed ) )
                direction = reflect( unit_direction, hit.WorldNormal );
            else
                direction = refract( unit_direction, hit.WorldNormal, refraction_ratio );

            
            
            Ray newRay;
            newRay.origin = hit.WorldPosition + direction * 0.001f;
            newRay.dir = direction;
            scattered = newRay;
            return true;
        }
        
        return false;
    }
    
    float3 emitted()
    {
        return emited_light * data[3].w;
    }
};

static const int MAX_OBJECTS = 128;

struct Scene
{
    int objectCount;
    int materialCount;
    Object objects[MAX_OBJECTS];
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

float signedDistanceBox( float3 p, float3 c, float3 b )
{
    float3 q = abs( p - c ) - b;
    return length( max( q, 0.0 ) ) + min( max( q.x, max( q.y, q.z ) ), 0.0 );
}

float signedDistanceTorus( float3 p, float3 center, float2 t)
{
    p = p - center;
    float2 q = float2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

ObjectDistance signedDistanceScene( float3 p, int objectIndex = -1 )
{
    ObjectDistance result;
    result.distance = 10000.0f;
    result.objectIndex = -1;
    
    for ( int i = 0; i < scene.objectCount; i++ )
    {
        if ( scene.objects[i].active > 0.0f && objectIndex != i)
        {
            float tempDistance = 10000.0f;
            
            switch ( scene.objects[i].id )
            {
                case 0:
                {
                    Sphere sphere = SphereFromObject( scene.objects[i] );
                    tempDistance = signedDistanceSphere( p, sphere.center, sphere.radius );
                    break;
                }
                case 1:
                {
                    Cube cube = CubeFromObject( scene.objects[i] );
                    tempDistance = signedDistanceBox( p, cube.center, cube.size );
                    break;
                }
                case 2:
                {
                    Torus torus = TorusFromObject(scene.objects[i]);
                    tempDistance = signedDistanceTorus(p, torus.posision, torus.size);
                    break;
                }
                default:
                {
                    break;
                }
            }
            
            if( tempDistance < result.distance )
            {
                result.distance = tempDistance;
                result.objectIndex = i;
            }
        }
    }
    
    return result;
}

HitPayload MarchRay( Ray ray, int maxIterations, float surfaceDistance, float maxDistance, int currentObject = -1)
{
    float3 origin = ray.origin;
    
    for ( int i = 0; i < maxIterations; i++ )
    {
        ObjectDistance d = signedDistanceScene( ray.origin, currentObject );
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
            hit.WorldNormal = normalize( hit.WorldNormal );
            
            
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

    float3 RayColor2( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor1( seed, scattered, maxIterations, minDistance, maxDistance );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }

    float3 RayColor3( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor2( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor4( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor3( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor5( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor4( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor6( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor5( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor7( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor6( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor8( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor7( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor9( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor8( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor10( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor9( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor11( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor10( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor12( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor11( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor13( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor12( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor14( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor13( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor15( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor14( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor16( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor15( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor17( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor16( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor18( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor17( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor19( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor18( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
            }
        }
    
        //Skybox
        float theta = acos( ray.dir.y ) / -PI;
        float phi = atan2( ray.dir.x, -ray.dir.z ) / -PI * 0.5f;
        float4 color = SkyboxTexture.SampleLevel( sampler_SkyboxTexture, float2( phi, -theta ), 0 );
        return color.xyz;
    }
    
    float3 RayColor20( inout uint seed, Ray ray, uint maxIterations, float minDistance, float maxDistance, int lastHitIndex )
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if ( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor19( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
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
            float3 color_from_emission = scene.materials[scene.objects[hit.ObjectIndex].materialIndex].emitted();
            if( scene.materials[scene.objects[hit.ObjectIndex].materialIndex].scatter( ray, hit, attenuation, scattered, seed ) )
            {
                float3 color_from_scatter = attenuation * RayColor20( seed, scattered, maxIterations, minDistance, maxDistance, hit.ObjectIndex );
                return color_from_emission + color_from_scatter;
            }
            else
            {
                return color_from_emission;
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