#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba16f, binding = 0) uniform image2D heightOutput;
layout(rgba16f, binding = 1) uniform image2D normOuput;

uniform float t;

float rand(float n){return fract(sin(n) * 43758.5453123);}
float noise(float p){ float fl = floor(p); float fc = fract(p); return mix(rand(fl), rand(fl + 1.0), fc);}

// Consts
const int WAVE_COUNT = 16;

void main() {

    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
    vec4 height = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 normal = vec4(0.0, 1.0, 0.0, 0.0);

    float Amp = 0.3;
    float Omega = 1.0;
    float Speeds[WAVE_COUNT];
    vec2  Dirs[WAVE_COUNT];

    for (uint i = 0; i < WAVE_COUNT; i++)
    {
        float r0 = rand(float(i) * 12.9898);
        float r1 = rand(float(i) * 78.233);

        Speeds[i]   = mix(0.05, 2.5, r0);

        float angle = r1 * 6.28318530718;
        Dirs[i]     = vec2(cos(angle), sin(angle)); // normalized
    }

    float dfdx = 0.0;
    float dfdz = 0.0;

    for(int i = 0; i<WAVE_COUNT; i++) {
        
        float Speed = Speeds[i];
        float Phase = Speed * Omega;
        vec2  D = normalize(Dirs[i]);

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
    normal.y = 1.0;
    normal.z = -normal.z;
    normal = normalize(normal);
	
    imageStore(heightOutput, texelCoord, height);
    imageStore(normOuput, texelCoord, normal);

}