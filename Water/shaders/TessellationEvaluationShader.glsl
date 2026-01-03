// tessellation evaluation shader
#version 410 core

layout (quads, fractional_odd_spacing, ccw) in;

uniform sampler2D heightMap;  // the texture corresponding to our height map
uniform sampler2D normalMap;  // the texture corresponding to our normal map
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

// received from Tessellation Control Shader - all texture coordinates for the patch vertices
in vec2 TextureCoord[];

// send to Fragment Shader for coloring
out vec3 Normal;
out float Height;
out vec3 FragPos;

void main()
{
    // Get patch coordinate - This is the relative coordinate in the patch itself.
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // ----------------------------------------------------------------------
    // Retrieve control point texture coordinates
    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

    // Bilinearly interpolate texture coordinate across patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;

    // Lookup texel at patch coordinate for height and scale + shift as desired
    Height = texture(heightMap, texCoord).x;
    Normal = texture(normalMap, texCoord).xyz;

    // ----------------------------------------------------------------------
    // Retrieve control point position coordinates
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // Compute patch surface normal
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    // Bilinearly interpolate position coordinate across patch
    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

    // Displace point along normal
    p += normal * Height;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
    gl_Position = proj_mat * view_mat * world_mat * p;
    FragPos = (world_mat * p).xyz;

}