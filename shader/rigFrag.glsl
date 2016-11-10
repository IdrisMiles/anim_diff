#version 330

in vec3 vertColour;
out vec4 fragColor;


void main()
{
   fragColor = vec4(vertColour, 1.0);
}
