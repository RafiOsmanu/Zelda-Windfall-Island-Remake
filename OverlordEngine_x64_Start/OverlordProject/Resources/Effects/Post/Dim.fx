//Global Variable
float gDim = 0.4f;

//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

/// Create Rasterizer State (Backface culling) 
RasterizerState gRasterizerStage
{
    //FillMode = SOLID;
    CullMode = BACK;
};

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

Texture2D gTexture;



//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	
    //set position
    output.Position = float4( input.Position, 1.0f );
    
	// Set the TexCoord
    output.TexCoord = input.TexCoord;
	
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
     // Step 1: Sample the texture
    float4 color = gTexture.Sample(samPoint, input.TexCoord);
    
    // Step 2: Return the dimmed color
    return float4(color.r * gDim, color.g * gDim, color.b * gDim, 1.0f);
}


//TECHNIQUE
//---------
technique11 Dim
{
    pass P0
    {          
        // Set states...
        SetDepthStencilState( EnableDepth, 0 );
        SetRasterizerState( gRasterizerStage );
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

