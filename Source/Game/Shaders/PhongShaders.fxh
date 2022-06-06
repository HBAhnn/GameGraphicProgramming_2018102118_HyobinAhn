//--------------------------------------------------------------------------------------
// File: PhongShaders.fx
//
// Copyright (c) Kyung Hee University.
//--------------------------------------------------------------------------------------

#define NUM_LIGHTS (1)
#define NEAR_PLANE (0.01f)
#define FAR_PLANE (1000.0f)

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Declare a diffuse texture and a sampler state (remove the comment)
--------------------------------------------------------------------*/
Texture2D txDiffuse[2] : register( t0 );
SamplerState samLinear[2] : register(s0);

Texture2D shadowMapTexture : register(t2);
SamplerState shadowMapSampler : register(s2);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnCameraMovement

  Summary:  Constant buffer used for view transformation and shading
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangeOnCameraMovement definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbChangeOnCameraMovement : register(b0)
{
    matrix View;
    float4 CameraPosition;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnResize

  Summary:  Constant buffer used for projection transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangeOnResize definition (remove the comment)
--------------------------------------------------------------------*/

cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
};


/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangesEveryFrame

  Summary:  Constant buffer used for world transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangesEveryFrame definition (remove the comment)
--------------------------------------------------------------------*/

cbuffer cbChangesEveryFrame : register( b2 )
{
	matrix World;
    float4 OutputColor;
    bool HasNormalMap;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbLights

  Summary:  Constant buffer used for shading
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbLights definition (remove the comment)
--------------------------------------------------------------------*/
struct APointLight
{
    float4 Position;
    float4 Color;
    float4 AttenuationDistance;
};

cbuffer cbLights : register(b3)
{
    //float4 LightPositions[NUM_LIGHTS];
    //float4 LightColors[NUM_LIGHTS];
    //matrix LightViews[NUM_LIGHTS];
    //matrix LightProjections[NUM_LIGHTS];
    
    APointLight PointLights[NUM_LIGHTS];
};

//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   VS_PHONG_INPUT

  Summary:  Used as the input to the vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: VS_PHONG_INPUT definition (remove the comment)
--------------------------------------------------------------------*/
struct VS_PHONG_INPUT
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    row_major matrix mTransform : INSTANCE_TRANSFORM;

};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   PS_PHONG_INPUT

  Summary:  Used as the input to the pixel shader, output of the 
            vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: PS_PHONG_INPUT definition (remove the comment)
--------------------------------------------------------------------*/

struct PS_PHONG_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGE;
    float4 LightViewPosition : TEXCOORD1;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   PS_LIGHT_CUBE_INPUT

  Summary:  Used as the input to the pixel shader, output of the 
            vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: PS_LIGHT_CUBE_INPUT definition (remove the comment)
--------------------------------------------------------------------*/

struct PS_LIGHT_CUBE_INPUT
{
	float4 Position : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Vertex Shader function VSPhong definition (remove the comment)
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
  TODO: Vertex Shader function VSLightCube definition (remove the comment)
--------------------------------------------------------------------*/

PS_PHONG_INPUT VSPhong(VS_PHONG_INPUT input)
{
	PS_PHONG_INPUT output = (PS_PHONG_INPUT)0;
	
	output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	
    output.TexCoord = input.TexCoord;
	
    output.Normal = normalize(mul(float4(input.Normal, 0), World).xyz);
    output.WorldPosition = mul(input.Position, World);
    
    if (HasNormalMap)
    {
    // Calculate the tangent vector against the world matrix only and then normalize the final value.
        output.Tangent = normalize(mul(float4(input.Tangent, 0), World).xyz);

    // Calculate the binormal vector against the world matrix only and then normalize the final value.
        output.Bitangent = normalize(mul(float4(input.Bitangent, 0), World).xyz);
    }

	return output;
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return ((2.0 * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - z * (FAR_PLANE - NEAR_PLANE))) / FAR_PLANE;
}

PS_LIGHT_CUBE_INPUT VSLightCube(VS_PHONG_INPUT input)
{
	PS_LIGHT_CUBE_INPUT output = (PS_LIGHT_CUBE_INPUT)0;

	output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Pixel Shader function PSPhong definition (remove the comment)
--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
  TODO: Pixel Shader function PSLightCube definition (remove the comment)
--------------------------------------------------------------------*/


float4 PSPhong(PS_PHONG_INPUT input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    
    if (HasNormalMap)
    {
        // Sample the pixel in the normal map.     
        float4 bumpMap = txDiffuse[1].Sample(samLinear[1], input.TexCoord);
        
        // Expand the range of the normal value from (0, +1) to (-1, +1).    
        bumpMap = (bumpMap * 2.0f) - 1.0f;
        
        // Calculate the normal from the data in the normal map.  
        float3 bumpNormal = (bumpMap.x * input.Tangent) + (bumpMap.y * input.Bitangent) + (bumpMap.z * normal);
        
        // Normalize the resulting bump normal and replace existing normal     
        normal = normalize(bumpNormal);
        
    }
    
    float3 ambient = float3(0, 0, 0);
    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);
    
    float3 viewdirection = normalize(CameraPosition.xyz - input.WorldPosition);
       
	for (uint i = 0; i < NUM_LIGHTS; ++i)
	{
        float r2 = dot(input.WorldPosition - PointLights[i].Position.xyz, input.WorldPosition - PointLights[i].Position.xyz);
        
        float r0 = PointLights[i].AttenuationDistance.x;
        
        float attenuation = (r0 * r0) / (r2 + 0.000001f);
        
        ambient += float4(float3(0.1f, 0.1f, 0.1f) * PointLights[i].Color.xyz, 1.0f) * attenuation;
        
        float3 lightDirection = normalize(input.WorldPosition - PointLights[i].Position.xyz);
        diffuse += saturate(dot(normal, -lightDirection)) * PointLights[i].Color * attenuation;
        
        float3 reflectDirection = reflect(lightDirection, normal);
        specular += pow(saturate(dot(reflectDirection, viewdirection)), 20) * PointLights[0].Color * attenuation;
    }
    
    ambient *= txDiffuse[0].Sample(samLinear[0], input.TexCoord);
	diffuse *= txDiffuse[0].Sample(samLinear[0], input.TexCoord);
	specular *= txDiffuse[0].Sample(samLinear[0], input.TexCoord);
    
 
    //return float4(input.WorldPosition.xyz, 1);
    return float4(ambient + diffuse + specular, 1);
}


float4 PSLightCube(PS_LIGHT_CUBE_INPUT input) : SV_Target
{
	return OutputColor;
}