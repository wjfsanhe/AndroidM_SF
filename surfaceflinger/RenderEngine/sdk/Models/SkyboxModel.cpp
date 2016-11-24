#include "SkyboxModel.h"

namespace Baofeng
{
    namespace Mojing
    {
		SkyboxModel::SkyboxModel(int textureType) : Model(textureType)
		{
			SetClassName(__FUNCTION__);
		}
		void SkyboxModel::Init()
		{
			prepareData();
			Setup();
		}
		SkyboxModel::~SkyboxModel()
		{
		}
        void SkyboxModel::prepareData()
        {
			//if (HasEXT_multiview_ovr)
			if (0)
			{
				data.vertexShader =
					"#version 300 es\n"
					"#extension GL_OVR_multiview : enable\n"
					"layout(num_views = " LAYER_COUNT_STRING ") in;\n"

					"uniform mat4 modelViewProjection[" LAYER_COUNT_STRING "];\n"
					"uniform mat4 model;\n"

					"in vec3 vertexPosition;\n"
					"in vec3 vertexNormal;\n"
					"in vec2 vertexUV;\n"
					"out vec2 textureCoord;\n"

					"void main()\n"
					"{\n"
					"   gl_Position = modelViewProjection[gl_ViewID_OVR] * vec4(vertexPosition, 1.0);\n"
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
			}
			else
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

				if (_textureType == 0)
				{
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
				}
				else // movie texture
				{
					data.fragmentShader =
						"#version 300 es\n"
						"#extension GL_OES_EGL_image_external : require\n"
						"precision mediump float;\n"
						"uniform samplerExternalOES sTexture;" //
						"uniform int layerIndex;\n"
						"in vec2 textureCoord;\n"
						"out vec4 finalColor;\n"
						"void main()\n"
						"{\n"
						"   finalColor = texture(sTexture, textureCoord);\n"
						"}\n";
				}
				
				
			}

			float fDefaultV[] = {
                /*Right*/
                +1.0f , +1.0f,  -1.0f ,// ��ǰ��0
                +1.0f , +1.0f,  +1.0f ,// �Һ���1
                +1.0f , -1.0f,  +1.0f ,// �Һ���2
                +1.0f , -1.0f,  -1.0f ,// ��ǰ��3
                /*����*/
                -1.0f , +1.0f , +1.0f ,// ����� 4
                -1.0f , +1.0f , -1.0f ,// ��ǰ��5
                -1.0f , -1.0f , -1.0f ,// ��ǰ��6
                -1.0f , -1.0f , +1.0f ,// �����7

                /*����*/
                -1.0f, +1.0f, +1.0f,//�����8
                +1.0f, +1.0f, +1.0f,//�Һ���9
                +1.0f, +1.0f, -1.0f,//��ǰ��10
                -1.0f, +1.0f, -1.0f,//��ǰ�� 11
                /*����*/
                -1.0f, -1.0f, -1.0f,//��ǰ��12
                +1.0f, -1.0f, -1.0f,//��ǰ��13
                +1.0f, -1.0f, +1.0f,//�Һ���14
                -1.0f, -1.0f, +1.0f,//�����15
                /*ǰ��*/
                -1.0f , 1.0f , -1.0f, // ��ǰ�� - 16
                +1.0f , 1.0f , -1.0f, // ��ǰ�� - 17
                +1.0f ,-1.0f , -1.0f, // ��ǰ�� - 18
                -1.0f ,-1.0f , -1.0f, // ��ǰ�� - 19
                /*����*/
                +1.0f , 1.0f , +1.0f, // �Һ��� - 20
                -1.0f , 1.0f , +1.0f, // ����� - 21
                -1.0f ,-1.0f , +1.0f,  // ����� - 22
                +1.0f ,-1.0f , +1.0f // �Һ��� - 23
            };

			for (int iIndex = 0 ; iIndex < sizeof(fDefaultV) / sizeof(float) ; iIndex++)
				data.vertices.push_back(fDefaultV[iIndex]);

            float fBoxHalfSize = 1000.0f;
			for (auto& v : data.vertices)
			{
				v = v * fBoxHalfSize;
			}

            GLushort fDefaultIndex[]= {
                // ����
                0,2,3 ,
                0,1,2 ,
                // ����
                4,6,7,
                4,5,6,

                // ����
                8,10,11,
                8,9,10,

                // ����
                12,14,15,
                12,13,14,
                // ǰ��
                16,18,19,
                16,17,18,
                // ����
                20,22,23,
                20,21,22
            };

		for (int iIndex = 0 ; iIndex < sizeof(fDefaultIndex) / sizeof(GLushort) ; iIndex++)
				data.indices.push_back(fDefaultIndex[iIndex]);

            float fDefaultUV[] = {
                /*����*/
                0 , 0 ,
                1 , 0 ,
                1 , 1 ,
                0 , 1 ,
                /*����*/
                1 , 0 ,
                2 , 0 ,
                2 , 1 ,
                1 , 1 ,
                /*���� */
                2 , 0 ,
                3 , 0 ,
                3 , 1 ,
                2 , 1 ,
                /*���� */
                0 , 1 ,
                1 , 1 ,
                1 , 2 ,
                0 , 2 ,
                /*ǰ��*/
                1 , 1 ,
                2 , 1 ,
                2 , 2 ,
                1 , 2 ,
                /*����*/
                2 , 1 ,
                3 , 1 ,
                3 , 2 ,
                2 , 2
            };


			for (int iIndex = 0 ; iIndex < sizeof(fDefaultUV) / sizeof(float) ; iIndex++)
				data.uvs.push_back(fDefaultUV[iIndex]);

            float SubImageSizeWidth = 1.0f/3.0f;
            float SubImageSizeHeight = 1.0f/2.0f;
			float fSafeEdge = 1.0f / 202.0f;
            for (int Said = 0 ; Said < 6 ; Said ++)
            {
                int iIndexBase = Said * 4 * 2;
                // ����
				data.uvs[iIndexBase + 0] = data.uvs[iIndexBase + 0] * SubImageSizeWidth + fSafeEdge;
				data.uvs[iIndexBase + 1] = data.uvs[iIndexBase + 1] * SubImageSizeHeight + fSafeEdge;
                // ����
				data.uvs[iIndexBase + 2] = data.uvs[iIndexBase + 2] * SubImageSizeWidth - fSafeEdge;
				data.uvs[iIndexBase + 3] = data.uvs[iIndexBase + 3] * SubImageSizeHeight + fSafeEdge;
                // ����
				data.uvs[iIndexBase + 4] = data.uvs[iIndexBase + 4] * SubImageSizeWidth - fSafeEdge;
				data.uvs[iIndexBase + 5] = data.uvs[iIndexBase + 5] * SubImageSizeHeight - fSafeEdge;
                // ����
				data.uvs[iIndexBase + 6] = data.uvs[iIndexBase + 6] * SubImageSizeWidth + fSafeEdge;
				data.uvs[iIndexBase + 7] = data.uvs[iIndexBase + 7] * SubImageSizeHeight - fSafeEdge;
            }
        }
    }
}
