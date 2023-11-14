#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

uniform float time;
uniform int frame_col;
uniform int frame_row;
uniform float frame_width;
uniform float frame_height;
uniform bool rainbow_enabled;

// Output color
layout(location = 0) out  vec4 color;

// For the following functions:
//  HUEtoRGB
//  HSLtoRGB
//  RGBtoHCV
//  RGBtoHSL
// Source: https://www.shadertoy.com/view/4dKcWK
const float EPSILON = 1e-10;

vec3 HUEtoRGB(in float hue)
{
    // Hue [0..1] to RGB [0..1]
    // See http://www.chilliant.com/rgb2hsv.html
    vec3 rgb = abs(hue * 6. - vec3(3, 2, 4)) * vec3(1, -1, -1) + vec3(-1, 2, 2);
    return clamp(rgb, 0., 1.);
}

vec3 HSLtoRGB(in vec3 hsl)
{
    // Hue-Saturation-Lightness [0..1] to RGB [0..1]
    vec3 rgb = HUEtoRGB(hsl.x);
    float c = (1. - abs(2. * hsl.z - 1.)) * hsl.y;
    return (rgb - 0.5) * c + hsl.z;
}

vec3 RGBtoHCV(in vec3 rgb)
{
    // RGB [0..1] to Hue-Chroma-Value [0..1]
    // Based on work by Sam Hocevar and Emil Persson
    vec4 p = (rgb.g < rgb.b) ? vec4(rgb.bg, -1., 2. / 3.) : vec4(rgb.gb, 0., -1. / 3.);
    vec4 q = (rgb.r < p.x) ? vec4(p.xyw, rgb.r) : vec4(rgb.r, p.yzx);
    float c = q.x - min(q.w, q.y);
    float h = abs((q.w - q.y) / (6. * c + EPSILON) + q.z);
    return vec3(h, c, q.x);
}

vec3 RGBtoHSL(in vec3 rgb)
{
    // RGB [0..1] to Hue-Saturation-Lightness [0..1]
    vec3 hcv = RGBtoHCV(rgb);
    float z = hcv.z - hcv.y * 0.5;
    float s = hcv.y / (1. - abs(z * 2. - 1.) + EPSILON);
    return vec3(hcv.x, s, z);
}

float zig(float x, float m)
{
    // range [0..1] with constant slope +/-m
    return 2.0 * abs(x / m - floor(x / m) - 0.5);
}

vec4 rainbow_shift(vec4 in_rgb_color)
{
    vec3 in_hsl_color = RGBtoHSL(vec3(in_rgb_color.x, in_rgb_color.y, in_rgb_color.z));
    float hue = zig(time, 100.0);
    float saturation = 0.6;
    float luminance = in_hsl_color.z == 0.0 ? 0.0 : in_hsl_color.z * 0.6 + 0.40;
    vec3 out_hsl_color = vec3(hue, saturation, luminance);
    vec3 out_rgb_color = HSLtoRGB(out_hsl_color);
    return vec4(out_rgb_color, in_rgb_color.a);
}

void main()
{
	vec2 uv = texcoord;
	uv.x += frame_width * frame_col;
    uv.y += frame_height * frame_row;
    vec4 out_color = texture(sampler0, uv);
    color = rainbow_enabled ? rainbow_shift(out_color) : out_color;
}
