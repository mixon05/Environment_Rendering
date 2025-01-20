#version 330 core
out vec4 FragColor;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 viewPos;

void main()
{

    // Właściwości materiału
    vec3 objectColor = vec3(0.7, 0.0, 0.0);
    float ambientStrength = 0.1;
    float specularStrength = 0.5;
    float shininess = 32.0;

    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // Attenuation
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    float height = FragPos.y;

    vec4 grassColor = texture(grassTexture, TexCoords);
    vec4 rockColor = texture(rockTexture, TexCoords);

    float blendFactor = smoothstep(10.0, 15.0, height);

    vec4 textureColor = mix(grassColor, rockColor, blendFactor);

    // Final result
    vec3 result = (ambient + (diffuse + specular) * attenuation * lightIntensity) * textureColor.rgb;
    FragColor = vec4(result, 1.0);
}