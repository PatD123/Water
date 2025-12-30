# Water Simulation
Water simulation from OpenGL

Learning to use the following:
- Compute shaders
- Tessellation shaders (TCS, TES)
- Rasterization

CPU creates the patches before tessellation process. The compute shader recomputes the new sum of sines for wave formation 
per frame. This results in a new texture buffer per frame. When the Tessellation Evaluation Shader evaluates the texture at
the new intermediate interpolated points, it samples the updated texture buffer.


<img width="535" height="541" alt="Screenshot 2025-12-29 235515" src="https://github.com/user-attachments/assets/2144a89a-52cf-480e-9330-77f8a5040a05" />
<img width="785" height="543" alt="Screenshot 2025-12-29 235541" src="https://github.com/user-attachments/assets/13a113d0-a01a-4a2b-a206-83d80ce3749c" />
