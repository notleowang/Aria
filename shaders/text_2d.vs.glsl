#version 330 core

layout (location = 0) 
in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

// Application data
uniform mat3 transform;
uniform mat3 projection;

void main()
{
    vec3 pos = projection * transform * vec3(vertex.xy, 1.0);
	gl_Position = vec4(pos.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}  