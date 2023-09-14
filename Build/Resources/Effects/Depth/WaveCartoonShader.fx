float4x4 WorldViewProj : WorldViewProjection;
float4 gDepthGradientShallow = { 0.325, 0.807, 0.971, 0.725 };
float4 gDepthGradientDeep = { 0.086, 0.407, 1, 0.749 };

float gDepthMaxDistance = 0.5f;
Texture2D gCameraDepthTexture;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
    
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 noiseUV : TEXCOORD0;
    float2 distortUV : TEXCOORD1;
};



Texture2D gSurfaceNoise;
Texture2D gSurfaceDistortion;
float gSurfaceDistortionAmount = 0.27f;
float4 gSurfaceDistortion_ST = float4(1.f, 1.f, 0.f, 0.f);

float4 gSurfaceNoise_ST = float4(1.f, 1.f, 0.f, 0.f);
SamplerState gSurfaceNoiseSampler;
float gSurfaceNoiseCutoff = 0.77f;
float gFoamDistance = 0.4f;
float2 gSurfaceNoiseScroll = float2(0.03f, 0.03f);
float gTime;

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
    // Calculate pos in screenspace
    output.position = mul(float4(input.position, 1.0f), WorldViewProj);
    
    // Calculate the noise UV using DirectX 11 functions
    float2 noiseUV = input.uv;
    noiseUV = noiseUV * gSurfaceNoise_ST.xy + gSurfaceNoise_ST.zw; // Apply texture transformation
    
    // Pass the noise UV to the pixel shader
    output.noiseUV = noiseUV;
    
    //distortion
    output.distortUV = input.uv * gSurfaceDistortion_ST.xy + gSurfaceDistortion_ST.zw;
    
    return output;
}

//------------------------------------------------------
//Pixel Shader
//------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    
    float existingDepth01 = gCameraDepthTexture.SampleLevel(samLinear, input.position.xy / input.position.w, 1).r;
    float existingDepthLinear = LinearEyeDepth(existingDepth01);
    
    float depthDifference = existingDepthLinear - input.position.w;
    
    float waterDepthDifference01 = saturate(gDepthMaxDistance);
    float4 waterColor = lerp(gDepthGradientShallow, gDepthGradientDeep, waterDepthDifference01);
    
    
    //float4 waterColor = float4(102.f / 255.f, 153.f / 255.f, 204.f / 255.f, 1.f); // Darker blue sea color
    //Noise
    
    // Distortion
    float2 distortUV = input.distortUV;
    float surfaceDistortionSample = gSurfaceDistortion.Sample(gSurfaceNoiseSampler, distortUV).r;
    float2 distortSample = (gSurfaceDistortion.Sample(gSurfaceNoiseSampler, distortUV).xy * 2 - 1) * gSurfaceDistortionAmount;

    // Increase the tiling of the distortion
   //distortSample *= 1.5f;

    float2 noiseUV = float2((input.noiseUV.x + gTime * gSurfaceNoiseScroll.x) + distortSample.x, (input.noiseUV.y + gTime * gSurfaceNoiseScroll.y) + distortSample.y);

    // Increase the tiling of the surface noise
    //noiseUV *= 1.5f;
    
    
    
    float surfaceNoiseSample = gSurfaceNoise.Sample(gSurfaceNoiseSampler, noiseUV).r;

    float foamDepthDifference01 = saturate(depthDifference / gFoamDistance);
    float surfaceNoiseCutoff = foamDepthDifference01 * gSurfaceNoiseCutoff;

    

    float surfaceNoise = surfaceNoiseSample > surfaceNoiseCutoff ? 1 : 0;
  
    // Calculate the foam color
    float4 foamColor = float4(1.f, 1.f, 1.f, 1.f); // White foam color
    
    // Mix the water color with the foam color based on the surface noise
    float4 finalColor = lerp(waterColor, foamColor, surfaceNoise);

    return finalColor;
}
//------------------------------------------------------
//Technique
//------------------------------------------------------
technique10 Default
{
    pass p0
    {
        VertexShader = compile vs_4_0 VS();
        PixelShader = compile ps_4_0 PS();
    }
}
