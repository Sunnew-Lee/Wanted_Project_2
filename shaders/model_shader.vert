#version 400 core

/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: model_shader.vert
Purpose: 
	Vertex shader for models.
Language: GLSL
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_1
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nrm;
layout (location = 2) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 mapping;

out vec2 UV;
out vec3 NRM;
out vec3 frag_pos;

void main(void) 
{
    gl_Position = projection * view * model * mapping * vec4(pos, 1.0);
    frag_pos = vec3(model * mapping * vec4(pos, 1.0));

    UV = uv;
    NRM = mat3(transpose(inverse(model))) * nrm;
}