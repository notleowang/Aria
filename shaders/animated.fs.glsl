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

vec3 colorByFrame(int frame) {
	if (frame == 0) {
		return vec3(1.0, 0.0, 0.0);
	} else if (frame == 1) {
		return vec3(0.0, 1.0, 0.0);
	} else if (frame == 2) {
		return vec3(0.0, 0.0, 1.0);
	} else if (frame == 3) {
		return vec3(0.0, 0.5, 0.5);
	} else {
		return vec3(0.0, 0.0, 0.0);
	}
}

void main()
{
	vec2 uv = texcoord;
	uv.x += frame_width * frame;
	color = vec4(fcolor, 1.0) * texture(sampler0, uv);
}
