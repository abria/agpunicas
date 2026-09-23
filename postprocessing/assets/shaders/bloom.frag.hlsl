cbuffer EffectParams : register(b0, space3) { float4 effect; };
Texture2D sceneTexture : register(t0, space2);
SamplerState sceneSampler : register(s0, space2);
struct PSInput { float4 color : COLOR0; float2 uv : TEXCOORD0; };

// Keep highlights only, so dark areas do not add glow.
float3 bright(float3 color)
{
    float luma = dot(color, float3(0.299, 0.587, 0.114));
    return color * smoothstep(0.35, 0.85, luma);
}

float4 main(PSInput input) : SV_Target
{
    // Blend nearby highlights at two distances into the original pixel.
    float2 pixel = 4.0 / effect.yz;
    float4 scene = sceneTexture.Sample(sceneSampler, input.uv) * input.color;
    float3 glow = bright(scene.rgb) * 0.25;
    glow += bright(sceneTexture.Sample(sceneSampler, input.uv + float2(pixel.x, 0.0)).rgb) * 0.12;
    glow += bright(sceneTexture.Sample(sceneSampler, input.uv - float2(pixel.x, 0.0)).rgb) * 0.12;
    glow += bright(sceneTexture.Sample(sceneSampler, input.uv + float2(0.0, pixel.y)).rgb) * 0.12;
    glow += bright(sceneTexture.Sample(sceneSampler, input.uv - float2(0.0, pixel.y)).rgb) * 0.12;
    pixel *= 2.0;
    glow += bright(sceneTexture.Sample(sceneSampler, input.uv + pixel).rgb) * 0.07;
    glow += bright(sceneTexture.Sample(sceneSampler, input.uv - pixel).rgb) * 0.07;
    glow += bright(sceneTexture.Sample(sceneSampler, input.uv + float2(pixel.x, -pixel.y)).rgb) * 0.07;
    glow += bright(sceneTexture.Sample(sceneSampler, input.uv + float2(-pixel.x, pixel.y)).rgb) * 0.07;
    scene.rgb = saturate(scene.rgb * 0.85 + glow * float3(1.15, 0.95, 1.25));
    return scene;
}
