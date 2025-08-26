/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: object.h
Purpose:
    Declaration for Object class
    Sends uniform data and draw primitives
Language: C++ 17
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_1
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/
#pragma once
#include <mesh.h>

using shdr_vec = std::vector<std::pair<GLenum, std::string>>;

class Object
{
public:

    Object(Vec3 pos = { 0.f,0.f,0.f }, Vec3 s = { 1.f,1.f,1.f }, Vec3 rot = { 0.f,0.f,0.f }, Vec4 col = { 0.f,0.f,0.f,1.f });
    void init(Mesh* m, shdr_vec& shdr_files);
    void update(double dt, Mat4& view, Mat4& projection, Vec3& lightpos);
    void draw(bool show_fnormal = { false }, bool show_vnormal = { false });
    void draw_orbit();
    void cleanup();

    void set_color(Vec4 color) 
    { 
        if (color.r > 1.f) { color.r = 1.f; }
        if (color.g > 1.f) { color.g = 1.f; }
        if (color.b > 1.f) { color.b = 1.f; }

        selfColor = color; 
        is_updated = true;
    }

    void set_position(Vec3 pos)
    {
        position = pos;
        is_updated = true;
    }
    void set_scale(Vec3 s)
    {
        scale = s;
        is_updated = true;
    }
    void set_rotation(Vec3 angle)
    {
        rotation = angle;
        is_updated = true;
    }

    Vec3& Get_position() { return position; }
    Vec3& Get_scale() { return scale; }
    Vec3& Get_Rotation() { return rotation; }
    GLSLShader GetShdr_pgm() { return shdr_pgm; };

private:
    GLSLShader shdr_pgm;
    GLuint shdr_handle{ 0 };


	Vec3 position{ 0,0,0 };
	Vec3 scale{ 1,1,1 };
	Vec3 rotation{ 0,0,0 };
	Vec4 selfColor{ 0,0,0,1 };
    glm::mat4 model{ glm::mat4(1.f) };

    bool is_updated{ false };
	
    Mesh* mesh;

};