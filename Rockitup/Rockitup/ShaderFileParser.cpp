#include "ShaderFileParser.h"

#include <assert.h>
#include <fstream>

void ShaderFileParser::LoadShaderFile(std::string filePath)
{
	mData.clear();
	std::ifstream in(filePath.c_str(),std::ios::ate);
	if (!in.is_open())
	{
		printf("Failed to open shader file: %s!\n", filePath.c_str());
		return;
	}

	int fileLength = in.tellg();
	mData.resize(fileLength);
	in.seekg(0, std::ios::beg);
	in.read(&mData[0], fileLength);
	in.close();

	srcStr = std::string(mData.begin(), mData.end());
}

void ShaderFileParser::Parse()
{
	std::string vs("--Vertex Shader--");
	std::string gs("--Geometry Shader--");
	std::string fs("--Fragment Shader--");
	// Find Vertex Shader
	int vStart = srcStr.find(vs);

	int gStart = srcStr.find(gs);
	int fStart = srcStr.find(fs);

	assert(vStart >= 0 && fStart > 0);

	auto loadShaderSource = [&](int beg, ShaderType st) 
	{
		if (beg < 0)
			return;
		int srcStart = beg;
		int srcLength = 0;
		switch (st)
		{
		case Vertex:
			srcStart += vs.length();
			srcLength = gStart > 0 ? gStart - srcStart : fStart - srcStart;
			break;
		case Geometry:
			srcStart += gs.length();
			srcLength = fStart - srcStart;
			break;
		case Fragment:
			srcStart += fs.length();
			srcLength = srcStr.length() - srcStart;
			break;
		default:
			break;
		}
		Shader s;
		s.mType = st;
		s.mSource = srcStr.substr(srcStart, srcLength);
		mShaders.push_back(s);
	};

	loadShaderSource(vStart, Vertex);
	loadShaderSource(gStart, Geometry);
	loadShaderSource(fStart, Fragment);
}