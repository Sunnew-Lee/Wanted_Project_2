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
Project: sunwoo.lee_CS300_1
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/
#pragma once
#include <object.h>

enum class MeshType
{
	BUNNY,
	SPHERE4,
	CUBE2,
	SPHERE,
	SPHERE_MOD,
	//CUP,
	//LUCY,
	//STARWARS,
	PROCEDURAL_SPHERE,
	ORBIT,
	COUNT
};

enum class ShdrType
{
	MODEL_PHONG,
	LINE,
	COUNT
};

class Scene
{
public:
	Scene() {};
	~Scene() {};

	void init(int w, int h);
	void update(double delta_time);
	void draw();
	void cleanup();

private:
	void mesh_setup();
	void shdr_file_setup();

	glm::mat4 view{ glm::mat4(1.0f) };
	glm::mat4 projection{ glm::mat4(1.0f) };
	Vec3 lightPos{ -5.f, 3.f, 5.f };

	static constexpr int SATELLITE_NUM{ 8 };
	static constexpr int ORBIT_VERT{ 100 };
	static constexpr float RAD = 3.f;
	static constexpr float ANGLE = 360.f / SATELLITE_NUM;

	std::vector<shdr_vec> shdr_files;
	std::vector<Mesh*> meshes;

	Object CenterOBJ;
	Object Satellites[SATELLITE_NUM];

	Object Orbit;

	bool show_fnormal{ false };
	bool show_vnormal{ false };

	static constexpr int ITEM_NUM = 5;
	const char* mesh_items[ITEM_NUM]
	{
		"BUNNY","SPHERE4","CUBE2","SPHERE","SPHERE_MOD"
	};
	int selected_item{ 0 };
};