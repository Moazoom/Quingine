#version 330 core

vec3 CalculateDiffusePoint(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColour);
vec3 CalculateDiffuseDirectional(vec3 normal, vec3 fragPos, vec3 direction, vec3 lightColour);

struct PointLight{
    vec3 position;
    vec3 colour;
};

struct DirectionalLight{
    vec3 direction;
    vec3 colour;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColour;

uniform float iTime;
uniform sampler2D diffuse1;
uniform PointLight lights[3];
uniform DirectionalLight dirlight;

void main()
{
    vec3 result;

    // ambient light, amience is hardcoded
    result = 0.25 * vec3(texture(diffuse1, TexCoords));
    
    // diffuse light
    for(int i = 0; i < 3; i++){
        result += CalculateDiffusePoint(Normal, FragPos, lights[i].position, lights[i].colour) * vec3(texture(diffuse1, TexCoords));
    }

    // dir ligth diffuse
    result += CalculateDiffuseDirectional(Normal, FragPos, dirlight.direction, dirlight.colour) * vec3(texture(diffuse1, TexCoords));

    // final result
    FragColour = vec4(result, 1.0);
    // FragColour = texture(diffuse1, TexCoords);
}

// this function cant take textures, so add those separately
vec3 CalculateDiffusePoint(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColour){
    normal = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float lightDist = length(lightPos - fragPos);
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColour;
    return diffuse / lightDist;
}

// same function as above but for dir lights
vec3 CalculateDiffuseDirectional(vec3 normal, vec3 fragPos, vec3 direction, vec3 lightColour){
    normal = normalize(normal);
    direction = -direction; // this is becuase out calculation expects a vector pointing To the light
    vec3 diffuse = max(dot(normal, direction), 0.0) * lightColour;
    return diffuse;
}