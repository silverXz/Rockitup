#pragma once

#include <string>
#include <vector>

enum ShaderType {
	Vertex = 0,
	Geometry,
	Fragment
};
struct Shader {
	ShaderType mType;
	std::string mSource;
};

class ShaderFileParser
{
public:
	std::vector<char> mData;

	std::string srcStr;

	std::vector<Shader> mShaders;

	void LoadShaderFile(std::string filePath);

	void Parse();
};