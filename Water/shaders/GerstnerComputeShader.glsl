#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba16f, binding = 0) uniform image2D heightOutput;
layout(rgba16f, binding = 1) uniform image2D normOuput;

uniform float t;

float rand(float n){return fract(sin(n) * 43758.5453123);}
float noise(float p){ float fl = floor(p); float fc = fract(p); return mix(rand(fl), rand(fl + 1.0), fc);}

// Consts
const int WAVE_COUNT = 32;

void main() {

    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec2 normTexelCoord = vec2(1.0, 1.0);
    normTexelCoord.x = float(texelCoord.x)/(gl_NumWorkGroups.x);
    normTexelCoord.y = float(texelCoord.y)/(gl_NumWorkGroups.y);
	
    vec4 height = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 normal = vec4(0.0, 0.0, 0.0, 0.0);

    float Amps[WAVE_COUNT];
    float Speeds[WAVE_COUNT];
    float Wavelens[WAVE_COUNT];
    vec2  Dirs[WAVE_COUNT];
    float Qs[WAVE_COUNT];

    for (int i = 0; i < WAVE_COUNT; i++)
    {
        float r0 = rand(float(i) * 12.9898);
        float r1 = rand(float(i) * 78.233);
        float r2 = rand(float(i) * 39.425);
        float r3 = rand(float(i) * 91.133);
        float r4 = rand(float(i) * 7.63981);

        Amps[i]     = mix(0.05, 0.6, r0);
        Speeds[i]   = mix(0.05, 2.5, r1);
        Wavelens[i] = mix(0.5, 2.0, r2);
        Qs[i]       = mix(0.0, 1.0, r4);

        float angle = r3 * 6.28318530718;
        Dirs[i]     = vec2(abs(cos(angle)), abs(sin(angle))); // normalized
    }

    for(uint i = 0; i<WAVE_COUNT; i++) {

        // Constants for this wave
        float Amp = Amps[i];
        float Speed = Speeds[i];
        float Wavelen = Wavelens[i];
        float Q = Qs[i];
        float Omega = 2 / Wavelen;
        float Phase = Speed * Omega;
        vec2  D = normalize(Dirs[i]);

        float inp = dot(D, texelCoord * 0.008) * Omega + t * Phase;

        // Developing heightmap for TES.
        height.x = Q * Amp * D.x * cos(inp);
        height.y = Amp * sin(inp);
        height.z += Q * Amp * D.y * cos(inp);

        // Developing normal map for FS.
        normal.x += Amp * Omega * D.x * cos(inp);
        normal.y += Amp * Omega * Q   * sin(inp);
        normal.z += Amp * Omega * D.y * cos(inp);
    }

    // Negate normal coordinates from derivating wave equations.
    normal.x = -normal.x;
    normal.y = 1 - normal.y;
    normal.z = -normal.z;
    normal = normalize(normal);
	
    imageStore(heightOutput, texelCoord, height);
    imageStore(normOuput, texelCoord, normal);

}