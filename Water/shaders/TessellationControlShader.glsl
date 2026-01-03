#version 410 core

layout (vertices = 4) out;

in vec2 TexCoord[];

out vec2 TextureCoord[];

void main() {
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

	if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = 2;
        gl_TessLevelOuter[1] = 2;
        gl_TessLevelOuter[2] = 2;
        gl_TessLevelOuter[3] = 2;

        gl_TessLevelInner[0] = 2;
        gl_TessLevelInner[1] = 2;
    }
}