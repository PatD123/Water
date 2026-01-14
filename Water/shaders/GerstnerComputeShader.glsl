#version 430 core

layout (local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D heightOutput;
layout(rgba32f, binding = 1) uniform image2D normOuput;

uniform float t;

float rand(float n){return fract(sin(n) * 43758.5453123);}
float noise(float p){ float fl = floor(p); float fc = fract(p); return mix(rand(fl), rand(fl + 1.0), fc);}

// Consts
const int WAVE_COUNT = 8;

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

        Amps[i]     = mix(0.05, 2.0, r0);
        Speeds[i]   = mix(0.05, 2.5, r1);
        Wavelens[i] = mix(0.5, 2.0, r2);
        Qs[i]       = mix(0.0, 1.0, r4);

        float angle = r3 * 6.28318530718;
        Dirs[i]     = abs(vec2(cos(angle), sin(angle)));
    }

    float dfdx = 0.0;
    float dfdz = 0.0;

    for(int i = 0; i<WAVE_COUNT; i++) {

        // Constants for this wave
        float Amp = Amps[i];
        float Speed = Speeds[i];
        float Wavelen = Wavelens[i];
        float Q = Qs[i];
        float Omega = 2 / Wavelen;
        float Phase = Speed * Omega;
        vec2  D = normalize(Dirs[i]);

        float inp = dot(D, texelCoord * 0.005) * Omega + t * Phase;

        // Developing heightmap for TES.
        height.x += Q * Amp * D.x * cos(inp);
        height.y += Amp * sin(inp);
        height.z += Q * Amp * D.y * cos(inp);

        // Developing normal map for FS.
        dfdx = Amp * Omega * D.x * cos(inp);
        dfdz = Amp * Omega * D.y * cos(inp);
        normal.x += dfdx;
        normal.y += Amp * Omega * Q   * sin(inp);
        normal.z += dfdz;

        // Amp *= 0.82;
        // Omega *= 1.18;
    }

    float Amp = 0.6;
    float Omega = 1.5;
    dfdx = 0.0;
    dfdz = 0.0;

    for(int i = 0; i<WAVE_COUNT*2; i++) {

        float r0 = rand(float(i) * 12.9898);
        float r1 = rand(float(i) * 78.233) * 6.28318530718;
        
        float Speed = mix(0.05, 2.5, r0);
        float Phase = Speed * Omega;
        vec2  D = vec2(cos(r1), sin(r1)); // normalized

        float inp = dot(D, (texelCoord + vec2(dfdx, dfdz)) * 0.01) * Omega + t * Phase;

        // Developing heightmap for TES.
        height.y += Amp * sin(inp);

        // Developing normal map for FS.
        dfdx = Amp * Omega * D.x * cos(inp);
        dfdz = Amp * Omega * D.y * cos(inp);
        normal.x += dfdx;
        normal.z += dfdz;

        // Fractional Brownian Noise
        Amp *= 0.82;
        Omega *= 1.18;
    }

    // Negate normal coordinates from derivating wave equations.
    normal.x = -normal.x;
    normal.y = 1 - normal.y;
    normal.z = -normal.z;
    normal = normalize(normal);
	
    imageStore(heightOutput, texelCoord, height);
    imageStore(normOuput, texelCoord, normal);

}