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

float hash(float2 p)
{
    return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453);
}

float noise(float2 p)
{
    float2 cell = floor(p);
    float2 local = frac(p);
    local = local * local * (3.0 - 2.0 * local);
    return lerp(lerp(hash(cell), hash(cell + float2(1.0, 0.0)), local.x),
                lerp(hash(cell + float2(0.0, 1.0)), hash(cell + 1.0), local.x), local.y);
}

float4 main(PSInput input) : SV_Target
{
    float aspect = effect.y / effect.z;
    float2 position = float2(input.uv.x * aspect, input.uv.y);
    float2 drift = float2(effect.x * 0.24, effect.x * 0.07);
    float billow = noise(position * 3.0 + drift);
    float detail = noise(position * 7.0 - drift * 0.7);
    float density = saturate((billow * 0.75 + detail * 0.25 - 0.25) * 1.7);
    float lowerScreen = smoothstep(0.05, 0.95, input.uv.y);
    float amount = density * (0.55 + 0.45 * lowerScreen) * 0.80 * effect.w;

    float4 scene = sceneTexture.Sample(sceneSampler, input.uv) * input.color;
    scene.rgb = lerp(scene.rgb, float3(0.73, 0.78, 0.80), amount);
    return scene;
}
