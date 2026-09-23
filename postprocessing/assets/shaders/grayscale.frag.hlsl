// effect.w controls the filter strength; 1 means fully grayscale.
cbuffer EffectParams : register(b0, space3) { float4 effect; };
Texture2D sceneTexture : register(t0, space2);
SamplerState sceneSampler : register(s0, space2);
struct PSInput { float4 color : COLOR0; float2 uv : TEXCOORD0; };

float4 main(PSInput input) : SV_Target
{
    float4 scene = sceneTexture.Sample(sceneSampler, input.uv) * input.color;
    // Green contributes more to perceived brightness than red or blue.
    float gray = dot(scene.rgb, float3(0.299, 0.587, 0.114));
    // Blend between the original RGB and the computed gray value.
    scene.rgb = lerp(scene.rgb, float3(gray, gray, gray), effect.w);
    return scene;
}
