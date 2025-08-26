/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: scene.cpp
Purpose: 
	This file contains definitions of member functions of class Scene.
Language: C++ 17
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_1
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/
#include <scene.h>
#include <imgui.h>

void Scene::init(int width, int height)
{
	glViewport(0, 0, width, height);
	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, 10.0f));
	view = glm::lookAt(glm::vec3(0.0f, 3.0f, 7.50f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	shdr_file_setup();

	mesh_setup();

	CenterOBJ.init(meshes[static_cast<int>(MeshType::BUNNY)], shdr_files[static_cast<int>(ShdrType::MODEL_PHONG)]);
	CenterOBJ.set_color({ 0.75f,0.45f,0.3f,1.f });

	for (int i{ 0 }; i < SATELLITE_NUM; i++)
	{
		Satellites[i].init(meshes[static_cast<int>(MeshType::PROCEDURAL_SPHERE)], shdr_files[static_cast<int>(ShdrType::MODEL_PHONG)]);
		Satellites[i].set_position(glm::vec3(RAD * glm::cos(glm::radians(ANGLE * i)), 0.f, RAD * glm::sin(glm::radians(ANGLE * i))));
		Satellites[i].set_scale({ 0.1f,0.1f ,0.1f });

		if (i % 2 == 0) 
		{
			Satellites[i].set_color({ 0.8f,0.25f,0.25f, 1.f });
		}
		else
		{
			Satellites[i].set_color({ 0.25f,0.25f,0.8f, 1.f });
		}
	}

	Orbit.init(meshes[static_cast<int>(MeshType::ORBIT)], shdr_files[static_cast<int>(ShdrType::LINE)]);
	Orbit.set_color(glm::vec4(1.f));
}

void Scene::update(double delta_time)
{
	{
		ImGui::Begin("Scene");
		if (ImGui::Combo("Mesh", &selected_item, mesh_items, IM_ARRAYSIZE(mesh_items)))
		{
			switch (selected_item)
			{
			case 0:CenterOBJ.init(meshes[static_cast<int>(MeshType::BUNNY)], shdr_files[static_cast<int>(ShdrType::MODEL_PHONG)]); break;
			case 1:CenterOBJ.init(meshes[static_cast<int>(MeshType::SPHERE4)], shdr_files[static_cast<int>(ShdrType::MODEL_PHONG)]); break;
			case 2:CenterOBJ.init(meshes[static_cast<int>(MeshType::CUBE2)], shdr_files[static_cast<int>(ShdrType::MODEL_PHONG)]); break;
			case 3:CenterOBJ.init(meshes[static_cast<int>(MeshType::SPHERE)], shdr_files[static_cast<int>(ShdrType::MODEL_PHONG)]); break;
			case 4:CenterOBJ.init(meshes[static_cast<int>(MeshType::SPHERE_MOD)], shdr_files[static_cast<int>(ShdrType::MODEL_PHONG)]); break;

			default: CenterOBJ.init(meshes[static_cast<int>(MeshType::BUNNY)], shdr_files[static_cast<int>(ShdrType::MODEL_PHONG)]); break;
			}
		}

		ImGui::Checkbox("Show face normal ", &show_fnormal);
		ImGui::Checkbox("Show vertex normal", &show_vnormal);

		ImGui::SliderFloat3("Light Position", &lightPos.x, -10.f, 10.f);
		ImGui::End();
	}

	CenterOBJ.set_rotation(glm::vec3(0.f, (float)glfwGetTime() * glm::radians(-50.0f), 0.f));
	CenterOBJ.update(delta_time, view, projection, lightPos);

	for (int i{ 0 }; i < SATELLITE_NUM; i++)
	{
		Satellites[i].set_position(glm::vec3(3.f * glm::cos(glm::radians(45.0f * i - (float)glfwGetTime()*20.0)), 0.f, 3.f * glm::sin(glm::radians(45.0f * i - (float)glfwGetTime() * 20.0))));
		Satellites[i].update(delta_time, view, projection, lightPos);
	}

	Orbit.update(delta_time, view, projection, lightPos);
}

void Scene::draw()
{
	CenterOBJ.draw(show_fnormal, show_vnormal);

	for (Object& satellite : Satellites)
	{
		satellite.draw();
	}

	Orbit.draw_orbit();

}

void Scene::cleanup()
{
	CenterOBJ.cleanup();

	for (Object& satellite : Satellites)
	{
		satellite.cleanup();
	}

	Orbit.cleanup();

	for (Mesh* mesh : meshes)
	{
		mesh->cleanup();
		delete mesh;
	}
}

void Scene::mesh_setup()
{
	std::string sphere4{ "../3Dmodels/4Sphere.obj" };
	std::string bunny{ "../3Dmodels/bunny_high_poly.obj" };
	std::string cube2{ "../3Dmodels/cube2.obj" };
	std::string sphere{ "../3Dmodels/sphere.obj" };
	std::string sphere_mod{ "../3Dmodels/sphere_modified.obj" };
	//std::string Cen_path{"../3Dmodels/cup.obj"};				\par?
	//std::string Cen_path{"../3Dmodels/lucy_princeton.obj"};	?
	//std::string Cen_path{"../3Dmodels/starwars1.obj"};		many obj in one file


	meshes.push_back(new Mesh(bunny));
	meshes.push_back(new Mesh(sphere4));
	meshes.push_back(new Mesh(cube2));
	meshes.push_back(new Mesh(sphere));
	meshes.push_back(new Mesh(sphere_mod));

	int stacks{ 50 }, slices{ 50 };
	meshes.push_back(CreateSphere(stacks, slices));
	meshes.push_back(CreateOrbit(RAD, ORBIT_VERT));
}

void Scene::shdr_file_setup()
{
	shdr_vec model_shdr_files;
	model_shdr_files.push_back(std::make_pair(GL_VERTEX_SHADER, "../shaders/model_shader.vert"));
	model_shdr_files.push_back(std::make_pair(GL_FRAGMENT_SHADER, "../shaders/model_shader.frag"));
	shdr_files.push_back(model_shdr_files);

	shdr_vec line_shdr_files;
	line_shdr_files.push_back(std::make_pair(GL_VERTEX_SHADER, "../shaders/line_shader.vert"));
	line_shdr_files.push_back(std::make_pair(GL_FRAGMENT_SHADER, "../shaders/line_shader.frag"));
	shdr_files.push_back(line_shdr_files);
}
