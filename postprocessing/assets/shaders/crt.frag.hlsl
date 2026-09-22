cbuffer EffectParams : register(b0, space3) { float4 effect; };
Texture2D sceneTexture : register(t0, space2);
SamplerState sceneSampler : register(s0, space2);
struct PSInput { float4 color : COLOR0; float2 uv : TEXCOORD0; };

float4 main(PSInput input) : SV_Target
{
    float2 centered = input.uv * 2.0 - 1.0;
    float2 warped = centered * (1.0 + 0.075 * dot(centered, centered));
    float2 uv = warped * 0.5 + 0.5;
    if (any(uv < 0.0) || any(uv > 1.0))
        return float4(0.01, 0.01, 0.015, 1.0);

    float offset = 1.2 / effect.y;
    float red = sceneTexture.Sample(sceneSampler, uv + float2(offset, 0.0)).r;
    float green = sceneTexture.Sample(sceneSampler, uv).g;
    float blue = sceneTexture.Sample(sceneSampler, uv - float2(offset, 0.0)).b;
    float scan = 0.84 + 0.16 * sin(uv.y * effect.z * 3.14159265);
    float triad = frac(floor(uv.x * effect.y) / 3.0) * 3.0;
    float3 mask = float3(0.84, 0.84, 0.84);
    mask += 0.16 * float3(1.0 - step(0.5, triad),
                            step(0.5, triad) * (1.0 - step(1.5, triad)),
                            step(1.5, triad));
    float vignette = saturate(1.15 - dot(centered, centered) * 0.28);
    float flicker = 0.98 + 0.02 * sin(effect.x * 47.0);
    return float4(float3(red, green, blue) * mask * scan * vignette * flicker,
                  1.0) * input.color;
}
