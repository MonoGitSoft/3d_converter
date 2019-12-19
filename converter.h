#ifndef CONVERTER_H
#define CONVERTER_H

#include "common.h"
#include "obj_reader.h"
#include "stl_converter_writer.h"

class Converter
{
public:
   Converter(std::function<MeshFrom*()> reader, std::function<MeshTo*(MeshFrom*)> converter, std::function<void(MeshTo*)> writer):
       m_Reader(reader), m_Converter(converter), m_Writer(writer)
   {

   }

   std::function<MeshFrom*()> m_Reader;
   std::function<MeshTo*(MeshFrom*)> m_Converter;
   std::function<void(MeshTo*)> m_Writer;
};

class ConverterFactory
{
public:
    static Converter createConverter(const std::string& inputFileName, const std::string& outPutFileName)
    {
        std::string inputExtension = split(inputFileName, "\\.").back();
        std::string outputExtension = split(outPutFileName, "\\.").back();

        if(inputExtension.compare("obj") == 0 && outputExtension.compare("stl") == 0)
        {
            std::function<MeshFrom*()> reader = std::bind(&OBJReader::read, inputFileName);
            std::function<MeshTo*(MeshFrom*)> converter(&STLWriter::convert);
            std::function<void(MeshTo*)> writer = std::bind(&STLWriter::write, outPutFileName, std::placeholders::_1);
            return Converter(reader, converter, writer);
        }
    }
};

#endif // CONVERTER_H
