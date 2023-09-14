float4x4 WorldViewProj : WorldViewProjection;

float4 gDepthGradientShallow = (0.325, 0.807, 0.971, 0.725);
float4 gDepthGradientDeep = (0.086, 0.407, 1, 0.749);

float gDepthMaxDistance = 1;

sampler2D gCameraDepthTexture;



struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

//------------------------------------------------------
//Functions
//------------------------------------------------------
float LinearEyeDepth(float depth01)
{
    float depth = depth01 * gDepthMaxDistance;
    float near = 0.1f;
    float far = 1000.0f;
    float z = depth * (far - near) + near;
    return z;
}

//------------------------------------------------------
//Vertex Shader
//------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    //calculate pos in screenspace
    output.position = mul(float4(input.position, 1.0f), WorldViewProj);
    return output;
}

//------------------------------------------------------
//Pixel Shader
//------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    //samples the depth texture using tex2Dproj and our screen position. 
    //This will return the depth of the surface behind our water, in a range of 0 to 1. This value is non-linear
    float existingDepth01 = tex2Dproj(gCameraDepthTexture, input.position).r;
    
    //converts the non-linear depth to be linear, in world units from the camera.
    float existingDepthLinear = LinearEyeDepth(existingDepth01);
    
    float depthDifference = existingDepthLinear - input.position.w;

    return float4(1.f, 1.f, 1.f, depthDifference);
}

//------------------------------------------------------
//Technique
//------------------------------------------------------
technique technique0
{
    pass p0
    {
        CullMode = None;
        VertexShader = compile vs_3_0 VS();
        PixelShader = compile ps_3_0 PS();
    }
}
