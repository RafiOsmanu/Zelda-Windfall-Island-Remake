float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
int gMaxBones = 70;
float4x4 gBones[70];

Texture2D gDiffuseMap;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
    float4 indices : BLENDINDICES;
    float4 weights : BLENDWEIGHTS;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

BlendState NoBlending
{
	BlendEnable[0] = FALSE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;
	
	// Initialize transformed position and normal
    float4 transformedPosition = float4(0, 0, 0, 0);
    float3 transformedNormal = float3(0, 0, 0);
	
	// Loop through each bone
    for (int i = 0; i < 4; i++)
    {
		//Get the current bone index and weight
        int boneIndex = input.indices[i];
        if (boneIndex < 0) continue;
        float boneWeight = input.weights[i];
        
        //Calculate bone space position and normal
        float3 boneSpacePosition = mul(float4(input.pos, 1.0f), gBones[boneIndex]);
        float3 boneSpaceNormal = normalize(mul(input.normal, (float3x3) gBones[boneIndex]));
		
		//Append to transformed position and normal based on bone weight
         transformedPosition.xyz += boneWeight * boneSpacePosition;
         transformedNormal += boneWeight * boneSpaceNormal;
        
    }
	
	// Set w-component of transformed position to 1
    transformedPosition[3] = 1.f;
	
    // Transform transformed position to clip space
    output.pos = mul(transformedPosition, gWorldViewProj);

    // Transform transformed normal to world space (rotation only)
    output.normal = normalize(mul(transformedNormal, (float3x3) gWorld));
	
    output.texCoord = input.texCoord;
	
    return output;

}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	return float4( color_rgb , color_a );
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

