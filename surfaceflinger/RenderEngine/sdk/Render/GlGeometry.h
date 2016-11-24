
#ifndef MJ_Geometry_h
#define MJ_Geometry_h
#include "../Base/GlUtils.h"
#include "../Base/MojingTypes.h"
#include "../Base/MojingMath.h"

namespace Baofeng
{
	namespace Mojing
	{
		enum VertexAttributeLocation
		{
			VERTEX_ATTRIBUTE_LOCATION_POSITION = 0,
			VERTEX_ATTRIBUTE_DISTORTION_R = 1,
			VERTEX_ATTRIBUTE_DISTORTION_B = 2,
			VERTEX_ATTRIBUTE_LOCATION_BINORMAL = 3,
			VERTEX_ATTRIBUTE_LOCATION_COLOR = 4,
			VERTEX_ATTRIBUTE_DISTORTION_G = 5,
			VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO = 6,
			VERTEX_ATTRIBUTE_LOCATION_JOINT_INDICES = 7,
			VERTEX_ATTRIBUTE_LOCATION_JOINT_WEIGHTS = 8,
			VERTEX_ATTRIBUTE_LOCATION_FONT_PARMS = 9
		};

		/*======================================================

		类: GlGeometry
		这个类负责管理顶点缓冲区与索引缓冲区，
		构建缓冲区与 glDrawElements 函数都在此类中完成。

		=======================================================*/

		class GlGeometry
		{
		public:
			GlGeometry();
			virtual ~GlGeometry();

			bool IsAvailable();

			virtual bool BuildGeometry(int iWidth = 0, int iHeight = 0, void *pBuffer = NULL);
			//			void Clear();
			virtual void BindVBO();
			virtual void UnBindVBO();

			virtual void DrawElements(int eye) = 0;
			virtual void DrawElementsRange(int eye) = 0;
			void ClearBuffers();

		protected:
			unsigned 	m_VertexBuffer;
			unsigned 	m_IndexBuffer;
			unsigned 	m_VertexArrayObject;
			int			m_VertexCount;
			int 		m_IndexCount;
		};
		/*======================================================

		类: GlGeometryTriangles

		=======================================================*/
		class GlGeometryTriangles : public GlGeometry
		{
		public:
			virtual bool BuildGeometry(int iWidth = 0, int iHeight = 0, void *pBuffer = NULL);
			virtual void DrawElements(int eye);
			virtual void DrawElementsRange(int eye);
		};
	}
}

#endif	// OVR_Geometry_h
