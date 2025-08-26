#version 400 core

/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: line_shader.vert
Purpose: 
	Vertex shader for lines.
Language: GLSL
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_1
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/

layout (location=0) in vec3 pos;

uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view  * vec4(pos, 1.0);
}
