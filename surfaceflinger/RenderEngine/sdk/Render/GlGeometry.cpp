
#include "GlGeometry.h"
#include <cstdint>
//add by tw
#ifndef UNREAL_DISTORTION_PARAMETES_COUNT
#define UNREAL_DISTORTION_PARAMETES_COUNT 9
#endif
namespace Baofeng
{
#define CELLS_COUNT 32
	namespace Mojing
	{

		/*======================================================

		类: GlGeometry

		=======================================================*/
		GlGeometry::GlGeometry() :
			m_VertexBuffer(0),
			m_IndexBuffer(0),
			m_VertexArrayObject(0),
			m_VertexCount(0),
			m_IndexCount(0)
		{}

		GlGeometry::~GlGeometry()
		{
			ClearBuffers();
		}

	
		/*------------------------------------------------------------------
		2015/08/17: 绑定缓冲区
		------------------------------------------------------------------*/
		void GlGeometry::BindVBO()
		{
			const int attribCount = 10;
			glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_POSITION, 2, GL_FLOAT, false, attribCount * sizeof(float), (void *)(0 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_R);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_R, 2, GL_FLOAT, false, attribCount * sizeof(float), (void *)(2 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_G);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_G, 2, GL_FLOAT, false, attribCount * sizeof(float), (void *)(4 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_B);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_B, 2, GL_FLOAT, false, attribCount * sizeof(float), (void *)(6 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO, 2, GL_FLOAT, false, attribCount * sizeof(float), (void *)(8 * sizeof(float)));

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		}

		/*------------------------------------------------------------------
		2015/08/17: 解除绑定缓冲区
		------------------------------------------------------------------*/
		void GlGeometry::UnBindVBO()
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		/*------------------------------------------------------------------
		2015/08/17: 清理缓冲区
		------------------------------------------------------------------*/
		void GlGeometry::ClearBuffers()
		{
			//============================ 释放顶点 ============================//

			if (m_VertexBuffer != 0 && glIsBuffer(m_VertexBuffer))
			{
				glDeleteBuffers(1, &m_VertexBuffer);
			}
			m_VertexBuffer = 0;
			m_VertexCount = 0;

			//============================ 释放索引 ============================//

			if (m_IndexBuffer != 0 && glIsBuffer(m_IndexBuffer))
			{
				glDeleteBuffers(1, &m_IndexBuffer);
			}
			m_IndexBuffer = 0;
			m_IndexCount = 0;

			m_VertexArrayObject = 0;
		}

		/*------------------------------------------------------------------
		2015/08/17: 判断缓冲区是否已经成功建立
		------------------------------------------------------------------*/
		bool GlGeometry::IsAvailable()
		{
			return m_VertexBuffer != 0 && glIsBuffer(m_VertexBuffer);
		}

		bool GlGeometry::BuildGeometry(int iWidth /*= 0*/, int iHeight /*= 0*/, void *pBuffer /*= NULL*/)
		{
			ClearBuffers();
			return true;
		}

		/*======================================================

		类: GlGeometryTriangles

		=======================================================*/
		/*------------------------------------------------------------------
		2015/08/17:  绘制图元 参数：左右眼
		------------------------------------------------------------------*/
		void GlGeometryTriangles::DrawElements(int eye)
		{
			const int indexCount = m_IndexCount / 2;
			const int indexOffset = eye * indexCount;
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)(long)(indexOffset * 2));
		}
		void GlGeometryTriangles::DrawElementsRange(int eye)
		{
			const int indexCount = m_IndexCount / 2;
			const int indexOffset = eye * indexCount;
			glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_SHORT, (void*)(long)(indexOffset * 2));
		}
		/*------------------------------------------------------------------
		2015/08/17: 建立顶点与索引缓冲区
		------------------------------------------------------------------*/
#define USING_NEW_MESH 1
		bool GlGeometryTriangles::BuildGeometry(int iWidth /*= 0*/, int iHeight /*= 0*/ , void *pBuffer /* = NULL*/)
		{
			GlGeometry::BuildGeometry(iWidth /*= 0*/, iHeight /*= 0*/);

			const int NUM_SLICES_PER_EYE = 1;
			const float fovScale = 1.0f;


			const int tesselationsX = ((int *)pBuffer)[1];
			const int tesselationsY = ((int *)pBuffer)[2];

			//const int vertexBytes = 12 + 2 * (tesselationsX + 1) * (tesselationsY + 1) * 6 * sizeof(float);
			const float * bufferVerts = &((float *)pBuffer)[3];

			const int attribCount = 10;
			const int sliceTess = tesselationsX / NUM_SLICES_PER_EYE;

			this->m_VertexCount = 2 * NUM_SLICES_PER_EYE*(sliceTess + 1)*(tesselationsY + 1);
			const int floatCount = m_VertexCount * attribCount;
			float* pTessVertices = new float[floatCount];
			//  0   1   2   3   4   5   6   7   8   9   
			// [X   Y   Rx  Ry  Gx  Gy  Bx  By  A   0]
			int	verts = 0;
			for (int eye = 0; eye < 2; eye++)
			{
				for (int slice = 0; slice < NUM_SLICES_PER_EYE; slice++)
				{
					const int vertBase = verts;
					for (int y = 0; y <= tesselationsY; y++)
					{
						const float	yf = (float)y / (float)tesselationsY;
						for (int x = 0; x <= sliceTess; x++)
						{
							const int sx = slice * sliceTess + x;
							const float	xf = (float)sx / (float)tesselationsX;
							float * v = &pTessVertices[attribCount * (vertBase + y * (sliceTess + 1) + x)];

							const float * vSrc = bufferVerts+((y*(tesselationsX + 1) * 2 + sx + eye * (tesselationsX + 1)) * UNREAL_DISTORTION_PARAMETES_COUNT );

							v[0] = vSrc[7];
							v[1] = vSrc[8];

							v[2] = vSrc[0];
							v[3] = vSrc[1];

							v[4] = vSrc[2];
							v[5] = vSrc[3];

							v[6] = vSrc[4];
							v[7] = vSrc[5];

							v[8] = (float)x / sliceTess;
							v[9] = 1;
						}
					}
					verts += (tesselationsY + 1)*(sliceTess + 1);
				}
			}

			m_IndexCount = 2 * tesselationsX * tesselationsY * 6;
			unsigned short* pTessIndices = new unsigned short[m_IndexCount];

			int	index = 0;
			verts = 0;
			for (int eye = 0; eye < 2; eye++)
			{
				for (int slice = 0; slice < NUM_SLICES_PER_EYE; slice++)
				{
					const int vertBase = verts;
					// The order of triangles doesn't matter for tiled rendering,
					// but when we can do direct rendering to the screen, we want the
					// order to follow the raster order to minimize the chance
					// of tear lines.
					//
					// This can be checked by quartering the number of indexes, and
					// making sure that the drawn pixels are the first pixels that
					// the raster will scan.
					// 下面的代码用于绘制各个矩形，使用六个顶点将一个矩形切分成两个三角形；
					// 并且分别采用了从左上到右下和从右上到左下两种分割矩形的方式分割不同象限中的矩形。
					// 以下注释所描述的三角形方向和象限均是以左上角为原点坐标
					for (int x = 0; x < sliceTess; x++)
					{
						for (int y = 0; y < tesselationsY; y++)
						{
							// flip the triangulation in opposite corners
							if ((slice*sliceTess + x < tesselationsX / 2) ^ (y < (tesselationsY / 2)))
							{
								// 第一三象限，斜边从左上角到右下角,0与3、2与4重叠
								/*
								03 - - -1
								| \     |
								|   \   |
								|     \ |
								5 - - - 24
								*/
								pTessIndices[index + 0] = vertBase + y * (sliceTess + 1) + x;
								pTessIndices[index + 1] = vertBase + y * (sliceTess + 1) + x + 1;
								pTessIndices[index + 2] = vertBase + (y + 1) * (sliceTess + 1) + x + 1;

								pTessIndices[index + 3] = vertBase + y * (sliceTess + 1) + x;
								pTessIndices[index + 4] = vertBase + (y + 1) * (sliceTess + 1) + x + 1;
								pTessIndices[index + 5] = vertBase + (y + 1) * (sliceTess + 1) + x;
							}
							else
							{
								// 第二四象限，斜边从右上角到左下角,1与4、2与3重叠
								/*
								0  - - -14
								|     / |
								|   /   |
								| /     |
								23- - - 5
								*/
								pTessIndices[index + 0] = vertBase + y * (sliceTess + 1) + x;
								pTessIndices[index + 1] = vertBase + y * (sliceTess + 1) + x + 1;
								pTessIndices[index + 2] = vertBase + (y + 1) * (sliceTess + 1) + x;

								pTessIndices[index + 3] = vertBase + (y + 1) * (sliceTess + 1) + x;
								pTessIndices[index + 4] = vertBase + y * (sliceTess + 1) + x + 1;
								pTessIndices[index + 5] = vertBase + (y + 1) * (sliceTess + 1) + x + 1;
							}
							index += 6;
						}
					}
					verts += (tesselationsY + 1)*(sliceTess + 1);
				}
			}

			if (0 == this->m_VertexBuffer || !glIsBuffer(this->m_VertexBuffer))
			{
				glGenBuffers(1, &this->m_VertexBuffer);
			}
			glBindBuffer(GL_ARRAY_BUFFER, this->m_VertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, floatCount * sizeof(*pTessVertices), (void*)pTessVertices, GL_STATIC_DRAW);
			delete[] pTessVertices;

			if (0 == m_IndexBuffer || !glIsBuffer(m_IndexBuffer))
			{
				glGenBuffers(1, &m_IndexBuffer);
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexCount * sizeof(*pTessIndices), (void*)pTessIndices, GL_STATIC_DRAW);
			delete[] pTessIndices;

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_POSITION, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(0 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_R);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_R, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(2 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_G);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_G, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(4 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_B);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_B, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(6 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(8 * sizeof(float)));

			return true;
		}
	}
}
