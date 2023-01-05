#version 330 core

// Outputs colors in RGBA
out vec4 FragColor2;


// Inputs the color from the Vertex Shader
in vec3 color2;
// Inputs the texture coordinates from the Vertex Shader
in vec2 texCoord2;

// Gets the Texture Unit from the main function
uniform sampler2D tex1;


void main()
{
	FragColor2 = texture(tex1, texCoord2);
}