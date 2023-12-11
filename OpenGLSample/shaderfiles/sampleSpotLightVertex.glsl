#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPosition;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    FragPosition = vec3(model * vec4(aPos, 1.0)); //Get the fragment's world position
    //Normal = mat3(transpose(inverse(model))) * aNormal; //Generate the normal matrix using inverse/transpose for non-uniform scaling. This is costly though. Better to do before the shader on the CPU.
    Normal = aNormal;
    TexCoords = aTexCoords;
}