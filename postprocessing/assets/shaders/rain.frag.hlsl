cbuffer EffectParams : register(b0, space3)
{
    float4 effect;
};

Texture2D sceneTexture : register(t0, space2);
SamplerState sceneSampler : register(s0, space2);

struct PSInput
{
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

// Repeatable pseudo-random value for a column, drop, or lightning cycle.
float hash(float2 p)
{
    return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453);
}

// Draw short slanted streaks that move down separate columns.
float rainLayer(float2 pixel, float time, float spacing, float speed, float slant)
{
    float columnPosition = (pixel.x + pixel.y * slant) / spacing;
    float column = floor(columnPosition);
    float lane = abs(frac(columnPosition) - 0.5);
    float fallPosition = (pixel.y - time * speed + hash(float2(column, 0.0)) * 190.0) / 190.0;
    float segment = floor(fallPosition);
    float tail = 1.0 - smoothstep(0.10, 0.48, frac(fallPosition));
    float streak = 1.0 - smoothstep(0.025, 0.10, lane);
    float present = step(0.64, hash(float2(column, segment)));
    return streak * tail * present;
}

float4 main(PSInput input) : SV_Target
{
    float2 pixel = input.uv * effect.yz;
    float rain = rainLayer(pixel, effect.x, 11.0, 490.0, 0.20);
    rain += rainLayer(pixel + float2(43.0, 71.0), effect.x, 17.0, 345.0, 0.14) * 0.72;
    rain += rainLayer(pixel + float2(119.0, 27.0), effect.x, 27.0, 245.0, 0.10) * 0.42;

    // Each five-second cycle can contain one brief, sometimes double, flash.
    float cycle = floor(effect.x / 5.0);
    float localTime = effect.x - cycle * 5.0;
    float strikeTime = 0.6 + hash(float2(cycle, 19.0)) * 3.3;
    float flash = step(0.56, hash(float2(cycle, 71.0))) *
                  saturate(exp(-abs(localTime - strikeTime) * 24.0) +
                           0.45 * exp(-abs(localTime - strikeTime - 0.16) * 35.0));
    float glow = 0.70 + 0.30 * exp(-abs(input.uv.x - hash(float2(cycle, 31.0))) * 4.0);

    float4 scene = sceneTexture.Sample(sceneSampler, input.uv) * input.color;
    scene.rgb = saturate(scene.rgb * (1.0 - 0.18 * effect.w) +
                         float3(0.68, 0.82, 0.98) * rain * 0.58 * effect.w +
                         float3(0.52, 0.63, 0.78) * flash * glow * effect.w);
    return scene;
}
