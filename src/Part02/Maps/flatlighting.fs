#version 330 core

struct Material {
	vec3 ambient;
	sampler2D diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float falloff;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColour;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
	// calculate distance to the light
	float lightDist = length(FragPos - light.position);
	float attenuation = clamp( light.falloff / pow(lightDist, 2.0), 0.0, 1.0);

	// ambient
    vec3 ambient = (light.ambient + material.ambient) * texture(material.diffuse, TexCoords).rgb;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb * attenuation;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular * attenuation;

    vec3 result = ambient + diffuse + specular;
    FragColour = vec4(result, 1.0);
}
