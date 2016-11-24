﻿#include "GLProgram.h"
#include "../Base/GlUtils.h"
#define ONLE_USING_R_LIMIT 1
//#define HIGH_RES 1

namespace Baofeng
{
	namespace Mojing
	{
		const char* VertexShader = "uniform mat4 Mvpm;\n"
			"uniform mat4 Texm;\n"
			"attribute vec4 Position;\n"
			"attribute vec2 TexCoord;\n"	// green
			"attribute vec2 TexCoord1;\n"	// .x = interpolated warp frac, .y = intensity scale
			"attribute vec2 dR;\n"		// red
			"attribute vec2 dB;\n"		// blue
			"varying  vec2 oTexCoord1r;\n"
			"varying  vec2 oTexCoord1g;\n"
			"varying  vec2 oTexCoord1b;\n"
			"varying  float	intensity;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = Mvpm * Position;\n"
			"	vec3 proj;\n"
			"	float projIZ;\n"
			"   proj = vec3( Texm * vec4(dR,-1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1r = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(TexCoord,-1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1g = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(dB,-1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1b = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			""
			"	intensity = TexCoord1.y;\n"
			"}\n";
		const char* VertexShaderLayout = "uniform mat4 Mvpm;\n"
			"uniform mat4 Texm;\n"
			"attribute vec4 Position;\n"
			"attribute vec2 TexCoord;\n"	// green
			"attribute vec2 TexCoord1;\n"	// .x = interpolated warp frac, .y = intensity scale
			"attribute vec2 dR;\n"		// red
			"attribute vec2 dB;\n"		// blue
			"varying  vec2 oTexCoord1r;\n"
			"varying  vec2 oTexCoord1g;\n"
			"varying  vec2 oTexCoord1b;\n"
			"varying  float	intensity;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = Mvpm * Position;\n"
			"	vec3 proj;\n"
			"	float projIZ;\n"
			"   proj = vec3( Texm * vec4(dR,1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1r = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(TexCoord,1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1g = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(dB,1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1b = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			""
			"	intensity = TexCoord1.y;\n"
			"}\n";
		const char* VertexShaderDistortionRange = VertexShader;

		const char* FragmentShader = "uniform sampler2D Texture0;\n"
			"varying highp vec2 oTexCoord1r;\n"
			"varying highp vec2 oTexCoord1g;\n"
			"varying highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			//				"uniform highp float TestR;\n"
			"varying mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.01;\n"
			"	highp vec4 color1r = texture2D(Texture0, oTexCoord1r);\n"
			"	highp vec4 color1g = texture2D(Texture0, oTexCoord1g);\n"
			"	highp vec4 color1b = texture2D(Texture0, oTexCoord1b);\n"
			"	mediump float TestR = step(TexClipX.x + Edge , oTexCoord1r.s) * step(oTexCoord1r.s , TexClipX.y - Edge) * step(Edge , oTexCoord1r.t) * step(oTexCoord1r.t , 1.0 - Edge);\n"
#if ONLE_USING_R_LIMIT// 只用红色通道过滤边界
			"	highp vec4 color1 = vec4( TestR * color1r.x, TestR * color1g.y, TestR * color1b.z, 1.0);\n"//注意: 主图层的Alpha是1
			//"	highp vec4 color1 = vec4( 0.0, 0.0, 1.0 , TestR);\n"//注意: 主图层的Alpha是1
#else
			"	mediump float TestG = TestR * step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	mediump float TestB = TestG * step(TexClipX.x + Edge , oTexCoord1b.s) * step(oTexCoord1b.s , TexClipX.y - Edge) * step(Edge , oTexCoord1b.t) * step(oTexCoord1b.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = vec4( TestR * color1r.x, TestG * color1g.y, TestB * color1b.z, 1);\n"//注意: 主图层的Alpha是1
#endif
			"	gl_FragColor = intensity * color1;\n"
			"}\n";
		// 无色散的
		const char* FragmentShaderNoDispersion = "uniform sampler2D Texture0;\n"
			"varying highp vec2 oTexCoord1r;\n"
			"varying highp vec2 oTexCoord1g;\n"
			"varying highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			"varying mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.001;\n"
			"	mediump float TestG = step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = texture2D(Texture0, oTexCoord1g) * TestG;\n"//注意: 主图层的Alpha是1
			//"	highp vec4 color1 = texture2D(Texture0, oTexCoord1g);\n"
			"	color1.w = 1.0;\n"
			"	gl_FragColor = intensity * color1;\n"
			"}\n";

		const char* FragmentShaderLayout = "uniform sampler2D Texture0;\n"
			"varying highp vec2 oTexCoord1r;\n"
			"varying highp vec2 oTexCoord1g;\n"
			"varying highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			"varying mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.01;\n"
			"	highp vec4 color1r = texture2D(Texture0, oTexCoord1r);\n"
			"	highp vec4 color1g = texture2D(Texture0, oTexCoord1g);\n"
			"	highp vec4 color1b = texture2D(Texture0, oTexCoord1b);\n"
			"	mediump float TestR = step(TexClipX.x + Edge , oTexCoord1r.s) * step(oTexCoord1r.s , TexClipX.y - Edge) * step(Edge , oTexCoord1r.t) * step(oTexCoord1r.t , 1.0 - Edge);\n"
#if ONLE_USING_R_LIMIT
			"	highp vec4 color1 = vec4( TestR * color1r.x, TestR * color1g.y, TestR * color1b.z, TestR*color1r.w);\n"//color1r.x, color1g.y, color1b.z
#else
			"	mediump float TestG = TestR * step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	mediump float TestB = TestG * step(TexClipX.x + Edge , oTexCoord1b.s) * step(oTexCoord1b.s , TexClipX.y - Edge) * step(Edge , oTexCoord1b.t) * step(oTexCoord1b.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = vec4( TestR * color1r.x, TestG * color1g.y, TestB * color1b.z, TestR*color1r.w);\n"//color1r.x, color1g.y, color1b.z
#endif
			"	gl_FragColor = color1;\n"
			"}\n";

		const char* FragmentShaderLayoutNoDispersion = "uniform sampler2D Texture0;\n"
			"varying highp vec2 oTexCoord1r;\n"
			"varying highp vec2 oTexCoord1g;\n"
			"varying highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			//"uniform highp float TestR;\n"
			"varying mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.01;\n"
			"	mediump float TestG = step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = texture2D(Texture0, oTexCoord1g) * TestG;\n"
			"	gl_FragColor = intensity * color1;\n"
			"}\n";

		const char* FragmentShaderDistortionRange = "uniform sampler2D Texture0;\n"
			"varying highp vec2 oTexCoord1r;\n"
			"varying highp vec2 oTexCoord1g;\n"
			"varying highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			//				"uniform highp float TestR;\n"
			"varying mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.01;\n"
			"	mediump float TestR = step(TexClipX.x + Edge , oTexCoord1r.s) * step(oTexCoord1r.s , TexClipX.y - Edge) * step(Edge , oTexCoord1r.t) * step(oTexCoord1r.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = vec4( max(0.5 , 1.0 * TestR) , 1.0 * TestR, 0.0 , 1.0);\n"//注意: 主图层的Alpha是1
			"	gl_FragColor = intensity * color1;\n"
			"}\n";

		const char* FragmentShaderDistortionRangeDark = "uniform sampler2D Texture0;\n"
			"varying highp vec2 oTexCoord1r;\n"
			"varying highp vec2 oTexCoord1g;\n"
			"varying highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			//				"uniform highp float TestR;\n"
			"varying mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"	highp vec4 color1 = vec4( 0.0 , 0.0, 0.0 , 1.0);\n"//注意: 主图层的Alpha是1
			"	gl_FragColor = color1;\n"
			"}\n";

		GlProgram::GlProgram() :
			program(-1),
			vertexShader(-1),
			fragmentShader(-1),
			uMvp(-1),
			uModel(-1),
			uView(-1),
			uTexm(-1),
			uTexClipX(-1),
			uTexture0(-1)
		{
		}


		GlProgram::~GlProgram()
		{
		}
		
		bool GlProgram::CheckIsReady()
		{
			return glIsProgram(program) &&
			vertexShader != (unsigned int )-1  &&
			fragmentShader != (unsigned int )-1  &&
			uMvp != (unsigned int )-1  &&
			uTexture0 != (unsigned int )-1 
			;
		}
		void GlProgram::Release()
		{
			program = 0;
			vertexShader = 0;
			fragmentShader = 0;
			uMvp = 0;
			uModel = 0;
			uView = 0;
			uTexm = 0;
			uTexClipX = 0;
			uTexture0 = 0;
		}
	}
}

