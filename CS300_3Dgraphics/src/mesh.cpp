/*-------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: mesh.cpp
Purpose: 
    This file contains definitions of member functions of class Mesh and Mesh creating functions.
Language: C++ 17
Platform: VS 19 / version 16.9.0 / Windows
Project: sunwoo.lee_CS300_1
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

void Mesh::vert_mapping(float(*xyz_minmax)[2])
{
    float W = xyz_minmax[0][1] - xyz_minmax[0][0];  //x width
    float H = xyz_minmax[1][1] - xyz_minmax[1][0];  //y height
    float D = xyz_minmax[2][1] - xyz_minmax[2][0];  //z depth

    glm::vec3 center{glm::vec3((xyz_minmax[0][0] + xyz_minmax[0][1]),(xyz_minmax[1][0] + xyz_minmax[1][1]),(xyz_minmax[2][0] + xyz_minmax[2][1]))};
    center *= 0.5f;

    glm::mat4 T = glm::mat4(1.0f);
    glm::mat4 S = glm::mat4(1.0f);

    float scale_factor = std::max({ W, H, D });

    OBJ_Translate = glm::translate(glm::mat4(1.0f), -center);
    OBJ_Scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.f / scale_factor, 2.f / scale_factor, 2.f / scale_factor));
    
    mapping = OBJ_Scale * OBJ_Translate;

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

void Mesh::SendVertexData()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(3, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    /*  Copy vertex attributes to GPU */
    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(Vertex), &vertexBuffer[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    /*  Copy vertex attributes to GPU */
    glBufferData(GL_ARRAY_BUFFER, fnBuffer.size() * sizeof(glm::vec3), &fnBuffer[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));


    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    /*  Copy vertex attributes to GPU */
    glBufferData(GL_ARRAY_BUFFER, vnBuffer.size() * sizeof(glm::vec3), &vnBuffer[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));


    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    /*  Copy vertex indices to GPU */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(int), &indexBuffer[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::UpdateVertexData()
{
    //glBindVertexArray(VAO);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    ///*  Copy vertex attributes to GPU */
    //glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(Vertex), &vertexBuffer[0], GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    ///*  Copy vertex indices to GPU */
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(int), &indexBuffer[0], GL_STATIC_DRAW);

    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));

    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, nrm)));

    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);
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
    float xyz_minmax[3][2];     //[0][0]->x min, [2][1]->z max
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
                        xyz_minmax[i][j] = vert.pos[i];
                    }
                }
                is_init = true;
            }
            else
            {
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

    vert_mapping(xyz_minmax);
    calc_vert_normal();

}

void Mesh::setup_mesh(GLSLShader& shdr)
{
    shdr.Use();

    /*  Lets use map */
    //modelLoc = glGetUniformLocation(renderProg.GetHandle(), "model");
    //viewLoc = glGetUniformLocation(renderProg.GetHandle(), "view");
    //colorLoc = glGetUniformLocation(renderProg.GetHandle(), "color");
    //projectionLoc = glGetUniformLocation(renderProg.GetHandle(), "projection");
    //LightLoc = glGetUniformLocation(renderProg.GetHandle(), "lightPos");
    //ViewPosLoc = glGetUniformLocation(renderProg.GetHandle(), "viewPos");

    //SendVertexData();

    /*  Bind framebuffer to 0 to render to the screen (by default already 0) */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*  Initially drawing using filled mode */
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /*  Hidden surface removal */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);     /*  For efficiency, not drawing back-face */

    shdr.UnUse();
}

void Mesh::cleanup()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(3, VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::BuildIndexBuffer(int stacks, int slices)
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
            if (!DegenerateTri(vertexBuffer[p0].pos,
                vertexBuffer[p1].pos,
                vertexBuffer[p2].pos))
            {
                /*  Add the indices for the first triangle */
                indexBuffer.push_back(p0);
                indexBuffer.push_back(p1);
                indexBuffer.push_back(p2);

                glm::vec3 P = vertexBuffer[p0].pos;
                glm::vec3 Q = vertexBuffer[p1].pos;
                glm::vec3 R = vertexBuffer[p2].pos;

                glm::vec3 face_nrm = glm::normalize(glm::cross(Q - P, R - P));
                faces.emplace(p0, std::vector<glm::vec3>());
                faces.emplace(p1, std::vector<glm::vec3>());
                faces.emplace(p2, std::vector<glm::vec3>());
                faces[p0].push_back(face_nrm);
                faces[p1].push_back(face_nrm);
                faces[p2].push_back(face_nrm);

                glm::vec3 tricenter = (P + Q + R) / 3.f;
                fnBuffer.push_back(tricenter);
                fnBuffer.push_back(tricenter + face_nrm);
            }

            /*  You need to compute the indices for the second triangle here */
            /*  ... */
            p3 = p2;
            p4 = p3 - 1;
            p5 = p0;

            /*  Ignore degenerate triangle */
            if (!DegenerateTri(vertexBuffer[p3].pos,
                vertexBuffer[p4].pos,
                vertexBuffer[p5].pos))
            {
                /*  Add the indices for the second triangle */
                indexBuffer.push_back(p3);
                indexBuffer.push_back(p4);
                indexBuffer.push_back(p5);

                glm::vec3 P = vertexBuffer[p3].pos;
                glm::vec3 Q = vertexBuffer[p4].pos;
                glm::vec3 R = vertexBuffer[p5].pos;

                glm::vec3 face_nrm = glm::normalize(glm::cross(Q - P, R - P));
                faces.emplace(p3, std::vector<glm::vec3>());
                faces.emplace(p4, std::vector<glm::vec3>());
                faces.emplace(p5, std::vector<glm::vec3>());
                faces[p3].push_back(face_nrm);
                faces[p4].push_back(face_nrm);
                faces[p5].push_back(face_nrm);

                glm::vec3 tricenter = (P + Q + R) / 3.f;
                fnBuffer.push_back(tricenter);
                fnBuffer.push_back(tricenter + face_nrm);
            }
        }
    }
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
        Vec4 temp = Vec4(glm::normalize(v.nrm), 1.f) / OBJ_Scale;

        vnBuffer.push_back(v.pos + Vec3(temp) * LINE_SCALE);
    }

    //calc fnBuffer
    int size = static_cast<int>(fnBuffer.size());
    for (int i{ 1 }; i < size; i+=2)
    {
        Vec4 temp = Vec4(fnBuffer[i] - fnBuffer[i - 1], 1.f);
        fnBuffer[i] = fnBuffer[i - 1] + Vec3(temp / OBJ_Scale) * LINE_SCALE;
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
    mesh->BuildIndexBuffer(stacks, slices);
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