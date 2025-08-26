#version 450 core

/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: model_shader.frag
Purpose: 
	Fragment shader for models.
Language: GLSL
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_1
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/

struct PerLight
{
    int l_type;               //4         0
    vec3 l_ambient_color;     //16        16
    vec3 l_diffuse_color;     //16        32
    vec3 l_specular_color;    //16        48
    vec3 l_pos;               //16        64
    vec3 l_dir;               //16        80
    vec2 inner_outer_angle;   //12        96
    float falloff;            //4         108
};

layout( std140 , binding = 1) uniform Light 
{
    PerLight light[16];
};

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;


uniform vec3 m_ambient;
uniform vec3 m_diffuse;
uniform vec3 m_specular;
uniform vec3 m_emissive;

uniform vec4 color;
uniform vec3 viewPos;

uniform vec3 att_const;
uniform int active_lights;
uniform vec3 g_ambient;

in vec2 UV;
in vec3 NRM;

in vec3 frag_pos;
out vec4 outColor;

// calculates the color when using a point light.
vec3 CalcPointLight(PerLight l, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(l.l_pos - frag_pos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = 2.f * dot(lightDir, normal) * normal - (lightDir);    //vec3 reflectDir = reflect(-lightDir, normal);
    float spec;
    if(texture(texture_diffuse1, UV).rgb == vec3(0.0,0.0,0.0))
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    else
        spec = pow(max(dot(viewDir, reflectDir), 0.0), texture(texture_specular1, UV).r * texture(texture_specular1, UV).r);

    // attenuation
    float distance = length(l.l_pos - frag_pos);
    float attenuation = 1.0 / (att_const.x + att_const.y * distance + att_const.z * (distance * distance));  
    
    // combine results
    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;

    if(texture(texture_diffuse1, UV).rgb == vec3(0.0,0.0,0.0))
    {
        ambient = l.l_ambient_color * color.xyz;     
        diffuse = l.l_diffuse_color * diff;
        specular = l.l_specular_color * spec;
    }
    else
    {
        ambient = l.l_ambient_color * vec3(texture(texture_diffuse1, UV)) * m_ambient;                      // * vec3(texture(material.diffuse, UV));
        diffuse = l.l_diffuse_color * diff * vec3(texture(texture_diffuse1, UV)) * m_diffuse;            // * vec3(texture(material.diffuse, TexCoords));
        specular = l.l_specular_color * spec * vec3(texture(texture_specular1, UV)) * m_specular;          // * vec3(texture(material.specular, TexCoords));
    }
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}


// calculates the color when using a directional light.
vec3 CalcDirLight(PerLight l, vec3 normal, vec3 viewDir)
{
    vec3 lightDir =  normalize(-l.l_dir);          //normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = 2.f * dot(lightDir, normal) * normal - (lightDir);    //vec3 reflectDir = reflect(-lightDir, normal);
    float spec;
    if(texture(texture_diffuse1, UV).rgb == vec3(0.0,0.0,0.0))
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    else
        spec = pow(max(dot(viewDir, reflectDir), 0.0), texture(texture_specular1, UV).r * texture(texture_specular1, UV).r);





    // combine results
    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;
    if(texture(texture_diffuse1, UV).rgb == vec3(0.0,0.0,0.0))
    {
        ambient = l.l_ambient_color * color.xyz;         
        diffuse = l.l_diffuse_color * diff;
        specular = l.l_specular_color * spec;
    }
    else
    {
        ambient = l.l_ambient_color * vec3(texture(texture_diffuse1, UV)) * m_ambient;                      // * vec3(texture(material.diffuse, UV));
        diffuse = l.l_diffuse_color * diff * vec3(texture(texture_diffuse1, UV)) * m_diffuse;            // * vec3(texture(material.diffuse, TexCoords));
        specular = l.l_specular_color * spec * vec3(texture(texture_specular1, UV)) * m_specular;          // * vec3(texture(material.specular, TexCoords));
    }

    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(PerLight l, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(l.l_pos - frag_pos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = 2.f * dot(lightDir, normal) * normal - (lightDir);    //vec3 reflectDir = reflect(-lightDir, normal);
    float spec;
    if(texture(texture_diffuse1, UV).rgb == vec3(0.0,0.0,0.0))
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    else
        spec = pow(max(dot(viewDir, reflectDir), 0.0), texture(texture_specular1, UV).r * texture(texture_specular1, UV).r);

    // attenuation
    float distance = length(l.l_pos - frag_pos);
    float attenuation = 1.0 / (att_const.x + att_const.y * distance + att_const.z * (distance * distance));    

    // spotlight intensity
    float Theta = radians(l.inner_outer_angle.x);
    float Phi = radians(l.inner_outer_angle.y);
    float intensity;
    float Alpha = dot(lightDir, normalize(l.l_pos)); 

    
    if(Alpha < cos(Phi))
    {
        intensity = 0.f;
    }

    else if (Alpha > cos(Theta))
    {
        intensity = 1.f;
    }

    else
    {
        float a = Alpha - cos(Phi);
        float b = cos(Theta) - cos(Phi);
        intensity = pow(a/b , l.falloff);
    }
    
    // combine results
    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;
    if(texture(texture_diffuse1, UV).rgb == vec3(0.0,0.0,0.0))
    {
        ambient = l.l_ambient_color * color.xyz;         
        diffuse = l.l_diffuse_color * diff;
        specular = l.l_specular_color * spec;
    }
    else
    {
        ambient = l.l_ambient_color * vec3(texture(texture_diffuse1, UV)) * m_ambient;                      // * vec3(texture(material.diffuse, UV));
        diffuse = l.l_diffuse_color * diff * vec3(texture(texture_diffuse1, UV)) * m_diffuse;            // * vec3(texture(material.diffuse, TexCoords));
        specular = l.l_specular_color * spec * vec3(texture(texture_specular1, UV)) * m_specular;          // * vec3(texture(material.specular, TexCoords));
    }
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}


void main(void)
{
    if(color.a > 0.f)
    {   
        // properties
        vec3 norm = normalize(NRM);
        vec3 viewDir = normalize(viewPos - frag_pos);
        
        // == =====================================================
        // Our lighting is set up in 3 phases: point lights, directional, and an optional flashlight
        // For each phase, a calculate function is defined that calculates the corresponding color
        // per lamp. In the main() function we take all the calculated colors and sum them up for
        // this fragment's final color.
        // == =====================================================

        vec3 result = vec3(0.0);

        for(int i = 0; i < active_lights; i++)
        {
            if(light[i].l_type == 0)    // point
            {
                result += CalcPointLight(light[i], norm, viewDir);
            }

            else if(light[i].l_type == 1)    // direction
            {
                result += CalcDirLight(light[i], norm, viewDir);
            }

            else if(light[i].l_type == 2)    // spot
            {
                result += CalcSpotLight(light[i], norm, viewDir); 
            }
        }

        outColor = vec4( (m_emissive +g_ambient+ result ) , 1.0);
    }
    else
    {
       outColor = vec4(color.xyz, 1.0);
    }
    //outColor =  texture(texture_diffuse1, UV);
}