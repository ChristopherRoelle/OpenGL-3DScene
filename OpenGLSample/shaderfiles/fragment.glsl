#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D baseTexture;
uniform sampler2D overlayTexture;

void main()
{
    vec4 tex1 = texture(baseTexture, TexCoord);

    //overlay is stretched too much, so splitting in half on x.
    vec2 overlayTexCoord = vec2(TexCoord.x * 2.0, TexCoord.y);
    vec4 tex2 = texture(overlayTexture, overlayTexCoord);

    //Mix based on the alpha, as I want this overlayed, not blended.
    if(tex2.a == 0.0){
        FragColor = texture(baseTexture, TexCoord);
    } else {
        FragColor = mix(tex1, tex2, tex2.a);
    }
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    //FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);
}