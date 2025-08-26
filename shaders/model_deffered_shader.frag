#version 450 core

/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: line_shader.frag
Purpose: 
	Fragment shader for lines.
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

layout( std140 , binding = 0) uniform Light 
{
    PerLight light[16];
};

uniform sampler2D gPosition;    //gbuff pos
uniform sampler2D gNormal;      //gbuff normal
uniform sampler2D gDiff;        //gbuff diffuse
uniform sampler2D gSpec;        //gbuff specular

uniform vec3 m_ambient;
uniform vec3 m_diffuse;
uniform vec3 m_specular;
uniform vec3 m_emissive;

uniform vec4 color;

uniform vec3 viewPos;

uniform vec3 att_const;
uniform int active_lights;
//uniform vec3 g_ambient;

in vec2 UV;

out vec4 outColor;

vec3 CalcPointLight(PerLight l,vec3 frag_pos, vec3 normal, vec3 viewDir, vec3 Diffuse, vec3 Specular);
vec3 CalcDirLight(PerLight l,vec3 frag_pos, vec3 normal, vec3 viewDir, vec3 Diffuse, vec3 Specular);
vec3 CalcSpotLight(PerLight l,vec3 frag_pos, vec3 normal, vec3 viewDir, vec3 Diffuse, vec3 Specular);


void main(void)
{
    // retrieve data from gbuffer
    vec3 frag_pos = texture(gPosition, UV).rgb;
    vec3 NRM = texture(gNormal, UV).rgb;
    vec3 Diffuse = texture(gDiff, UV).rgb;
    vec3 Specular = texture(gSpec, UV).rgb;

 
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
            result += CalcPointLight(light[i], frag_pos, norm, viewDir, Diffuse, Specular);
        }

        else if(light[i].l_type == 1)    // direction
        {
            result += CalcDirLight(light[i], frag_pos, norm, viewDir, Diffuse, Specular);
        }

        else if(light[i].l_type == 2)    // spot
        {
            result += CalcSpotLight(light[i], frag_pos, norm, viewDir, Diffuse, Specular); 
        }
    }

    //outColor = vec4( ( m_ambient*result ) , 1.0);       //FragColor = vec4(result, 1.0);
    outColor = vec4( ( m_diffuse*result ) , 1.0);       //FragColor = vec4(result, 1.0);
    //outColor = vec4(NRM, 1.0);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PerLight l, vec3 frag_pos, vec3 normal, vec3 viewDir, vec3 Diffuse, vec3 Specular)
{
    vec3 lightDir = normalize(l.l_pos - frag_pos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = 2.f * dot(lightDir, normal) * normal - (lightDir);    //vec3 reflectDir = reflect(-lightDir, normal);
    float spec;
    if(Diffuse == vec3(0.0,0.0,0.0))
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    else
        spec = pow(max(dot(viewDir, reflectDir), 0.0), Specular.r * Specular.r);

    // attenuation
    float distance = length(l.l_pos - frag_pos);
    float attenuation = 1.0 / (att_const.x + att_const.y * distance + att_const.z * (distance * distance));  
    
    // combine results
    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;

    if(Diffuse == vec3(0.0,0.0,0.0))
    {
        ambient = l.l_ambient_color * color.xyz;     
        diffuse = l.l_diffuse_color * diff;
        specular = l.l_specular_color * spec;
    }
    else
    {
        ambient = l.l_ambient_color * Diffuse * m_ambient;                      // * vec3(texture(material.diffuse, UV));
        diffuse = l.l_diffuse_color * diff * Diffuse * m_diffuse;            // * vec3(texture(material.diffuse, TexCoords));
        specular = l.l_specular_color * spec * Specular * m_specular;          // * vec3(texture(material.specular, TexCoords));
    }
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}


// calculates the color when using a directional light.
vec3 CalcDirLight(PerLight l, vec3 frag_pos, vec3 normal, vec3 viewDir, vec3 Diffuse, vec3 Specular)
{
    vec3 lightDir =  normalize(-l.l_dir);          //normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = 2.f * dot(lightDir, normal) * normal - (lightDir);    //vec3 reflectDir = reflect(-lightDir, normal);
    float spec;
    if(Diffuse == vec3(0.0,0.0,0.0))
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    else
        spec = pow(max(dot(viewDir, reflectDir), 0.0), Specular.r * Specular.r);





    // combine results
    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;
    if(Diffuse == vec3(0.0,0.0,0.0))
    {
        ambient = l.l_ambient_color * color.xyz;         
        diffuse = l.l_diffuse_color * diff;
        specular = l.l_specular_color * spec;
    }
    else
    {
        ambient = l.l_ambient_color * Diffuse * m_ambient;                      // * vec3(texture(material.diffuse, UV));
        diffuse = l.l_diffuse_color * diff * Diffuse * m_diffuse;            // * vec3(texture(material.diffuse, TexCoords));
        specular = l.l_specular_color * spec * Specular * m_specular;          // * vec3(texture(material.specular, TexCoords));
    }

    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(PerLight l, vec3 frag_pos, vec3 normal, vec3 viewDir, vec3 Diffuse, vec3 Specular)
{
    vec3 lightDir = normalize(l.l_pos - frag_pos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = 2.f * dot(lightDir, normal) * normal - (lightDir);    //vec3 reflectDir = reflect(-lightDir, normal);
    float spec;
    if(Diffuse == vec3(0.0,0.0,0.0))
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    else
        spec = pow(max(dot(viewDir, reflectDir), 0.0), Specular.r * Specular.r);

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
    if(Diffuse == vec3(0.0,0.0,0.0))
    {
        ambient = l.l_ambient_color * color.xyz;         
        diffuse = l.l_diffuse_color * diff;
        specular = l.l_specular_color * spec;
    }
    else
    {
        ambient = l.l_ambient_color * Diffuse * m_ambient;                      // * vec3(texture(material.diffuse, UV));
        diffuse = l.l_diffuse_color * diff * Diffuse * m_diffuse;            // * vec3(texture(material.diffuse, TexCoords));
        specular = l.l_specular_color * spec * Specular * m_specular;          // * vec3(texture(material.specular, TexCoords));
    }
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}