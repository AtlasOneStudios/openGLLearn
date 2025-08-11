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

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);


struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    /*if(light.cutOff > 0) {
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
    }*/
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcPointLight(pointLights[0], norm, FragPos,viewDir);
    result += CalcPointLight(pointLights[1], norm, FragPos,viewDir);
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(ourColor, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(vec3(-light.direction));

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * texture(material.diffuse, TexCoord).rgb;
    vec3 specular = light.specular * (spec * texture(material.specular, TexCoord).rgb);
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}