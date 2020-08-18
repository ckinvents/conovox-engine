#version 330 core
struct SunSource {
    vec3 dir;
    vec3 color;
    float ambFact;
    float diffFact;
    float specFact;
};

struct SpotSource {
    vec3 pos;
    vec3 dir;
    vec3 color;
    float inArc;
    float outArc;
    float ambFact;
    float diffFact;
    float specFact;
    float kConst;
    float kLinear;
    float kQuad;
};

struct PointSource {
    vec3 pos;
    vec3 color;
    float ambFact;
    float diffFact;
    float specFact;
    float kConst;
    float kLinear;
    float kQuad;
};

// Lighting functions
vec3 getAmbient(vec3 color, float amount);
vec3 getDiffuse(vec3 color, vec3 dir, vec3 norm, float amount);
vec3 getSpecular(vec3 color, vec3 dir, vec3 norm, vec3 view, float amount, float shine);
// Light source functions
vec3 getSunLighting(SunSource sun, vec3 pos, vec3 norm, vec3 view);
vec3 getSpotLighting(SpotSource spot, vec3 pos, vec3 norm, vec3 view);
vec3 getPointLighting(PointSource point, vec3 pos, vec3 norm, vec3 view);

in vec3 fragPos;
in vec3 fragNorm;
in vec3 color;

uniform vec3 cameraPos;

#define SHINE_TEMP 0.5f

#define MAX_SUNS 2
uniform int numSuns;
uniform SunSource suns[MAX_SUNS];
#define MAX_SPOTS 32
uniform int numSpots;
uniform SpotSource spots[MAX_SPOTS];
#define MAX_POINTS 64
uniform int numPoints;uniform PointSource points[MAX_POINTS];

out vec4 fragColor;

void main()
{
    vec3 fragView = normalize(cameraPos - fragPos);
    vec3 light = vec3(0.0f);
    // Calculate net sun lighting
    for (int i = 0; i < numSuns; i++)
    {
        light += getSunLighting(suns[i], fragPos, fragNorm, fragView);
    }
    // Calculate net spot lighting
    for (int i = 0; i < numSpots; i++)
    {
        light += getSpotLighting(spots[i], fragPos, fragNorm, fragView);
    }
    // Calculate net point lighting
    for (int i = 0; i < numPoints; i++)
    {
        light+= getPointLighting(points[i], fragPos, fragNorm, fragView);
    }
    // Return lit voxel fragment
    fragColor = vec4(light * color, 1.0f);
}

/**
 * @brief Gets the ambient color from a light source
 * @param color Color of light source
 * @param amount Intensity of lighting component
 * @return Ambient color vector
 **/
vec3 getAmbient(vec3 color, float amount)
{
    return amount * color;
}

/**
 * @brief Gets the diffuse color from a light source
 * @param color Color of light source
 * @param dir Normalized direction of fragment to light source
 * @param norm Normalized normal vector from fragment
 * @param amount Intensity of lighting component
 * @return Diffuse color vector
 **/
vec3 getDiffuse(vec3 color, vec3 dir, vec3 norm, float amount)
{
    return amount * max(0.0f, dot(dir, norm)) * color;
}

/**
 * @brief Gets the specular color from a light source
 * @param color Color of light source
 * @param dir Normalized direction of fragment to light source
 * @param norm Normalized normal vector from fragment
 * @param view Normalized direction of fragment to camera
 * @param amount Intensity of lighting component
 * @param shine Exponential factor of specular intensity
 * @return Specular color vector
 **/
vec3 getSpecular(vec3 color, vec3 dir, vec3 norm, vec3 view, float amount, float shine)
{
    vec3 reflection = reflect(-dir, norm);
    return amount * pow(max(0.0f, dot(reflection, view)), shine * 128.0f) * color;
}

/**
 * @brief calculates the color factor from a sun source
 * @param sun Individual sun source to calculate factor from
 * @param pos Position of fragment in world space
 * @param norm Normal from fragment surface
 * @param view Normalized vector from fragment to camera
 **/
vec3 getSunLighting(SunSource sun, vec3 pos, vec3 norm, vec3 view)
{
    vec3 dir = -normalize(sun.dir);
    vec3 ambient = getAmbient(sun.color, sun.ambFact);
    vec3 diffuse = getDiffuse(sun.color, dir, norm, sun.diffFact);
    vec3 specular = getSpecular(sun.color, dir, norm, view, sun.specFact, SHINE_TEMP);
    return(ambient + diffuse + specular);
}

/**
 * @brief calculates the color factor from a spot source
 * @param spot Individual spot source to calculate factor from
 * @param pos Position of fragment in world space
 * @param norm Normal from fragment surface
 * @param view Normalized vector from fragment to camera
 **/
vec3 getSpotLighting(SpotSource spot, vec3 pos, vec3 norm, vec3 view)
{
    vec3 dir = normalize(spot.pos - pos);
    float fragCos = dot(dir, -normalize(spot.dir));
    vec3 ambient = getAmbient(spot.color, spot.ambFact);
    vec3 spotResult;
    if (fragCos > spot.outArc)
    {
        // Get attenuation based on distance from source
        float dist = length(spot.pos - pos);
        float atten = 1.0f /
            (spot.kConst + spot.kLinear * dist + spot.kQuad * dist * dist);
        // Get falloff for outer edge of spot source
        float falloff;
        if (fragCos < spot.inArc)
        {
            float edgeFact = spot.inArc - fragCos;
            falloff = 1.0f - (edgeFact * edgeFact) / 
                ((spot.inArc - spot.outArc) * (spot.inArc - spot.outArc));
        }
        else
        {
            falloff = 1.0f;
        }
        // Calculate diffuse and specular as for a directional source
        vec3 diffuse = getDiffuse(spot.color, dir, norm, spot.diffFact);
        vec3 specular = getSpecular(spot.color, dir, norm, view, spot.specFact, SHINE_TEMP);
        // Combine attenuation, falloff, and directional light factors
        vec3 spotResult = atten * falloff * (diffuse + specular);
    }
    else
    {
        // Ignore spot lighting factors, only use ambient (object in shadow)
        spotResult = vec3(0.0f);
    }
    return(ambient + spotResult);
}

/**
 * @brief calculates the color factor from a point source
 * @param point Individual point source to calculate factor from
 * @param pos Position of fragment in world space
 * @param norm Normal from fragment surface
 * @param view Normalized vector from fragment to camera
 **/
vec3 getPointLighting(PointSource point, vec3 pos, vec3 norm, vec3 view)
{
    vec3 dir = normalize(point.pos - pos);
    // Get attenuation
    float dist = length(point.pos - pos);
    float atten = 1.0f /
        (point.kConst + point.kLinear * dist + point.kQuad * dist * dist);
    // Calculate components
    vec3 ambient = getAmbient(point.color, point.ambFact);
    vec3 diffuse = getDiffuse(point.color, dir, norm, point.diffFact);
    vec3 specular = getSpecular(point.color, dir, norm, view, point.specFact, SHINE_TEMP);
    return ambient + atten * (diffuse + specular);
}