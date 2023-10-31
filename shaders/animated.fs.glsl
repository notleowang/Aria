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
	vec2 uv = texcoord;
	// animation frame slides to the next on the same row
	uv.x += frame_width * frame;
	color = vec4(fcolor, 1.0) * texture(sampler0, uv);
}
