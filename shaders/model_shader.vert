#version 450 core

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

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 UV;
out vec3 NRM;
out vec3 frag_pos;


void main(void) 
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    frag_pos = vec3(model * vec4(pos, 1.0));


    UV = uv;
    
    //else if(pipeline == 1)
    //{
    //    vec3 t_pos;
    //
    //    if(entity==0)
    //        t_pos = vec3(mapping * vec4(pos, 1.0));
    //    else if(entity==1)
    //        t_pos = normalize(nrm);
    //
    //    if(project_type == 0)   //Cylinder
    //    {
    //        //float theta = degrees(atan(t_pos.z/t_pos.x))+90.;
    //        float theta = degrees(atan(t_pos.x / t_pos.z)) + 90;
    //        UV = vec2(theta/360., (t_pos.y - (-1)) / 2);
    //    }
    //    else if(project_type == 1)   //Sphere
    //    {
    //        //float theta = degrees(atan(t_pos.z/t_pos.x))+90.;
    //        float theta = degrees(atan(t_pos.x / t_pos.z)) + 90.;
    //        float phi = degrees(acos((t_pos.y/length(t_pos))));
    //        UV = vec2(theta/360., phi/180.);
    //    }
    //    //else if(project_type == 2)   //Cube
    //    //{
    //    //    vec3 abspos = vec3(abs(t_pos));
    //    //    //// +-X
	//	//    if (abspos.x >= abspos.y && abspos.x >= abspos.z)
	//	//    {
	//	//    	(t_pos.x < 0.0) ? (UV.x = t_pos.y) : (UV.x = -t_pos.y);
	//	//    	UV.y = t_pos.z;
	//	//    }
	//	//    // +-Y
	//	//    else if (abspos.y >= abspos.x && abspos.y >= abspos.z)
	//	//    {
	//	//    	(t_pos.y < 0.0) ? (UV.y = t_pos.z) : (UV.y = -t_pos.z);
	//	//    	UV.x = t_pos.x;
	//	//    }
	//	//    // +-Z
	//	//    else if (abspos.z >= abspos.x && abspos.z >= abspos.y)
	//	//    {
	//	//    	(t_pos.z < 0.0) ? (UV.x = -t_pos.x) : (UV.x = t_pos.x);
	//	//    	UV.y = -t_pos.y;
	//	//    }
//  //
    //    //    UV = (UV+vec2(1.f)) * 0.5f;
    //    //}
    //    else if(project_type == 2)   //Cube
    //    {
    //        vec3 abspos = vec3(abs(t_pos));
    //        //// +-X
	//	    if (abspos.x >= abspos.y && abspos.x >= abspos.z)
    //        {
    //            UV.y = t_pos.y/abspos.x;
    //            if(t_pos.x < 0.0)
    //            {
    //                UV.x = -t_pos.z/abspos.x;
    //            }
    //            else
    //            {
    //                UV.x = t_pos.z/abspos.x;
    //            }
    //        }
    //        // +-Y
    //        else if (abspos.y >= abspos.x && abspos.y >= abspos.z)
    //        {
    //            UV.x = t_pos.x/abspos.y;
    //    
    //            if(t_pos.y < 0.0)
    //            {
    //                UV.y = -t_pos.z/abspos.y;
    //            }
    //            else
    //            {
    //                UV.y = t_pos.z/abspos.y;
    //            }
    //        }
    //        // +-Z
    //        else if (abspos.z >= abspos.x && abspos.z >= abspos.y)
    //        {
    //            UV.y = t_pos.y/abspos.z;
    //            if(t_pos.z < 0.0)
    //            {
    //                UV.x = t_pos.x/abspos.z;
    //            }
    //            else
    //            {
    //                UV.x = -t_pos.x/abspos.z;
    //            }
    //        }
    //        
    //        UV = (UV+vec2(1.f)) * 0.5f;
    //    }
    //    else if(project_type == 3)   //Planer
    //    {
    //        UV = vec2((t_pos.x - (-1)) / 2, (t_pos.y - (-1)) / 2);
    //    }
    //}

    NRM = mat3(transpose(inverse(model))) * nrm;
}