#include "MojingRenderBase.h"
#include "GlGeometry.h"
#include <stdio.h>
namespace Baofeng
{
	namespace Mojing
	{
		GlProgram MojingRenderBase::BuildProgram(const char * vertexSrc,
			const char * fragmentSrc)
		{
			GlProgram prog;
			prog.vertexShader = glCreateShader(GL_VERTEX_SHADER);
			if (!CompileShader(prog.vertexShader, vertexSrc))
			{
				return prog;
			}
			//m_shaderIds.push_back(prog.vertexShader);

			prog.fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			if (!CompileShader(prog.fragmentShader, fragmentSrc))
			{
				return prog;
			}
			//m_shaderIds.push_back(prog.fragmentShader);

			prog.program = glCreateProgram();
			// m_programIds.push_back(prog.program);
			glAttachShader(prog.program, prog.vertexShader);
			glAttachShader(prog.program, prog.fragmentShader);

			// set attributes before linking
			glBindAttribLocation(prog.program, VERTEX_ATTRIBUTE_LOCATION_POSITION, "Position");
			glBindAttribLocation(prog.program, VERTEX_ATTRIBUTE_DISTORTION_R, "dR");
			glBindAttribLocation(prog.program, VERTEX_ATTRIBUTE_DISTORTION_B, "dB");

			glBindAttribLocation(prog.program, VERTEX_ATTRIBUTE_LOCATION_COLOR, "VertexColor");
			glBindAttribLocation(prog.program, VERTEX_ATTRIBUTE_DISTORTION_G, "TexCoord");
			glBindAttribLocation(prog.program, VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO, "TexCoord1");


			// link and error check
			glLinkProgram(prog.program);
			GLint r;
			glGetProgramiv(prog.program, GL_LINK_STATUS, &r);
			if (r != GL_FALSE)
			{
				prog.uMvp = glGetUniformLocation(prog.program, "Mvpm");
				prog.uModel = glGetUniformLocation(prog.program, "Modelm");
				prog.uView = glGetUniformLocation(prog.program, "Viewm");
				prog.uTexm = glGetUniformLocation(prog.program, "Texm");
				prog.uTexClipX = glGetUniformLocation(prog.program, "TexClipX");
				prog.uTexture0 = glGetUniformLocation(prog.program, "Texture0");
				prog.uLayerIndex = glGetUniformLocation(prog.program, "layerIndex");
				glUseProgram(prog.program);

				// texture and image_external bindings
				for (int i = 0; i < 8; i++)
				{
					char name[32];
					sprintf(name, "Texture%i", i);
					const GLint uTex = glGetUniformLocation(prog.program, name);
					if (uTex != -1)
					{
						glUniform1i(uTex, i);
					}
				}

				glUseProgram(0);
			}
			else
			{
				GLchar msg[1024];
				glGetProgramInfoLog(prog.program, sizeof(msg), 0, msg);
			}
			return prog;
		}
		
		bool MojingRenderBase::CompileShader(const GLuint shader, const char * src)
		{
			glShaderSource(shader, 1, &src, 0);
			glCompileShader(shader);

			GLint r;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
			if (r == GL_FALSE)
			{
				GLchar msg[4096];
				glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
				//MOJING_ERROR(g_APIlogger, "MojingRenderBase::CompileShader failed! msg : " << msg);
				return false;
			}
			return true;
		}
	}
}
