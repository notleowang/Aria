#version 330

uniform sampler2D screen_texture;
uniform vec2 window_size;
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

vec4 spotlight(vec4 in_color)
{
	vec2 coord_px = texcoord * window_size;
	float dist_px = distance(vec2(window_size.x/2.0, window_size.y/2.0), coord_px);
	float scale_factor = max(window_size.x, window_size.y) * 0.75;
	float radius_px = radius * scale_factor;

	float blurDistance = 25.0;
    float diff = radius_px - blurDistance;

	if (dist_px < radius_px) {
		if (dist_px > diff) {
			float diff2 = radius_px - dist_px;
			return in_color * (diff2/blurDistance);
		} else {
			return in_color;
		}
	} else {
		return vec4(0.0, 0.0, 0.0, 1.0);
	}
}

vec4 dim_light(vec4 in_color) {
	float dist = abs(distance(vec2(0.5, 0.5), texcoord));

    // If outside the light radius, darken the pixel
    if (dist > light_radius) {
        return vec4(0.0, 0.0, 0.0, in_color.a);
    } else {
        // Otherwise, apply lighting based on distance
        float intensity = 1.0 - (dist / light_radius)*0.9;
        return in_color * intensity;
    }
}

void main()
{
	vec4 in_color = texture(screen_texture, texcoord);
	color = dim_light(in_color);
	color = apply_spotlight ? spotlight(color) : color;
	color = fade_color(color);
}