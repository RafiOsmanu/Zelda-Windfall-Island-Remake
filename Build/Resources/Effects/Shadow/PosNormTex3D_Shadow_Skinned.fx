float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.77f, 0.577f);
float gShadowMapBias = .0001f;
float4x4 gBones[70];

Texture2D gDiffuseMap;
Texture2D gShadowMap;

SamplerComparisonState cmpSampler
{
	// sampler state
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;
    AddressV = MIRROR;
 
   // sampler comparison state
    ComparisonFunc = LESS_EQUAL;
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 BoneIndices : BLENDINDICES;
	float4 BoneWeights : BLENDWEIGHTS;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 lPos : TEXCOORD1;
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

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
	
	// Initialize transformed position and normal
    float4 transformedPosition = float4(0, 0, 0, 0);
    float3 transformedNormal = float3(0, 0, 0);
	
	// Loop through each bone
    for (int i = 0; i < 4; i++)
    {
		//Get the current bone index and weight
        int boneIndex = input.BoneIndices[i];
        if (boneIndex < 0)
            continue;
        float boneWeight = input.BoneWeights[i];
        
        //Calculate bone space position and normal
        float4 boneSpacePosition = mul(float4(input.pos, 1.0f), gBones[boneIndex]);
        float3 boneSpaceNormal = normalize(mul(input.normal, (float3x3) gBones[boneIndex]));
		
		//Append to transformed position and normal based on bone weight
        transformedPosition.xyz += boneWeight * boneSpacePosition;
        transformedNormal += boneWeight * boneSpaceNormal;
        
    }
	
	// Set w-component of transformed position to 1
    transformedPosition.w = 1.f;
	
    // Transform transformed position to clip space
    output.pos = mul(transformedPosition, gWorldViewProj);
	
	//store worldspace projected to light clip space with
    //a texcoord semantic to be interpolated across the surface
    output.lPos = mul(transformedPosition, gWorldViewProj_Light);

    // Transform transformed normal to world space (rotation only)
    output.normal = normalize(mul(transformedNormal, (float3x3) gWorld));
	
    output.texCoord = input.texCoord;
	
    return output;
}

float2 texOffset(int u, int v)
{
	//TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
    return float2(u * 1.0f / 1280.f, v * 1.0f / 720.f);
}

float EvaluateShadowMap(float4 lpos)
{
   // TODO: complete
    lpos.xyz /= lpos.w;
	
   //if position is not visible to the light - dont illuminate it
   //results in hard light frustum
    if( lpos.x < -1.0f || lpos.x > 1.0f ||
        lpos.y < -1.0f || lpos.y > 1.0f ||
        lpos.z < 0.0f || lpos.z > 1.0f)
        return 0.f;
 
    //transform clip space coords to texture space coords (-1:1 to 0:1)
    lpos.x = lpos.x/ 2 + 0.5;
    lpos.y = lpos.y/ -2 + 0.5;
 
    //apply shadow map bias
    lpos.z -= gShadowMapBias;
 
    //PCF sampling for shadow map
    float sum = 0;
    float x, y;
 
    float shadowMapDepth = 0.f;
    //perform PCF filtering on a 4 x 4 texel neighborhood
    for (y = -3.5; y <= 3.5; y += 1.0)
    {
        for (x = -3.5; x <= 3.5; x += 1.0)
        {
            sum += gShadowMap.SampleCmpLevelZero( cmpSampler, lpos.xy + texOffset(x,y), lpos.z);
        }
    }
    
    //basic hardware PCF - single texel
    shadowMapDepth = (sum / 64.f)  + 0.4f;
	
    return shadowMapDepth;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float shadowValue = EvaluateShadowMap(input.lPos);

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;
    
    clip(color_a - 0.1f);
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	return float4( color_rgb * shadowValue, color_a );
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

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
