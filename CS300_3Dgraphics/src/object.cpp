/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: object.cpp
Purpose: 
    This file contains definitions of member functions of class Object.
Language: C++ 17
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_1
Author: Sunwoo Lee / sunwoo.lee / 0055087
Creation date: 09.17.2022
--------------------------------------------------------*/
#include <object.h>

Object::Object(Vec3 pos, Vec3 s, Vec3 rot, Vec4 col) :position(pos), scale(s), rotation(rot), selfColor(col), mesh(nullptr)
{}

void Object::init(Mesh* m, shdr_vec& shdr_files)
{
	mesh = m;

    shdr_pgm.CompileLinkValidate(shdr_files);
    if (GL_FALSE == shdr_pgm.IsLinked()) {
        std::cout << "Unable to compile/link/validate shader programs" << "\n";
        std::cout << shdr_pgm.GetLog() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    shdr_handle = shdr_pgm.GetHandle();

    mesh->setup_mesh(shdr_pgm);
}

void Object::update(double /*dt*/, Mat4& view, Mat4& projection, Vec3& lightpos)
{
    shdr_pgm.Use();

    //mesh->UpdateVertexData();

    if (is_updated == true)
    {
        model = glm::translate(glm::mat4(1.f), position);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
    }
    
    glUniformMatrix4fv(glGetUniformLocation(shdr_handle, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shdr_handle, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shdr_handle, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shdr_handle, "mapping"), 1, GL_FALSE, glm::value_ptr(mesh->Get_mapping()));

    glUniform3fv(glGetUniformLocation(shdr_handle, "light_pos"), 1, glm::value_ptr(lightpos));
    glUniform4fv(glGetUniformLocation(shdr_handle, "color"), 1, glm::value_ptr(selfColor));

    shdr_pgm.UnUse();
}


void Object::draw(bool show_fnormal, bool show_vnormal)
{
    shdr_pgm.Use();

    glBindVertexArray(mesh->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->indexBuffer.size()), GL_UNSIGNED_INT, nullptr);

    if (show_fnormal == true)
    {
        // enable face normal
        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO[1]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)(0));
        glUniform4fv(glGetUniformLocation(shdr_handle, "color"), 1, glm::value_ptr(glm::vec4(0.2f, 0.5f, 0.2f, -1.f)));   // green for face normal
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(mesh->fnBuffer.size()));
    }

    if (show_vnormal == true)
    {
        // enable vertex normal
        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO[2]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)(0));
        glUniform4fv(glGetUniformLocation(shdr_handle, "color"), 1, glm::value_ptr(glm::vec4(0.4f, 0.3f, 0.6f, -1.f)));  // blue for vertex normal
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(mesh->vnBuffer.size()));
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    shdr_pgm.UnUse();
}

void Object::draw_orbit()
{
    shdr_pgm.Use();

    glBindVertexArray(mesh->VAO);

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(mesh->lineposBuffer.size()));

    glBindVertexArray(0);

    shdr_pgm.UnUse();
}

void Object::cleanup()
{
    shdr_pgm.DeleteShaderProgram();
}
