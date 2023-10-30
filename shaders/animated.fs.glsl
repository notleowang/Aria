#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

uniform int frame;
uniform float frame_width;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	// Eli TODO: implement spritesheets with multiple rows
	vec2 uv = texcoord;
	uv.x += frame_width * frame;
	color = vec4(fcolor, 1.0) * texture(sampler0, uv);
}
