#include "common.h"

varying vec3 vNormal;
varying vec3 vViewDir;
varying vec3 vPosWorld;

// options
//#define USE_SCHLICK_FRESNEL

#if defined( USE_NORMALMAP )
varying vec3 vTangent;
varying vec3 vBinormal;

uniform sampler2D normalMap;
uniform sampler2D specularMap;
#endif

#if defined( USE_COLOR )
varying vec4 vColor;
#endif

#ifdef USE_MAP
varying vec4 vTexcoord0;
uniform sampler2D map;
#endif

#ifdef USE_ENV_MAP
uniform samplerCube envMap;
uniform float       reflectivity;
#	ifdef ENV_MAP_REFRACT
		uniform float  refractionRatio;
#	endif
#endif

// Data that we can read or derive from the surface shader outputs
struct SurfaceData
{
    vec3 positionWorld;       // world space position
	vec3 viewDirWorld;
    vec3 normal;              // world space normal
    vec4 albedo;
    vec3 specularAmount;      // Treated as a multiplier on albedo
};

SurfaceData gSurface;

void ComputeSurfaceDataFromGeometry()
{
	gSurface.viewDirWorld = vViewDir;
	gSurface.positionWorld = vPosWorld;
    
    gSurface.specularAmount = g_specularColor.rgb;
    
#if defined( USE_NORMALMAP )
    vec4 normMapColor;
#   ifdef DI_GLES2
    normMapColor = texture2D(normalMap, vTexcoord0.xy);
#   else
    normMapColor.rgb = texture2D(normalMap, vTexcoord0.xy).agb;
    normMapColor.a = texture2D(specularMap, vTexcoord0.xy).g;
#   endif
    gSurface.specularAmount *= normMapColor.a;
    vec3 texNormal = vec3(normMapColor.rgb * 2.0 - 1.0);
    mat3 rot = mat3(vTangent, vBinormal, vNormal);
    gSurface.normal = normalize(rot * texNormal);
#else
    gSurface.normal = normalize(vNormal);
#endif
	
#ifdef USE_MAP
	gSurface.albedo = texture2D( map, vTexcoord0.xy );
	#ifdef GAMMA_INPUT
		gSurface.albedo.rgb *= gSurface.albedo.rgb;
	#endif
#else
	gSurface.albedo = vec4(1.0,1.0,1.0,1.0);
#endif

#ifdef USE_COLOR
	gSurface.albedo = gSurface.albedo * vec4( vColor, 1.0 );
#endif
}

void AccumulatePhong(vec3 normal,
                         vec3 lightDir,
                         vec3 viewDir,
                         vec3 lightContrib,
                         inout vec3 litDiffuse,
                         inout vec3 litSpecular) 
{						 
    float NdotL = max(dot(normal, lightDir), 0.0);
	vec3 halfVec = normalize(lightDir + viewDir);
	float dotHalf = max(dot(normal, halfVec), 0.0);
	float specWeight = max(pow(dotHalf, g_shininess), 0.0);
		
#ifdef USE_SCHLICK_FRESNEL
	float specularNormalization = ( g_shininess + 2.0001 ) / 8.0;
	vec3 schlick = gSurface.specularAmount + vec3( 1.0 - gSurface.specularAmount ) 
					* pow( 1.0 - dot( lightDir, halfVec ), 5.0 );
					
	vec3 specular = schlick * specWeight * NdotL * specularNormalization;
#else
	vec3 specular = gSurface.specularAmount * specWeight;
#endif
		
	litDiffuse += lightContrib * NdotL;
    litSpecular += lightContrib * specular;
}

void AccumulateDirLight(vec3 dir, vec4 color, inout vec3 diffuse, inout vec3 specular){
	vec3 litDiffuse = vec3(0.0);
    vec3 litSpecular = vec3(0.0);
	
    AccumulatePhong(gSurface.normal, normalize(-dir), normalize(gSurface.viewDirWorld),
        color.rgb * color.a, litDiffuse, litSpecular);
	
    diffuse += litDiffuse;
	specular += litSpecular;
}

void AccumulatePointLight(vec3 position, float attenBegin,  float attenEnd, vec4 color,
		inout vec3 diffuse, inout vec3 specular) {
	vec3 directionToLight = position - gSurface.positionWorld;
    float distanceToLight = length(directionToLight);
	
    vec3 litDiffuse = vec3(0.0);
    vec3 litSpecular = vec3(0.0);
	
	if (distanceToLight < attenEnd) {
		float attenuation = linstep(attenEnd, attenBegin, distanceToLight);
        directionToLight /= distanceToLight;
		
		AccumulatePhong(gSurface.normal, directionToLight, normalize(gSurface.viewDirWorld),
			attenuation * color.rgb * color.a, litDiffuse, litSpecular);
		
        diffuse += litDiffuse;
        specular += litSpecular;
	}
}

void main(){

	ComputeSurfaceDataFromGeometry();
    
#ifdef ALPHATEST
	if ( gSurface.albedo.a * g_opacity < ALPHATEST ) discard;
#endif
	
	vec3 vDiffuse = g_globalAmbient.rgb;
    vec3 vSpecular = vec3(0.0, 0.0, 0.0);
	
	for(int i = 0; i < g_numDirLights; i++){
		AccumulateDirLight(g_dirLightsDir[i].xyz, g_dirLightsColor[i], vDiffuse, vSpecular);
	}

	vec3 vFinalColor = gSurface.albedo.rgb * vDiffuse + vSpecular;
	
	gl_FragColor.rgb = vFinalColor.rgb;
    gl_FragColor.a = gSurface.albedo.a * g_opacity;
	
	
	// environment mapping
#if defined(USE_ENV_MAP)

	#ifdef ENV_MAP_REFRACT
		vec3 reflVect = refract(normalize(vViewDir), gSurface.normal, refractionRatio);
	#else
		vec3 reflVect = reflect(normalize(vViewDir), gSurface.normal);
	#endif
	
	vec4 cubeColor = textureCube( envMap, reflVect.xyz );
	
	#ifdef GAMMA_INPUT
		cubeColor.rgb *= cubeColor.rgb;
	#endif
	
	#if defined( ENV_MAP_ADD )
		gl_FragColor.rgb += cubeColor.rgb * reflectivity;
	#elif defined( ENV_MAP_MIX )
		gl_FragColor.rgb = mix( gl_FragColor.rgb, cubeColor.rgb, reflectivity );
	#else
		gl_FragColor.rgb = mix( gl_FragColor.rgb, gl_FragColor.rgb * cubeColor.rgb, reflectivity );
	#endif
#endif

#ifdef GAMMA_OUTPUT
	gl_FragColor.rgb = sqrt( gl_FragColor.xyz );
#endif
}
