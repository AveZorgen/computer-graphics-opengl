#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
  
uniform Material material;

struct Light {
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

uniform Light light; 

uniform vec3 viewPos;
  
out vec4 FragColor;

void main()
{
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(texture(material.diffuse, TexCoords)) * diff * light.diffuse;
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = vec3(texture(material.specular, TexCoords)) * spec * light.specular;

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (
                    light.constant + light.linear * distance + 
                    light.quadratic * (distance * distance));

    vec3 result = (ambient + diffuse + specular) * attenuation;
    FragColor = vec4(result, 1.0);
}