#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float fraction;
uniform float logoRatio;
uniform float barRatio;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	float filled = fraction * barRatio + logoRatio;
	float offset = texcoord.x <= filled ? 0.5 : 0.0;
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y + offset));
}
