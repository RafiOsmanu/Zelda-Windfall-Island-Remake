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
    output.Position = float4(input.Position, 1.0f);
    
	// Set the TexCoord
    output.TexCoord = input.TexCoord;
	
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float textureWidth;
    float textureHeight;
	// Step 1: find the dimensions of the texture (the texture has a method for that)	
    gTexture.GetDimensions(textureWidth, textureHeight);
	// Step 2: calculate dx and dy (UV space for 1 pixel)	
    float dx = 1.0f / textureWidth;
    float dy = 1.0f / textureHeight;
	// Step 3: Create a double for loop (5 iterations each)
	//		   Inside the loop, calculate the offset in each direction. Make sure not to take every pixel but move by 2 pixels each time
	//			Do a texture lookup using your previously calculated uv coordinates + the offset, and add to the final color
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            //Inside the loop, calculate the offset in each direction. Make sure not to take every pixel but move by 2 pixels each time
            float2 offset = float2(i * dx * 2, j * dy * 2);
            //texture look up
            float4 color = gTexture.Sample(samPoint, input.TexCoord + offset);
            finalColor += color;
            
        }
    }
	// Step 4: Divide the final color by the number of passes (in this case 5*5)
    finalColor /= 25;
    
	// Step 5: return the final color
    return finalColor;
}


//TECHNIQUE
//---------
technique11 Blur
{
    pass P0
    {
		// Set states...
        SetDepthStencilState(EnableDepth, 0);
        SetRasterizerState(gRasterizerStage);
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}