#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
    int lit;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;  
};

#define NR_LIGHTS 4

in vec2 TexCoords;
in vec3 FragPos;  
in vec3 Normal;  
in mat3 TBN;

uniform samplerCube depthMap0;
uniform samplerCube depthMap1;
uniform samplerCube depthMap2;
uniform samplerCube depthMap3;
uniform float farPlanes[NR_LIGHTS];
 
uniform int diffuseSet;
uniform sampler2D diffuseMap;
uniform int normalSet;
uniform sampler2D normalMap;
uniform float normalStrength;

uniform vec3 viewPos;
uniform Material material;
uniform int receiveShadows;
uniform Light lights[NR_LIGHTS];

// array of offset direction for sampling
vec3 sampleOffset[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos, Light light, float farPlane, samplerCube depthMap)
{
    //vector between frag and light
    vec3 fragToLight = fragPos - light.position;
    //current depth from light is distance of that vector
    float currentDepth = length(fragToLight);
    
    //shadow strenght variable
    float shadow = 0.0;

    //bias to counteract shadowacne
    float bias = 0.15;

    //samplerate for PCF
    int samples = 20;

    //distance to camera
    float viewDistance = length(viewPos - fragPos);

    //offset for sampling is dependend on viewdistance and farplane
    float offsetRadius = (1.0 + (viewDistance / farPlane)) / 45.0;

    //for each sample
    for(int i = 0; i < samples; ++i)
    {
        //get closest distance from camera in direction of current fragment to any fragment(+ offset for sampling)
        float closestDepth = texture(depthMap, fragToLight + sampleOffset[i] * offsetRadius).r;

        //transform to actual distance
        closestDepth *= farPlane;   // undo mapping [0;1]
        //if this distance is smaller then the current distance the light hits something before the current frag, thus it is in shadow
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    //average shadow value for every sample
    shadow /= float(samples);
        
    return shadow;
}
vec3 CalculateLight(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, Light light, vec3 normal, vec3 fragPos, vec3 viewDir, float farPlane, samplerCube depthMap){
   
   //simple phong lighting with attenuation
   
   vec3 lightDir =light.position - fragPos;
   float distance = length(lightDir);
   float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance));    
  

   

   // ambient
   vec3 ambient = light.ambient * ambientColor;
   ambient*=attenuation;

   // diffuse 
   lightDir = normalize(lightDir);
   float diff = max(dot(normal, lightDir), 0.0);
   vec3 diffuse = light.diffuse * diff * diffuseColor;
   diffuse*=attenuation;

   // specular
   vec3 reflectDir = reflect(-lightDir, normal);  
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   vec3 specular = light.specular * (spec * specularColor);  
   specular*=attenuation;
   
   //calculating shadows
   float shadow = ShadowCalculation(fragPos, light, farPlane, depthMap)*receiveShadows;   
   //combining shadows with lighting, by multipling inverse with diffuse and specular
   //(includes small check for unlit materials)
   vec3 result = ((1.0-material.lit)*material.diffuse)+(material.lit)*(ambient + (1.0 - shadow) * (diffuse + specular));
   return result;
}

void main()
{
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 result = vec3(0,0,0);

   //apply Diffuse Texture
   vec3 ambientColor = material.ambient;
   vec3 diffuseColor = material.diffuse;
   vec3 specularColor = material.specular;
   if(diffuseSet == 1){
        vec4 textureColor = texture(diffuseMap, TexCoords);
        diffuseColor*=textureColor.rgb;
        ambientColor*=textureColor.rgb;
        specularColor*=textureColor.rgb;
        
   }

   //apply Normal Texture
   vec3 normal = normalize(Normal);
   if(normalSet == 1){
        normal = texture(normalMap, TexCoords).rgb;
        normal = normal * 2.0 - 1.0;
        //apply normalstrength
        normal.rg*=normalStrength;
        normal = normalize(normal);
        normal = TBN * normal;
   }

   //Calculate result for every light and add them together
   for(int i = 0; i<NR_LIGHTS; i++){
      if(i==0) result += CalculateLight(ambientColor, diffuseColor, specularColor, lights[i], normal, FragPos, viewDir, farPlanes[i], depthMap0);
      else if (i==1) result += CalculateLight(ambientColor, diffuseColor, specularColor,lights[i], normal, FragPos, viewDir, farPlanes[i], depthMap1);
      else if (i==2) result += CalculateLight(ambientColor, diffuseColor, specularColor,lights[i], normal, FragPos, viewDir, farPlanes[i], depthMap2);
      else result += CalculateLight(ambientColor, diffuseColor, specularColor, lights[i], normal, FragPos, viewDir, farPlanes[i], depthMap3);
   }
   FragColor = vec4(result, 1.0);
} 