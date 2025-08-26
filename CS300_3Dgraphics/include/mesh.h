/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: mesh.h
Purpose:
    Declaration for Mesh struct and Vertex struct
    Setup VAO, VBO and EBO
    Loads .obj files and calculates to translate to origin and map to [-1,1]
    Generate and bind buffers
Language: C++ 17
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_1
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/
#pragma once
#include <glslshader.h>
#include <filesystem>

#include <GL/glew.h> // for access to OpenGL API declarations 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>

using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Mat4 = glm::mat4;

struct Vertex
{
    Vertex(glm::vec3 pos, glm::vec3 nrm, glm::vec2 uv) : pos(pos), nrm(nrm), uv(uv) {}

    Vertex() : pos({ 0,0,0 }), nrm({ 0,0,0 }), uv({ 0,0 }) {}

    glm::vec3 pos, nrm;
    glm::vec2 uv;

};

struct Mesh
{
    Mesh() {};
    Mesh(std::string path);

    /*  Storing the actual vertex/index data */
    std::vector<Vertex> vertexBuffer;
    std::vector<GLint> indexBuffer;  // note that we start from 0

    std::map<int, std::vector<glm::vec3>> faces;

    std::vector<glm::vec3> fnBuffer;
    std::vector<glm::vec3> vnBuffer;
    std::vector<glm::vec3> lineposBuffer;

    static constexpr float  EPSILON = 0.00001f;
    static constexpr float  LINE_SCALE = 0.125f;

    GLuint VBO[3] = { 0 };
    GLuint VAO = 0;
    GLuint EBO = 0;

    glm::mat4 mapping{ glm::mat4(1.f) };
    glm::mat4 OBJ_Translate{ glm::mat4(1.f) };
    glm::mat4 OBJ_Scale{ glm::mat4(1.f) };
    
    void SendVertexData();
    void UpdateVertexData();
    void readOBJ(const std::filesystem::path& fileName);
    void vert_mapping(float (*xyz_minmax)[2]);
    void setup_mesh(GLSLShader& shdr);
    void calc_vert_normal();

    void cleanup();

    void BuildIndexBuffer(int stacks, int slices);
    bool DegenerateTri(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);

    void calc_BufferDatas();
    void LineVertexData();

    glm::mat4 Get_mapping(void)
    {
        return mapping;
    }
    glm::mat4 Get_OBJ_Translate(void)
    {
        return OBJ_Translate;
    }
    glm::mat4 Get_OBJ_Scale(void)
    {
        return OBJ_Scale;
    }
};

Mesh* CreateSphere(int stacks, int slices);
Mesh* CreateOrbit(float radius, GLuint vert);