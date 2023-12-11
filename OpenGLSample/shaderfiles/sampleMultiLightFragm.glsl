#version 330 core
out vec4 FragColor;

//Fragment Material
struct Material {
    sampler2D diffuse; //The Diffuse Map
    sampler2D specular; //The Specular Map

    bool useOverlayTexture;
    sampler2D overlayDiffuse; //Optional Diffuse
    sampler2D overlaySpecular; //Optional Specular
    float shininess;
};

//Light Structs
struct DirLight{
    bool useDirectionalLight;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    vec3 position;

    vec3 ambient; //Usually set to low intensity to prevent dominance.
    vec3 diffuse; //Usually set to color of the light
    vec3 specular; //Usually kept at 1.0 for full shining

    //Attenuation variables
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight{
    bool useSpotLight;

    vec3 position; //Where the light is positioned
    vec3 direction; //Which way the light is facing

    float cutOff; //Inner cone cutoff
    float outerCutOff; //Outer cone cutoff, this is used to soften the edge of the light

    //Attenuation variables
    float constant;
    float linear;
    float quadratic;

    vec3 ambient; //Usually set to low intensity to prevent dominance.
    vec3 diffuse; //Usually set to color of the light
    vec3 specular; //Usually kept at 1.0 for full shining
};

//Ins
in vec3 FragPosition;
in vec3 Normal;
in vec2 TexCoords;

//Uniforms
uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform SpotLight spotLight;

//Prototypes
vec3 CalculateDirectionalLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec2 overlayTexCoord; //Coordinates for overlay texuture

void main(){
    //properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPosition);
    vec3 result;
    overlayTexCoord = vec2(TexCoords.x * 2.0, TexCoords.y); //for halfing the overlay to prevent overstrecthing

    //Phase 1: Directional Light
    if(dirLight.useDirectionalLight){
        result = CalculateDirectionalLight(dirLight, norm, viewDir);
    }

    //Phase 2: Point Lights (loop through them all)
    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += CalculatePointLight(pointLights[i], norm, FragPosition, viewDir);
    }

    //Phase 3: Spot Light (flashlight)
    if(spotLight.useSpotLight){
        result += CalculateSpotLight(spotLight, norm, FragPosition, viewDir);
    }

    FragColor = vec4(result, 1.0);
}

//Helper Functions

//Calculate the directional light's impact on the fragment
vec3 CalculateDirectionalLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction); //Normalized vector of the light direction (lights Pos - fragments pos)

    //Diffuse
    float diff = max(dot(normal, lightDir), 0.0); //Get the dot product of the normals/light dir, and ensure it never goes negative (if over 90 deg, it will go negative)

    //Specular
    vec3 reflectDir = reflect(-lightDir, normal); //reflect the light in the opposite direction it hit the normal from.
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); //Get the dot product of the view/reflect, and ensure its not negative. Then raise to the power of material's shininess.

    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;

    // Check if the overlayDiffuse texture is used
    vec4 overlayDiffuseColor = vec4(1.0);
    vec4 overlaySpecularColor = vec4(1.0);
    if (material.useOverlayTexture) {
        overlayDiffuseColor = texture(material.overlayDiffuse, overlayTexCoord);
        overlaySpecularColor = texture(material.overlaySpecular, overlayTexCoord);

        // Combine the results with the overlay
        ambient = light.ambient * mix(texture(material.diffuse, overlayTexCoord).rgb, overlayDiffuseColor.rgb, overlayDiffuseColor.a);
        diffuse = light.diffuse * diff * mix(texture(material.diffuse, overlayTexCoord).rgb, overlayDiffuseColor.rgb, overlayDiffuseColor.a);
        specular = light.specular * spec * mix(texture(material.specular, overlayTexCoord).rgb, overlaySpecularColor.rgb, overlaySpecularColor.a);
    }
    else {
        // Combine the results without the overlay
        ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
        diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
        specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    }

    return (ambient + diffuse + specular);
}

//Calculate a Point light's impact on the fragment
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos); //Normalized vector of the light direction (lights Pos - fragments pos)

    //Diffuse
    float diff = max(dot(normal, lightDir), 0.0); //Get the dot product of the normals/light dir, and ensure it never goes negative (if over 90 deg, it will go negative)

    //Specular
    vec3 reflectDir = reflect(-lightDir, normal); //reflect the light in the opposite direction it hit the normal from.
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); //Get the dot product of the view/reflect, and ensure its not negative. Then raise to the power of material's shininess.

    //Attenuation - Light intensity fall off for spot/area lights
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); //F-att = 1.0 / Kc + (Kl * d) + Kq * d^2

    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;

    // Check if the overlayDiffuse texture is used
    vec4 overlayDiffuseColor = vec4(1.0);
    vec4 overlaySpecularColor = vec4(1.0);
    if (material.useOverlayTexture) {
        overlayDiffuseColor = texture(material.overlayDiffuse, overlayTexCoord);
        overlaySpecularColor = texture(material.overlaySpecular, overlayTexCoord);

        // Combine the results with the overlay
        ambient = light.ambient * mix(texture(material.diffuse, overlayTexCoord).rgb, overlayDiffuseColor.rgb, overlayDiffuseColor.a);
        diffuse = light.diffuse * diff * mix(texture(material.diffuse, overlayTexCoord).rgb, overlayDiffuseColor.rgb, overlayDiffuseColor.a);
        specular = light.specular * spec * mix(texture(material.specular, overlayTexCoord).rgb, overlaySpecularColor.rgb, overlaySpecularColor.a);
    }
    else {
        // Combine the results without the overlay
        ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
        diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
        specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    }

    // Multiply the attenuation for all components
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

//Calculate a Point light's impact on the fragment
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPosition); //Normalized vector of the light direction (lights Pos - fragments pos)

    //Diffuse
    float diff = max(dot(normal, lightDir), 0.0); //Get the dot product of the normals/light dir, and ensure it never goes negative (if over 90 deg, it will go negative)

    //Specular
    vec3 reflectDir = reflect(-lightDir, normal); //reflect the light in the opposite direction it hit the normal from.
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); //Get the dot product of the view/reflect, and ensure its not negative. Then raise to the power of material's shininess.

    //Attenuation - Light intensity fall off for spot/area lights
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); //F-att = 1.0 / Kc + (Kl * d) + Kq * d^2

    //Spotlight (soft edge calculations)
    //Check if the light is inside the spotlight cone
    float theta = dot(lightDir, normalize(-light.direction)); //Get the theta from the inverse light direction and the light direction
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;

    // Check if the overlayDiffuse texture is used
    vec4 overlayDiffuseColor = vec4(1.0);
    vec4 overlaySpecularColor = vec4(1.0);
    if (material.useOverlayTexture) {
        overlayDiffuseColor = texture(material.overlayDiffuse, overlayTexCoord);
        overlaySpecularColor = texture(material.overlaySpecular, overlayTexCoord);

        // Combine the results with the overlay
        ambient = light.ambient * mix(texture(material.diffuse, overlayTexCoord).rgb, overlayDiffuseColor.rgb, overlayDiffuseColor.a);
        diffuse = light.diffuse * diff * mix(texture(material.diffuse, overlayTexCoord).rgb, overlayDiffuseColor.rgb, overlayDiffuseColor.a);
        specular = light.specular * spec * mix(texture(material.specular, overlayTexCoord).rgb, overlaySpecularColor.rgb, overlaySpecularColor.a);
    }
    else {
        // Combine the results without the overlay
        ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
        diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
        specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    }

    //Combine the attenuation * intensity to the components
    ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}