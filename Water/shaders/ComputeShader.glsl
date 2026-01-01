#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(r16f, binding = 0) uniform image2D heightOutput;
layout(rgba16f, binding = 1) uniform image2D normOuput;

uniform float t;

float rand(float n){return fract(sin(n) * 43758.5453123);}
float noise(float p){ float fl = floor(p); float fc = fract(p); return mix(rand(fl), rand(fl + 1.0), fc);}

void main() {

    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
    vec4 height = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 normal = vec4(0.0, 0.0, 0.0, 0.0);

    float Amps[] = {1.0, 1.0, 1.0, 1.0};
    float Speeds[] = {4.0, 2.5, 3.2, 1.7};
    float Wavelens[] = {2.1, 5.1, 0.8, 3.8};
    vec2  Dirs[] = {
        vec2(1.0, 1.0),
        vec2(2.1, 1.5),
        vec2(1.4, 1.2),
        vec2(1.8, 3.2)
    };

    for(uint i = 0; i<Amps.length(); i++) {

        // Constants for this wave
        float Amp = Amps[i];
        float Speed = Speeds[i];
        float Wavelen = Wavelens[i];
        float Omega = 2 / Wavelen;
        float Phase = Speed * Omega;
        vec2  D = Dirs[i];

        float inp = dot(D, texelCoord * 0.008) * Omega + t * Phase;

        // Developing heightmap for TES.
        height.x += Amp * sin(inp);

        // Developing normal map for FS.
        normal.x += Amp * Omega * D.x * cos(dot(D, texelCoord * 0.008) * Omega + t * Phase);
        normal.y += Amp * Omega * D.y * cos(dot(D, texelCoord * 0.008) * Omega + t * Phase);
    }

    // Normalizing height after adding many sines.
    height.x /= 4.0;

    // Negate normal coordinates from derivating wave equations.
    normal *= -1.0;
	
    imageStore(heightOutput, texelCoord, height);
    imageStore(normOuput, texelCoord, normal);

}