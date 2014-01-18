
#include "GLTypeMappings.h"
#include "VertexFormat.h"

namespace Demi
{
    GLenum DiGLTypeMappings::GetGLUsage(uint32 usage)
    {
        switch (usage)
        {
        case RU_STATIC:
            return GL_STATIC_DRAW_ARB;
        case RU_DYNAMIC:
            return GL_DYNAMIC_DRAW_ARB;
        case RU_WRITE_ONLY:
            return GL_STREAM_DRAW_ARB;
        default:
            return GL_DYNAMIC_DRAW_ARB;
        };
    }

    GLenum DiGLTypeMappings::GetGLType(uint32 type)
    {
        switch (type)
        {
        case VERT_TYPE_FLOAT1:
        case VERT_TYPE_FLOAT2:
        case VERT_TYPE_FLOAT3:
        case VERT_TYPE_FLOAT4:
            return GL_FLOAT;
        case VERT_TYPE_SHORT2:
        case VERT_TYPE_SHORT4:
            return GL_SHORT;
        case VERT_TYPE_COLOR:
        case VERT_TYPE_UBYTE4:
            return GL_UNSIGNED_BYTE;
        default:
            return 0;
        };
    }

    GLuint DiGLTypeMappings::GetFixedAttributeIndex(uint8 semantic, uint8 index)
    {
        // Some drivers (e.g. OS X on nvidia) incorrectly determine the attribute binding automatically
        // and end up aliasing existing built-ins. So avoid! Fixed builtins are: 

        //  a  builtin				custom attrib name
        // ----------------------------------------------
        //	0  gl_Vertex			vertex
        //  1  n/a					blendWeights		
        //	2  gl_Normal			normal
        //	3  gl_Color				colour
        //	4  gl_SecondaryColor	secondary_colour
        //	5  gl_FogCoord			fog_coord
        //  7  n/a					blendIndices
        //	8  gl_MultiTexCoord0	uv0
        //	9  gl_MultiTexCoord1	uv1
        //	10 gl_MultiTexCoord2	uv2
        //	11 gl_MultiTexCoord3	uv3
        //	12 gl_MultiTexCoord4	uv4
        //	13 gl_MultiTexCoord5	uv5
        //	14 gl_MultiTexCoord6	uv6, tangent
        //	15 gl_MultiTexCoord7	uv7, binormal

        switch (semantic)
        {
        case VERT_USAGE_POSITION:
            return 0;
        case VERT_USAGE_BLENDWEIGHT:
            return 1;
        case VERT_USAGE_NORMAL:
            return 2;
        case VERT_USAGE_COLOR:
            return 3;
        case VERT_USAGE_BLENDINDICES:
            return 7;
        case VERT_USAGE_TEXCOORD:
            return 8 + index;
        case VERT_USAGE_TANGENT:
            return 14;
        case VERT_USAGE_BINORMAL:
            return 15;
        default:
            DI_ASSERT(false && "Missing attribute!");
            return 0;
        };
    }
}