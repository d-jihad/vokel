#version 460 core

vec2 poistions[3] = vec2[](
    vec2(0.0, -0.05),
    vec2(0.05, 0.05),
    vec2(-0.05, 0.05));

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0));

layout(push_constant) uniform constants
{
    mat4 model;
}
ObjectData;

layout(location = 0) out vec3 fragColor;

void main()
{
    fragColor = colors[gl_VertexIndex];
    gl_Position = ObjectData.model * vec4(poistions[gl_VertexIndex], 0.0, 1.0);
}