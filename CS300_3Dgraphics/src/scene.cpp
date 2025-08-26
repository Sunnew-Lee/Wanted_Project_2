/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: scene.cpp
Purpose: 
	This file contains definitions of member functions of class Scene.
Language: C++ 17
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_2
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/
#include <scene.h>
#include <imgui.h>
#include <Camera.h>



void Scene::init(int width, int height, Camera* camera)
{
	glViewport(0, 0, width, height);
	Width = width;
	Height = height;

	cam = camera;
	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, Z_near_far.x, Z_near_far.y);
	view = cam->GetViewMatrix();
	//view = glm::lookAt(cam_pos,
	//	glm::vec3(0.0f, 0.0f, 0.0f),
	//	glm::vec3(0.0f, 1.0f, 0.0f));

	g_buffer_setup();
	shdr_file_setup();
	light_setup();

	mesh_setup();
	read_section();

	texture_setup();
	//CenterOBJ.init("ppsection4/part_a/g1.obj");
	//CenterOBJ.init("ppsection4/part_a/g2.obj");
	//CenterOBJ.init("ppsection4/part_a/g0.obj");
	//CenterOBJ.init("ppsection4/part_b/g1.obj");
	//CenterOBJ.init("ppsection4/part_b/g0.obj");
	for (auto v : sections)
	{
		for (std::string s : v)
		{
			CenterOBJ.init(s);
		}
	}

	CenterOBJ.BVH_Setup();
	CenterOBJ.Octree_Setup();
	CenterOBJ.BSP_Setup();
	//CenterOBJ.init("../3Dmodels/bunny_high_poly.obj", textures);
	CenterOBJ.set_color({ 0.75f,0.45f,0.3f,1.f });
	
	gQuad.Add_mesh(meshes[static_cast<int>(MeshType::PROCEDURAL_QUAD)]);
	//CenterOBJ.set_color({ 0.75f,0.45f,0.3f,1.f });
	//Plane.init(meshes[static_cast<int>(MeshType::PROCEDURAL_PLANE)]);

	//CenterOBJ.init(meshes[static_cast<int>(MeshType::BUNNY)], shdr_files[static_cast<int>(ShdrType::MODEL_PHONG)], textures);

		
	//Orbit.init(meshes[static_cast<int>(MeshType::ORBIT)], shaders[static_cast<int>(ShdrType::LINE)]);
	//Orbit.set_color(glm::vec4(1.f));

	//Plane.init(meshes[static_cast<int>(MeshType::PROCEDURAL_PLANE)], shdr_files[static_cast<int>(ShdrType::MODEL_PHONG)]);
	//Plane.Add_mesh(meshes[static_cast<int>(MeshType::PROCEDURAL_PLANE)]);
	//Plane.set_color({ 0.5f, 0.5f, 0.5f, 1.0f });
	//Plane.set_scale({ 15.f,15.f,1.f });
	//Plane.set_rotation(glm::vec3(glm::radians(90.0f), 0.f, 0.f));
	//Plane.set_position({ 0.f,-1.f,0.f });

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/*  Hidden surface removal */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
}

void Scene::update(double delta_time)
{
	update_IMGUI(delta_time);

	view = cam->GetViewMatrix();
	cam_pos = cam->Position;

	

	//Test.update(delta_time, shaders[static_cast<int>(ShdrType::G_BUFFER)], view, projection, cam_pos, g, pl, m);


	//CenterOBJ.set_rotation(glm::vec3(0.f, (float)glfwGetTime() * glm::radians(-50.0f), 0.f));
	//CenterOBJ.update(delta_time, view, projection, g, pl, m);


	CenterOBJ.update(shaders[static_cast<int>(ShdrType::NRM)], view, projection, cam_pos, g, pl, m);
	//Orbit.update(shaders[static_cast<int>(ShdrType::LINE)], view, projection, cam_pos, g, pl, m);

	//Plane.update(delta_time, view, projection, g, pl,m);
}

void Scene::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(g.fog_color.x, g.fog_color.y, g.fog_color.z, 1.f);

	// 1. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Plane.g_buffer_update(shaders[static_cast<int>(ShdrType::G_BUFFER)], view, projection, g, m, false);
	//Plane.g_buffer_draw(shaders[static_cast<int>(ShdrType::G_BUFFER)]);
	if (show_bv)
	{
		CenterOBJ.g_buffer_update(shaders[static_cast<int>(ShdrType::G_BUFFER)], view, projection, g, m, true);
		CenterOBJ.g_buffer_draw(shaders[static_cast<int>(ShdrType::G_BUFFER)]);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
	// -----------------------------------------------------------------------------------------------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (show_bv)
	{
		gQuad.lightpass_update(shaders[static_cast<int>(ShdrType::MODEL_DEFFERED)], cam_pos, g, pl, m, g_data, Plane.Get_Color());
		gQuad.renderQuad(shaders[static_cast<int>(ShdrType::MODEL_DEFFERED)]);
	}


	// 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
		// ----------------------------------------------------------------------------------
	//IMGUI 
	float ANGLE = 360.f / g.active_lights;

	if (do_forward == true)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
		glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//IMGUI
		//Orbit.draw_orbit(shaders[static_cast<int>(ShdrType::LINE)]);

		for (int i{ 0 }; i < g.active_lights; i++)
		{
			if (is_paused == true)
			{
				//Lights[i].update(shaders[static_cast<int>(ShdrType::LIGHT)], view, projection, cam_pos, g, pl, m);
				//Lights[i].Draw(shaders[static_cast<int>(ShdrType::LIGHT)]);
				continue;
			}

			Lights[i].set_position(glm::vec3(3.f * glm::cos(glm::radians(ANGLE * i - static_cast<float>(elapsed_time) * 20.f)), 0.f, 3.f * glm::sin(glm::radians(ANGLE * i - static_cast<float>(elapsed_time) * 20.f))));
			pl[i].pos = Lights[i].Get_position();
			pl[i].dir = -pl[i].pos;
			//Lights[i].update(shaders[static_cast<int>(ShdrType::LIGHT)], view, projection, cam_pos, g, pl, m);
			//Lights[i].Draw(shaders[static_cast<int>(ShdrType::LIGHT)]);
		}
		CenterOBJ.Draw_NRM(shaders[static_cast<int>(ShdrType::NRM)], show_vnormal, show_fnormal, selected_bv);
		if (show_bv)
		{
			CenterOBJ.Draw_BVH(shaders[static_cast<int>(ShdrType::NRM)], selected_bv);
		}
		if (show_oct)
		{
			CenterOBJ.Draw_OCT(shaders[static_cast<int>(ShdrType::NRM)], show_oct_aabb);
		}
		if (show_bsp)
		{
			CenterOBJ.Draw_BSP(shaders[static_cast<int>(ShdrType::NRM)]);
		}
			
	}
	else
	{
		for (int i{ 0 }; i < g.active_lights; i++)
		{
			if (is_paused == true)
			{
				continue;
			}

			Lights[i].set_position(glm::vec3(3.f * glm::cos(glm::radians(ANGLE * i - static_cast<float>(elapsed_time) * 20.f)), 0.f, 3.f * glm::sin(glm::radians(ANGLE * i - static_cast<float>(elapsed_time) * 20.f))));
			pl[i].pos = Lights[i].Get_position();
			pl[i].dir = -pl[i].pos;
		}
	}
	//ImGui::Image((ImTextureID)static_cast<GLuint64>(g_data.gPosition), ImVec2(Width * 0.1f, Height * 0.1f), ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::SameLine();
	//ImGui::Image((ImTextureID)static_cast<GLuint64>(g_data.gNormal), ImVec2(Width * 0.1f, Height * 0.1f), ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::SameLine();
	//ImGui::Image((ImTextureID)static_cast<GLuint64>(g_data.gDiff), ImVec2(Width * 0.1f, Height * 0.1f), ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::SameLine();
	//ImGui::Image((ImTextureID)static_cast<GLuint64>(g_data.gSpec), ImVec2(Width * 0.1f, Height * 0.1f), ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::Image((ImTextureID)static_cast<GLuint64>(g_data.gAmbient), ImVec2(Width * 0.1f, Height * 0.1f), ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::SameLine();
	//ImGui::Image((ImTextureID)static_cast<GLuint64>(g_data.gEmissive), ImVec2(Width * 0.1f, Height * 0.1f), ImVec2(0, 1), ImVec2(1, 0));
}

void Scene::cleanup()
{
	Test.cleanup();
	//CenterOBJ.cleanup();

	//for (Object& satellite : Lights)
	//{
	//	satellite.cleanup();
	//}

	//Orbit.cleanup();
	//Plane.cleanup();

	//for (Mesh* mesh : meshes)
	//{
	//	mesh->cleanup();
	//	delete mesh;
	//}
}

void Scene::g_buffer_setup()
{
	// configure g-buffer framebuffer
	// ------------------------------
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	
	// position color buffer
	glGenTextures(1, &g_data.gPosition);
	glBindTexture(GL_TEXTURE_2D, g_data.gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_data.gPosition, 0);
	// normal color buffer
	glGenTextures(1, &g_data.gNormal);
	glBindTexture(GL_TEXTURE_2D, g_data.gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_data.gNormal, 0);
	// diffuse color buffer
	glGenTextures(1, &g_data.gDiff);
	glBindTexture(GL_TEXTURE_2D, g_data.gDiff);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_data.gDiff, 0);
	// specular color buffer
	glGenTextures(1, &g_data.gSpec);
	glBindTexture(GL_TEXTURE_2D, g_data.gSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, g_data.gSpec, 0);

	glGenTextures(1, &g_data.gAmbient);
	glBindTexture(GL_TEXTURE_2D, g_data.gAmbient);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, g_data.gAmbient, 0);

	glGenTextures(1, &g_data.gEmissive);
	glBindTexture(GL_TEXTURE_2D, g_data.gEmissive);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, g_data.gEmissive, 0);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5
	};
	glDrawBuffers(6, attachments);

	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::mesh_setup()
{
	//std::string sphere4{ "../3Dmodels/4Sphere.obj" };
	//std::string bunny{ "../3Dmodels/bunny_high_poly.obj" };
	//std::string cube2{ "../3Dmodels/cube2.obj" };
	//std::string sphere{ "../3Dmodels/sphere.obj" };
	//std::string sphere_mod{ "../3Dmodels/sphere_modified.obj" };
	////std::string Cen_path{"../3Dmodels/cup.obj"};				\par?
	////std::string Cen_path{"../3Dmodels/lucy_princeton.obj"};	?
	////std::string Cen_path{"../3Dmodels/starwars1.obj"};		many obj in one file


	//meshes.push_back(new Mesh(bunny));
	//meshes.push_back(new Mesh(sphere4));
	//meshes.push_back(new Mesh(cube2));
	//meshes.push_back(new Mesh(sphere));
	//meshes.push_back(new Mesh(sphere_mod));

	int stacks{ 50 }, slices{ 50 };
	meshes.push_back(CreateOrbit(RAD, ORBIT_VERT));
	meshes.push_back(CreatePlane(stacks, slices));
	meshes.push_back(CreateQuad());
	//meshes.push_back(CreateSphere(stacks, slices));
}

void Scene::texture_setup()
{
	//std::string metal_roof_diff = "../textures/metal_roof_diff_512x512.ppm";
	//read_PPM(metal_roof_diff);

	//std::string metal_roof_spec = "../textures/metal_roof_spec_512x512.ppm";
	//read_PPM(metal_roof_spec);

	//std::string grid = "../textures/grid.ppm";
	//read_PPM(grid);

	Texture diff;
	diff.id = Test.TextureFromFile("../textures/metal_roof_diff_512x512.png");
	diff.path = "../textures/metal_roof_diff_512x512.png";
	diff.type = "texture_diffuse";
	textures.push_back(diff);

	Texture spec;
	spec.id = Test.TextureFromFile("../textures/metal_roof_spec_512x512.png");
	spec.path = "../textures/metal_roof_spec_512x512.png";
	spec.type = "texture_specular";
	textures.push_back(spec);
}

void Scene::read_PPM(std::string path)
{
	std::ifstream infile(path, std::ifstream::binary);
	// Examine if the file could be opened successfully
	if (!infile.is_open())
	{
		std::cout << "Failed to open " << path << std::endl;
	}

	std::string mMagic{ 0 };
	int W{ 0 }, H{ 0 }, mMax{ 0 };
	infile >> mMagic;

	infile.seekg(1, infile.cur);
	char c;
	infile.get(c);
	if (c == '#')
	{
		// We got comments in the PPM image and skip the comments
		while (c != '\n')
		{
			infile.get(c);
		}
	}
	else
	{
		infile.seekg(-1, infile.cur);
	}


	infile >> W >> H >> mMax;
	//if (mMax != 255)
	//{
	//	std::cout << "Failed to read " << path << std::endl;
	//	std::cout << "Got PPM maximum value: " << mMax << std::endl;
	//	std::cout << "Maximum pixel has to be 255" << std::endl;
	//}

	char* mBuffer = new char[W * H * 3];

	// ASCII
	if (mMagic == "P3")
	{
		for (int i = 0; i < W * H * 3; i++)
		{
			std::string pixel_str;
			infile >> pixel_str;
			mBuffer[i] = static_cast<char>(std::stoi(pixel_str));
		}
	}
	// Binary
	else if (mMagic == "P6")
	{
		// Move curser once to skip '\n'
		infile.seekg(1, infile.cur);
		infile.read(reinterpret_cast<char*>(mBuffer), W * H * 3);
	}
	else
	{
		std::cout << "Unrecognized PPM format" << std::endl;
		std::cout << "Got PPM magic number: " << mMagic << std::endl;
		std::cout << "PPM magic number should be either P3 or P6" << std::endl;
	}


	GLuint texobj_hdl;
	// define and initialize a handle to texture object that will
	// encapsulate two-dimensional textures
	//glCreateTextures(GL_TEXTURE_2D, 1, &texobj_hdl);
	//// allocate GPU storage for texture image data loaded from file
	//glTextureStorage2D(texobj_hdl, 1, GL_RGB, W, H);
	//// copy image data from client memory to GPU texture buffer memory
	//glTextureSubImage2D(texobj_hdl, 0, 0, 0, W, H,
	//	GL_RGB, GL_UNSIGNED_BYTE, mBuffer);

	//glGenTextures(1, &texobj_hdl);

	//glBindTexture(GL_TEXTURE_2D, texobj_hdl);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, mBuffer);
	//glGenerateMipmap(GL_TEXTURE_2D);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glGenTextures(1, &texobj_hdl);
	glBindTexture(GL_TEXTURE_2D, texobj_hdl);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, mBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);

	// client memory not required since image is buffered in GPU memory
	delete[] mBuffer;

	//textures.push_back(texobj_hdl);
}

void Scene::shdr_file_setup()
{
	shdr_vec model_shdr_files;
	model_shdr_files.push_back(std::make_pair(GL_VERTEX_SHADER, "../shaders/model_shader.vert"));
	model_shdr_files.push_back(std::make_pair(GL_FRAGMENT_SHADER, "../shaders/model_shader.frag"));
	//shdr_files.push_back({ model_shdr_files,static_cast<unsigned>(ShdrType::MODEL_PHONG) });
	shaders.push_back(GLSLShader(model_shdr_files));

	//shdr_vec blinn_shdr_files;
	//blinn_shdr_files.push_back(std::make_pair(GL_VERTEX_SHADER, "../shaders/model_blinn_shader.vert"));
	//blinn_shdr_files.push_back(std::make_pair(GL_FRAGMENT_SHADER, "../shaders/model_blinn_shader.frag"));
	//shdr_files.push_back({ blinn_shdr_files,static_cast<unsigned>(ShdrType::MODEL_BLINN) });

	//shdr_vec phong_light_shdr_files;
	//phong_light_shdr_files.push_back(std::make_pair(GL_VERTEX_SHADER, "../shaders/phong_light_shader.vert"));
	//phong_light_shdr_files.push_back(std::make_pair(GL_FRAGMENT_SHADER, "../shaders/phong_light_shader.frag"));
	//shdr_files.push_back({ phong_light_shdr_files,static_cast<unsigned>(ShdrType::MODEL_PHONG_L) });
	shdr_vec g_buff_shdr_files;
	g_buff_shdr_files.push_back(std::make_pair(GL_VERTEX_SHADER, "../shaders/g_buffer.vert"));
	g_buff_shdr_files.push_back(std::make_pair(GL_FRAGMENT_SHADER, "../shaders/g_buffer.frag"));
	shaders.push_back(GLSLShader(g_buff_shdr_files));

	shdr_vec model_deffered_shdr_files;
	model_deffered_shdr_files.push_back(std::make_pair(GL_VERTEX_SHADER, "../shaders/model_deffered_shader.vert"));
	model_deffered_shdr_files.push_back(std::make_pair(GL_FRAGMENT_SHADER, "../shaders/model_deffered_shader.frag"));
	shaders.push_back(GLSLShader(model_deffered_shdr_files));

	shdr_vec line_shdr_files;
	line_shdr_files.push_back(std::make_pair(GL_VERTEX_SHADER, "../shaders/line_shader.vert"));
	line_shdr_files.push_back(std::make_pair(GL_FRAGMENT_SHADER, "../shaders/line_shader.frag"));
	//shdr_files.push_back({ line_shdr_files,static_cast<unsigned>(ShdrType::LINE) });
	shaders.push_back(GLSLShader(line_shdr_files));

	shdr_vec light_shdr_files;
	light_shdr_files.push_back(std::make_pair(GL_VERTEX_SHADER, "../shaders/light_shader.vert"));
	light_shdr_files.push_back(std::make_pair(GL_FRAGMENT_SHADER, "../shaders/light_shader.frag"));
	//shdr_files.push_back({ light_shdr_files,static_cast<unsigned>(ShdrType::LIGHT) });
	shaders.push_back(GLSLShader(light_shdr_files));

	shdr_vec normal_shdr_files;
	normal_shdr_files.push_back(std::make_pair(GL_VERTEX_SHADER, "../shaders/normal_shader.vert"));
	normal_shdr_files.push_back(std::make_pair(GL_FRAGMENT_SHADER, "../shaders/normal_shader.frag"));
	//shdr_files.push_back({ light_shdr_files,static_cast<unsigned>(ShdrType::LIGHT) });
	shaders.push_back(GLSLShader(normal_shdr_files));
}

void Scene::light_setup(int scenario)
{
	switch (scenario)
	{
	case 0:	// init
		for (int i{ 0 }; i < MAX_LIGHT_NUM; i++)
		{
			//Lights[i].init(meshes[static_cast<int>(MeshType::PROCEDURAL_SPHERE)], shdr_files[static_cast<int>(ShdrType::LIGHT)]);
			//Lights[i].init("../3Dmodels/sphere.obj");
			Lights[i].set_scale({ 0.1f,0.1f ,0.1f });
			Lights[i].set_color(Vec4(pl[i].diffuse_color, 1.f));
		}
		pl[0].l_type = LightType::DIRECTIONAL;
		pl[1].l_type = LightType::DIRECTIONAL;
		break;

	case 1:	// Scenario 1: All lights have the same color parameters and type 
		g.active_lights = 5;

		for (int i{ 0 }; i < g.active_lights; i++)
		{
			pl[i].l_type = LightType::POINT;
			pl[i].ambient_color = Vec3(1.f);
			pl[i].diffuse_color = Vec3(1.f);
			pl[i].specular_color = Vec3(1.f);
			pl[i].inner_outer_angle = Vec2(15.f, 20.f);
			pl[i].falloff = 1.f;
			Lights[i].set_color(Vec4(pl[i].diffuse_color, 1.f));
		}
		break;

	case 2:	// Scenario 2: All lights have different color parameters and same type 
	{
		g.active_lights = 7;
		Vec3 colors[7]{
			Vec3(59,234,21),Vec3(22,89,248),Vec3(244,14,14),Vec3(203,29,226),
			Vec3(29,226,180),Vec3(226,215,29),Vec3(226,104,29)
		};
		for (int i{ 0 }; i < g.active_lights; i++)
		{
			pl[i].l_type = LightType::SPOTLIGHT;
			pl[i].ambient_color = colors[i] / 255.f;
			pl[i].diffuse_color = colors[i] / 255.f;
			pl[i].specular_color = colors[i] / 255.f;
			pl[i].inner_outer_angle = Vec2(15.f, 20.f);
			pl[i].falloff = 1.f;
			Lights[i].set_color(Vec4(pl[i].diffuse_color, 1.f));
		}
		break;
	}

	case 3:	// Scenario 3: Implement a third scenario that mixes the lights and their types.
	{
		g.active_lights = 11;
		Vec3 colors[11]{
			Vec3(59,234,21),Vec3(22,89,248),Vec3(226,104,29),Vec3(203,29,226),
			Vec3(226,215,29),Vec3(29,226,180),Vec3(203,29,226),Vec3(203,29,226),
			Vec3(29,226,180),Vec3(226,29,220),Vec3(244,14,14)
		};
		for (int i{ 0 }; i < g.active_lights; i++)
		{
			switch (i %2)
			{
			case 0:pl[i].l_type = LightType::POINT; break;
			//case 1:pl[i].l_type = LightType::DIRECTIONAL; break;
			case 1:pl[i].l_type = LightType::SPOTLIGHT; break;
			default:pl[i].l_type = LightType::POINT; break;
			}
			pl[i].ambient_color = colors[i] / 255.f;
			pl[i].diffuse_color = colors[i] / 255.f;
			pl[i].specular_color = colors[i] / 255.f;
			pl[i].inner_outer_angle = Vec2(15.f, 20.f);
			pl[i].falloff = 1.f;
			Lights[i].set_color(Vec4(pl[i].diffuse_color, 1.f));
		}
		break;
	}

	default:break;
	}
}

void Scene::update_IMGUI(double dt)
{
	{
		ImGui::Begin("Scene");
		if (ImGui::CollapsingHeader("Model"))
		{
			//if (ImGui::Combo("Mesh", &selected_mesh, mesh_items, IM_ARRAYSIZE(mesh_items)))
			//{
			//	CenterOBJ.cleanup();
			//	switch (selected_mesh)
			//	{
			//	case 0:CenterOBJ.init("../3Dmodels/bunny_high_poly.obj", textures); break;
			//	case 1:CenterOBJ.init("../3Dmodels/4Sphere.obj", textures); break;
			//	case 2:CenterOBJ.init("../3Dmodels/cube2.obj", textures); break;
			//	case 3:CenterOBJ.init("../3Dmodels/sphere.obj", textures); break;
			//	case 4:CenterOBJ.init("../3Dmodels/sphere_modified.obj", textures); break;

			//	default: CenterOBJ.init("../3Dmodels/bunny_high_poly.obj", textures); break;
			//	}
			//}
			// 
			ImGui::Checkbox("Show BV", &show_bv);
			if (show_bv)
			{
				ImGui::Combo("Bounding Volume", &selected_bv, bv_items, IM_ARRAYSIZE(bv_items));
				ImGui::Combo("Hierarchy", &selected_hierarchy, Hierarchy_items, IM_ARRAYSIZE(Hierarchy_items));
			}
			ImGui::Checkbox("Show Octree", &show_oct);
			if (show_oct)
			{
				ImGui::Checkbox("Show Octree AABB", &show_oct_aabb);
			}
			ImGui::Checkbox("Show BSP", &show_bsp);
			ImGui::Checkbox("Show face normal", &show_fnormal);
			ImGui::Checkbox("Show vertex normal", &show_vnormal);
		}

		if (ImGui::CollapsingHeader("Material"))
		{
			ImGui::Text("Surface Color Tints");
			ImGui::ColorEdit3("Ambient", &m.ambient.x);
			ImGui::ColorEdit3("Diffuse", &m.diffuse.x);
			ImGui::ColorEdit3("Specular", &m.specular.x);
			ImGui::ColorEdit3("Emissive", &m.emissive.x);
			//ImGui::Checkbox("Visualize UV", &is_paused);
			//if (ImGui::Combo("Tex projection mode", &m.selected_projection, t_projection_items, IM_ARRAYSIZE(t_projection_items)))
			//{
			//	switch (m.selected_projection)
			//	{
			//	case 0:  break;	//Cylinder
			//	case 1:  break;	//Sphere
			//	case 2:  break;	//Cube
			//	case 3:  break;	//Planer
			//	case 4:  break;	//None

			//	default:  break;
			//	}

			//	CenterOBJ.change_projection(m.selected_projection, m.selected_entity);
			//}
			//if (ImGui::Combo("Tex projection pipeline", &m.selected_pipeline, t_pipeline_items, IM_ARRAYSIZE(t_pipeline_items)))
			//{
			//	switch (m.selected_pipeline)
			//	{
			//	case 0:  break;	//CPU
			//	case 1:  break;	//GPU

			//	default:  break;
			//	}
			//}
			//if (ImGui::Combo("Tex Entity", &m.selected_entity, t_entity_items, IM_ARRAYSIZE(t_entity_items)))
			//{
			//	switch (m.selected_entity)
			//	{
			//	case 0:  break;	//Position
			//	case 1:  break;	//Normal

			//	default:  break;
			//	}
			//}
			//if (ImGui::Button("Recalculate UV"))
			//{
			//}
		}
		//if (ImGui::CollapsingHeader("Global Constant"))
		//{
		//	ImGui::SliderFloat3("Attenuation Constant", &g.att_const.x, 0.f, 2.f);
		//	//ImGui::ColorEdit3("Global Ambient", &g.g_ambient.x);
		//	ImGui::ColorEdit3("Fog Color", &g.fog_color.x);
		//	ImGui::SliderFloat("Fog Density", &g.fog_min_max.x, 0.01f, 0.2f);
		//	ImGui::SliderFloat("Fog Gradient", &g.fog_min_max.y, 0.01f, 20.f);
		//}
		ImGui::Checkbox("Show Debug", &do_forward);
		ImGui::End();

		ImGui::Begin("Light");
		ImGui::SliderInt("Active Lights", &g.active_lights, 1, 16);
		//pause rotation
		ImGui::Text("Light Orbit");
		ImGui::Checkbox("Pause Rotation", &is_paused);
		if (is_paused == false)
			elapsed_time += dt;

		//senarios 1,2,3 button
		ImGui::Text("Lighting Scenarios");
		if (ImGui::Button("Senario 1"))
		{
			light_setup(1);
		}
		ImGui::SameLine();
		if (ImGui::Button("Senario 2"))
		{
			light_setup(2);
		}
		ImGui::SameLine();
		if (ImGui::Button("Senario 3"))
		{
			light_setup(3);
		}

		ImGui::Combo("Select Light", &selected_light, light_items, IM_ARRAYSIZE(light_items));
		{
			//change light type pl[selected_light].l_type
			Per_Light& l = pl[selected_light];

			if (selected_lType != static_cast<int>(l.l_type))
				selected_lType = static_cast<int>(l.l_type);

			ImGui::ColorEdit3("Ambient", &l.ambient_color.x);
			ImGui::ColorEdit3("Diffuse", &l.diffuse_color.x);
			Lights[selected_light].set_color(Vec4(l.diffuse_color, 1.f));
			ImGui::ColorEdit3("Specular", &l.specular_color.x);

			if (ImGui::Combo("Light Type", &selected_lType, lType_items, IM_ARRAYSIZE(lType_items)))
			{
				switch (selected_lType)
				{
				case 0:l.l_type = LightType::POINT; break;
				case 1:l.l_type = LightType::DIRECTIONAL; break;
				case 2:l.l_type = LightType::SPOTLIGHT; break;

				default:l.l_type = LightType::POINT; break;
				}
			}

			if (l.l_type == LightType::SPOTLIGHT)
			{
				if (ImGui::SliderFloat("Inner Angle", &l.inner_outer_angle.x, 0.f, 90.f))
				{
					if (l.inner_outer_angle.x > l.inner_outer_angle.y)
						l.inner_outer_angle.y = l.inner_outer_angle.x;
				}

				if (ImGui::SliderFloat("Outer Angle", &l.inner_outer_angle.y, 0.f, 90.f))
				{
					if (l.inner_outer_angle.x > l.inner_outer_angle.y)
						l.inner_outer_angle.x = l.inner_outer_angle.y;
				}
				ImGui::SliderFloat("Falloff", &l.falloff, 0.f, 10.f);
			}
		}

		ImGui::End();
	}
}

void Scene::read_section()
{
	std::string section = "../3Dmodels/Section";
	sections.resize(6);

	//pushback sections[0]~[5]
	for (int i{ 0 }; i < 6; i++)
	{
		std::ifstream inFile(section + std::to_string(i+1) + ".txt");
		if (inFile.is_open() == false) 
		{
			//throw std::runtime_error("Failed to load Section" + std::to_string(i));
			continue;
		}

		std::string label;
		while (inFile.eof() == false)
		{
			getline(inFile, label);
			if (label.empty() || std::string(label.end() - 4, label.end()) != ".obj")
			{
				//throw std::runtime_error("Bad Filetype.  " + label + " not a .obj file");
				continue;
			}

			sections[i].push_back(label);
		}
	}
}
