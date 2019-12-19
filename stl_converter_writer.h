#ifndef STL_CONVERTER_WRITER_H
#define STL_CONVERTER_WRITER_H

#include "common.h"
#include <vector>
#include <fstream>
#include <ostream>
#include <iostream>
#include <iostream>
#include <endian.h>
#include <ios>

struct Triangle
{
    Triangle(Vertex* A,Vertex* B, Vertex* C, VertexNormal* normal):
        m_A(A), m_B(B), m_C(C), m_Normal(*normal), m_Attribute_Byte_Count(0)
    {
    }

    Triangle(Vertex* A,Vertex* B, Vertex* C):
        m_A(A), m_B(B), m_C(C), m_Normal(getNormal(*A, *B, *C)), m_Attribute_Byte_Count(0)
    {
    }

    void static fillUpBufferWithVertexNormal(char buffer[], VertexNormal* vn, size_t& iter)
    {
        uint32_t i = 0;
        uint32_t j = 0;
        uint32_t k = 0;
        std::memcpy(&i, &(vn->m_I), sizeof i);
        std::memcpy(&j, &(vn->m_J), sizeof j);
        std::memcpy(&k, &(vn->m_K), sizeof k);

        i = htole32(i);
        j = htole32(j);
        k = htole32(k);

        std::memcpy(&buffer[iter], &i, sizeof i);
        iter += sizeof i;
        std::memcpy(&buffer[iter], &j, sizeof j);
        iter += sizeof j;
        std::memcpy(&buffer[iter], &k, sizeof k);
        iter += sizeof k;
    }

    void static fillUpBufferWithAttributeByte(char buffer[], uint16_t attribute, size_t& iter)
    {
        std::memcpy(&buffer[iter], &attribute, sizeof attribute);
        iter += sizeof attribute;
    }

    void static fillUpBufferWithVertex(char buffer[], Vertex* v, size_t& iter)
    {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t z = 0;
        std::memcpy(&x, &(v->m_X), sizeof x);
        std::memcpy(&y, &(v->m_Y), sizeof y);
        std::memcpy(&z, &(v->m_Z), sizeof z);

        x = htole32(x);
        y = htole32(y);
        z = htole32(z);

        std::memcpy(&buffer[iter], &x, sizeof x);
        iter += sizeof x;
        std::memcpy(&buffer[iter], &y, sizeof y);
        iter += sizeof y;
        std::memcpy(&buffer[iter], &z, sizeof z);
        iter += sizeof z;

    }

    void static fillUpBuffer(char buffer[], Triangle& triangle)
    {
        size_t iter = 0;
        fillUpBufferWithVertexNormal(buffer, &triangle.m_Normal, iter);
        fillUpBufferWithVertex(buffer, triangle.m_A, iter);
        fillUpBufferWithVertex(buffer, triangle.m_B, iter);
        fillUpBufferWithVertex(buffer, triangle.m_C, iter);
        fillUpBufferWithAttributeByte(buffer, triangle.m_Attribute_Byte_Count, iter);
    }

    VertexNormal m_Normal;
    Vertex* m_A;
    Vertex* m_B;
    Vertex* m_C;
    uint16_t m_Attribute_Byte_Count;
};

struct Triangles: MeshTo
{
    std::vector<Triangle>* m_Triangles;
};

std::ostream& operator<<(std::ostream& os, const Triangle& v)
{
    os << "Triangle: " << "\n" <<*v.m_A << '/' << *v.m_B << '/' << *v.m_C << "/" << v.m_Normal << std::endl;
    return os;
}

class STLWriter
{
public:
    static MeshTo* convert(MeshFrom* meshfrom)
    {
        Faces* faces = dynamic_cast<Faces*>(meshfrom);
        if(faces != nullptr)
        {
            for(auto& face : *(faces->m_Faces))
            {
                addTriangle(face);
            }
            m_MeshTriangles.m_Triangles = &m_Triangles;
            return &m_MeshTriangles;
        }
        else
        {
            throw std::runtime_error("Unsupported MeshFrom struct now only Faces is supported");
        }
    }

    static void write(const std::string& fileName, MeshTo* meshTo)
    {
        Triangles* triangles = dynamic_cast<Triangles*>(meshTo);
        if(triangles != nullptr)
        {
            std::ofstream stlFile(fileName, std::ios::out | std::ios::binary);
            size_t headerSize = 80;
            char header[headerSize]; //generally ignored
            for(int i = 0; i < 80; i++)
            {
                header[i] = 0;
            }
            char buffer[50]; // contains one triangle (vertexNormal 3*vertex + uint16_t)
            char numOfTrianglesBuf[4];
            uint32_t numOfTriangles = m_Triangles.size();

            numOfTriangles = htole32(numOfTriangles);
            std::memcpy(numOfTrianglesBuf, &numOfTriangles, sizeof numOfTriangles);
            stlFile.write(header, headerSize);
            stlFile.write(numOfTrianglesBuf, sizeof numOfTriangles);
            for(auto& triange : m_Triangles)
            {
                Triangle::fillUpBuffer(buffer, triange);
                stlFile.write(buffer, 50);
            }
            stlFile.close();
        }
        else
        {
            throw std::runtime_error("Unsupported MeshTo struct now only Triangles is supported");
        }
    }

    static void addTriangle(Face& face)
    {
        auto vertices = face.getVertices();
        if (face.hasNormal())
        {
            for(int i = 1; i < vertices.size() - 1; i++)
            {
                m_Triangles.emplace_back(vertices[0], vertices[i], vertices[i + 1], face.getVerticesNormal());
            }
        }
        else
        {
            for(int i = 1; i < vertices.size() - 1; i++)
            {
                m_Triangles.emplace_back(vertices[0], vertices[i], vertices[i + 1]);
            }
        }
    }
public:
    static std::vector<Triangle> m_Triangles;
    static Triangles m_MeshTriangles;
};

std::vector<Triangle> STLWriter::m_Triangles = std::vector<Triangle>();
Triangles STLWriter::m_MeshTriangles = Triangles();

#endif // STL_CONVERTER_WRITER_H
