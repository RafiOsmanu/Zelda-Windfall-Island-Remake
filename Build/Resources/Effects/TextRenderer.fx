//INCOMPLETE!

float4x4 gTransform : WORLDVIEWPROJECTION;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

BlendState EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};

RasterizerState BackCulling
{
	CullMode = BACK;
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
	int Channel : TEXCOORD2; //Texture Channel
	float3 Position : POSITION; //Left-Top Character Quad Starting Position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Left-Top Character Texture Coordinate on Texture
	float2 CharSize: TEXCOORD1; //Size of the character (in screenspace)
};

struct GS_DATA
{
	float4 Position : SV_POSITION; //HOMOGENEOUS clipping space position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Texcoord of the vertex
	int Channel : TEXCOORD1; //Channel of the vertex
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, int channel)
{
	//Create a new GS_DATA object
	//Geometry Vertex Output
	//Fill in all the fields
    GS_DATA geomData = (GS_DATA) 0;
    geomData.Position = mul(float4(pos, 1.0f), gTransform);
    geomData.Color = col;
    geomData.TexCoord = texCoord;
    geomData.Channel = channel;
    triStream.Append(geomData);
	//Append it to the TriangleStream
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//REMOVE THIS >
	//GS_DATA dummyData = (GS_DATA)0; //Just some dummy data
	//triStream.Append(dummyData); //The geometry shader needs to emit something, see what happens if it doesn't emit anything.
	//< STOP REMOVING

	// Extract the necessary data from the input vertex
    int channel = vertex[0].Channel;
    float3 pos = vertex[0].Position;
    float4 col = vertex[0].Color;
    float2 texCoord = vertex[0].TexCoord;
    float2 charSize = vertex[0].CharSize;
    float2 uvCoord = charSize / gTextureSize;

    // Compute the positions of the four vertices of the quad
    float3 topLeft = pos;
    float3 topRight = pos + float3(charSize.x, 0.0f, 0.0f);
    float3 bottomLeft = pos + float3(0.0f, charSize.y, 0.0f);
    float3 bottomRight = pos + float3(charSize.x, charSize.y, 0.0f);

    // Emit the four vertices to the output stream
    CreateVertex(triStream, topLeft, col, texCoord, channel);
    CreateVertex(triStream, topRight, col, texCoord + float2(uvCoord.x, 0.0f), channel);
    CreateVertex(triStream, bottomLeft, col, texCoord + float2(0.0f, uvCoord.y), channel);
    CreateVertex(triStream, bottomRight, col, texCoord + float2(uvCoord.x, uvCoord.y), channel);

}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET
{

	//Sample the texture and return the correct channel [Vertex.Channel]
	//You can iterate a float4 just like an array, using the index operator
	//Also, don't forget to colorize ;) [Vertex.Color]

    // Return the channel value, multiplied by the vertex color
    return gSpriteTexture.Sample(samPoint, input.TexCoord)[input.Channel] * input.Color;
}
    

// Default Technique
technique10 Default {

	pass p0 {
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}
