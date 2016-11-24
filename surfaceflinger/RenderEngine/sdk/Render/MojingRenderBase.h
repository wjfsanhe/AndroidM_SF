#pragma once
#include "../Base/GlUtils.h"
#include "GLProgram.h"

namespace Baofeng
{
	namespace Mojing
	{
		class MojingRenderBase
		{
			public:			
			static GlProgram BuildProgram(const char * vertexSrc, const char * fragmentSrc);
			protected:
			static bool CompileShader(const GLuint shader, const char * src);
		};
	}
}


