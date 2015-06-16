#pragma once

#include <glew.h>
#include <vector>
#include <map>
#include <set>
#include "SiMath.h"
#include "Texture.h"

namespace iv
{
	struct Edge
	{
		int mVertexID[2];
		int mFaceID[2];

		int mCursor;

		Edge(int v1, int v2)
		{
			if (v1 < v2)
			{
				mVertexID[0] = v1;
				mVertexID[1] = v2;
			}
			else
			{
				mVertexID[0] = v2;
				mVertexID[1] = v1;
			}

			mCursor = 0;

			mFaceID[0] = mFaceID[1] = -1;
		}

		void AddShareFace(int id)
		{
			assert(mCursor < 2);
			mFaceID[mCursor++] = id;
		}
	};

	struct EdgeCmp
	{
		bool operator() (const Edge& e1, const Edge& e2)
		{
			if (e1.mVertexID[0] < e2.mVertexID[0])
				return true;
			else if (e1.mVertexID[0] > e2.mVertexID[0])
				return false;
			else
				return e1.mVertexID[1] < e2.mVertexID[1];
		}
	};

	struct Face
	{
		int mVertexID[3];
		int mAdjFaceID[3];

		Face(int v1, int v2, int v3)
		{
			mVertexID[0] = v1;
			mVertexID[1] = v2;
			mVertexID[2] = v3;
			
			for (int i = 0; i < 3; i++)
				mAdjFaceID[i] = -1;
		}
	};

	struct MeshEntity
	{
	public:
		int mStartIndex;
		int mVertexCount;

		int mMaterialIndex;
		
		int mDiffuseTexIndex;
		int mHeightTexIndex;

		bool mbHasTextureCoordinates;
		bool mbHasBumpMap;
		
		MeshEntity(int start, int count) : mStartIndex(start),
			mVertexCount(count),
			mMaterialIndex(-1),
			mDiffuseTexIndex(-1),
			mHeightTexIndex(-1),
			mbHasTextureCoordinates(false),
			mbHasBumpMap(false)
		{

		}
	};


	class SilMesh
	{
	public:

		typedef std::set<int> FacePair;
		typedef std::map<Edge, FacePair, EdgeCmp> EdgeMap;

		SilMesh();
		~SilMesh();

		bool LoadMesh(const char* filePath);

		bool LoadMesh2(const char* filePath);


		void GenEdgeInfo(EdgeMap& edgeMap);

		void GenAdjInfo();

		void Render();

	private:

		std::string GetDirectory(const std::string& filePath);

		GLuint m_hVAO;
		GLuint m_hVBO[3];

		int					mSubMeshCnt;

		std::vector<MeshEntity>	mMeshEntities;

		std::vector<vec3>	mTangentArray;
		std::vector<vec3>	m_posArray;
		std::vector<vec3>	m_normArray;
		std::vector<vec2>	m_texArray;
		std::vector<int>	m_indicesArray;
		std::vector<MeshEntity>	mMeshEntity;
		std::vector<Texture>	mTextures;


	};
}