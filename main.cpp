#include "converter.h"
#include <iostream>

using namespace std;

std::string inputParam = "-i";
std::string outputParam = "-o";

void checkInputParams(std::map<std::string, std::string>& params)
{
    bool isParamPresent = true;

    std::stringstream errMsg;
    if(params.find(inputParam) == params.end())
    {
        errMsg << inputParam << " ";
        isParamPresent = false;
    }

    if(params.find(outputParam) == params.end())
    {
        isParamPresent = false;
        errMsg << outputParam;
    }
    if(!isParamPresent)
    {
        std::cout << "Usage example: -i input_file_name.obj -o output_file_name.stl" << std::endl;
        std::cout << "Missing parameters: " << errMsg.str()<<endl;
        exit(-1);
    }
}

int main(int argc, char* argv[])
{
    std::map<std::string, std::string> params;
    for(int i = 0; i < argc - 1; i++)
    {
        if( std::strcmp(argv[i], inputParam.c_str()) == 0)
        {
            params[inputParam] = argv[i + 1];
        }
        if( std::strcmp(argv[i], outputParam.c_str()) == 0)
        {
            params[outputParam] = argv[i + 1];
        }
    }

    checkInputParams(params);

    Converter converter = ConverterFactory::createConverter(params[inputParam], params[outputParam]);

    std::cout << " Loading... " << std::endl;

    MeshFrom* meshFrom = converter.m_Reader();

    std::cout << params[inputParam] << "file is loaded" << std::endl;

    MeshTo* meshTo = converter.m_Converter(meshFrom);

    std::cout << "Converted" << std::endl;

    converter.m_Writer(meshTo);

    return 0;
}
