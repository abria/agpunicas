cbuffer EffectParams : register(b0, space3) { float4 effect; };
Texture2D sceneTexture : register(t0, space2);
SamplerState sceneSampler : register(s0, space2);
struct PSInput { float4 color : COLOR0; float2 uv : TEXCOORD0; };

float4 main(PSInput input) : SV_Target
{
    // Turn RGB into brightness before tinting it green.
    float4 scene = sceneTexture.Sample(sceneSampler, input.uv) * input.color;
    float luma = dot(scene.rgb, float3(0.299, 0.587, 0.114));
    // Animated grain, scanlines, and darkened corners mimic a camera signal.
    float noise = frac(sin(dot(floor(input.uv * effect.yz) + effect.x * 17.0,
                               float2(12.9898, 78.233))) * 43758.5453) - 0.5;
    float2 center = input.uv * 2.0 - 1.0;
    float vignette = saturate(1.2 - dot(center, center) * 0.55);
    float scan = 0.92 + 0.08 * sin(input.uv.y * effect.z * 2.4);
    float signal = saturate(pow(luma, 0.7) * 1.55 + noise * 0.12) * vignette * scan;
    scene.rgb = signal * float3(0.18, 1.0, 0.38);
    return scene;
}
