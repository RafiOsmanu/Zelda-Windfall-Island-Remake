float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gViewInverse : ViewInverse;
Texture2D gParticleTexture;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

//STATES
//******
BlendState AlphaBlending 
{     
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
};

DepthStencilState DisableDepthWriting
{
	//Enable Depth Rendering
    DepthEnable = TRUE;

	//Disable Depth Writing
    DepthWriteMask = ZERO;
	
};

RasterizerState BackCulling
{
	CullMode = BACK;
};



//SHADER STRUCTS
//**************
struct VS_DATA
{
	float3 Position : POSITION;
	float4 Color: COLOR;
	float Size: TEXCOORD0;
	float Rotation: TEXCOORD1;
};

struct GS_DATA
{
	float4 Position : SV_POSITION;
	float2 TexCoord: TEXCOORD0;
	float4 Color : COLOR;
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float2 texCoord, float4 col, float2x2 uvRotation)
{
	//Step 1. Create a GS_DATA object
    GS_DATA geomData = (GS_DATA) 0;
	//Step 2. Transform the position using the WVP Matrix and assign it to (GS_DATA object).Position (Keep in mind: float3 -> float4, Homogeneous Coordinates)
    geomData.Position = mul(float4(pos, 1.0f), gWorldViewProj);
	//Step 3. Assign texCoord to (GS_DATA object).TexCoord
	//This is a little formula to do texture rotation by transforming the texture coordinates (Can cause artifacts)
    //texCoord -= float2(0.5f, 0.5f);
    //texCoord = mul(texCoord, uvRotation);
    //texCoord += float2(0.5f, 0.5f);
    geomData.TexCoord = texCoord;
	//Step 4. Assign color to (GS_DATA object).Color
    geomData.Color = col;
	//Step 5. Append (GS_DATA object) to the TriangleStream parameter (TriangleStream::Append(...))
    triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Use these variable names
	float3 topLeft, topRight, bottomLeft, bottomRight;
	float size = vertex[0].Size;
	float3 origin = vertex[0].Position;
    float rotation = vertex[0].Rotation;

	//Vertices (Keep in mind that 'origin' contains the center of the quad
    float halfSize = size * 0.5f;
	
    topLeft =float3(-halfSize, halfSize, 0);
    topRight = float3(halfSize, halfSize, 0.0f);
    bottomLeft = float3(-halfSize, -halfSize, 0.0f);
    bottomRight = float3(halfSize, -halfSize, 0.0f);
	

    topLeft = mul(topLeft, (float3x3)gViewInverse);
    topRight = mul(topRight, (float3x3) gViewInverse);
    bottomLeft = mul(bottomLeft, (float3x3) gViewInverse);
    bottomRight = mul(bottomRight, (float3x3) gViewInverse);
	
	//place the vertices at the correct position, relative to the origin
    topLeft += origin;
    topRight += origin;
    bottomLeft += origin;
    bottomRight += origin;

	//This is the 2x2 rotation matrix we need to transform our TextureCoordinates (Texture Rotation)
	float2x2 uvRotation = {cos(vertex[0].Rotation), - sin(vertex[0].Rotation), sin(vertex[0].Rotation), cos(vertex[0].Rotation)};
	
	//Create Geometry (Trianglestrip)
	CreateVertex(triStream,bottomLeft, float2(0,1), vertex[0].Color, uvRotation);
	CreateVertex(triStream,topLeft, float2(0,0), vertex[0].Color, uvRotation);
	CreateVertex(triStream,bottomRight, float2(1,1), vertex[0].Color, uvRotation);
	CreateVertex(triStream,topRight, float2(1,0), vertex[0].Color, uvRotation);
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET {
	
	//Simple Texture Sampling
	float4 result = gParticleTexture.Sample(samPoint,input.TexCoord);
	return input.Color * result;
}

// Default Technique
technique10 Default {

	pass p0 {
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
		
		SetRasterizerState(BackCulling);       
		SetDepthStencilState(DisableDepthWriting, 0);
        SetBlendState(AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
	}
}
