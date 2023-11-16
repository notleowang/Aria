#version 330

uniform sampler2D screen_texture;
uniform float screen_darken_factor;
uniform float radius;
uniform bool apply_spotlight;

uniform float light_radius;

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

	// float radius = 0.3;
	float dist = abs(distance(vec2(0.5, 0.5), texcoord));

    // If outside the light radius, darken the pixel
    if (dist > light_radius) {
        color = vec4(0.0, 0.0, 0.0, in_color.a);
    } else {
        // Otherwise, apply lighting based on distance
        float intensity = 1.0 - (dist / light_radius);
        color = in_color * intensity;
    }

    if (apply_spotlight && dist > radius) {
		color.r = 0.0;
		color.g = 0.0;
		color.b = 0.0;
	}

	color = fade_color(color);
}