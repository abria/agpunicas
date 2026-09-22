cbuffer EffectParams : register(b0, space3) { float4 effect; };
Texture2D sceneTexture : register(t0, space2);
SamplerState sceneSampler : register(s0, space2);
struct PSInput { float4 color : COLOR0; float2 uv : TEXCOORD0; };

float4 main(PSInput input) : SV_Target
{
    float2 block = float2(6.0, 6.0);
    float2 cell = floor(input.uv * effect.yz / block);
    float2 uv = (cell + 0.5) * block / effect.yz;
    float4 scene = sceneTexture.Sample(sceneSampler, uv) * input.color;
    float dither = frac(sin(dot(cell, float2(12.9898, 78.233))) * 43758.5453);
    scene.rgb = floor(saturate(scene.rgb) * 6.0 + dither * 0.8) / 6.0;
    float grid = step(0.18, frac(input.uv.x * effect.y / block.x)) *
                 step(0.18, frac(input.uv.y * effect.z / block.y));
    scene.rgb *= lerp(0.80, 1.0, grid);
    return scene;
}
