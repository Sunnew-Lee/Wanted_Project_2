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

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiff;
layout (location = 3) out vec4 gSpec;
layout (location = 4) out vec3 gAmbient;
layout (location = 5) out vec3 gEmissive;


in vec3 frag_pos;
in vec3 NRM;
in vec2 UV;
uniform vec3 m_ambient;
uniform vec3 m_emissive;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = frag_pos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(NRM);
	// also store the per-fragment texcoord into the gbuffer
    //gTexCoord = UV;
	
	//gDepth = vec3(length(frag_pos));
    // and the diffuse per-fragment color
    gDiff = texture(texture_diffuse1, UV);
    // and the specular per-fragment color
    gSpec = texture(texture_specular1, UV);

    gAmbient = m_ambient;
    gEmissive = m_emissive;
}