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
    // Step 1: sample the texture
    float4 color = gTexture.Sample(samPoint, input.TexCoord);
	// Step 2: calculate the mean value
    float mean = (color.r + color.g + color.b) / 3.0f;
	// Step 3: return the mean of the color
    return float4(mean, mean, mean, 1.0f);
}


//TECHNIQUE
//---------
technique11 Grayscale
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

