/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: scene.h
Purpose:
	Declaration for Scene class, enum class for MeshType and ShaderType
	Handles ImGui
Language: C++ 17
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_2
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/
#pragma once
#include <object.h>

enum class MeshType
{
	//BUNNY=0,
	//SPHERE4,
	//CUBE2,
	//SPHERE,
	//SPHERE_MOD,
	//CUP,
	//LUCY,
	//STARWARS,
	//PROCEDURAL_SPHERE,
	ORBIT,
	PROCEDURAL_PLANE,
	PROCEDURAL_QUAD,
	COUNT
};

enum class ShdrType
{
	MODEL_PHONG = 0,
	//MODEL_BLINN,
	//MODEL_PHONG_L,
	G_BUFFER,
	MODEL_DEFFERED,
	LINE,
	LIGHT,
	NRM,
	COUNT
};

enum class TexType
{
	METAL_ROOF_DIFF = 0,
	METAL_ROOF_SPEC,
	GRID,
	COUNT
};

class Camera;
class Scene
{
public:
	Scene():Width(0),Height(0),cam(nullptr),gBuffer(0) {};
	~Scene() {};

	void init(int w, int h, Camera* cam);
	void update(double delta_time);
	void draw();
	void cleanup();

private:

	void g_buffer_setup();
	void mesh_setup();
	void texture_setup();
	void read_PPM(std::string path);
	void shdr_file_setup();
	void light_setup(int scenario = 0);
	void update_IMGUI(double dt);
	void read_section();

	glm::mat4 view{ glm::mat4(1.0f) };
	glm::mat4 projection{ glm::mat4(1.0f) };
	Vec3 cam_pos{ 0.f,0.f,0.f };
	Vec2 Z_near_far{0.1f, 10000000.f};

	static constexpr int MAX_LIGHT_NUM{ 16 };
	static constexpr int ORBIT_VERT{ 100 };
	static constexpr float RAD = 3.f;
	//static constexpr float ANGLE = 360.f / MAX_LIGHT_NUM;

	std::vector<GLSLShader> shaders;
	//std::vector<std::pair<shdr_vec, unsigned>> shdr_files;
	std::vector<Mesh*> meshes;
	//std::vector<GLuint> textures;
	std::vector<std::vector<std::string>> sections;

	Object Test;
	Object CenterOBJ;
	std::vector<Texture> textures;
	Object Lights[MAX_LIGHT_NUM];

	Object Orbit;
	Object Plane;

	Object gQuad;

	Global g;
	Per_Light pl[MAX_LIGHT_NUM];
	Material m;

	bool show_bv{ false };
	bool show_oct{ true };
	bool show_oct_aabb{ true };
	bool show_bsp{ false };
	bool show_fnormal{ false };
	bool show_vnormal{ false };

	static constexpr int ITEM_NUM = 5;
	const char* mesh_items[ITEM_NUM]
	{
		"BUNNY","SPHERE4","CUBE2","SPHERE","SPHERE_MOD"
	};
	int selected_mesh{ 0 };

	const char* light_items[MAX_LIGHT_NUM]
	{
		"Light #1","Light #2","Light #3","Light #4",
		"Light #5","Light #6","Light #7","Light #8",
		"Light #9","Light #10","Light #11","Light #12",
		"Light #13","Light #14","Light #15","Light #16"
	};
	int selected_light{ 0 };

	const char* lType_items[static_cast<int>(LightType::COUNT)]
	{
		"Point","Direction","Spot"
	};
	int selected_lType{ 0 };

	static constexpr int BV_NUM = 4;
	const char* bv_items[BV_NUM]
	{
		"AABB","SPHERE_Ritter","SPHERE_Larsson","SPHERE_PCA"
	};
	int selected_bv{ 0 };


	static constexpr int Hierarchy_NUM = 2;
	const char* Hierarchy_items[Hierarchy_NUM]
	{
		"Bottom-Up","Top-Down"
	};
	int selected_hierarchy{ 0 };

	bool is_paused{ false };

	double elapsed_time{ 0.0 };

	//Vec3 emissive{ 0.f,0.f ,0.f };
	Camera* cam;
	GLuint gBuffer;
	G_Data g_data;
	int Width, Height;

	bool do_forward = true;
};