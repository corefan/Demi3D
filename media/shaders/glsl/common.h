
#ifdef DI_GLES2
#   define MAX_BONES        32
#   define MAX_MODEL_MATS   32
#else
#   define MAX_BONES        75
#   define MAX_MODEL_MATS   75
#endif

#define MAX_LIGHTS          8

#define INSTANCE_NONE       0
#define INSTANCE_SHADER     1
#define INSTANCE_HARDWARE   2

uniform mat4    g_modelMatrix;
uniform mat4    g_viewMatrix;
uniform mat4    g_projMatrix;
uniform mat4    g_modelViewMatrix;
uniform mat4    g_modelViewProjMatrix;
uniform mat4    g_viewProjMatrix;

#ifdef DI_GLES2
uniform vec4    g_boneMatrices[MAX_BONES * 3];
uniform vec4    g_modelMatrices[MAX_MODEL_MATS * 3];
#else
uniform mat3x4  g_boneMatrices[MAX_BONES];
uniform mat3x4  g_modelMatrices[MAX_MODEL_MATS];
#endif

uniform vec3    g_eyePosition;
uniform vec3    g_eyeDirection;
uniform vec3    g_eyePositionObjSpace;
			    
uniform vec2    g_farnearPlane;
			    
uniform float   g_time;
uniform vec4    g_viewportSize;
			    
uniform vec4    g_globalAmbient;
uniform vec4    g_ambientColor;
uniform vec3    g_diffuseColor;
uniform vec3    g_specularColor;
uniform float   g_opacity;
uniform float   g_shininess;

uniform vec4    g_texelOffsets;
			    
uniform int     g_numDirLights;
uniform vec4    g_dirLightsColor[MAX_LIGHTS];
uniform vec4    g_dirLightsDir[MAX_LIGHTS];
			    
uniform int     g_numPointLights;
uniform vec4    g_pointLightsColor[MAX_LIGHTS];
uniform vec4    g_pointLightsPosition[MAX_LIGHTS];
uniform vec4    g_pointLightsAttenuation[MAX_LIGHTS];

uniform int     g_hasSkyLight;
uniform vec4    g_skyLightColor;
uniform vec4    g_groundColor;
uniform vec3    g_skyLightDir;

uniform vec4    g_depthRange;
uniform mat4    g_texViewProjMatrix;
uniform sampler2D g_shadowMap;

// global functions
float linstep(float min, float max, float v)
{
    return clamp((v - min) / (max - min), 0.0, 1.0);
}

#ifdef ALPHA_TEST
#   define ALPHATEST 0.5
#endif 