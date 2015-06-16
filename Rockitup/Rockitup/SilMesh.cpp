#include "SilMesh.h"
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Clock.h"

namespace iv
{

	SilMesh::SilMesh() : mSubMeshCnt(0)
	{
		m_hVAO = 0;
		for (int i = 0; i < 3; ++i)
			m_hVBO[i] = 0;
	}

	SilMesh::~SilMesh()
	{
		if (m_hVAO > 0)
			glDeleteVertexArrays(1, &m_hVAO);
		for (int i = 0; i < 3; ++i)
		{
			if (m_hVBO[i] > 0)
				glDeleteBuffers(1, &m_hVBO[i]);
		}
		auto deleteTexture = [](Texture& t) {
			t.DeleteTexture();
		};
		std::for_each(mTextures.begin(), mTextures.end(), deleteTexture);
	}

	bool SilMesh::LoadMesh(const char* filePath)
	{
		Assimp::Importer importer;

		double t0 = iv::Clock::GetCurrentTimeUS();

		printf("Loading Meshes......");

		const aiScene* scene = importer.ReadFile(filePath,
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		if (!scene)
		{
			printf("Load Mesh Failed!\n");
			return false;
		}

		double t1 = iv::Clock::GetCurrentTimeUS();

		//printf("Reading File Time Cost:%fms\n", (float)(t1 - t0));

		// positon, normal , TexCoord
		const int iVertexAttributeTotalSize = sizeof(aiVector3D) * 2 + sizeof(aiVector2D);

		int iTotalVertices = 0;

		mSubMeshCnt = scene->mNumMeshes;

		//printf("There is total %d meshes!\n", mSubMeshCnt);

		int vertexTotalCount = 0;

		// FOR EACH MESH
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[i];
			int iMeshFaces = mesh->mNumFaces;

			MeshEntity entity(vertexTotalCount, 3 * iMeshFaces);
			entity.mMaterialIndex = mesh->mMaterialIndex;

			// FOR EACH FACE
			for (int j = 0; j < iMeshFaces; ++j)
			{
				const aiFace& face = mesh->mFaces[j];

				// FOR EACH VERTEX
				for (int k = 0; k < 3; ++k)
				{
					aiVector3D pos = mesh->mVertices[face.mIndices[k]];
					m_posArray.push_back(vec3(pos.x,pos.y,pos.z));

					aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
					m_texArray.push_back(vec2(uv.x, uv.y));
					
					aiVector3D tangent = mesh->mTangents[k];
					mTangentArray.push_back(vec3(tangent.x, tangent.y, tangent.z));
					if (tangent.x == tangent.x)
						entity.mbHasTextureCoordinates = true;

					aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
					m_normArray.push_back(vec3(normal.x,normal.y,normal.z));

					m_indicesArray.push_back(face.mIndices[k]);
				}
			}
			vertexTotalCount += 3 * iMeshFaces;
			mMeshEntities.push_back(entity);
		}


		printf("Done\n");

		printf("Loading Textures....");

		std::string xf(filePath);
		int fuck = xf.find_last_of("\\");
		std::string dir = xf.substr(0, fuck + 1);

		for (int i = 0; i < mMeshEntities.size(); ++i)
		{
			const aiMaterial* material = scene->mMaterials[mMeshEntities[i].mMaterialIndex];
			aiString filePath;

			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &filePath) == AI_SUCCESS)
			{
				std::string texName = filePath.data;
				std::string fullPath = dir + texName;
				int texFound = -1;
				for (int j = 0; j < mTextures.size(); ++j)
				{
					if (fullPath == mTextures[j].GetPath())
					{
						texFound = j;
						break;
					}
				}
				
				// new texture
				if (texFound == -1)
				{
					Texture t;
					t.CreateFromFile(fullPath.c_str(), true);
					t.SetSamplerParameters(GL_TEXTURE_WRAP_S, GL_REPEAT);
					t.SetSamplerParameters(GL_TEXTURE_WRAP_T, GL_REPEAT);
					t.SetSamplerParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					t.SetSamplerParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					mMeshEntities[i].mDiffuseTexIndex = mTextures.size();
					mTextures.push_back(t);
				}
				else
					mMeshEntities[i].mDiffuseTexIndex = texFound;
			}

			if (material->GetTexture(aiTextureType_HEIGHT, 0, &filePath) == AI_SUCCESS)
			{
				std::string texName = filePath.data;
				std::string fullPath = dir + texName;
				int texFound = -1;
				for (int j = 0; j < mTextures.size(); ++j)
				{
					if (fullPath == mTextures[j].GetPath())
					{
						texFound = j;
						break;
					}
				}
				if (texFound == -1)
				{
					Texture t;
					t.CreateFromFile(fullPath.c_str(), true);
					t.SetSamplerParameters(GL_TEXTURE_WRAP_S, GL_REPEAT);
					t.SetSamplerParameters(GL_TEXTURE_WRAP_T, GL_REPEAT);
					t.SetSamplerParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					t.SetSamplerParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					mMeshEntities[i].mHeightTexIndex = mTextures.size();
					mTextures.push_back(t);
				}
				else
					mMeshEntities[i].mHeightTexIndex = texFound;
				mMeshEntities[i].mbHasBumpMap = true;
			}
		}
		
		int meshThatHasUsefullTextureCoordinates = 0;
		for (int i = 0; i < mMeshEntities.size(); ++i)
			if (mMeshEntities[i].mbHasTextureCoordinates)
				meshThatHasUsefullTextureCoordinates++;
		//printf("Mesh Has Useful TexCoords : %d.\n", meshThatHasUsefullTextureCoordinates);

		printf("Done.\n");

		if (m_hVAO <= 0)
			glGenVertexArrays(1, &m_hVAO);
		if (m_hVBO[0] <= 0)
			glGenBuffers(3, m_hVBO);

		glBindVertexArray(m_hVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_hVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, m_posArray.size() * sizeof(vec3), &m_posArray[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_hVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, m_normArray.size() * sizeof(vec3), &m_normArray[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_hVBO[2]);
		glBufferData(GL_ARRAY_BUFFER, m_texArray.size()* sizeof(vec2), &m_texArray[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);


		glBindVertexArray(0);

		return true;
	}

	void SilMesh::Render()
	{
		glBindVertexArray(m_hVAO);

		for (int i = 0; i < mMeshEntities.size(); ++i)
		{
			int iMatIndex = mMeshEntities[i].mDiffuseTexIndex;
			if (iMatIndex >= 0)
				mTextures[iMatIndex].BindTexture();

			glDrawArrays(GL_TRIANGLES, mMeshEntities[i].mStartIndex, mMeshEntities[i].mVertexCount);
		}
	}

	void SilMesh::GenEdgeInfo(EdgeMap& edgeMap)
	{
		edgeMap.clear();

		size_t numVertex = m_posArray.size();
		if (numVertex == 0)
			return;

		size_t numFace = numVertex / 3;

		EdgeMap::iterator itr;

		for (size_t i = 0; i < numFace; i++)
		{
			int idBase = 3 * i;
		
			for (int j = 0; j < 3; ++j)
			{
				Edge edge(idBase + j % 3, idBase + (j + 1) % 3);
				
				itr = edgeMap.find(edge);
				
		
				// Already in the map
				if (itr != edgeMap.end())
				{
					FacePair& faces = itr->second;
					faces.insert(i);
				}
				else
				{
					FacePair faces;
					faces.insert(i);
					edgeMap[edge] = faces;
				}
			}
		}
	}

	std::string SilMesh::GetDirectory(const std::string& filePath)
	{
		int x = filePath.find_last_of("\\");
		return filePath.substr(x);
	}

}