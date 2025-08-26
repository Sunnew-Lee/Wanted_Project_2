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
Project: sunwoo.lee_CS300_2
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/
#pragma once
#include <mesh.h>

using shdr_vec = std::vector<std::pair<GLenum, std::string>>;

struct Global
{
    int active_lights{ 2 };
    Vec3 att_const{ 1.0f,1.0f,1.0f };
    Vec3 g_ambient{ 0.f,0.f,0.1f };
    Vec3 fog_color{ 0.f,0.f ,0.f };
    Vec2 fog_min_max{ 0.01f,20.f };
};

struct Material
{
    Vec3 ambient{ 0.2f,0.2f,0.2f };
    Vec3 diffuse{ 1.f,1.f,1.f };
    Vec3 specular{ 1.f,1.f,1.f };
    Vec3 emissive{ 0.2f,0.2f,0.2f };

    int selected_projection{ 0 };
    int selected_pipeline{ 0 };
    int selected_entity{ 0 };
};

enum class LightType
{
    POINT,
    DIRECTIONAL,
    SPOTLIGHT,
    COUNT
};
struct Per_Light
{
    LightType l_type{ LightType::POINT };
    Vec3 ambient_color{ 1.f,1.f ,1.f };
    Vec3 diffuse_color{ 1.f,1.f ,1.f };
    Vec3 specular_color{ 1.f,1.f ,1.f };
    Vec3 pos{ 0.5f,0.5f ,0.5f };
    Vec3 dir{ 0.5f,0.5f ,0.5f };
    Vec2 inner_outer_angle{ 15.f,20.f };
    float falloff{ 1.0f };
};
struct G_Data
{
    GLuint gPosition{ 0 }, gNormal{ 0 }, gDiff{ 0 }, gSpec{ 0 }, gVisibility{ 0 }, gAmbient{ 0 }, gEmissive{ 0 };
};

struct aiNode;
struct aiScene;
struct aiMesh;


struct TreeNode;
struct S_TreeNode;
struct OctreeNode;
struct Triangle;


class Object
{
public:
    Object(Vec3 pos = { 0.f,0.f,0.f }, Vec3 s = { 1.f,1.f,1.f }, Vec3 rot = { 0.f,0.f,0.f }, Vec4 col = { 0.f,0.f,0.f,1.f });
    void init(const std::string& path, std::vector<Texture> tex = std::vector<Texture>());
    void init(Mesh* m, GLSLShader& shader, std::vector<Texture> tex = std::vector<Texture>());
    void init(Mesh* m);
    void update(GLSLShader& shader, Mat4& view, Mat4& projection, Vec3& eye,Global& g, Per_Light* pl, Material& m);
    void draw(bool show_fnormal = { false }, bool show_vnormal = { false });
    void draw_orbit(GLSLShader& shader);
    void change_mesh(Mesh* m);
    void change_shdr(std::pair<shdr_vec, unsigned>);
    void change_projection(int projection, int entity);
    void cleanup();
    void g_buffer_update(GLSLShader& shader, Mat4& view, Mat4& projection, Global& g, Material& m, bool use_material);
    void g_buffer_draw(GLSLShader& shader);
    void lightpass_update(GLSLShader& shader, Vec3& eye, Global& g, Per_Light* pl, Material& m, G_Data g_data, Vec4 plane_color);
    void renderQuad(GLSLShader& shader);

    void load_object(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    unsigned int TextureFromFile(const char* path, bool gamma = false);
    void Draw(GLSLShader& shader);
    void Draw_NRM(GLSLShader& shader, bool show_vn, bool show_fn, int selected_bv);
    void Draw_BVH(GLSLShader& shader, int selected_bv);
    void Draw_OCT(GLSLShader& shader, bool show_oct_aabb);
    void Draw_BSP(GLSLShader& shader);
    void inorderTraversal(GLSLShader& shdr, TreeNode* root);
    void inorderTraversal(GLSLShader& shdr, S_TreeNode* root);
    void inorderTraversal(GLSLShader& shdr, OctreeNode* root);
    void inorderTraversal(GLSLShader& shdr, BSPNode* root);
    void inorderTraversal_NoAABB(GLSLShader& shdr, OctreeNode* root);
    void Add_mesh(Mesh* m);

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
    Vec4& Get_Color() { return selfColor; }
    //GLSLShader GetShdr_pgm() { return shdr_pgm; };

    void BVH_Setup();
    void Octree_Setup();
    void BSP_Setup();
    void FindNodesToMerge(std::vector<TreeNode*>& nodes, TreeNode* &first, TreeNode* &second);
    void FindNodesToMerge(std::vector<S_TreeNode*>& nodes, S_TreeNode*& first, S_TreeNode*& second);
    TreeNode* BottomUpConstructionAABB(std::vector<TreeNode*>& nodes);
    S_TreeNode* BottomUpConstructionSPHERE(std::vector<S_TreeNode*>& nodes);
    AABB* ExtendAABB(TreeNode* first, TreeNode* second);
    SPHERE* ExtendSPHERE(S_TreeNode* first, S_TreeNode* second);

    void Build_Octree(OctreeNode* node);
    void DivideOrAdd(OctreeNode* node, Triangle tri);
    void DivideOrAdd(BSPNode* node, Triangle tri);
    std::pair<bool, std::vector<Triangle>> IntersectRayPlane(OctreeNode* node, int axisIndex, Triangle v);
    std::pair<bool, std::vector<std::pair<bool, Triangle>>> IntersectRayPlane(BSPNode* node, int axisIndex, Triangle v);
    bool Tri_check(const Triangle& tri, AABB* aabb);

    void Build_BSP(BSPNode* node);

private:
    //GLSLShader shdr_pgm;
    //GLuint shdr_handle{ 0 };
    unsigned shdr_type{ 0 };


	Vec3 position{ 0,0,0 };
	Vec3 scale{ 1,1,1 };
	Vec3 rotation{ 0,0,0 };
	Vec4 selfColor{ 0,0,0,1 };
    glm::mat4 model{ glm::mat4(1.f) };

    bool is_updated{ false };
	
    Mesh* temp;
    //std::vector<GLuint> textureHandle;
    //GLuint textureHandle;

    Vec3 emissive{ 0.f,0.f ,0.f };
    int projection_type{ 0 };

    std::string directory;
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;

    TreeNode* BottomUp_AABB_root;
    S_TreeNode* BottomUp_Ritter_root;
    S_TreeNode* BottomUp_Larsson_root;
    S_TreeNode* BottomUp_PCA_root;

    OctreeNode* Octree_root;
    GLuint Oct_VAO = 0;
    GLuint Oct_VBO = 0;

    BSPNode* BSP_root;
    GLuint BSP_VAO = 0;
    GLuint BSP_VBO = 0;

    //GLuint BottomUp_AABB_VAO = 0;
    //GLuint BottomUp_AABB_VBO[7] = { 0 };

    std::list<Triangle> triangles;
    std::vector<glm::vec4> colors;
};