#include "common.h"
#include "transform.h"

attribute vec3  Position;

#if defined( SKINNED )
attribute vec4  BlendWeights;
attribute vec4  BlendIndices;
#endif

#if defined( USE_COLOR )
attribute vec4  Color;
varying   vec4	vColor;
#endif

#if defined( USE_MAP ) 
attribute vec2  Texcoord0;
#endif

#if defined( USE_LIGHTMAP )
attribute vec2  Texcoord1;
#endif

void main()
{
	GET_SPACE_POS(objPos);
	
	gl_Position = g_modelViewProjMatrix * objPos;
	
#if defined( USE_COLOR )
	vColor = Color;
#endif
	
#if defined( USE_MAP ) 
	gl_TexCoord[0].xy = Texcoord0;
#endif

#if defined( USE_LIGHTMAP ) 
	gl_TexCoord[1].xy = Texcoord1;
#endif
}