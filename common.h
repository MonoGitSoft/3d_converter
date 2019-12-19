#ifndef COMMON_H
#define COMMON_H

#include <ostream>
#include <string>
#include <istream>
#include <vector>
#include <math.h>
#include <iterator>
#include <regex>
#include <algorithm>
#include <functional>

struct MeshFrom
{
    virtual ~MeshFrom()
    {

    }
};

struct MeshTo
{
    virtual ~MeshTo()
    {

    }
};

struct VertexNormal
{
    VertexNormal(float i, float j, float k)
        :m_I(i), m_J(j), m_K(k)
    {
    }

    VertexNormal(const VertexNormal&) = default;

    VertexNormal(VertexNormal&&) noexcept = default;

    //VertexNormal(VertexNormal&&) = default;
    float m_I;
    float m_J;
    float m_K;

    bool operator == (const VertexNormal& other)
    {
        return (m_I == other.m_I && m_J == other.m_J && m_K == other.m_K);
    }
};

std::ostream& operator<<(std::ostream& os, const VertexNormal& v)
{
    os << "VertexNormal: " <<v.m_I << '/' << v.m_J << '/' << v.m_K << std::endl;
    return os;
}

struct Vertex
{
    Vertex() = delete;
    Vertex(float x, float y, float z, float w = 1.0):
        m_X(x), m_Y(y), m_Z(z), m_W(w)
    {
    }

    Vertex(const Vertex&) = default;

    Vertex(Vertex&&) = default;

    float m_X;
    float m_Y;
    float m_Z;
    float m_W;

};


std::ostream& operator<<(std::ostream& os, const Vertex& v)
{
    os << "Vertex: " <<v.m_X << '/' << v.m_Y << '/' << v.m_Z << std::endl;
    return os;
}

Vertex operator-(const Vertex& a, const Vertex& b)
{
    return Vertex(a.m_X - b.m_X, a.m_Y - b.m_Y, a.m_Z - b.m_Z);
}

VertexNormal getNormal(const Vertex& v_1, const Vertex& v_2, const Vertex& v_3)
{
    float i, j, k;
    Vertex a = v_2 - v_1;
    Vertex b = v_3 - v_1;
    i = a.m_Y*b.m_Z - a.m_Z*b.m_Y;
    j = -(a.m_X*b.m_Z - a.m_Z*b.m_X);
    k = a.m_X*b.m_Y - a.m_Y*b.m_X;

    if(i == 0 && j == 0 && k == 0)
    {
        throw std::runtime_error("Cannot calculate the norm vector of the given triangle");
    }

    float norm = std::sqrt(i*i + j*j + k*k);
    i /= norm;
    j /= norm;
    k /= norm;
    return VertexNormal(i,j,k);
}

struct TextureVertex
{
    TextureVertex(float u, float v = 0.0, float w = 0.0):
        m_U(u), m_V(v), m_W(w)
    {
    }

    TextureVertex(const TextureVertex&) = default;

    TextureVertex(TextureVertex&&) = default;
    float m_U;
    float m_V;
    float m_W;
};

std::vector<std::string> split(const std::string& vec, const std::string& delimiter)
{
    std::regex re(delimiter);
    std::sregex_token_iterator first{vec.begin(), vec.end(), re, -1}, last;
    std::vector<std::string> splitPath {first, last};
    return splitPath;
}

#endif // COMMON_H
