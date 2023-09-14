float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.005f;

Texture2D gDiffuseMap;
Texture2D gShadowMap;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;// or Mirror or Clamp or Border
	AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerComparisonState cmpSampler
{
	// sampler state
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;
    AddressV = MIRROR;

	// sampler comparison state
    ComparisonFunc = LESS_EQUAL;
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = NONE;
};

BlendState gBlendState
{
    BlendEnable[0] = false;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    blendOp = add;
    srcBlendAlpha = zero;
    DestBlendAlpha = one;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = ALL;
    DepthFunc = less;
    StencilEnable = false;
};

RasterizerState NoCulling
{
    CullMode = NONE;
    FrontCounterClockwise = false;
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 lPos : TEXCOORD1;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
	
	//TODO: complete Vertex Shader
	//Hint: Don't forget to project our position to light clip space and store it in lPos
	
    output.pos = mul(float4(input.pos, 1.f), gWorldViewProj);
    output.normal = normalize(mul(input.normal, (float3x3) gWorld));
 
    //store worldspace projected to light clip space with
    //a texcoord semantic to be interpolated across the surface
    output.lPos = mul(float4(input.pos, 1.f), gWorldViewProj_Light);
    output.texCoord = input.texCoord;
	
    return output;
}

float2 texOffset(int u, int v)
{
	//TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
    return float2(u * 1.0f / 1280, v * 1.0f / 720);
}

float EvaluateShadowMap(float4 lpos)
{
	// TODO: complete
    lpos.xyz /= lpos.w;
	
   //if position is not visible to the light - dont illuminate it
   //results in hard light frustum
    if (lpos.x < -1.0f || lpos.x > 1.0f ||
        lpos.y < -1.0f || lpos.y > 1.0f ||
        lpos.z < 0.0f || lpos.z > 1.0f)
        return 1.5;
 
    //transform clip space coords to texture space coords (-1:1 to 0:1)
    lpos.x = lpos.x / 2 + 0.5;
    lpos.y = lpos.y / -2 + 0.5;
 
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
            sum += gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x, y), lpos.z);
        }
    }
    
    //basic hardware PCF - single texel
    shadowMapDepth = (sum / 64.f) + 0.5f;
	
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
    diffuseStrength *= 1.6;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	return float4( color_rgb * shadowValue  , color_a );
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
        SetRasterizerState(Solid);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_5_0, PS() ) );


    }
}

