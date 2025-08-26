/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: object.cpp
Purpose: 
    This file contains definitions of member functions of class Object.
Language: C++ 17
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_2
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/
#include <object.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

glm::vec4 generateRandomColor();

Object::Object(Vec3 pos, Vec3 s, Vec3 rot, Vec4 col) :position(pos), scale(s), rotation(rot), selfColor(col), temp(nullptr)//, mesh(nullptr)
{
}

void Object::init(const std::string& path, std::vector<Texture> tex)
{
    textures = tex;
    load_object(path);
}



void Object::init(Mesh* m, GLSLShader& shader, std::vector<Texture> tex)
{
    meshes.push_back(*m);
    textures = tex;

    //meshes[0].setup_mesh(shader);
}

void Object::init(Mesh* m)
{
    temp = m;
}

void Object::update(GLSLShader& shader, Mat4& view, Mat4& projection, Vec3& eye, Global& g, Per_Light* pl, Material& m)
{
    //projection_type = m.selected_projection;
    //mesh->calc_vert_uv(projection_type,m.selected_entity);
    //shdr_pgm.Use();
    //if (!textureHandle.empty())
        //mesh->UpdateVertexData();
    shader.Use();
    if (is_updated == true)
    {
        model = glm::translate(glm::mat4(1.f), position);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
        is_updated = false;
    }

    GLuint shdr_handle = shader.GetHandle();
    glUniformMatrix4fv(glGetUniformLocation(shdr_handle, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shdr_handle, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shdr_handle, "model"), 1, GL_FALSE, glm::value_ptr(model));


    //glUniform3fv(glGetUniformLocation(shdr_handle, "light_pos"), 1, glm::value_ptr(lightpos));
    glUniform4fv(glGetUniformLocation(shdr_handle, "color"), 1, glm::value_ptr(selfColor));

    //glUniform2fv(glGetUniformLocation(shdr_handle, "fog_min_max"), 1, glm::value_ptr(g.fog_min_max));
    //glUniform3fv(glGetUniformLocation(shdr_handle, "fogColor"), 1, glm::value_ptr(g.fog_color));

    glUniform3fv(glGetUniformLocation(shdr_handle, "att_const"), 1, glm::value_ptr(g.att_const));
    
    glUniform1iv(glGetUniformLocation(shdr_handle, "active_lights"), 1, &g.active_lights);
    glUniform3fv(glGetUniformLocation(shdr_handle, "g_ambient"), 1, glm::value_ptr(g.g_ambient));

    glUniform3fv(glGetUniformLocation(shdr_handle, "viewPos"), 1, glm::value_ptr(eye));

    
        
    //update_UBO();
    //if (!textureHandle.empty())
    //{
    //    glUniform1iv(glGetUniformLocation(shdr_handle, "pipeline"), 1, &(m.selected_pipeline));
    //    glUniform1iv(glGetUniformLocation(shdr_handle, "project_type"), 1, &(m.selected_projection));
    //    glUniform1iv(glGetUniformLocation(shdr_handle, "entity"), 1, &(m.selected_entity));

    //    //uniform int pipeline;

    //    glUniform3fv(glGetUniformLocation(shdr_handle, "m_ambient"), 1, glm::value_ptr(m.ambient));
    //    glUniform3fv(glGetUniformLocation(shdr_handle, "m_diffuse"), 1, glm::value_ptr(m.diffuse));
    //    glUniform3fv(glGetUniformLocation(shdr_handle, "m_specular"), 1, glm::value_ptr(m.specular));
    //    glUniform3fv(glGetUniformLocation(shdr_handle, "m_emissive"), 1, glm::value_ptr(m.emissive));

    //    //glBindTextureUnit(textureHandle[0], textureHandle[0]);
    //    //glTextureParameteri(textureHandle[0], GL_TEXTURE_WRAP_S, GL_REPEAT);
    //    //glTextureParameteri(textureHandle[0], GL_TEXTURE_WRAP_T, GL_REPEAT);
    //    //GLuint tex_loc = glGetUniformLocation(shdr_handle, "tex1");
    //    //glUniform1i(tex_loc, textureHandle[0]);

    //    //glBindTextureUnit(textureHandle[1], textureHandle[1]);
    //    //glTextureParameteri(textureHandle[1], GL_TEXTURE_WRAP_S, GL_REPEAT);
    //    //glTextureParameteri(textureHandle[1], GL_TEXTURE_WRAP_T, GL_REPEAT);
    //    //tex_loc = glGetUniformLocation(shdr_handle, "tex2");
    //    //glUniform1i(tex_loc, textureHandle[1]);

    //    //UV ¸¸µé±â
    //    //if (projection_type != 4)
    //    //{
    //    //    glUniform1i(glGetUniformLocation(shdr_handle, "tex1"), 0);
    //    //    glUniform1i(glGetUniformLocation(shdr_handle, "tex2"), 1);
    //    //    glUniform1i(glGetUniformLocation(shdr_handle, "tex3"), 2);
    //    //}


    //}

    //// First get the block index
    //GLuint uboIndex;
    //uboIndex = glGetUniformBlockIndex(shdr_handle, "Light");
    //// Now get the size
    //GLint uboSize;
    //glGetActiveUniformBlockiv(shdr_handle, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);

    //for (Mesh& mesh : meshes)
    //{
    //    //glBindBuffer(GL_UNIFORM_BUFFER, mesh.UBO[0]);
    //    //glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    //    //glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

    //    if (shdr_type < 3)
    //    {
    //        //glBindBuffer(GL_UNIFORM_BUFFER, mesh.UBO[1]);
    //        int offset{ 0 };

    //        for (int i{ 0 }; i < g.active_lights; i++)
    //        {
    //            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(LightType), &pl[i].l_type);
    //            offset += 16;
    //            glBufferSubData(GL_UNIFORM_BUFFER, offset, 1 * 16, glm::value_ptr(pl[i].ambient_color));
    //            offset += 16;
    //            glBufferSubData(GL_UNIFORM_BUFFER, offset, 1 * 16, glm::value_ptr(pl[i].diffuse_color));
    //            offset += 16;
    //            glBufferSubData(GL_UNIFORM_BUFFER, offset, 1 * 16, glm::value_ptr(pl[i].specular_color));
    //            offset += 16;
    //            glBufferSubData(GL_UNIFORM_BUFFER, offset, 1 * 16, glm::value_ptr(pl[i].pos));
    //            offset += 16;
    //            glBufferSubData(GL_UNIFORM_BUFFER, offset, 1 * 16, glm::value_ptr(pl[i].dir));
    //            offset += 16;
    //            glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, glm::value_ptr(pl[i].inner_outer_angle));
    //            offset += 12;
    //            glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &(pl[i].falloff));
    //            offset += 4;
    //        }
    //        //glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Per_Light) * 16, pl);
    //    }
    //}
    //glBindBuffer(GL_UNIFORM_BUFFER, 0);

    shader.UnUse();
}


void Object::draw(bool show_fnormal, bool show_vnormal)
{
    //if (!textureHandle.empty())
    //{
    //    glActiveTexture(GL_TEXTURE0);
    //    glBindTexture(GL_TEXTURE_2D, textureHandle[0]);

    //    glActiveTexture(GL_TEXTURE1);
    //    glBindTexture(GL_TEXTURE_2D, textureHandle[1]);

    //    glActiveTexture(GL_TEXTURE2);
    //    glBindTexture(GL_TEXTURE_2D, textureHandle[2]);
    //}
    //else
    //{
    //    glActiveTexture(GL_TEXTURE0);
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //    glActiveTexture(GL_TEXTURE1);
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //    glActiveTexture(GL_TEXTURE2);
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //}

    //shdr_pgm.Use();
    //glBindVertexArray(mesh->VAO);


    //glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO[0]);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
    //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->indexBuffer.size()), GL_UNSIGNED_INT, nullptr);

    //if (show_fnormal == true)
    //{
    //    // enable face normal
    //    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO[1]);
    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)(0));
    //    glUniform4fv(glGetUniformLocation(shdr_handle, "color"), 1, glm::value_ptr(glm::vec4(0.2f, 0.5f, 0.2f, -1.f)));   // green for face normal
    //    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(mesh->fnBuffer.size()));
    //}

    //if (show_vnormal == true)
    //{
    //    // enable vertex normal
    //    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO[2]);
    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)(0));
    //    glUniform4fv(glGetUniformLocation(shdr_handle, "color"), 1, glm::value_ptr(glm::vec4(0.4f, 0.3f, 0.6f, -1.f)));  // blue for vertex normal
    //    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(mesh->vnBuffer.size()));
    //}

    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    //glBindVertexArray(0);

    ////glActiveTexture(GL_TEXTURE0);
    ////glBindTexture(GL_TEXTURE_2D, textureHandle);
    ////GLint texSamplerLoc = glGetUniformLocation(shdr_handle,"tex1");
    ////glUniform1i(texSamplerLoc, 0);


    //shdr_pgm.UnUse();
}

void Object::draw_orbit(GLSLShader& shader)
{
    //meshes[0].setup_mesh(shader);
    shader.Use();
    //glDisable(GL_DEPTH_TEST);
    glBindVertexArray(meshes[0].VAO);
    
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(meshes[0].lineposBuffer.size()));

    glBindVertexArray(0);
    //glEnable(GL_DEPTH_TEST);
    shader.UnUse();
}

void Object::change_mesh(Mesh* m)
{
    /*mesh = m;

    mesh->setup_mesh(shdr_pgm);
    mesh->setup_UBO(shdr_type);*/
}

void Object::change_shdr(std::pair<shdr_vec, unsigned> shdr)
{
    //GLSLShader temp;

    //shdr_type = shdr.second;

    //temp.CompileLinkValidate(shdr.first);
    //if (GL_FALSE == temp.IsLinked()) {
    //    std::cout << "Unable to compile/link/validate shader programs" << "\n";
    //    std::cout << temp.GetLog() << std::endl;
    //    std::exit(EXIT_FAILURE);
    //}
    //shdr_pgm = temp;
    //shdr_handle = shdr_pgm.GetHandle();

    //mesh->setup_mesh(shdr_pgm);
    //mesh->setup_UBO(shdr_type);
}

void Object::change_projection(int projection, int entity)
{
    projection_type = projection;

    //if(projection_type!=4)
    //    mesh->calc_vert_uv(projection_type, entity);
}

void Object::cleanup()
{
    for (Mesh mesh : meshes)
    {
        mesh.cleanup();
    }
    meshes.clear();
    //shdr_pgm.DeleteShaderProgram();
}

void Object::load_object(const std::string& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        //std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Object::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}

unsigned int Object::TextureFromFile(const char* path, bool gamma)
{
    //Texture tex;
    stbi_set_flip_vertically_on_load(true);

    std::string filename = std::string(path);
    //filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        //GLenum format;
        //if (nrComponents == 1)
        //    format = GL_RED;
        //else if (nrComponents == 3)
        //    format = GL_RGB;
        //else if (nrComponents == 4)
        //    format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        //glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void Object::Draw(GLSLShader& shader)
{
    for (Mesh& mesh : meshes)
    {
        mesh.setup_mesh(shader);
        mesh.Draw(shader);
    }
}

void Object::Draw_NRM(GLSLShader& shader, bool show_vn, bool show_fn, int selected_bv)
{
    for (Mesh& mesh : meshes)
    {
        mesh.setup_mesh(shader);
        mesh.Draw_NRM(shader, show_vn, show_fn, selected_bv);
    }
}

void Object::Draw_BVH(GLSLShader& shader, int selected_bv)
{
    shader.Use();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    switch (selected_bv)
    {
    case 0: inorderTraversal(shader, BottomUp_AABB_root); break;
    case 1: inorderTraversal(shader, BottomUp_Ritter_root); break;
    case 2: inorderTraversal(shader, BottomUp_Larsson_root); break;
    case 3: inorderTraversal(shader, BottomUp_PCA_root); break;
    default:inorderTraversal(shader, BottomUp_AABB_root); break;
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    shader.UnUse();
}

void Object::Draw_OCT(GLSLShader& shader, bool show_oct_aabb)
{
    shader.Use();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if(show_oct_aabb)
        inorderTraversal(shader, Octree_root);
    else
        inorderTraversal_NoAABB(shader, Octree_root);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    shader.UnUse();
}

void Object::Draw_BSP(GLSLShader& shader)
{
    shader.Use();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    inorderTraversal(shader, BSP_root);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    shader.UnUse();
}

void Object::inorderTraversal(GLSLShader& shader, TreeNode* root)
{
    if (root != nullptr) 
    {
        inorderTraversal(shader, root->lChild);

        glBindVertexArray(root->TREE_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, root->TREE_VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)(0));

		switch (root->level)
		{
		case 1:
            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(1.f, 0.f, 0.f, -1.f))); 
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(reinterpret_cast<AABB*>(root->bv)->lines.size()));
            break;
		case 2:
            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(1.f, 0.5f, 0.0f, -1.f))); 
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(reinterpret_cast<AABB*>(root->bv)->lines.size()));
            break;
		case 3:
            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(1.f, 1.f, 0.f, -1.f)));
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(reinterpret_cast<AABB*>(root->bv)->lines.size()));
            break;
		case 4:
            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(0.f, 1.f, 0.f, -1.f))); 
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(reinterpret_cast<AABB*>(root->bv)->lines.size()));
            break;
		default:
            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(0.5f, 0.5f, 0.5f, -1.f)));
            break;
		}
        
        inorderTraversal(shader, root->rChild);
    }
}

void Object::inorderTraversal(GLSLShader& shader, S_TreeNode* root)
{
    if (root != nullptr)
    {
        inorderTraversal(shader, root->lChild);

        glBindVertexArray(root->TREE_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, root->TREE_VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, root->TREE_EBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
        
        switch (root->level)
        {
        case 1:
            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(1.f, 0.f, 0.f, -1.f)));
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(root->bv->ibuffer.size()), GL_UNSIGNED_INT, 0);
            break;
        case 2:
            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(1.f, 0.5f, 0.0f, -1.f)));
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(root->bv->ibuffer.size()), GL_UNSIGNED_INT, 0);
            break;
        case 3:
            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(1.f, 1.f, 0.f, -1.f)));
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(root->bv->ibuffer.size()), GL_UNSIGNED_INT, 0);
            break;
        case 4:
            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(0.f, 1.f, 0.f, -1.f)));
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(root->bv->ibuffer.size()), GL_UNSIGNED_INT, 0);
            break;
        default:
            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(0.5f, 0.5f, 0.5f, -1.f)));
            break;
        }
        //glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(root->bv->ibuffer.size()), GL_UNSIGNED_INT, 0);

        inorderTraversal(shader, root->rChild);
    }
}

void Object::inorderTraversal(GLSLShader& shader, OctreeNode* root)
{
    if (root != nullptr)
    {
        if (root->triangles.size() != 0)
        {
            glBindVertexArray(root->TREE_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, root->TREE_VBO);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)(0));

            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(0.f, 1.f, 0.f, -1.f)));
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(reinterpret_cast<AABB*>(root->aabb)->lines.size()));
            glBindVertexArray(0);
        }


        if (root->triangles.size() > 500)
        {
            for (OctreeNode* c : root->pChildren)
            {
                inorderTraversal(shader, c);
            }
        }
        else
        {
            glBindVertexArray(Oct_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, Oct_VBO);

            std::vector<glm::vec3> temp;
            for (Triangle& tri : root->triangles)
            {
                temp.push_back(tri.vertices[0]);
                temp.push_back(tri.vertices[1]);
                temp.push_back(tri.vertices[2]);
            }

            glBufferData(GL_ARRAY_BUFFER, temp.size() * sizeof(glm::vec3), temp.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);


            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(colors[root->level]));
            glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(temp.size()));
            glBindVertexArray(0);
        }
    }
}

void Object::inorderTraversal(GLSLShader& shader, BSPNode* root)
{
    if (root != nullptr)
    {
        if (root->triangles.size() > 3000)
        {
            inorderTraversal(shader, root->FrontNode);
            inorderTraversal(shader, root->BackNode);
        }
        else
        {
            glBindVertexArray(BSP_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, BSP_VBO);

            std::vector<glm::vec3> temp;
            for (Triangle& tri : root->triangles)
            {
                temp.push_back(tri.vertices[0]);
                temp.push_back(tri.vertices[1]);
                temp.push_back(tri.vertices[2]);
            }

            glBufferData(GL_ARRAY_BUFFER, temp.size() * sizeof(glm::vec3), temp.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);


            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(root->color));
            glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(temp.size()));
            glBindVertexArray(0);
        }
    }
}

void Object::inorderTraversal_NoAABB(GLSLShader& shader, OctreeNode* root)
{
    if (root != nullptr)
    {
        if (root->triangles.size() > 500)
        {
            for (OctreeNode* c : root->pChildren)
            {
                inorderTraversal_NoAABB(shader, c);
            }
        }
        else
        {
            glBindVertexArray(Oct_VAO);
            glBindBuffer(GL_ARRAY_BUFFER, Oct_VBO);

            std::vector<glm::vec3> temp;
            for (Triangle& tri : root->triangles)
            {
                temp.push_back(tri.vertices[0]);
                temp.push_back(tri.vertices[1]);
                temp.push_back(tri.vertices[2]);
            }

            glBufferData(GL_ARRAY_BUFFER, temp.size() * sizeof(glm::vec3), temp.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);


            glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(colors[root->level]));
            glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(temp.size()));
            glBindVertexArray(0);
        }
    }
}

void Object::Add_mesh(Mesh* m)
{
    meshes.push_back(*m);
}

void Object::BVH_Setup()
{
    for (int i{ 0 }; i < 100; i++)
    {
        colors.push_back(generateRandomColor());
    }

    std::vector<TreeNode*> aabbs;
    std::vector<S_TreeNode*> rittrs;
    std::vector<S_TreeNode*> larssons;
    std::vector<S_TreeNode*> PCAs;

    for (auto m : meshes)
    {
        aabbs.push_back(new TreeNode(m.bounding_box));
        rittrs.push_back(new S_TreeNode(m.Ritter));
        larssons.push_back(new S_TreeNode(m.Larsson));
        PCAs.push_back(new S_TreeNode(m.PCA));
    }


    BottomUp_AABB_root = BottomUpConstructionAABB(aabbs);
    BottomUp_Ritter_root = BottomUpConstructionSPHERE(rittrs);
    BottomUp_Larsson_root = BottomUpConstructionSPHERE(larssons);
    BottomUp_PCA_root = BottomUpConstructionSPHERE(PCAs);
}

void Object::Octree_Setup()
{
    glCreateVertexArrays(1, &Oct_VAO);
    glBindVertexArray(Oct_VAO);

    glGenBuffers(1, &Oct_VBO);
    glBindVertexArray(0);

    for (auto m : meshes)
    {
        int size = m.vertexBuffer.size();
        for (int i{ 0 }; i < size; i+=3)
        {
            triangles.push_back(Triangle(m.vertexBuffer[i].pos, m.vertexBuffer[i + 1].pos, m.vertexBuffer[i + 2].pos));
        }
    }

    Octree_root = new OctreeNode(BottomUp_AABB_root->bv, 1, triangles);
    Build_Octree(Octree_root);
}

void Object::BSP_Setup()
{
    glCreateVertexArrays(1, &BSP_VAO);
    glBindVertexArray(BSP_VAO);

    glGenBuffers(1, &BSP_VBO);
    glBindVertexArray(0);

    BSP_root = new BSPNode(1, triangles);
    Build_BSP(BSP_root);
}

void Object::FindNodesToMerge(std::vector<TreeNode*>& nodes, TreeNode* &first, TreeNode* &second)
{
    float min = FLT_MAX;
    std::pair<int, int> index(0, 0);
    for (int i{ 0 }; i < nodes.size(); i++)
    {
        for (int j{ i + 1 }; j < nodes.size(); j++)
        {
            float dist = glm::distance(nodes[i]->bv->center, nodes[j]->bv->center);
            if (dist < min)
            {
                min = dist;
                index.first = i;
                index.second = j;
            }
        }
    }

    first = nodes[index.first];
    second = nodes[index.second];
}

void Object::FindNodesToMerge(std::vector<S_TreeNode*>& nodes, S_TreeNode*& first, S_TreeNode*& second)
{
    float min = FLT_MAX;
    std::pair<int, int> index(0, 0);
    for (int i{ 0 }; i < nodes.size(); i++)
    {
        for (int j{ i + 1 }; j < nodes.size(); j++)
        {
            float dist = glm::distance(nodes[i]->bv->center, nodes[j]->bv->center);
            if (dist < min)
            {
                min = dist;
                index.first = i;
                index.second = j;
            }
        }
    }

    first = nodes[index.first];
    second = nodes[index.second];
}

TreeNode* Object::BottomUpConstructionAABB(std::vector<TreeNode*>& nodes)
{
    while (nodes.size() > 1)
    {
        TreeNode* first{ nullptr }, * second{ nullptr };
        FindNodesToMerge(nodes, first, second);
        TreeNode* parent = new TreeNode(first, second);
        parent->bv = ExtendAABB(first,second);
        parent->Gen_VAO();
        
        parent->level = std::max(first->level, second->level) + 1;

        // Remove first & second from the vector
        auto it = std::find(nodes.begin(), nodes.end(), first);
        if (it != nodes.end())
            nodes.erase(it);
		it = std::find(nodes.begin(), nodes.end(), second);
		if (it != nodes.end())
			nodes.erase(it);

        // Add the parent node to the vector
        nodes.push_back(parent);
    }
    return nodes[0]; // return the root node
}

S_TreeNode* Object::BottomUpConstructionSPHERE(std::vector<S_TreeNode*>& nodes)
{
    while (nodes.size() > 1)
    {
        S_TreeNode* first{ nullptr }, * second{ nullptr };
        FindNodesToMerge(nodes, first, second);
        S_TreeNode* parent = new S_TreeNode(first, second);
        parent->bv = ExtendSPHERE(first, second);
        parent->Gen_VAO();

        parent->level = std::max(first->level, second->level) + 1;

        // Remove first & second from the vector
        auto it = std::find(nodes.begin(), nodes.end(), first);
        if (it != nodes.end())
            nodes.erase(it);
        it = std::find(nodes.begin(), nodes.end(), second);
        if (it != nodes.end())
            nodes.erase(it);

        // Add the parent node to the vector
        nodes.push_back(parent);
    }
    return nodes[0]; // return the root node
}

AABB* Object::ExtendAABB(TreeNode* first, TreeNode* second)
{
    AABB* aabb = new AABB((first->bv->center + second->bv->center) * 0.5f);

    for (int i{ 0 }; i < 3; i++)
    {
        if (first->bv->min[i] > second->bv->min[i])
            aabb->min[i] = second->bv->min[i];
        else
            aabb->min[i] = first->bv->min[i];

        if (first->bv->max[i] > second->bv->max[i])
            aabb->max[i] = first->bv->max[i];
        else
            aabb->max[i] = second->bv->max[i];
    }

    aabb->CreateAABB();

    return aabb;
}

SPHERE* Object::ExtendSPHERE(S_TreeNode* first, S_TreeNode* second)
{
    SPHERE* sphere = new SPHERE((first->bv->center + second->bv->center) * 0.5f);
    sphere->radius = glm::distance(sphere->center, first->bv->center) + std::max(first->bv->radius, second->bv->radius);

    sphere->CreateSphere(50, 50);

    return sphere;
}

void Object::Build_Octree(OctreeNode* node)
{
    if (node->triangles.size() <= 500)
    {
        return;
    }
    //AABB* aabb = new AABB((first->bv->center + second->bv->center) * 0.5f);
    //divide this->AABB in to 8 pieces and assign to Children
    //node->pChildren[0] = new OctreeNode(AABB, this->level + 1);
    glm::vec3 min = node->aabb->min;
    glm::vec3 max = node->aabb->max;
    glm::vec3 center = node->aabb->center;
    //glm::vec3 halfsize = (max - min) * 0.5f;

    AABB* aabb0 = new AABB(min, center);
    aabb0->CreateAABB();
    node->pChildren[0] = new OctreeNode(aabb0,node->level+1);

    AABB* aabb1 = new AABB(glm::vec3(center.x, min.y, min.z), glm::vec3(max.x, center.y, center.z));
    aabb1->CreateAABB();
    node->pChildren[1] = new OctreeNode(aabb1, node->level + 1);

    AABB* aabb2 = new AABB(glm::vec3(center.x, min.y, center.z), glm::vec3(max.x, center.y, max.z));
    aabb2->CreateAABB();
    node->pChildren[2] = new OctreeNode(aabb2, node->level + 1);

    AABB* aabb3 = new AABB(glm::vec3(min.x, min.y, center.z), glm::vec3(center.x, center.y, max.z));
    aabb3->CreateAABB();
    node->pChildren[3] = new OctreeNode(aabb3, node->level + 1);

    AABB* aabb4 = new AABB(glm::vec3(min.x, center.y, min.z), glm::vec3(center.x, max.y, center.z));
    aabb4->CreateAABB();
    node->pChildren[4] = new OctreeNode(aabb4, node->level + 1);

    AABB* aabb5 = new AABB(glm::vec3(center.x, center.y, min.z), glm::vec3(max.x, max.y, center.z));
    aabb5->CreateAABB();
    node->pChildren[5] = new OctreeNode(aabb5, node->level + 1);

    AABB* aabb6 = new AABB(center, max);
    aabb6->CreateAABB();
    node->pChildren[6] = new OctreeNode(aabb6, node->level + 1);

    AABB* aabb7 = new AABB(glm::vec3(min.x, center.y, center.z), glm::vec3(center.x, max.y, max.z));
    aabb7->CreateAABB();
    node->pChildren[7] = new OctreeNode(aabb7, node->level + 1);

    for (const auto& tri : node->triangles) 
    {
        DivideOrAdd(node, tri);
    }
    //int size{ 0 };
    //for (int i = 0; i < 8; i++)
    //{
    //    size += node->pChildren[i]->triangles.size();
    //}

    for (int i = 0; i < 8; i++) 
    {
        Build_Octree(node->pChildren[i]);
    }
}

#include <stack>
void Object::DivideOrAdd(OctreeNode* node, Triangle tri)
{

    for (int i = 0; i < 8; ++i)
    {
        if (Tri_check(tri, node->pChildren[i]->aabb))
        {
            node->pChildren[i]->triangles.push_back(tri);
            return;
        }
    }

    std::stack<Triangle> temp_triangles;
    temp_triangles.push(tri);

    for (int i = 0; i < 3; i++) // x,y,z
    {
        std::vector<Triangle> temp;
        while (!temp_triangles.empty())
        {
            auto triangle = temp_triangles.top();

            auto newTriangleInfo = IntersectRayPlane(node, i, triangle);
            bool isGenerate = newTriangleInfo.first;

            if (isGenerate)
            {
                auto newTriangles = newTriangleInfo.second;
                temp.insert(temp.end(), newTriangles.begin(), newTriangles.end());
            }
            else
            {
                temp.push_back(triangle);
            }

            temp_triangles.pop();
        }

        for (auto newTriangle : temp)
            temp_triangles.push(newTriangle);
    }


    while (!temp_triangles.empty())
    {
        auto newTriangle = temp_triangles.top();
        for (int i = 0; i < 8; i++)
        {
            if (Tri_check(newTriangle, node->pChildren[i]->aabb))
            {
                node->pChildren[i]->triangles.push_back(newTriangle);
            }
        }
        temp_triangles.pop();
    }
}

void Object::DivideOrAdd(BSPNode* node, Triangle tri)
{
    //std::stack<Triangle> temp_triangles;
    //temp_triangles.push(tri);

    //std::vector<Triangle> front_temp;
    //std::vector<Triangle> back_temp;

    auto newTriangleInfo = IntersectRayPlane(node, 0, tri);
    bool isGenerate = newTriangleInfo.first;
    auto newTriangles = newTriangleInfo.second;

    //if (isGenerate)
    //{
    //    for (auto t : newTriangles)
    //    {
    //        if (t.first == true)
    //            node->FrontNode->triangles.push_back(t.second);
    //        else
    //            node->BackNode->triangles.push_back(t.second);
    //    }
    //    //temp.insert(temp.end(), newTriangles.begin(), newTriangles.end());
    //}
    //else
    //{
    //    if (newTriangles[0].first == true)
    //        node->FrontNode->triangles.push_back(newTriangles[0].second);
    //    else
    //        node->BackNode->triangles.push_back(newTriangles[0].second);
    //}
    if (newTriangles[0].first == true)
        node->FrontNode->triangles.push_back(newTriangles[0].second);
    else
        node->BackNode->triangles.push_back(newTriangles[0].second);

}

bool Object::Tri_check(const Triangle& tri, AABB* aabb)
{
    for (int i = 0; i < 3; i++) {
        if (tri.vertices[i].x < aabb->min.x || tri.vertices[i].x > aabb->max.x ||
            tri.vertices[i].y < aabb->min.y || tri.vertices[i].y > aabb->max.y ||
            tri.vertices[i].z < aabb->min.z || tri.vertices[i].z > aabb->max.z) 
        {
            return false;
        }
    }


    return true;
}

void Object::Build_BSP(BSPNode* node)
{
    if (node->triangles.size() <= 3000)
    {
        node->color = generateRandomColor();
        return;
    }

    for (const auto& tri : node->triangles)
    {
        node->center+=tri.vertices[0];
        node->center+=tri.vertices[1];
        node->center+=tri.vertices[2];
    }
    node->center = node->center / (node->triangles.size() * 3.f);

    node->FrontNode = new BSPNode(node->level + 1);
    node->BackNode = new BSPNode(node->level + 1);

    for (const auto& tri : node->triangles)
    {
        DivideOrAdd(node, tri);
    }

    Build_BSP(node->FrontNode);
    Build_BSP(node->BackNode);

}

#include <glm/gtx/intersect.hpp>
std::pair<bool, std::vector<Triangle>> Object::IntersectRayPlane(OctreeNode* node, int axisIndex, Triangle v)
{
    std::vector<Triangle> resultT;
    glm::vec3 normal;

    switch (axisIndex)
    {
    case 0: //X
        normal = glm::vec3(1, 0, 0);
        break;

    case 1: //Y
        normal = glm::vec3(0, 1, 0);
        break;

    case 2: //Z
        normal = glm::vec3(0, 0, 1);
        break;
    }
    std::vector<glm::vec3> front;
    std::vector<glm::vec3> back;
    std::vector<glm::vec3> inside;

    for (int i = 0; i < 3; ++i)
    {
        if (node->aabb->center[axisIndex] < v.vertices[i][axisIndex])
            front.push_back(v.vertices[i]);
        else if (node->aabb->center[axisIndex] > v.vertices[i][axisIndex])
            back.push_back(v.vertices[i]);
        else
            inside.push_back(v.vertices[i]);
    }

    if (back.empty() || front.empty())
        return { false, resultT };

    if (!inside.empty())
    {
        glm::vec3 v1 = front[0];
        glm::vec3 v2 = back[0];
        glm::vec3 dir = glm::normalize(v2 - v1);
        float distance = 0.0f;

        bool intersect = glm::intersectRayPlane(v1, dir, node->aabb->center, normal, distance);

        if (!intersect)
            return { false, resultT };

        glm::vec3 insideV = inside[0];
        glm::vec3 i1 = v1 + distance * dir;

        Triangle t1{ v1, i1, insideV };
        Triangle t2{ i1, v2, insideV };

        resultT.insert(resultT.end(), { t1,t2 });
    }
    else
    {
        glm::vec3 v1;
        glm::vec3 v2;
        glm::vec3 v3;
        if (back.size() < front.size())
        {
            v1 = back[0];
            v2 = front[0];
            v3 = front[1];
        }
        else
        {
            v1 = front[0];
            v2 = back[0];
            v3 = back[1];
        }

        glm::vec3 dir = glm::normalize(v2 - v1);
        float distance;

        bool intersect = glm::intersectRayPlane(v1, dir, node->aabb->center, normal, distance);
        if (!intersect)
            return { false, resultT };
        glm::vec3 i1 = v1 + distance * dir;

        dir = glm::normalize(v3 - v1);
        intersect = glm::intersectRayPlane(v1, dir, node->aabb->center, normal, distance);
        if (!intersect)
            return { false, resultT };
        glm::vec3 i2 = v1 + distance * dir;

        Triangle t1{ v1, i2, i1 };
        Triangle t2{ i1, i2, v2 };
        Triangle t3{ i2, v3, v2 };
        resultT.insert(resultT.end(), { t1,t2,t3 });
    }

    return { true, resultT };
}

std::pair<bool, std::vector<std::pair<bool, Triangle>>> Object::IntersectRayPlane(BSPNode* node, int axisIndex, Triangle v)
{
    std::vector<std::pair<bool, Triangle>> resultT; // 0false back , 1true front
    glm::vec3 normal = glm::vec3(1, 0, 0);

    std::vector<glm::vec3> front;
    std::vector<glm::vec3> back;
    std::vector<glm::vec3> inside;

    for (int i = 0; i < 3; ++i)
    {
        if (node->center[axisIndex] < v.vertices[i][axisIndex])
            front.push_back(v.vertices[i]);
        else if (node->center[axisIndex] > v.vertices[i][axisIndex])
            back.push_back(v.vertices[i]);
        else
            inside.push_back(v.vertices[i]);
    }

    if (front.size() == 3)
    {
        resultT.push_back({ true,v });
        return { false, resultT };
    }
    else if (back.size() == 3)
    {
        resultT.push_back({ false,v });
        return { false, resultT };
    }
    else if (inside.size() == 3)
    {
        resultT.push_back({ true,v });
        return { false, resultT };
    }        

    if (!inside.empty())
    {
        glm::vec3 v1 = front[0];
        glm::vec3 v2 = back[0];
        glm::vec3 dir = glm::normalize(v2 - v1);
        float distance = 0.0f;

        bool intersect = glm::intersectRayPlane(v1, dir, node->center, normal, distance);

        if (!intersect)
        {
            resultT.push_back({ true,v });
            return { false, resultT };
        }

        glm::vec3 insideV = inside[0];
        glm::vec3 i1 = v1 + distance * dir;

        Triangle t1{ v1, i1, insideV };
        Triangle t2{ i1, v2, insideV };

        resultT.insert(resultT.end(), { {true,t1},{false,t2} });
    }
    else
    {
        glm::vec3 v1;
        glm::vec3 v2;
        glm::vec3 v3;
        if (back.size() < front.size())
        {
            v1 = back[0];
            v2 = front[0];
            v3 = front[1];
        }
        else
        {
            v1 = front[0];
            v2 = back[0];
            v3 = back[1];
        }

        glm::vec3 dir = glm::normalize(v2 - v1);
        float distance;

        bool intersect = glm::intersectRayPlane(v1, dir, node->center, normal, distance);
        if (!intersect)
        {
            resultT.push_back({ true,v });
            return { false, resultT };
        }
        glm::vec3 i1 = v1 + distance * dir;

        dir = glm::normalize(v3 - v1);
        intersect = glm::intersectRayPlane(v1, dir, node->center, normal, distance);
        if (!intersect)
        {
            resultT.push_back({ true,v });
            return { false, resultT };
        }
        glm::vec3 i2 = v1 + distance * dir;

        Triangle t1{ v1, i2, i1 };
        Triangle t2{ i1, i2, v2 };
        Triangle t3{ i2, v3, v2 };

        if (back.size() < front.size())
            resultT.insert(resultT.end(), { {false,t1},{true,t2},{true,t3} });
        else
            resultT.insert(resultT.end(), { {true,t1},{false,t2},{false,t3} });
    }

    return { true, resultT };
}

Mesh Object::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    //std::vector<Texture> textures;
    float xyz_minmax[3][2]{};
    std::vector<glm::vec3> fnBuffer;

    // walk through each of the mesh's vertices

	for (int i{ 0 }; i < 2; i++)
	{
		xyz_minmax[0][i] = mesh->mVertices[0].x;
		xyz_minmax[1][i] = mesh->mVertices[0].y;
		xyz_minmax[2][i] = mesh->mVertices[0].z;
	}
    
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vert;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vert.pos = vector;
        for (int i{ 0 }; i < 3; i++)
        {
            if (xyz_minmax[i][0] > vert.pos[i])
            {
                xyz_minmax[i][0] = vert.pos[i];
            }
            else if (xyz_minmax[i][1] < vert.pos[i])
            {
                xyz_minmax[i][1] = vert.pos[i];
            }
        }
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            //mesh->mFaces[0].mIndices[0]
            vert.nrm = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            //glm::vec2 vec;
            //// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            //// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            //vec.x = mesh->mTextureCoords[0][i].x;
            //vec.y = mesh->mTextureCoords[0][i].y;
            //vertex.uv = vec;
            //// tangent
            //vector.x = mesh->mTangents[i].x;
            //vector.y = mesh->mTangents[i].y;
            //vector.z = mesh->mTangents[i].z;
            //vertex.Tangent = vector;
            //// bitangent
            //vector.x = mesh->mBitangents[i].x;
            //vector.y = mesh->mBitangents[i].y;
            //vector.z = mesh->mBitangents[i].z;
            //vertex.Bitangent = vector;
        }
        else
        {
            vert.uv = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vert);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        Vec3 index;
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
            index[j] = face.mIndices[j];
        }
        glm::vec3 P = vertices[index[0]].pos;
        glm::vec3 Q = vertices[index[1]].pos;
        glm::vec3 R = vertices[index[2]].pos;

        glm::vec3 face_nrm = glm::normalize(glm::cross(Q - P, R - P));

        glm::vec3 tricenter = (P + Q + R) / 3.f;
        fnBuffer.push_back(tricenter);
        fnBuffer.push_back(tricenter + face_nrm);

    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    //// 1. diffuse maps
    //vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    //textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    //// 2. specular maps
    //vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    //textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    //// 3. normal maps
    //std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    //textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    //// 4. height maps
    //std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    //textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    //// return a mesh object created from the extracted mesh data
    //return Mesh(vertices, indices, textures);

    return Mesh(vertices, indices, textures, xyz_minmax, fnBuffer);
}

void Object::g_buffer_update(GLSLShader& shader, Mat4& view, Mat4& projection, Global& g, Material& m, bool use_material)
{
    shader.Use();
    if (is_updated == true)
    {
        model = glm::translate(glm::mat4(1.f), position);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
    }
    GLuint shdr_handle = shader.GetHandle();
    glUniformMatrix4fv(glGetUniformLocation(shdr_handle, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shdr_handle, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shdr_handle, "model"), 1, GL_FALSE, glm::value_ptr(model));
    //glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "mapping"), 1, GL_FALSE, glm::value_ptr(meshes[0].mapping));

    //glUniform2fv(glGetUniformLocation(shdr_handle, "fog_min_max"), 1, glm::value_ptr(g.fog_min_max));
    if (use_material)
    {
        glUniform3fv(glGetUniformLocation(shdr_handle, "m_ambient"), 1, glm::value_ptr(m.ambient));
        glUniform3fv(glGetUniformLocation(shdr_handle, "m_emissive"), 1, glm::value_ptr(m.emissive));
    }
    else
    {
        glUniform3f(glGetUniformLocation(shdr_handle, "m_ambient"), 0.f,0.f,0.f);
        glUniform3f(glGetUniformLocation(shdr_handle, "m_emissive"), 0.f, 0.f, 0.f);
    }
    
   
    shader.UnUse();
}

void Object::g_buffer_draw(GLSLShader& shader)
{
    shader.Use();

    for (auto mesh : meshes)
    {
        //mesh.setup_mesh(shader);
        mesh.Draw(shader);
    }
    
    //glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(selfColor));
    

    shader.UnUse();
}

void Object::lightpass_update(GLSLShader& shader, Vec3& eye, Global& g, Per_Light* pl, Material& m, G_Data g_data, Vec4 plane_color)
{
    shader.Use();

    if (is_updated == true)
    {
        model = glm::translate(glm::mat4(1.f), position);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);

    }

    GLuint shdr_handle = shader.GetHandle();
    glUniform1i(glGetUniformLocation(shdr_handle, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(shdr_handle, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(shdr_handle, "gDiff"), 2);
    glUniform1i(glGetUniformLocation(shdr_handle, "gSpec"), 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_data.gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_data.gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_data.gDiff);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, g_data.gSpec);

    glUniform4fv(glGetUniformLocation(shdr_handle, "color"), 1, glm::value_ptr(plane_color));

    //glUniform3fv(glGetUniformLocation(shdr_handle, "fogColor"), 1, glm::value_ptr(g.fog_color));

    glUniform3fv(glGetUniformLocation(shdr_handle, "att_const"), 1, glm::value_ptr(g.att_const));

    glUniform1iv(glGetUniformLocation(shdr_handle, "active_lights"), 1, &g.active_lights);
    glUniform3fv(glGetUniformLocation(shdr_handle, "g_ambient"), 1, glm::value_ptr(g.g_ambient));

    glUniform3fv(glGetUniformLocation(shdr_handle, "viewPos"), 1, glm::value_ptr(eye));



        //uniform int pipeline;

	glUniform3fv(glGetUniformLocation(shdr_handle, "m_ambient"), 1, glm::value_ptr(m.ambient));
	glUniform3fv(glGetUniformLocation(shdr_handle, "m_diffuse"), 1, glm::value_ptr(m.diffuse));
	glUniform3fv(glGetUniformLocation(shdr_handle, "m_specular"), 1, glm::value_ptr(m.specular));
	glUniform3fv(glGetUniformLocation(shdr_handle, "m_emissive"), 1, glm::value_ptr(m.emissive));


    
     
    // First get the block index
    //GLuint uboIndex;
    //uboIndex = glGetUniformBlockIndex(shdr_handle, "Light");
    // Now get the size
    //GLint uboSize;
    //glGetActiveUniformBlockiv(shdr_handle, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);
    //glUniformBlockBinding(shdr_handle, uboIndex, 0);

    for (Mesh& mesh : meshes)
    {
        //glBindBuffer(GL_UNIFORM_BUFFER, mesh.UBO[0]);
        //glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
        //glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
            glBindBuffer(GL_UNIFORM_BUFFER, meshes[0].UBO);
            int offset{ 0 };

            for (int i{ 0 }; i < g.active_lights; i++)
            {
                glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(LightType), &pl[i].l_type);
                offset += 16;
                glBufferSubData(GL_UNIFORM_BUFFER, offset, 1 * 16, glm::value_ptr(pl[i].ambient_color));
                offset += 16;
                glBufferSubData(GL_UNIFORM_BUFFER, offset, 1 * 16, glm::value_ptr(pl[i].diffuse_color));
                offset += 16;
                glBufferSubData(GL_UNIFORM_BUFFER, offset, 1 * 16, glm::value_ptr(pl[i].specular_color));
                offset += 16;
                glBufferSubData(GL_UNIFORM_BUFFER, offset, 1 * 16, glm::value_ptr(pl[i].pos));
                offset += 16;
                glBufferSubData(GL_UNIFORM_BUFFER, offset, 1 * 16, glm::value_ptr(pl[i].dir));
                offset += 16;
                glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, glm::value_ptr(pl[i].inner_outer_angle));
                offset += 12;
                glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &(pl[i].falloff));
                offset += 4;
            }        
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    shader.UnUse();
}

void Object::renderQuad(GLSLShader& shader)
{
    //temp->setup_mesh(shader);
    shader.Use();
    glBindVertexArray(meshes[0].VAO);
    glBindBuffer(GL_UNIFORM_BUFFER, meshes[0].UBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, meshes[0].UBO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    shader.UnUse();
}


#include <random>

// Define a function to generate a random color
glm::vec4 generateRandomColor()
{
    // Create a random number generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.3f, 1.0f);

    // Generate three random floats for RGB values
    float r = dis(gen);
    float g = dis(gen);
    float b = dis(gen);

    return glm::vec4(r, g, b, -1.f);
}