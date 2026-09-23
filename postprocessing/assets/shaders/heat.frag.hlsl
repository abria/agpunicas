cbuffer EffectParams : register(b0, space3) { float4 effect; };
Texture2D sceneTexture : register(t0, space2);
SamplerState sceneSampler : register(s0, space2);
struct PSInput { float4 color : COLOR0; float2 uv : TEXCOORD0; };

float4 main(PSInput input) : SV_Target
{
    // Time-varying sine waves shift the sampled position like hot air.
    float2 pixel = input.uv * effect.yz;
    float wave = sin(pixel.y * 0.052 - effect.x * 4.3 + sin(pixel.x * 0.018 + effect.x) * 2.0);
    wave += 0.5 * sin(pixel.y * 0.10 + effect.x * 6.1);
    float strength = (1.0 - input.uv.y * 0.45) * 5.0 / effect.y;
    float2 shift = float2(wave * strength, sin(pixel.x * 0.025 - effect.x * 2.0) * 1.5 / effect.z);
    float2 uv = saturate(input.uv + shift);
    // Sampling red and blue at different positions adds color fringing.
    float red = sceneTexture.Sample(sceneSampler, saturate(uv + float2(1.2 / effect.y, 0.0))).r;
    float4 scene = sceneTexture.Sample(sceneSampler, uv) * input.color;
    float blue = sceneTexture.Sample(sceneSampler, saturate(uv - float2(1.2 / effect.y, 0.0))).b;
    scene.rgb = saturate(float3(red, scene.g, blue) * float3(1.08, 0.97, 0.88));
    return scene;
}
