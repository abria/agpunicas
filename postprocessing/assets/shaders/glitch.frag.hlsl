cbuffer EffectParams : register(b0, space3) { float4 effect; };
Texture2D sceneTexture : register(t0, space2);
SamplerState sceneSampler : register(s0, space2);
struct PSInput { float4 color : COLOR0; float2 uv : TEXCOORD0; };

// Repeatable pseudo-random choice for each row and time step.
float hash(float2 p) { return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453); }

float4 main(PSInput input) : SV_Target
{
    // Move selected horizontal strips, then offset the RGB channels.
    float frame = floor(effect.x * 9.0);
    float row = floor(input.uv.y * effect.z / 18.0);
    float tear = step(0.58, hash(float2(row, frame)));
    float displacement = (hash(float2(row + 47.0, frame)) - 0.5) * 0.07 * tear;
    float2 uv = saturate(input.uv + float2(displacement, 0.0));
    float split = (2.0 + 7.0 * tear) / effect.y;
    float red = sceneTexture.Sample(sceneSampler, saturate(uv + float2(split, 0.0))).r;
    float green = sceneTexture.Sample(sceneSampler, uv).g;
    float blue = sceneTexture.Sample(sceneSampler, saturate(uv - float2(split, 0.0))).b;
    float stripe = step(0.90, hash(float2(floor(input.uv.y * effect.z / 3.0), frame)));
    float3 color = float3(red, green, blue) * (1.0 - stripe * 0.25);
    color += tear * float3(0.07, 0.00, 0.10);
    return float4(saturate(color), 1.0) * input.color;
}
