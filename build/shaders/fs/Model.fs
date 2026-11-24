#version 460 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D tex;

void main()
{
    // no lighting: just the color of the texture
    FragColor = vec4(texture(tex, texCoord).rgb, 1.0);
}
