#version 460 core
#define MAX_TEAPOTS 4

struct DirectionalLight {
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float exponent;
};

struct SpotlightLight {
    vec3 position;
    vec3 direction;
    float cosInnerCone;
    float cosOuterCone;
    vec3 diffuse;
    vec3 specular;
    int on;

    float constant;
    float linear;
    float exponent;
};

struct EmissiveLight {
    vec3 color;
    vec3 position;
    float radius;
};


uniform int SpotlightLightOn;             // zapnut� / vypnut�
uniform int pointLightOn;
uniform int directionalLightOn;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

uniform PointLight teapotLight[MAX_TEAPOTS];
uniform SpotlightLight spotLight;
uniform DirectionalLight directionLight;

uniform vec3 viewPos;

uniform sampler2D tex0;
uniform vec3 matAmbient;
uniform vec3 matSpecular;
uniform float matShininess;

// sun
uniform EmissiveLight sunEmissive;

// teapots
uniform EmissiveLight teapotEmissive[MAX_TEAPOTS];
uniform int teapotCount;

// ambient light
uniform vec3 ambient;
out vec4 frag_color;

vec3 calc_spotlight_light();
vec3 calc_point_light(PointLight light);
vec3 calc_directional_light();

void main() {
    // apply ambient light to base color
    vec3 finalColor = (matAmbient + ambient);

    // add SpotlightLight only if enabled
    if (SpotlightLightOn == 1) {
        finalColor += calc_spotlight_light();
    }

    if (pointLightOn == 1) {
        for (int i = 0; i < teapotCount; i++) {
            finalColor += calc_point_light(teapotLight[i]);

            // Add emissive effect for teapots
            float dist = length(teapotEmissive[i].position - fs_in.FragPos);
            float intensity = clamp(1.0 - dist / teapotEmissive[i].radius, 0.0, 1.0);
            finalColor += teapotEmissive[i].color * intensity;
        }
    }

    if (directionalLightOn == 1)
        // add directional light = sun
        finalColor += calc_directional_light();

    //the sun object can glow even underneath the ground - it doesnt matter
    float distSun = length(sunEmissive.position - fs_in.FragPos);
    float sunGlow = 1.0 / (0.01 + pow(distSun / sunEmissive.radius, 2.0));
    finalColor += sunEmissive.color * sunGlow;
    //finalColor += sunEmissiveColor * 1.0;

    //adding textures and putting it in the frag_color
    frag_color = vec4(finalColor, 1.0) * texture(tex0, fs_in.TexCoord);
}

vec3 calc_spotlight_light() {
    vec3 lightDir = normalize(spotLight.position - fs_in.FragPos);
    vec3 spotDir  = normalize(spotLight.direction);

    float cosDir = dot(-lightDir, spotDir);
    float spotIntensity = smoothstep(spotLight.cosOuterCone, spotLight.cosInnerCone, cosDir);

    vec3 normal = normalize(fs_in.Normal);
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = spotLight.diffuse * NdotL;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float NDotH = max(dot(normal, halfDir), 0.0);
    vec3 specular = spotLight.specular * matSpecular * pow(NDotH, matShininess);

    float distance = length(spotLight.position - fs_in.FragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.exponent * (distance * distance));

    return (diffuse + specular) * attenuation * spotIntensity;
}


vec3 calc_point_light(PointLight light) {
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 fragPos = fs_in.FragPos;

    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.exponent * (distance * distance));

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = light.specular * spec * matSpecular;

    return attenuation * (diffuse + specular);
}




vec3 calc_directional_light() {
    vec3 lightDir = normalize(-directionLight.direction);  // Light direction points *toward* surface

    // Diffuse
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = directionLight.diffuse * diff;

    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = directionLight.specular * spec * matSpecular;

    // No attenuation for directional light
    return diffuse + specular;
}