#ifndef OBJ_READER_H
#define OBJ_READER_H

#include "common.h"
#include <fstream>
#include <endian.h>
#include <iostream>

struct FaceIndexes
{
    FaceIndexes(std::vector<int>& verticesIndexes, std::vector<int>& verticesNormalIndexes, std::vector<int>& textureVerticesIndexes):
        m_VerticesIndexes(verticesIndexes), m_VerticesNormaIndexes(verticesNormalIndexes), m_TextureVerticesIndexes(textureVerticesIndexes)
    {
    }
    std::vector<int> m_VerticesIndexes;
    std::vector<int> m_VerticesNormaIndexes;
    std::vector<int> m_TextureVerticesIndexes;
};

struct FaceVertices
{
    FaceVertices(std::vector<Vertex>& vertices, std::vector<VertexNormal>& verticesNormal, std::vector<TextureVertex>& textureVertices):
        m_Vertices(vertices), m_VerticesNormal(verticesNormal), m_TextureVertices(textureVertices)
    {
    }

    std::vector<Vertex>& m_Vertices;
    std::vector<VertexNormal>& m_VerticesNormal;
    std::vector<TextureVertex>& m_TextureVertices;

};

//vt, vn are optional
struct Face
{
    Face(FaceIndexes faceIndexes, FaceVertices faceVertices):
        m_FaceIndexes(faceIndexes), m_FaceVertices(faceVertices)
    {
        if(!isConsistent())
        {
            size_t verticesNum = m_FaceIndexes.m_VerticesIndexes.size();
            size_t verticesNormalNum = m_FaceIndexes.m_VerticesNormaIndexes.size();
            size_t textureVerticesNum = m_FaceIndexes.m_TextureVerticesIndexes.size();
            std::cout << verticesNum << " " << verticesNormalNum << " " << textureVerticesNum << std::endl;
            throw std::runtime_error("The given Face is not consistent");
        }
    }

    std::vector<Vertex*> getVertices()
    {
        std::vector<Vertex*> vertices;
        for(auto& it : m_FaceIndexes.m_VerticesIndexes)
        {
            vertices.emplace_back(getVertex(it));
        }
        return vertices;
    }

    bool hasNormal() const
    {
        return m_FaceIndexes.m_VerticesNormaIndexes.size() == 0 ? false : true;
    }

    VertexNormal* getVerticesNormal()
    {
        if(!hasNormal())
        {
            throw std::runtime_error("This face has no vertex normal");
        }
        return &m_FaceVertices.m_VerticesNormal[m_FaceIndexes.m_VerticesNormaIndexes[0] - 1]; // they must be same
    }

    size_t vertexNumber()
    {
        return m_FaceIndexes.m_VerticesIndexes.size();
    }

    FaceIndexes m_FaceIndexes;
    FaceVertices m_FaceVertices;
private:
    Vertex* getVertex(size_t index)
    {
        return &m_FaceVertices.m_Vertices[index - 1];
    }

    bool isConsistent()
    {
        size_t verticesNum = m_FaceIndexes.m_VerticesIndexes.size();
        size_t verticesNormalNum = m_FaceIndexes.m_VerticesNormaIndexes.size();
        size_t textureVerticesNum = m_FaceIndexes.m_TextureVerticesIndexes.size();

        if(textureVerticesNum == 0)
        {
            if(verticesNormalNum == 0)
            {
                return verticesNum > 2;
            }
            else
            {
                return (verticesNum > 2) && (verticesNum == verticesNormalNum);
            }
        }
        else
        {
            return (verticesNum > 2) && (verticesNum == verticesNormalNum) && (verticesNormalNum == textureVerticesNum);
        }
    }
};

struct Faces: MeshFrom
{
    std::vector<Face>* m_Faces;
};

class OBJReader
{
private:
    static std::vector<Vertex> m_Vertesies;
    static std::vector<VertexNormal> m_VertesiesNormal;
    static std::vector<TextureVertex> m_TextureVertesies;
    static std::vector<int> m_VerticesIndexes;
    static std::vector<int> m_VerticesNormaIndexes;
    static std::vector<int> m_TextureVerticesIndexes;
    static std::vector<Face> m_Faces;
    static Faces m_MeshFaces;
public:
    static MeshFrom* read(const std::string& objFileName)
    {
        std::ifstream objFile;

        objFile.open(objFileName);
        if(!objFile.is_open())
        {
            throw std::runtime_error("Cannot open file");
        }
        FaceVertices faceVertices(m_Vertesies, m_VertesiesNormal, m_TextureVertesies);
        std::string line;
        while(std::getline(objFile, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> result{std::istream_iterator<std::string>(iss), {}};

            if(result.empty())
            {
                continue;
            }

            if(!result[0].compare("v"))
            {
                float x, y, z, w;
                if( !(result.size() == 4 || result.size() == 5))
                {
                    throw std::runtime_error("Not appropiate vertex statement");
                }

                if(result.size() >= 4)
                {
                    x = std::stof(result[1]);
                    y = std::stof(result[2]);
                    z = std::stof(result[3]);
                }

                if(result.size() == 5)
                {
                    w = std::stof(result[4]);
                    m_Vertesies.emplace_back(x, y, z, w);
                }
                else
                {
                    m_Vertesies.emplace_back(x, y, z);
                }

            }
            else if (!result[0].compare("vn"))
            {
                float i, j, k;
                if (result.size() != 4)
                {
                    throw std::runtime_error("Not appropiate vertex normal statement");
                }
                i = std::stof(result[1]);
                j = std::stof(result[2]);
                k = std::stof(result[3]);
                m_VertesiesNormal.emplace_back(i, j, k);


            }
            else if (!result[0].compare("vt"))
            {
                float u, v, w;
                if (!(result.size() == 2 || result.size() == 3 || result.size() == 4))
                {
                    throw std::runtime_error("Not appropiate texture vertex statement");
                }
                if (result.size() == 2)
                {
                    u = std::stof(result[1]);
                    m_TextureVertesies.emplace_back(u);
                }
                if (result.size() == 3)
                {
                    u = std::stof(result[1]);
                    v = std::stof(result[2]);
                    m_TextureVertesies.emplace_back(u, v);
                }
                if (result.size() == 4)
                {
                    u = std::stof(result[1]);
                    v = std::stof(result[2]);
                    w = std::stof(result[3]);
                    m_TextureVertesies.emplace_back(u, v, w);
                }
            }
            else if (!result[0].compare("f"))
            {
                if (result.size() < 4)
                {
                    throw std::runtime_error("Not appropiate face statement");
                }

                std::vector<int> vertesiesIndex;
                std::vector<int> vertesiesNormalIndex;
                std::vector<int> textureVerticesIndexes;

                for (auto it = ++result.begin(); it != result.end(); ++it)
                {
                    std::vector<std::string> faceParams = split(*it, "/");

                    if(faceParams.size() == 1) // its only containes vertex
                    {
                        vertesiesIndex.emplace_back(std::stof(faceParams[0]));
                    }
                    else
                    {
                        vertesiesIndex.emplace_back(std::stof(faceParams[0]));
                        if(!faceParams[1].empty())
                        {
                            textureVerticesIndexes.emplace_back(std::stof(faceParams[1]));
                        }
                        vertesiesNormalIndex.emplace_back(std::stof(faceParams[2]));
                    }
                }
                m_Faces.emplace_back(FaceIndexes(vertesiesIndex, vertesiesNormalIndex, textureVerticesIndexes), faceVertices);
            }
            else
            {
                std::stringstream errMsg;
                errMsg << "Unsopprted statement " << result[0] << "\n";
                //throw std::runtime_error(errMsg.str());
            }
            line.clear();
        }
        objFile.close();
        FaceIndexes faceIndexes(m_VerticesIndexes, m_VerticesNormaIndexes, m_TextureVerticesIndexes);
        m_MeshFaces.m_Faces = &m_Faces;
        return &m_MeshFaces;
    }
};

std::vector<Vertex> OBJReader::m_Vertesies = std::vector<Vertex>();
std::vector<VertexNormal> OBJReader::m_VertesiesNormal = std::vector<VertexNormal>();
std::vector<TextureVertex> OBJReader::m_TextureVertesies = std::vector<TextureVertex>();
std::vector<int> OBJReader::m_VerticesIndexes = std::vector<int>();
std::vector<int> OBJReader::m_VerticesNormaIndexes = std::vector<int>();
std::vector<int> OBJReader::m_TextureVerticesIndexes = std::vector<int>();
std::vector<Face> OBJReader::m_Faces = std::vector<Face>();
Faces OBJReader::m_MeshFaces = Faces();

#endif // OBJ_READER_H
