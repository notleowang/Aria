#version 330

uniform sampler2D screen_texture;
uniform vec2 window_size;
uniform float screen_darken_factor;
uniform float radius;
uniform bool apply_spotlight;

in vec2 texcoord;

layout(location = 0) out vec4 color;

vec4 fade_color(vec4 in_color) 
{
	if (screen_darken_factor > 0)
		in_color -= screen_darken_factor * vec4(0.8, 0.8, 0.8, 0);
	return in_color;
}

void main()
{
	vec4 in_color = texture(screen_texture, texcoord);
	color = in_color;

	vec2 coord_px = texcoord * window_size;
	float dist_px = distance(vec2(window_size.x/2.0, window_size.y/2.0), coord_px);
	float scale_factor = max(window_size.x, window_size.y) * 0.75;
	float radius_px = radius * scale_factor;

	if (apply_spotlight && dist_px > radius_px) {
		color.r = 0.0;
		color.g = 0.0;
		color.b = 0.0;
	}

	color = fade_color(color);
}