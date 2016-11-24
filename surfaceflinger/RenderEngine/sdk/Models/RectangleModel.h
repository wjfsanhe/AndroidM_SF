#ifndef MOJING_RENDER_MODEL_RECTANGLE
#define MOJING_RENDER_MODEL_RECTANGLE

#include "Model.h"
namespace Baofeng
{
	namespace Mojing
	{
		class RectangleModel : public Model
		{
		public:
			RectangleModel(int textureType);
			virtual ~RectangleModel();

			virtual void Init();
			CLASS_MEMBER(Vector3f, m_v3, TopLeft);
			CLASS_MEMBER(Vector3f, m_v3, BottomRight);
			CLASS_MEMBER(Vector3f, m_v3, TopRight);
			CLASS_MEMBER(Vector3f, m_v3, BottomLeft);

		protected:
			virtual void prepareData();

		};

	}
}

#endif // MOJING_RENDER_MODEL_RECTANGLE