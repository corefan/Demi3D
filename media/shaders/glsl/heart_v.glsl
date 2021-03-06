
#include "common.h"

attribute vec3  Position;
attribute vec2  Texcoord0;
varying vec2 vTexCoord0;

void main()
{
	gl_Position	= g_viewProjMatrix * vec4(Position.x, Position.y, 0.0, 1.0);
    vTexCoord0.xy = Texcoord0.xy;
}