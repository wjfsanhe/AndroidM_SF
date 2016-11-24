#include "RectangleModel.h"
namespace Baofeng
{
	namespace Mojing
	{
		RectangleModel::RectangleModel(int textureType) : Model(textureType),
			m_v3TopLeft(-1 , 1 , -1),
			m_v3TopRight(1, 1, -1),
			m_v3BottomLeft(-1, -1, -1),
			m_v3BottomRight(1, -1, -1)
		{
			SetClassName(__FUNCTION__);
		}
		RectangleModel::~RectangleModel()
		{

		}

		void RectangleModel::Init()
		{
			prepareData();
			Setup();
		}

		void RectangleModel::prepareData()
		{
			data.vertexShader =
				"#version 300 es\n"

				"uniform mat4 modelViewProjection;\n"
				"uniform mat4 model;\n"

				"in vec3 vertexPosition;\n"
				"in vec3 vertexNormal;\n"
				"in vec2 vertexUV;\n"
				"out vec2 textureCoord;\n"

				"void main()\n"
				"{\n"
				"   gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);\n"
				"   textureCoord = vertexUV;\n"
				"}\n";

			data.fragmentShader = 
				"#version 300 es\n"
				"precision mediump float;\n"
				"uniform sampler2D sTexture;\n"
				"uniform int layerIndex;\n"
				"in vec2 textureCoord;\n"
				"out vec4 finalColor;\n"
				"void main()\n"
				"{\n"
				"   finalColor = texture(sTexture, textureCoord);\n"
				"}\n";

			float fDefaultV[] = {
				//m_UpLeftX, m_UpLeftY, m_UpLeftZ,
				//m_DownLeftx, m_DownLeftY, m_DownLeftZ,
				//m_UpRightX, m_UpRightY, m_UpRightZ,
				//m_DownRightX, m_DownRightY, m_DownRightZ
				-1.0f, 1.0f, -10.0f, // 左前上 - 16
				+1.0f, 1.0f, -10.0f, // 右前上 - 17
				+1.0f, -1.0f, -10.0f, // 右前下 - 18
				-1.0f, -1.0f, -10.0f, // 左前下 - 19
			};
			for (int iIndex = 0 ; iIndex < sizeof(fDefaultV) / sizeof(float) ; iIndex++)
				data.vertices.push_back(fDefaultV[iIndex]);
			
			
			GLushort fDefaultIndex[] = {
				0, 2, 3,
				0, 1, 2
			};
			for (int iIndex = 0 ; iIndex < sizeof(fDefaultIndex) / sizeof(GLushort) ; iIndex++)
				data.indices.push_back(fDefaultIndex[iIndex]);
			
			
			float fDefaultUV[] = {
				0.0f, 1.0f,
				1.0f, 1.0f,
				1.0f, 0.0f,
				0.0f, 0.0f
			};
			
			for (int iIndex = 0 ; iIndex < sizeof(fDefaultUV) / sizeof(float) ; iIndex++)
				data.uvs.push_back(fDefaultUV[iIndex]);
		}
	}
}
