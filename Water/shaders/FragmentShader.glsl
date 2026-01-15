#version 410 core

in vec3 Normal;
in vec3 FragPos;

uniform vec3 CamPos;
uniform vec3 LightPos;
uniform vec3 LightColor;

out vec4 FragColor;

void main()
{
    
    vec3 color = vec3(6.0/255, 84.0/255, 176./255);
    // vec3 color = vec3(0.0, 0.0, 1.0);

	// Ambient

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * LightColor;

	// Diffuse

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * LightColor;

    // Specular

    float specularStrength = 0.7;
    vec3 viewDir = normalize(CamPos - FragPos); // From frag to cam
    vec3 reflectDir = reflect(-lightDir, norm);  // Across norm, pointing away from frag.
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * LightColor;

    vec3 lighting = ambient + diffuse;
    vec3 result = color * lighting + specular; // tint diffuse only
    FragColor = vec4(result, 1.0);
    // FragColor = vec4(Normal, 1.0);
}