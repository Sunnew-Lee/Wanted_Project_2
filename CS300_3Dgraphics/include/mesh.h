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
Project: sunwoo.lee_CS300_2
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

using Vec2 = glm::vec2;
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

struct Texture {
    unsigned int id{ 0 };
    std::string type;
    std::string path;
};

class BV
{
public:
    BV() {};
    BV(glm::vec3 center_):center(center_) {};
    glm::vec3 center;
private:
};
class AABB :public BV
{
public:
    AABB() :BV(glm::vec3(0.f)) {};
    AABB(glm::vec3 center) : BV(center) {};
    AABB(glm::vec3 min_, glm::vec3 max_) : BV(glm::vec3((min_.x + max_.x), (min_.y + max_.y), (min_.z + max_.z))*0.5f), min(min_), max(max_) {};

    std::vector<glm::vec3> lines = std::vector<glm::vec3>();
    glm::vec3 min= glm::vec3(0.f);
    glm::vec3 max= glm::vec3(0.f);
    void CreateAABB();
};
class SPHERE :public BV
{
public:
    SPHERE() :BV(glm::vec3(0.f)) {};
    SPHERE(glm::vec3 center) : BV(center) {};
    float radius;
    std::vector<Vertex> vbuffer;
    std::vector<unsigned int> ibuffer;
    void CreateSphere(int stacks, int slices);

private:
    void BuildIndexBuffer(int stacks, int slices);
    bool DegenerateTri(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
};

struct TreeNode
{
    TreeNode(AABB* bv_) :bv(bv_), level(1) 
    {
        Gen_VAO();
    };
    TreeNode(TreeNode* left, TreeNode* right) :lChild(left), rChild(right) {};
    int type;
    AABB* bv;
    int numObjects;
    int level = 1;
    TreeNode* lChild{ nullptr }, * rChild{ nullptr };
    GLuint TREE_VAO = 0;
    GLuint TREE_VBO = 0;
    void Gen_VAO();
};

struct S_TreeNode
{
    S_TreeNode(SPHERE* bv_) :bv(bv_), level(1)
    {
        Gen_VAO();
    };
    S_TreeNode(S_TreeNode* left, S_TreeNode* right) :lChild(left), rChild(right) {};
    int type;
    SPHERE* bv;
    int numObjects;
    int level = 1;
    S_TreeNode* lChild{ nullptr }, * rChild{ nullptr };
    GLuint TREE_VAO = 0;
    GLuint TREE_VBO = 0;
    GLuint TREE_EBO = 0;
    void Gen_VAO();
};

struct Triangle
{
    Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c)  
    {
        vertices.push_back(a);
        vertices.push_back(b);
        vertices.push_back(c);
    };

    std::vector<glm::vec3> vertices;
};

struct OctreeNode
{
    OctreeNode(AABB* aabb_, int level_, std::list<Triangle> tri = std::list<Triangle>()) :aabb(aabb_), level(level_), triangles(tri)
    {
        Gen_VAO();
    };
    int level;
    OctreeNode* pChildren[8]{nullptr};
    AABB* aabb;
    std::list<Triangle> triangles;
    GLuint TREE_VAO = 0;
    GLuint TREE_VBO = 0;
    void Gen_VAO();
};

struct BSPNode
{
    BSPNode(int level_, std::list<Triangle> tri = std::list<Triangle>()) : level(level_), triangles(tri) {};

    int level;
    glm::vec3 center{ glm::vec3(0.f) };
    BSPNode* FrontNode{ nullptr };
    BSPNode* BackNode{ nullptr };
    std::list<Triangle> triangles;
    glm::vec4 color{ glm::vec4(0.f) };
};

struct Mesh
{
    Mesh() {};
    Mesh(std::string path);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, float minmax[3][2], std::vector<Vec3> fnbuffer);

    /*  Storing the actual vertex/index data */
    std::vector<Vertex> vertexBuffer;
    std::vector<unsigned int> indexBuffer;  // note that we start from 0

    std::vector<Texture> textureBuffer;
    void Setup();
    void Draw(GLSLShader& shader);
    void Draw_NRM(GLSLShader& shader, bool show_vn, bool show_fn, int selected_bv);

    std::map<int, std::vector<glm::vec3>> faces;

    std::vector<glm::vec3> fnBuffer;
    std::vector<glm::vec3> vnBuffer;
    std::vector<glm::vec3> lineposBuffer;

    static constexpr float  EPSILON = 0.00001f;
    static constexpr float  LINE_SCALE = 0.125f;

    GLuint VBO[7] = { 0 };
    GLuint VAO = 0;
    GLuint EBO[4] = { 0 };
    GLuint UBO = 0;

    glm::mat4 mapping{ glm::mat4(1.f) };
    glm::mat4 OBJ_Translate{ glm::mat4(1.f) };
    glm::mat4 OBJ_Scale{ glm::mat4(1.f) };
    
    //float xyz_minmax[3][2]{};

    void SendVertexData();
    void UpdateVertexData();
    void readOBJ(const std::filesystem::path& fileName);
    void vert_mapping(float xyz_minmax[3][2]);
    void setup_mesh(GLSLShader& shdr);
    void calc_vert_normal();
    void calc_vert_uv(int type, int entity_type);

    void cleanup();

    
    bool DegenerateTri(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);

    void calc_BufferDatas();
    void LineVertexData();
    void setup_UBO(unsigned);

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

	void uv_Cylinder(int entity);
	void uv_Sphere();
	void uv_Cube(int entity);
	void uv_Planer(int entity);

    void AABB_Setup(float xyz_minmax[3][2]);
    //struct AABB
    //{
    //    glm::vec3 center;
    //    glm::vec3 half_extents;
    //    std::vector<glm::vec3> lines;
    //};
    AABB* bounding_box;

    void SPHERE_Setup(int num);
    std::pair<glm::vec3, glm::vec3> extremePointsAlongDirection(glm::vec3 dir);
    //struct SPHERE
    //{
    //    glm::vec3 center;
    //    float radius;
    //    std::vector<Vertex> vbuffer;
    //    std::vector<unsigned int> ibuffer;
    //};
    SPHERE* Ritter;
    SPHERE* Larsson;
    SPHERE* PCA;
    
    void BuildIndexBuffer(int stacks, int slices, SPHERE* sphere);
};

Mesh* CreateSphere(int stacks, int slices);
Mesh* CreatePlane(int stacks, int slices);
Mesh* CreateOrbit(float radius, GLuint vert);
Mesh* CreateQuad();