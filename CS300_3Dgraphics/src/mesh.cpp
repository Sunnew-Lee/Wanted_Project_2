/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: mesh.cpp
Purpose: 
    This file contains definitions of member functions of class Mesh and Mesh creating functions.
Language: C++ 17
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_2
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/
#include <mesh.h>

Mesh::Mesh(std::string path)
{
    readOBJ(path);
    calc_BufferDatas();
    SendVertexData();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, float minmax[3][2],std::vector<Vec3> fnbuffer):
    vertexBuffer(vertices),indexBuffer(indices),textureBuffer(textures), fnBuffer(fnbuffer)
{
    AABB_Setup(minmax);
     // now that we have all the required data, set the vertex buffers and its attribute pointers.
    SPHERE_Setup(200);
    calc_BufferDatas();
    uv_Sphere();
    Setup();
}

void Mesh::Setup()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(7, VBO);    
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(Vertex), &vertexBuffer[0], GL_STATIC_DRAW);

    // Mesh pos, nrm, uv
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    // Mesh face normal
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, fnBuffer.size() * sizeof(glm::vec3), &fnBuffer[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));

    // Mesh vertex normal
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, vnBuffer.size() * sizeof(glm::vec3), &vnBuffer[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));

    // Mesh AABB BV
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, bounding_box->lines.size() * sizeof(glm::vec3), &bounding_box->lines[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));

    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, Ritter->vbuffer.size() * sizeof(Vertex), &Ritter->vbuffer[0], GL_STATIC_DRAW);

    // Mesh pos, nrm, uv
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, Larsson->vbuffer.size() * sizeof(Vertex), &Larsson->vbuffer[0], GL_STATIC_DRAW);

    // Mesh pos, nrm, uv
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, PCA->vbuffer.size() * sizeof(Vertex), &PCA->vbuffer[0], GL_STATIC_DRAW);

    // Mesh pos, nrm, uv
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));


    glGenBuffers(4, EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    /*  Copy vertex indices to GPU */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(int), &indexBuffer[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Ritter->ibuffer.size() * sizeof(int), &Ritter->ibuffer[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Larsson->ibuffer.size() * sizeof(int), &Larsson->ibuffer[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, PCA->ibuffer.size() * sizeof(int), &PCA->ibuffer[0], GL_STATIC_DRAW);
    //if (shdr_type < 3)
    //{
        glGenBuffers(1, &UBO);
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferData(GL_UNIFORM_BUFFER, 16 * 112, NULL, GL_STATIC_DRAW);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 16 * 112);

    //}
    //else
    //{
    //    glGenBuffers(1, UBO);
    //}
    //glBindBuffer(GL_UNIFORM_BUFFER, UBO[0]);
    //glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

    //glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO[0], 0, 2 * sizeof(glm::mat4));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //// set the vertex attribute pointers
    //// vertex Positions
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    //// vertex normals
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    //// vertex texture coords
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    //// vertex tangent
    //glEnableVertexAttribArray(3);
    //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    //// vertex bitangent
    //glEnableVertexAttribArray(4);
    //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    //// ids
    //glEnableVertexAttribArray(5);
    //glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    //// weights
    //glEnableVertexAttribArray(6);
    //glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    //glBindVertexArray(0);
}

void Mesh::Draw(GLSLShader& shader)
{
    shader.Use();
    //glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "mapping"), 1, GL_FALSE, glm::value_ptr(mapping));
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < textureBuffer.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textureBuffer[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.GetHandle(), (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textureBuffer[i].id);
    }
    // draw mesh
    if (textureBuffer.empty())
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
    }

    glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indexBuffer.size()), GL_UNSIGNED_INT, 0);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
    shader.UnUse();
}

void Mesh::Draw_NRM(GLSLShader& shader, bool show_vn, bool show_fn, int selected_bv)
{
    shader.Use();
    //glUniformMatrix4fv(glGetUniformLocation(shader.GetHandle(), "mapping"), 1, GL_FALSE, glm::value_ptr(mapping));
    glBindVertexArray(VAO);

    if (show_fn == true)
    {
        // enable face normal
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)(0));
        glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(0.2f, 0.5f, 0.2f, -1.f)));   // green for face normal
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(fnBuffer.size()));
    }

    if (show_vn == true)
    {
        // enable vertex normal
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)(0));
        glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(0.4f, 0.3f, 0.6f, -1.f)));  // blue for vertex normal
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vnBuffer.size()));
    }

    //switch (selected_bv)
    //{
    //case 0://AABB
    //    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)(0));
    //    glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(0.5f, 0.5f, 0.5f, -1.f)));
    //    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(bounding_box->lines.size()));
    //    break;
    //case 1://SPHERE_Ritter
    //    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
    //    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));
    //    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //    glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(1.f, 0.5f, 0.5f, -1.f)));
    //    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(Ritter->ibuffer.size()), GL_UNSIGNED_INT, 0);
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //    break;
    //case 2://SPHERE_Larsson
    //    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
    //    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));
    //    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //    glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(0.5f, 1.f, 0.5f, -1.f)));
    //    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(Larsson->ibuffer.size()), GL_UNSIGNED_INT, 0);
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //    break;
    //case 3://SPHERE_PCA
    //    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
    //    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));
    //    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //    glUniform4fv(glGetUniformLocation(shader.GetHandle(), "color"), 1, glm::value_ptr(glm::vec4(0.5f, 0.5f, 1.f, -1.f)));
    //    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(PCA->ibuffer.size()), GL_UNSIGNED_INT, 0);
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //    break;
    //default:break;
    //}
 

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    shader.UnUse();
}

void Mesh::vert_mapping(float xyz_minmax[3][2])
{
    //float W = xyz_minmax[0][1] - xyz_minmax[0][0];  //x width
    //float H = xyz_minmax[1][1] - xyz_minmax[1][0];  //y height
    //float D = xyz_minmax[2][1] - xyz_minmax[2][0];  //z depth

    //glm::vec3 center{glm::vec3((xyz_minmax[0][0] + xyz_minmax[0][1]),(xyz_minmax[1][0] + xyz_minmax[1][1]),(xyz_minmax[2][0] + xyz_minmax[2][1]))};
    //center *= 0.5f;

    //glm::mat4 T = glm::mat4(1.0f);
    //glm::mat4 S = glm::mat4(1.0f);

    //float scale_factor = std::max({ W, H, D });

    //OBJ_Translate = glm::translate(glm::mat4(1.0f), -center);
    //OBJ_Scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.f / scale_factor, 2.f / scale_factor, 2.f / scale_factor));
    //
    //mapping = OBJ_Scale * OBJ_Translate;

}

void Mesh::calc_vert_normal()
{
    int size = static_cast<int>(vertexBuffer.size());
    for (int i{ 0 }; i < size; i++)
    {
        glm::vec3 result = glm::vec3(0.f);

        auto func = [](glm::vec3 left, glm::vec3 right) {return (glm::epsilonEqual(left.x, right.x, EPSILON) && glm::epsilonEqual(left.y, right.y, EPSILON) && glm::epsilonEqual(left.z, right.z, EPSILON)); };
        faces[i].erase(std::unique(faces[i].begin(), faces[i].end(), func), faces[i].end());

        for (glm::vec3& a : faces[i])
        {
            result += a;
        }
        vertexBuffer[i].nrm = glm::normalize(result);
    }
}

void Mesh::calc_vert_uv(int type, int entity_type)
{
    switch (type)
    {
    case 0:uv_Cylinder(entity_type); break;
    //case 1:uv_Sphere(entity_type); break;
    case 2:uv_Cube(entity_type); break;
    case 3:uv_Planer(entity_type); break;

    default: break;
    }
}

void Mesh::SendVertexData()
{
    //glGenVertexArrays(1, &VAO);
    //glBindVertexArray(VAO);

    //glGenBuffers(3, VBO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    ///*  Copy vertex attributes to GPU */
    //glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(Vertex), &vertexBuffer[0], GL_STATIC_DRAW);

    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));

    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));

    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    //glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    ///*  Copy vertex attributes to GPU */
    //glBufferData(GL_ARRAY_BUFFER, fnBuffer.size() * sizeof(glm::vec3), &fnBuffer[0], GL_STATIC_DRAW);

    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));


    //glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    ///*  Copy vertex attributes to GPU */
    //glBufferData(GL_ARRAY_BUFFER, vnBuffer.size() * sizeof(glm::vec3), &vnBuffer[0], GL_STATIC_DRAW);

    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));


    //glGenBuffers(1, &EBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    ///*  Copy vertex indices to GPU */
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(int), &indexBuffer[0], GL_STATIC_DRAW);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);
}

void Mesh::UpdateVertexData()
{
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    /*  Copy vertex attributes to GPU */
    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(Vertex), &vertexBuffer[0], GL_STATIC_DRAW);

    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));

    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));

    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    //glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    ///*  Copy vertex attributes to GPU */
    //glBufferData(GL_ARRAY_BUFFER, fnBuffer.size() * sizeof(glm::vec3), &fnBuffer[0], GL_STATIC_DRAW);

    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));


    //glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    ///*  Copy vertex attributes to GPU */
    //glBufferData(GL_ARRAY_BUFFER, vnBuffer.size() * sizeof(glm::vec3), &vnBuffer[0], GL_STATIC_DRAW);

    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    ///*  Copy vertex indices to GPU */
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(int), &indexBuffer[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::LineVertexData()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    /*  Copy vertex attributes to GPU */
    glBufferData(GL_ARRAY_BUFFER, lineposBuffer.size() * sizeof(glm::vec3), &lineposBuffer[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

void Mesh::setup_UBO(unsigned shdr_type)
{
    if(shdr_type<3)
    {
        glGenBuffers(1, &UBO);
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferData(GL_UNIFORM_BUFFER, 16 * 112, NULL, GL_STATIC_DRAW);

        //glBindBuffer(GL_UNIFORM_BUFFER, 0);

        //glBindBufferRange(GL_UNIFORM_BUFFER, 1, UBO[1], 0, 16 * 112);
        
    }
    //else
    //{
    //    glGenBuffers(1, UBO);
    //}
    //glBindBuffer(GL_UNIFORM_BUFFER, UBO[0]);
    //glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

    //glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO[0], 0, 2 * sizeof(glm::mat4));

}

void Mesh::uv_Cylinder(int entity)
{
    int size = static_cast<int>(vertexBuffer.size());
    //float y_min = xyz_minmax[2][0];
    //float y_max = xyz_minmax[2][1];
    
    for (int i{ 0 }; i < size; i++)
    {
        Vec3 t_pos{ 0.f };
        if (entity == 0)
            t_pos = Vec3(mapping * glm::vec4(vertexBuffer[i].pos, 1.f));
        else if (entity == 1)
            t_pos = vertexBuffer[i].nrm;

        float theta = glm::degrees(atan(t_pos.x / t_pos.z)) + 90; //degrees(atan(t_pos.z / t_pos.x)) + 90.;
        float u = theta / 360;
        //float u = (t_pos.x - (x_min)) / ((x_max)-(x_min));
        float v = (t_pos.y - (-1)) / ((1) - (-1));
        //float v = (t_pos.y - (y_min)) / ((y_max)-(y_min));

        vertexBuffer[i].uv = Vec2(u, v);
    }
}

void Mesh::uv_Sphere()
{
    int size = static_cast<int>(vertexBuffer.size());

    for (int i{ 0 }; i < size; i++)
    {
        Vec3 t_pos = Vec3(mapping * glm::vec4(vertexBuffer[i].pos, 1.f));
        //if (entity == 0)
        //    t_pos = Vec3(mapping * glm::vec4(vertexBuffer[i].pos, 1.f));
        //else if (entity == 1)
        //    t_pos = vertexBuffer[i].nrm;

        float theta = glm::degrees(atan(t_pos.x / t_pos.z)) + 90; //degrees(atan(t_pos.z / t_pos.x)) + 90.;
        float phi = glm::degrees(acos(t_pos.y / glm::length(t_pos)));
        float u = theta / 360;
        float v = phi / 180;

        vertexBuffer[i].uv = Vec2(u, v);
    }
}

void Mesh::uv_Cube(int entity)
{

    // and so on ¡¦ +-Y and +-Z
    // At this point, uv.x and uv.y should be in the range [-1, 1]
    // Convert range from -1 to 1 to 0 to 1
    //return (uv + vec3(1.0)) * 0.5;

    int size = static_cast<int>(vertexBuffer.size());
    float u{ 0.f }, v{ 0.f };

    for (int i{ 0 }; i < size; i++)
    {
        Vec3 t_pos{ 0.f };
        if (entity == 0)
            t_pos = Vec3(mapping * glm::vec4(vertexBuffer[i].pos, 1.f));
        else if (entity == 1)
            t_pos = vertexBuffer[i].nrm;

        Vec3 abspos = abs(t_pos);

        //// +-X
        //if (abspos.x >= abspos.y && abspos.x >= abspos.z)
        //{
        //    (t_pos.x < 0.0) ? (u = t_pos.z/ abspos.x) : (u = -t_pos.z/ abspos.x);
        //    v = t_pos.y/ abspos.x;
        //}
        //// +-Y
        //else if (abspos.y >= abspos.x && abspos.y >= abspos.z)
        //{
        //    (t_pos.y < 0.0) ? (v = t_pos.z/ abspos.y) : (v = -t_pos.z/ abspos.y);
        //    u = t_pos.x/ abspos.y;
        //}
        //// +-Z
        //else if (abspos.z >= abspos.x && abspos.z >= abspos.y)
        //{
        //    (t_pos.z < 0.0) ? (u = -t_pos.x/ abspos.z) : (u = t_pos.x/ abspos.z);
        //    v = t_pos.y/ abspos.z;
        //}
        // 
		// +-X
		if (abspos.x >= abspos.y && abspos.x >= abspos.z)
		{
			(t_pos.x < 0.0) ? (u = t_pos.y) : (u = -t_pos.y);
			v = t_pos.z;
		}
		// +-Y
		else if (abspos.y >= abspos.x && abspos.y >= abspos.z)
		{
			(t_pos.y < 0.0) ? (v = t_pos.z) : (v = -t_pos.z);
			u = t_pos.x;
		}
		// +-Z
		else if (abspos.z >= abspos.x && abspos.z >= abspos.y)
		{
			(t_pos.z < 0.0) ? (u = -t_pos.x) : (u = t_pos.x);
			v = -t_pos.y;
		}

        //// +-X
        if (abspos.x >= abspos.y && abspos.x >= abspos.z)
        {
            v = t_pos.y / abspos.x;
            if (t_pos.x < 0.0)
            {
                u = -t_pos.z / abspos.x;
            }
            else
            {
                u = t_pos.z / abspos.x;
            }
        }
        // +-Y
        else if (abspos.y >= abspos.x && abspos.y >= abspos.z)
        {
            u = t_pos.x / abspos.y;

            if (t_pos.y < 0.0)
            {
                v = -t_pos.z / abspos.y;
            }
            else
            {
                v = t_pos.z / abspos.y;
            }
        }
        // +-Z
        else if (abspos.z >= abspos.x && abspos.z >= abspos.y)
        {
            v = t_pos.y / abspos.z;
            if (t_pos.z < 0.0)
            {
                u = t_pos.x / abspos.z;
            }
            else
            {
                u = -t_pos.x / abspos.z;
            }
        }


        //vertexBuffer[i].uv = Vec2(u, v);
        vertexBuffer[i].uv = (Vec2(u, v) + Vec2(1.f)) * 0.5f;
    }
}

void Mesh::uv_Planer(int entity)
{
    int size = static_cast<int>(vertexBuffer.size());
    //float x_min = xyz_minmax[0][0];
    //float x_max = xyz_minmax[0][1];
    //float z_min = xyz_minmax[2][0];
    //float z_max = xyz_minmax[2][1];

    for (int i{ 0 }; i < size; i++)
    {
        Vec3 t_pos{ 0.f };
        if (entity == 0)
            t_pos = Vec3(mapping * glm::vec4(vertexBuffer[i].pos, 1.f));
        else if (entity == 1)
            t_pos = vertexBuffer[i].nrm;

        float u = (t_pos.x - (-1)) / ((1) - (-1));
        //float u = (t_pos.x - (x_min)) / ((x_max)-(x_min));
        float v = (t_pos.y - (-1)) / ((1) - (-1));
        //float v = (t_pos.z - (z_min)) / ((z_max)-(z_min));

        vertexBuffer[i].uv = Vec2(u, v);
    }
}

void Mesh::AABB_Setup(float xyz_minmax[3][2])
{
    bounding_box = new AABB();

    float W = xyz_minmax[0][1] - xyz_minmax[0][0];  //x width
    float H = xyz_minmax[1][1] - xyz_minmax[1][0];  //y height
    float D = xyz_minmax[2][1] - xyz_minmax[2][0];  //z depth
    //bounding_box->half_extents = glm::vec3(W, H, D) * 0.5f;
    bounding_box->min = glm::vec3(xyz_minmax[0][0], xyz_minmax[1][0], xyz_minmax[2][0]);
    bounding_box->max = glm::vec3(xyz_minmax[0][1], xyz_minmax[1][1], xyz_minmax[2][1]);

    bounding_box->center = glm::vec3((xyz_minmax[0][0] + xyz_minmax[0][1]), (xyz_minmax[1][0] + xyz_minmax[1][1]), (xyz_minmax[2][0] + xyz_minmax[2][1]));
    bounding_box->center *= 0.5f;

    bounding_box->CreateAABB();
}

void Mesh::SPHERE_Setup(int num)
{
    if (num > vertexBuffer.size())
        num = vertexBuffer.size();

    Ritter = new SPHERE();
    Larsson = new SPHERE();
    PCA = new SPHERE();

    std::vector<glm::vec3> dir;
    std::vector<std::pair<glm::vec3, glm::vec3>> extreme;
    //Ritter
    dir.push_back(glm::vec3(1.f, 0.f, 0.f));
    dir.push_back(glm::vec3(0.f, 1.f, 0.f));
    dir.push_back(glm::vec3(0.f, 0.f, 1.f));
    for (auto& d : dir)
    {
        extreme.push_back(extremePointsAlongDirection(d));
    }

    float max_dist = -FLT_MAX;

    for (const auto& p : extreme)
    {
        if (max_dist < glm::distance(p.first, p.second))
        {
            max_dist = glm::distance(p.first, p.second);
            Ritter->center = (p.first + p.second) * 0.5f;
            Ritter->radius = max_dist * 0.5f;
        }
    }

    for (const auto& v : vertexBuffer)
    {
        if (glm::distance(v.pos, Ritter->center) > Ritter->radius)
        {
            glm::vec3 d = glm::normalize(v.pos - Ritter->center);
            float rad_new = (Ritter->radius + abs(glm::distance(v.pos, Ritter->center))) * 0.5f;
            Ritter->center = Ritter->center + (rad_new - Ritter->radius) * d;
            Ritter->radius = rad_new;
        }
    }

    Ritter->CreateSphere(50, 50);


    dir.clear();
    dir.push_back(glm::vec3(1.f, 1.f, 1.f));
    dir.push_back(glm::vec3(1.f, 1.f, -1.f));
    dir.push_back(glm::vec3(1.f, -1.f, 1.f));
    dir.push_back(glm::vec3(1.f, -1.f, -1.f));
    for (auto& d : dir)
    {
        extreme.push_back(extremePointsAlongDirection(d));
    }

    max_dist = -FLT_MAX;

    for (const auto& p : extreme)
    {
        if (max_dist < glm::distance(p.first, p.second))
        {
            max_dist = glm::distance(p.first, p.second);
            Larsson->center = (p.first + p.second) * 0.5f;
            Larsson->radius = max_dist * 0.5f;
        }
    }

    for (const auto& v : vertexBuffer)
    {
        if (glm::distance(v.pos, Larsson->center) > Larsson->radius)
        {
            glm::vec3 d = glm::normalize(v.pos - Larsson->center);
            float rad_new = (Larsson->radius + abs(glm::distance(v.pos, Larsson->center))) * 0.5f;
            Larsson->center = Larsson->center + (rad_new - Larsson->radius) * d;
            Larsson->radius = rad_new;
        }
    }

    Larsson->CreateSphere(50, 50);



    glm::mat3 cov(0.f);
    glm::mat3 e_vec(1.f);
    glm::vec3 mean(0.f);
    for (const auto& v : vertexBuffer)
    {
        mean += v.pos;
    }
    mean /= vertexBuffer.size();


    for (int i{ 0 }; i < 3; i++)
    {
        for (int j{ i }; j < 3; j++)
        {
            for (const auto& v : vertexBuffer)
            {
                cov[i][j] += (v.pos[i] - mean[i]) * (v.pos[j] - mean[j]);
            }
            cov[i][j] /= vertexBuffer.size();

            if (j > i)
                cov[j][i] = cov[i][j];
        }
    }


    float curr_sum = FLT_MAX;
    float prev_sum{ 0.f };

    int iteration{ 0 };
    while (!(curr_sum > -0.00001f && curr_sum < 0.00001f) && !(abs(curr_sum - prev_sum) > -0.00001f && abs(curr_sum - prev_sum) < 0.00001f) && iteration < 100)
    {
        prev_sum = curr_sum;
        curr_sum = 0.f;

        float max = -FLT_MAX;
        float p, q;
        for (int i{ 0 }; i < 3; i++)
        {
            for (int j{ 0 }; j < 3; j++)
            {
                if (i == j)
                    continue;
                if (cov[i][j] > max)
                {
                    max = cov[i][j];
                    p = i; q = j;
                }
                curr_sum += cov[i][j];
            }
        }

        float beta = (cov[q][q] - cov[p][p]) / (2 * cov[p][q]);
        float t = sin(beta) / (abs(beta) + sqrtf(beta * beta + 1));
        float c = 1.f / sqrtf(t * t + 1);
        float s = c * t;

        glm::mat3 J(1.f);
        J[p][p] = c;
        J[p][q] = s;
        J[q][p] = -s;
        J[q][q] = c;

        glm::mat3 invJ = glm::inverse(J);

        cov = invJ * cov * J;
        e_vec = e_vec * J;
        iteration++;
    }
    //std::cout << iteration << std::endl;

    int index = (cov[0][0] > cov[1][1]) ? (cov[0][0] > cov[2][2] ? 0 : 2) : (cov[1][1] > cov[2][2] ? 1 : 2);
    const auto e = extremePointsAlongDirection(e_vec[index]);
    PCA->center = (e.first + e.second) * 0.5f;
    PCA->radius = glm::distance(e.first, e.second) * 0.5f;

    for (const auto& v : vertexBuffer)
    {
        if (glm::distance(v.pos, PCA->center) > PCA->radius)
        {
            glm::vec3 d = glm::normalize(v.pos - PCA->center);
            float rad_new = (PCA->radius + abs(glm::distance(v.pos, PCA->center))) * 0.5f;
            PCA->center = PCA->center + (rad_new - PCA->radius) * d;
            PCA->radius = rad_new;
        }
    }

    PCA->CreateSphere(50, 50);
}

std::pair<glm::vec3, glm::vec3> Mesh::extremePointsAlongDirection(glm::vec3 dir)
{
    float minProj = FLT_MAX;
    float maxProj = -FLT_MAX;
    std::pair<glm::vec3, glm::vec3> minmax;

    for (const auto& v : vertexBuffer)
    {
        float projDist = glm::dot(dir, v.pos);
        if (minProj > projDist)
        {
            minProj = projDist;
            minmax.first = v.pos;
        }
        if (maxProj <= projDist)
        {
            maxProj = projDist;
            minmax.second = v.pos;
        }
    }
    return minmax;
}

//void Mesh::CreateSphere(int stacks, int slices, SPHERE* sphere)
//{
//    for (int i = 0; i <= stacks; i++)
//    {
//        float row = static_cast<float>(i) / stacks;
//        float beta = glm::pi<float>() * (row - 0.5f);
//        for (int j = 0; j <= slices; j++)
//        {
//            Vertex vertex;
//            float col = static_cast<float>(j) / slices;
//            vertex.uv.x = -col;
//            vertex.uv.y = -row;
//
//            float alpha = col * glm::pi<float>() * 2.0f;
//            vertex.pos.x = sphere->center.x + sphere->radius * sin(alpha) * cos(beta);
//            vertex.pos.y = sphere->center.y + sphere->radius * sin(beta);
//            vertex.pos.z = sphere->center.z + sphere->radius * cos(alpha) * cos(beta);
//
//            vertex.nrm.x = vertex.pos.x;
//            vertex.nrm.y = vertex.pos.y;
//            vertex.nrm.z = vertex.pos.z;
//            //vertex.nrm /= radius;
//
//            sphere->vbuffer.push_back(vertex);
//        }
//    }
//    BuildIndexBuffer(stacks, slices, sphere);
//    //calc_BufferDatas();
//    //SendVertexData();
//    //Setup();
//}

void Mesh::BuildIndexBuffer(int stacks, int slices, SPHERE* sphere)
{
    int p0 = 0, p1 = 0, p2 = 0;
    int p3 = 0, p4 = 0, p5 = 0;
    int stride = slices + 1;

    for (int i = 0; i < stacks; ++i)
    {
        int curr_row = i * stride;

        for (int j = 0; j < slices; ++j)
        {
            /*  You need to compute the indices for the first triangle here */
            /*  ... */
            p0 = curr_row + j;
            p1 = p0 + 1;
            p2 = p1 + stride;

            /*  Ignore degenerate triangle */
            if (!DegenerateTri(sphere->vbuffer[p0].pos, sphere->vbuffer[p1].pos, sphere->vbuffer[p2].pos))
            {
                /*  Add the indices for the first triangle */
                sphere->ibuffer.push_back(p0);
                sphere->ibuffer.push_back(p1);
                sphere->ibuffer.push_back(p2);

                glm::vec3 P = sphere->vbuffer[p0].pos;
                glm::vec3 Q = sphere->vbuffer[p1].pos;
                glm::vec3 R = sphere->vbuffer[p2].pos;
            }

            /*  You need to compute the indices for the second triangle here */
            /*  ... */
            p3 = p2;
            p4 = p3 - 1;
            p5 = p0;

            /*  Ignore degenerate triangle */
            if (!DegenerateTri(sphere->vbuffer[p3].pos, sphere->vbuffer[p4].pos, sphere->vbuffer[p5].pos))
            {
                /*  Add the indices for the second triangle */
                sphere->ibuffer.push_back(p3);
                sphere->ibuffer.push_back(p4);
                sphere->ibuffer.push_back(p5);
            }
        }
    }
}

void Mesh::readOBJ(const std::filesystem::path& fileName)
{
    if (fileName.extension() != ".obj") {
        throw std::runtime_error("Bad Filetype.  " + fileName.generic_string() + " not a .obj file");
    }
    std::ifstream inFile(fileName);
    if (inFile.is_open() == false) {
        throw std::runtime_error("Failed to load " + fileName.generic_string());
    }

    std::string label;
    //float xyz_minmax[3][2];     //[0][0]->x min, [2][1]->z max
    bool is_init{ false };


    while (inFile.eof() == false) 
    {
        inFile >> label;
        
        if (label == "#")
        {
            getline(inFile, label);
        }

        else if (label == "v") 
        {
            Vertex vert;
            inFile >> vert.pos.x >> vert.pos.y >> vert.pos.z;

            if (is_init == false)
            {
                for (int i{ 0 }; i < 3; i++)
                {
                    for (int j{ 0 }; j < 2; j++)
                    {
                        //xyz_minmax[i][j] = vert.pos[i];
                    }
                }
                is_init = true;
            }
            else
            {
                for (int i{ 0 }; i < 3; i++)
                {
                    //if (xyz_minmax[i][0] > vert.pos[i])
                    //{
                    //    xyz_minmax[i][0] = vert.pos[i];
                    //}
                    //else if (xyz_minmax[i][1] < vert.pos[i])
                    //{
                    //    xyz_minmax[i][1] = vert.pos[i];
                    //}
                }
            }

            vertexBuffer.push_back(vert);            
 
        }

        else if (label == "f") 
        {
            int face[3]{ 0 };

            for (int i{ 0 }; i < 3; i++)
            {
                std::string index[3];
                int count{ 0 };

                inFile >> label;
                for (char c : label)
                {
                    if (c == '/')
                    {
                        count++;
                        continue;
                    }
                    index[count].push_back(c);
                }
                int num = (stoi(index[0]) - 1);
                indexBuffer.push_back(num);//pos vertex index
                //vtBuffer.push_back(stoi(index[1]) - 1);//texture vertex index
                //vnBuffer.push_back(stoi(index[2]) - 1);//normal vertex index.

                face[i] = num;
            }

            glm::vec3 P = vertexBuffer[face[0]].pos;
            glm::vec3 Q = vertexBuffer[face[1]].pos;
            glm::vec3 R = vertexBuffer[face[2]].pos;

            glm::vec3 face_nrm = glm::normalize(glm::cross(Q - P, R - P));

            faces.emplace(face[0], std::vector<glm::vec3>());
            faces.emplace(face[1], std::vector<glm::vec3>());
            faces.emplace(face[2], std::vector<glm::vec3>());
            faces[face[0]].push_back(face_nrm);
            faces[face[1]].push_back(face_nrm);
            faces[face[2]].push_back(face_nrm);
            
            glm::vec3 tricenter = (P + Q + R) / 3.f;
            fnBuffer.push_back(tricenter);
            fnBuffer.push_back(tricenter + face_nrm);
        }

        //else if (label == "vt")
        //{
        //    Vertex vert;
        //    inFile >> vert.uv.x >> vert.uv.y;

        //    vertexBuffer.push_back(vert);
        //}

        //else if (label == "vn")
        //{
        //    Vertex vert;
        //    inFile >> vert.nrm.x >> vert.nrm.y >> vert.nrm.z;

        //    vertexBuffer.push_back(vert);
        //}

        else 
        {
            //Error print;
            getline(inFile, label);
        }
    }

    //vert_mapping();
    calc_vert_normal();

}

void Mesh::setup_mesh(GLSLShader& shdr)
{
    //shdr.Use();

    ///*  Lets use map */
    ////modelLoc = glGetUniformLocation(renderProg.GetHandle(), "model");
    ////viewLoc = glGetUniformLocation(renderProg.GetHandle(), "view");
    ////colorLoc = glGetUniformLocation(renderProg.GetHandle(), "color");
    ////projectionLoc = glGetUniformLocation(renderProg.GetHandle(), "projection");
    ////LightLoc = glGetUniformLocation(renderProg.GetHandle(), "lightPos");
    ////ViewPosLoc = glGetUniformLocation(renderProg.GetHandle(), "viewPos");

    ////SendVertexData();

    ///*  Bind framebuffer to 0 to render to the screen (by default already 0) */
    ////glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ///*  Initially drawing using filled mode */
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    ///*  Hidden surface removal */
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    //glEnable(GL_CULL_FACE);     /*  For efficiency, not drawing back-face */

    //shdr.UnUse();
}

void Mesh::cleanup()
{
    //glInvalidateBufferData(VAO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(7, VBO);
    glDeleteBuffers(4, EBO);
    //glDeleteTextures
    //glDeleteBuffers(1, &UBO);
}

bool Mesh::DegenerateTri(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
    return (glm::distance(v0, v1) < EPSILON ||
        glm::distance(v1, v2) < EPSILON ||
        glm::distance(v2, v0) < EPSILON);
}

void Mesh::calc_BufferDatas()
{
    //calc vnBuffer
	for (Vertex& v : vertexBuffer)
	{
		vnBuffer.push_back(v.pos);
		Vec4 temp = Vec4(glm::normalize(v.nrm), 1.f);

		vnBuffer.push_back(v.pos + Vec3(temp)*500.f);
	}
 
    //calc fnBuffer
    int size = static_cast<int>(fnBuffer.size());
    for (int i{ 1 }; i < size; i+=2)
    {
        Vec4 temp = Vec4(fnBuffer[i] - fnBuffer[i - 1], 1.f);
        fnBuffer[i] = fnBuffer[i - 1] + Vec3(temp)*500.f;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Mesh* CreateSphere(int stacks, int slices)
{
    Mesh* mesh = new Mesh();

    for (int i = 0; i <= stacks; i++)
    {
        float row = static_cast<float>(i) / stacks;
        float beta = glm::pi<float>() * (row - 0.5f);
        for (int j = 0; j <= slices; j++)
        {
            Vertex vertex;
            float col = static_cast<float>(j) / slices;
            vertex.uv.x = -col;
            vertex.uv.y = -row;

            float alpha = col * glm::pi<float>() * 2.0f;
            vertex.pos.x = sin(alpha) * cos(beta);
            vertex.pos.y = sin(beta);
            vertex.pos.z = cos(alpha) * cos(beta);

            vertex.nrm.x = vertex.pos.x;
            vertex.nrm.y = vertex.pos.y;
            vertex.nrm.z = vertex.pos.z;
            //vertex.nrm /= radius;

            mesh->vertexBuffer.push_back(vertex);
        }
    }
    //mesh->BuildIndexBuffer(stacks, slices);
    mesh->calc_BufferDatas();
    mesh->SendVertexData();

    return mesh;
}

Mesh* CreatePlane(int stacks, int slices)
{
    Mesh* mesh = new Mesh();

    for (int i = 0; i <= stacks; ++i)
    {
        float row = static_cast<float>(i) / stacks;

        for (int j = 0; j <= slices; ++j)
        {
            float col = static_cast<float>(j) / slices;

            Vertex vertex;

            vertex.pos = Vec3(col - 0.5f, 0.5f - row, 0.0f);

            vertex.nrm = Vec3(vertex.pos.x, vertex.pos.y, -1.0f);

            vertex.uv = glm::vec2(col, row);

            mesh->vertexBuffer.push_back(vertex);
        }
    }

    //mesh->BuildIndexBuffer(stacks, slices);
    mesh->calc_BufferDatas();
    mesh->SendVertexData();

    return mesh;
}

Mesh* CreateOrbit(float radius, GLuint vert)
{
    Mesh* mesh = new Mesh();
    float angle = 360.f / vert;

    mesh->lineposBuffer.push_back(glm::vec3(radius * glm::cos(0.f), 0.f, radius * glm::sin(0.f)));

    for (GLuint i{ 1 }; i < vert; i++)
    {
        glm::vec3 pos = glm::vec3(radius * glm::cos(glm::radians(angle * i)), 0.f, radius * glm::sin(glm::radians(angle * i)));
        mesh->lineposBuffer.push_back(pos);
        mesh->lineposBuffer.push_back(pos);
    }

    mesh->lineposBuffer.push_back(mesh->lineposBuffer[0]);
    mesh->LineVertexData();

    return mesh;
}

Mesh* CreateQuad()
{
    Mesh* mesh = new Mesh();

    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, mesh->VBO);
    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glGenBuffers(1, &mesh->UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, mesh->UBO);
    glBufferData(GL_UNIFORM_BUFFER, 16 * 112, NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, mesh->UBO, 0, 16 * 112);

    return mesh;
}

void SPHERE::CreateSphere(int stacks, int slices)
{
    for (int i = 0; i <= stacks; i++)
    {
        float row = static_cast<float>(i) / stacks;
        float beta = glm::pi<float>() * (row - 0.5f);
        for (int j = 0; j <= slices; j++)
        {
            Vertex vertex;
            float col = static_cast<float>(j) / slices;
            vertex.uv.x = -col;
            vertex.uv.y = -row;

            float alpha = col * glm::pi<float>() * 2.0f;
            vertex.pos.x = this->center.x + this->radius * sin(alpha) * cos(beta);
            vertex.pos.y = this->center.y + this->radius * sin(beta);
            vertex.pos.z = this->center.z + this->radius * cos(alpha) * cos(beta);

            vertex.nrm.x = vertex.pos.x;
            vertex.nrm.y = vertex.pos.y;
            vertex.nrm.z = vertex.pos.z;
            //vertex.nrm /= radius;

            this->vbuffer.push_back(vertex);
        }
    }
    BuildIndexBuffer(stacks, slices);
}

void SPHERE::BuildIndexBuffer(int stacks, int slices)
{
    int p0 = 0, p1 = 0, p2 = 0;
    int p3 = 0, p4 = 0, p5 = 0;
    int stride = slices + 1;

    for (int i = 0; i < stacks; ++i)
    {
        int curr_row = i * stride;

        for (int j = 0; j < slices; ++j)
        {
            /*  You need to compute the indices for the first triangle here */
            /*  ... */
            p0 = curr_row + j;
            p1 = p0 + 1;
            p2 = p1 + stride;

            /*  Ignore degenerate triangle */
            if (!DegenerateTri(this->vbuffer[p0].pos, this->vbuffer[p1].pos, this->vbuffer[p2].pos))
            {
                /*  Add the indices for the first triangle */
                this->ibuffer.push_back(p0);
                this->ibuffer.push_back(p1);
                this->ibuffer.push_back(p2);

                glm::vec3 P = this->vbuffer[p0].pos;
                glm::vec3 Q = this->vbuffer[p1].pos;
                glm::vec3 R = this->vbuffer[p2].pos;
            }

            /*  You need to compute the indices for the second triangle here */
            /*  ... */
            p3 = p2;
            p4 = p3 - 1;
            p5 = p0;

            /*  Ignore degenerate triangle */
            if (!DegenerateTri(this->vbuffer[p3].pos, this->vbuffer[p4].pos, this->vbuffer[p5].pos))
            {
                /*  Add the indices for the second triangle */
                this->ibuffer.push_back(p3);
                this->ibuffer.push_back(p4);
                this->ibuffer.push_back(p5);
            }
        }
    }
}

bool SPHERE::DegenerateTri(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
    return (glm::distance(v0, v1) < 0.00001f ||
        glm::distance(v1, v2) < 0.00001f ||
        glm::distance(v2, v0) < 0.00001f);
}

void AABB::CreateAABB()
{
    lines.push_back(min);  //min,min,min
    lines.push_back(glm::vec3(min.x, min.y, max.z));  //min,min,max

    lines.push_back(min);  //min,min,min
    lines.push_back(glm::vec3(max.x, min.y, min.z));  //max,min,min

    lines.push_back(glm::vec3(max.x, min.y, min.z));  //max,min,min
    lines.push_back(glm::vec3(max.x, min.y, max.z));  //max,min,max

    lines.push_back(glm::vec3(max.x, min.y, min.z));  //max,min,min
    lines.push_back(glm::vec3(max.x, max.y, min.z));  //max,max,min

    lines.push_back(glm::vec3(max.x, max.y, min.z));  //max,max,min
    lines.push_back(max);  //max,max,max

    lines.push_back(glm::vec3(max.x, max.y, min.z));  //max,max,min
    lines.push_back(glm::vec3(min.x, max.y, min.z));  //min,max,min

    lines.push_back(glm::vec3(min.x, max.y, min.z));  //min,max,min
    lines.push_back(glm::vec3(min.x, max.y, max.z));  //min,max,max

    lines.push_back(glm::vec3(min.x, max.y, min.z));  //min,max,min
    lines.push_back(min);  //min,min,min
  
    lines.push_back(max);  //max,max,max
    lines.push_back(glm::vec3(max.x, min.y, max.z));  //max,min,max

    lines.push_back(glm::vec3(max.x, min.y, max.z));  //max,min,max
    lines.push_back(glm::vec3(min.x, min.y, max.z));  //min,min,max

    lines.push_back(glm::vec3(min.x, min.y, max.z));  //min,min,max
    lines.push_back(glm::vec3(min.x, max.y, max.z));  //min,max,max

    lines.push_back(glm::vec3(min.x, max.y, max.z));  //min,max,max
    lines.push_back(max);  //max,max,max
}

void TreeNode::Gen_VAO()
{
    glGenVertexArrays(1, &TREE_VAO);
    glBindVertexArray(TREE_VAO);

    glGenBuffers(1, &TREE_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, TREE_VBO);

    glBufferData(GL_ARRAY_BUFFER, (bv)->lines.size() * sizeof(glm::vec3), &(bv)->lines[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));
}

void S_TreeNode::Gen_VAO()
{
    glGenVertexArrays(1, &TREE_VAO);
    glBindVertexArray(TREE_VAO);

    glGenBuffers(1, &TREE_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, TREE_VBO);

    glBufferData(GL_ARRAY_BUFFER, (bv)->vbuffer.size() * sizeof(Vertex), &(bv)->vbuffer[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glGenBuffers(1, &TREE_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TREE_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bv->ibuffer.size() * sizeof(int), &bv->ibuffer[0], GL_STATIC_DRAW);
}

void OctreeNode::Gen_VAO()
{
    glGenVertexArrays(1, &TREE_VAO);
    glBindVertexArray(TREE_VAO);

    glGenBuffers(1, &TREE_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, TREE_VBO);

    glBufferData(GL_ARRAY_BUFFER, (aabb)->lines.size() * sizeof(glm::vec3), &(aabb)->lines[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));
}
