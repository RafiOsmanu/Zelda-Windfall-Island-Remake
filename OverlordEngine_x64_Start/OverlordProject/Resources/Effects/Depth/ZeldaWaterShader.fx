// Wind Waker style water - NekotoArts
// Adapted from https://www.shadertoy.com/view/3tKBDz
// After which I added in some fractal Brownian motion
// as well as vertex displacement

//------------------------------------------------------
//Global Variables
//------------------------------------------------------

float4x4 gMatrixWVP : WORLDVIEWPROJECTION;

float4 WATER_COL = float4(0.04, 0.38, 0.88, 1.0);
float4 WATER2_COL = float4(0.04, 0.35, 0.78, 1.0);
float4 FOAM_COL = float4(0.8125, 0.9609, 0.9648, 1.0);
float distortion_speed = 2.0;
float2 tile = float2(80.0, 80.0);
float height = 100.0;
float2 wave_size = float2(6.0, 6.0);
float wave_speed = 3.0f;
float gTime;

static const float M_2PI = 6.283185307;
static const float M_6PI = 18.84955592;



//------------------------------------------------------
//Functions
//------------------------------------------------------
float random(float2 uv)
{
    return frac(sin(dot(uv.xy,
        float2(12.9898f, 78.233f))) *
            43758.5453123f);
}

float noise(float2 uv)
{
    float2 uv_index = floor(uv);
    float2 uv_fract = frac(uv);

    // Four corners in 2D of a tile
    float a = random(uv_index);
    float b = random(uv_index + float2(1.f, 0.f));
    float c = random(uv_index + float2(0.f, 1.f));
    float d = random(uv_index + float2(1.f, 1.f));

    float2 blur = smoothstep(0.f, 1.f, uv_fract);

    return lerp(a, b, blur.x) +
            (c - a) * blur.y * (1.f - blur.x) +
            (d - b) * blur.x * blur.y;
}

float fbm(float2 uv)
{
    int octaves = 6;
    float amplitude = 0.5f;
    float frequency = 3.f;
    float value = 0.f;
	
    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * noise(frequency * uv);
        amplitude *= 0.5f;
        frequency *= 2.f;
    }
    return value;
}

float circ(float2 pos, float2 c, float s)
{
    c = abs(pos - c);
    c = min(c, 1.f - c);

    return smoothstep(0.f, 0.002f, sqrt(s) - sqrt(dot(c, c))) * -1.f;
}
// Foam pattern for the water constructed out of a series of circles
float waterlayer(float2 uv)
{
    uv = fmod(uv, 1.f); // Clamp to [0..1]
	
    float ret = 1.f;
    ret += circ(uv, float2(0.37378f, 0.277169f), 0.0268181f);
    ret += circ(uv, float2(0.0317477f, 0.540372f), 0.0193742f);
    ret += circ(uv, float2(0.430044f, 0.882218f), 0.0232337f);
    ret += circ(uv, float2(0.641033f, 0.695106f), 0.0117864f);
    ret += circ(uv, float2(0.0146398f, 0.0791346f), 0.0299458f);
    ret += circ(uv, float2(0.43871f, 0.394445f), 0.0289087f);
    ret += circ(uv, float2(0.909446f, 0.878141f), 0.028466f);
    ret += circ(uv, float2(0.310149f, 0.686637f), 0.0128496f);
    ret += circ(uv, float2(0.928617f, 0.195986f), 0.0152041f);
    ret += circ(uv, float2(0.0438506f, 0.868153f), 0.0268601f);
    ret += circ(uv, float2(0.308619f, 0.194937f), 0.00806102f);
    ret += circ(uv, float2(0.349922f, 0.449714f), 0.00928667f);
    ret += circ(uv, float2(0.0449556f, 0.953415f), 0.023126f);
    ret += circ(uv, float2(0.117761f, 0.503309f), 0.0151272f);
    ret += circ(uv, float2(0.563517f, 0.244991f), 0.0292322f);
    ret += circ(uv, float2(0.566936f, 0.954457f), 0.00981141f);
    ret += circ(uv, float2(0.0489944f, 0.200931f), 0.0178746f);
    ret += circ(uv, float2(0.569297f, 0.624893f), 0.0132408f);
    ret += circ(uv, float2(0.298347f, 0.710972f), 0.0114426f);
    ret += circ(uv, float2(0.878141f, 0.771279f), 0.00322719f);
    ret += circ(uv, float2(0.150995f, 0.376221f), 0.00216157f);
    ret += circ(uv, float2(0.119673f, 0.541984f), 0.0124621f);
    ret += circ(uv, float2(0.629598f, 0.295629f), 0.0198736f);
    ret += circ(uv, float2(0.334357f, 0.266278f), 0.0187145f);
    ret += circ(uv, float2(0.918044f, 0.968163f), 0.0182928f);
    ret += circ(uv, float2(0.965445f, 0.505026f), 0.006348f);
    ret += circ(uv, float2(0.514847f, 0.865444f), 0.00623523f);
    ret += circ(uv, float2(0.710575f, 0.0415131f), 0.00322689f);
    ret += circ(uv, float2(0.71403f, 0.576945f), 0.0215641f);
    ret += circ(uv, float2(0.748873f, 0.413325f), 0.0110795f);
    ret += circ(uv, float2(0.0623365f, 0.896713f), 0.0236203f);
    ret += circ(uv, float2(0.980482f, 0.473849f), 0.00573439f);
    ret += circ(uv, float2(0.647463f, 0.654349f), 0.0188713f);
    ret += circ(uv, float2(0.651406f, 0.981297f), 0.00710875f);
    ret += circ(uv, float2(0.428928f, 0.382426f), 0.0298806f);
    ret += circ(uv, float2(0.811545f, 0.62568f), 0.00265539f);
    ret += circ(uv, float2(0.400787f, 0.74162f), 0.00486609f);
    ret += circ(uv, float2(0.331283f, 0.418536f), 0.00598028f);
    ret += circ(uv, float2(0.894762f, 0.0657997f), 0.00760375f);
    ret += circ(uv, float2(0.525104f, 0.572233f), 0.0141796f);
    ret += circ(uv, float2(0.431526f, 0.911372f), 0.0213234f);
    ret += circ(uv, float2(0.658212f, 0.910553f), 0.000741023f);
    ret += circ(uv, float2(0.514523f, 0.243263f), 0.0270685f);
    ret += circ(uv, float2(0.0249494f, 0.252872f), 0.00876653f);
    ret += circ(uv, float2(0.502214f, 0.47269f), 0.0234534f);
    ret += circ(uv, float2(0.693271f, 0.431469f), 0.0246533f);
    ret += circ(uv, float2(0.415f, 0.884418f), 0.0271696f);
    ret += circ(uv, float2(0.149073f, 0.41204f), 0.00497198f);
    ret += circ(uv, float2(0.533816f, 0.897634f), 0.00650833f);
    ret += circ(uv, float2(0.0409132f, 0.83406f), 0.0191398f);
    ret += circ(uv, float2(0.638585f, 0.646019f), 0.0206129f);
    ret += circ(uv, float2(0.660342f, 0.966541f), 0.0053511f);
    ret += circ(uv, float2(0.513783f, 0.142233f), 0.00471653f);
    ret += circ(uv, float2(0.124305f, 0.644263f), 0.00116724f);
    ret += circ(uv, float2(0.99871f, 0.583864f), 0.0107329f);
    ret += circ(uv, float2(0.894879f, 0.233289f), 0.00667092f);
    ret += circ(uv, float2(0.246286f, 0.682766f), 0.00411623f);
    ret += circ(uv, float2(0.0761895f, 0.16327f), 0.0145935f);
    ret += circ(uv, float2(0.949386f, 0.802936f), 0.0100873f);
    ret += circ(uv, float2(0.480122f, 0.196554f), 0.0110185f);
    ret += circ(uv, float2(0.896854f, 0.803707f), 0.013969f);
    ret += circ(uv, float2(0.292865f, 0.762973f), 0.00566413f);
    ret += circ(uv, float2(0.0995585f, 0.117457f), 0.00869407f);
    ret += circ(uv, float2(0.377713f, 0.00335442f), 0.0063147f);
    ret += circ(uv, float2(0.506365f, 0.531118f), 0.0144016f);
    ret += circ(uv, float2(0.408806f, 0.894771f), 0.0243923f);
    ret += circ(uv, float2(0.143579f, 0.85138f), 0.00418529f);
    ret += circ(uv, float2(0.0902811f, 0.181775f), 0.0108896f);
    ret += circ(uv, float2(0.780695f, 0.394644f), 0.00475475f);
    ret += circ(uv, float2(0.298036f, 0.625531f), 0.00325285f);
    ret += circ(uv, float2(0.218423f, 0.714537f), 0.00157212f);
    ret += circ(uv, float2(0.658836f, 0.159556f), 0.00225897f);
    ret += circ(uv, float2(0.987324f, 0.146545f), 0.0288391f);
    ret += circ(uv, float2(0.222646f, 0.251694f), 0.00092276f);
    ret += circ(uv, float2(0.159826f, 0.528063f), 0.00605293f);
    return max(ret, 0.f);
}

// Procedural texture generation for the water
float3 water(float2 uv, float3 cdir, float iTime)
{
    uv *= float2(0.25f, 0.25f);
    uv += fbm(uv) * 0.2f;

    // Parallax height distortion with two directional waves at
    // slightly different angles.
    float2 a = 0.025f * cdir.xz / cdir.y; // Parallax offset
    float h = sin(uv.x + iTime); // Height at UV
    uv += a * h;
    h = sin(0.841471f * uv.x - 0.540302f * uv.y + iTime);
    uv += a * h;
    
    // Texture distortion
    float d1 = fmod(uv.x + uv.y, M_2PI);
    float d2 = fmod((uv.x + uv.y + 0.25f) * 1.3f, M_6PI);
    d1 = iTime * 0.07f + d1;
    d2 = iTime * 0.5f + d2;
    float2 dist = float2(
    	sin(d1) * 0.15f + sin(d2) * 0.05f,
    	cos(d1) * 0.15f + cos(d2) * 0.05f
    );
    
    float3 ret = lerp(WATER_COL.rgb, WATER2_COL.rgb, waterlayer(uv + dist.xy));
    ret = lerp(ret, FOAM_COL.rgb, waterlayer(float2(1.f, 1.f) - uv - dist.yx));
    return ret;
}


struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 vertexNormal : NORMAL;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 vertexNormal : NORMAL;
};

//------------------------------------------------------
//Vertex Shader
//------------------------------------------------------
VertexOutput VertexShaderFunction(VertexInput input)
{
    float time = gTime * wave_speed;
    float2 modifiedUV = input.uv * wave_size;
    float d1 = fmod(modifiedUV.x + modifiedUV.y, M_2PI);
    float d2 = fmod((modifiedUV.x + modifiedUV.y + 0.25f) * 1.3f, M_6PI);
    d1 = time * 0.07f + d1;
    d2 = time * 0.5f + d2;
    float2 dist = float2(
        sin(d1) * 0.15f + sin(d2) * 0.05f,
        cos(d1) * 0.15f + cos(d2) * 0.05f
    );
    input.vertexNormal.y += dist.y * height;

    VertexOutput output;
    output.position = mul(input.position, gMatrixWVP);
    output.uv = input.uv;
    output.vertexNormal = input.vertexNormal;

    return output;
}

//------------------------------------------------------
//Pixel Shader
//------------------------------------------------------

float4 PixelShaderFunction(VertexOutput input) : SV_TARGET
{
    float3 albedo = water(input.uv * tile, float3(0.f, 1.f, 0.f), gTime * distortion_speed);
    return float4(albedo, 1.f);
}

// Technique
technique10 WaterTechnique
{
    pass P0
    {
        VertexShader = compile vs_4_0 VertexShaderFunction();
        PixelShader = compile ps_4_0 PixelShaderFunction();
    }
}

