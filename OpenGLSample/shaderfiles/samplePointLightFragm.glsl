#version 330 core
out vec4 FragColor;

struct Material {
    //vec3 ambient; - Handled via the Diffuse Map
    //vec3 diffuse; - Handled via the Diffuse Map
    //vec3 specular; - Handled via the Specular Map
    sampler2D diffuse; //The Diffuse Map
    sampler2D specular; //The Specular Map
    float shininess;
};

struct Light{
    vec3 position;

    vec3 ambient; //Usually set to low intensity to prevent dominance.
    vec3 diffuse; //Usually set to color of the light
    vec3 specular; //Usually kept at 1.0 for full shining

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;

in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPosition;

uniform vec3 viewPos; //Position of the camera

void main()
{
    //Ambient
    //vec3 ambient = light.ambient * material.ambient; //Multiply our light's color by the material's ambient.
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    //Diffuse
    vec3 norm = normalize(Normal); //Unit vector of the normals
    vec3 lightDir = normalize(light.position - FragPosition); //Normalized vector of the light direction (lights Pos - fragments pos)
    //vec3 lightDir = normalize(-light.direction); //Directional light

    float diff = max(dot(norm, lightDir), 0.0); //Get the dot product of the normals/light dir, and ensure it never goes negative (if over 90 deg, it will go negative)
    //vec3 diffuse = light.diffuse * (diff * material.diffuse);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    //Specular
    vec3 viewDir = normalize(viewPos - FragPosition); //Get the direction we are viewing the Fragment from.
    vec3 reflectDir = reflect(-lightDir, norm); //reflect the light in the opposite direction it hit the normal from.

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); //Get the dot product of the view/reflect, and ensure its not negative. Then raise to the power of material's shininess.
    //vec3 specular = light.specular * (spec * material.specular); //light color multiplied by the product of the speculare and material's specular
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    //Attenuation - Light intensity fall off for spot/area lights
    float dist = length(light.position - FragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist)); //F-att = 1.0 / Kc + (Kl * d) + Kq * d^2

    //Multiply the attenuation for all components
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    //Combine our components and apply to the object color
    vec3 result = (ambient + diffuse + specular); //Combine the components
    FragColor = vec4(result, 1.0);
}