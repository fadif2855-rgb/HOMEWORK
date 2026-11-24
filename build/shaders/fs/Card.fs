#version 460 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D tex;
uniform vec3 objectColor;

void main()
{
    // no lighting: the color is just the texture multiplied by the object's color
    FragColor = vec4(texture(tex, texCoord).rgb * objectColor, 1.0);
}
