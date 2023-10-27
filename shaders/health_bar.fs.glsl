#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D emptyBarTexture;
uniform sampler2D fullBarTexture;
uniform vec3 fcolor;
uniform float currHealth;
uniform float maxHealth;

// Output color
layout(location = 0) out  vec4 color;

vec4 getTextureFromHP(float hp, vec2 uv, vec4 fullTexture, vec4 emptyTexture) {
	if (uv.x < hp) return fullTexture;
	return emptyTexture;
}

void main()
{
	float hp = currHealth/maxHealth;

	vec4 emptyTexture = texture(emptyBarTexture, vec2(texcoord.x, texcoord.y));
	vec4 fullTexture = texture(fullBarTexture, vec2(texcoord.x, texcoord.y));
	vec4 texture = getTextureFromHP(hp, texcoord, fullTexture, emptyTexture);

	color = vec4(fcolor, 1.0) * texture;
}
