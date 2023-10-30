#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float screen_darken_factor;
uniform float radius;

in vec2 texcoord;

layout(location = 0) out vec4 color;

void main()
{
    vec4 in_color = texture(screen_texture, texcoord);
	color = in_color;

	// float radius = 0.3;
	float dist = distance(vec2(0.5, 0.5), texcoord);

	if (dist > radius) {
		color.r = 0.0;
		color.g = 0.0;
		color.b = 0.0;
	}
}