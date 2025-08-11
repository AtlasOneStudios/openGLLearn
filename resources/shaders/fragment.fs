#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ourTexture;
uniform vec3 lightColor;
uniform vec3 viewPos;

struct Material {
    //vec3 ambient;
    sampler2D diffuse;
    //vec3 specular;
    sampler2D specular;
    float shininess;
};
uniform Material material;

struct Light {
    vec4 descriptor;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform Light light;

void main()
{
    //ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir;

    vec3 result;
    if (light.cutOff == 0) {
        float attenuation;
        if (light.descriptor.w == 1.0) {
            lightDir = normalize(vec3(light.descriptor) - FragPos);
            float distance    = length(vec3(light.descriptor) - FragPos);
            attenuation = 1.0 / (light.constant + light.linear * distance +
            light.quadratic * (distance * distance));
        }
        if (light.descriptor.w == 0.0) lightDir = normalize(vec3(-light.descriptor));
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;

        //specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * (spec * texture(material.specular, TexCoord).rgb);

        if (light.descriptor.w == 1.0) {
            ambient  *= attenuation;
            diffuse  *= attenuation;
            specular *= attenuation;
        }

        result = ambient + diffuse + specular;
    }
    if(light.cutOff > 0) {
        lightDir = normalize(vec3(light.descriptor) - FragPos);
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon   = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;

        //specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * (spec * texture(material.specular, TexCoord).rgb);

        diffuse  *= intensity;
        specular *= intensity;

        result = ambient + diffuse + specular;
    }

    FragColor = vec4(result, 1.0);
    //FragColor = vec4(ourColor, 1.0);
}