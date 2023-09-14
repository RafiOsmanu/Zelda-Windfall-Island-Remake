float4x4 gWorld;
float4x4 gLightViewProj;
float4x4 gBones[70];
 
DepthStencilState depthStencilState
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState rasterizerState
{
	FillMode = SOLID;
	CullMode = NONE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader [STATIC]
//--------------------------------------------------------------------------------------
float4 ShadowMapVS(float3 position:POSITION):SV_POSITION
{
	//TODO: return the position of the vertex in correct space (hint: seen from the view of the light)
    return mul(float4(position, 1.0f), mul(gWorld, gLightViewProj));
}

//--------------------------------------------------------------------------------------
// Vertex Shader [SKINNED]
//--------------------------------------------------------------------------------------
float4 ShadowMapVS_Skinned(float3 position:POSITION, float4 BoneIndices : BLENDINDICES, float4 BoneWeights : BLENDWEIGHTS) : SV_POSITION
{
	//TODO: return the position of the ANIMATED vertex in correct space (hint: seen from the view of the light)
	
	// Initialize transformed position and normal
    float4 transformedPosition = float4(0, 0, 0, 0);
   
	// Loop through each bone
    for (int i = 0; i < 4; i++)
    {
		//Get the current bone index and weight
        int boneIndex = BoneIndices[i];
        if (boneIndex < 0) continue;
        float boneWeight = BoneWeights[i];
        
        //Calculate bone space position and normal
        float3 boneSpacePosition = mul(float4(position, 1.0f), gBones[boneIndex]);
		
		//Append to transformed position and normal based on bone weight
        transformedPosition.xyz += boneWeight * boneSpacePosition;
    }
	
    transformedPosition.w = 1;
	
    return mul(transformedPosition, mul(gWorld, gLightViewProj));
}
 
//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------
void ShadowMapPS_VOID(float4 position:SV_POSITION){}

technique11 GenerateShadows
{
	pass P0
	{
		SetRasterizerState(rasterizerState);
	    SetDepthStencilState(depthStencilState, 0);
		SetVertexShader(CompileShader(vs_4_0, ShadowMapVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ShadowMapPS_VOID()));
	}
}

technique11 GenerateShadows_Skinned
{
	pass P0
	{
		SetRasterizerState(rasterizerState);
		SetDepthStencilState(depthStencilState, 0);
		SetVertexShader(CompileShader(vs_4_0, ShadowMapVS_Skinned()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ShadowMapPS_VOID()));
	}
}