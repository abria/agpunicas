cbuffer EffectParams : register(b0, space3) { float4 effect; };
Texture2D sceneTexture : register(t0, space2);
SamplerState sceneSampler : register(s0, space2);
struct PSInput { float4 color : COLOR0; float2 uv : TEXCOORD0; };

float4 main(PSInput input) : SV_Target
{
    float aspect = effect.y / effect.z;
    float2 p = (input.uv - 0.5) * float2(aspect, 1.0);
    float radius = length(p);
    float angle = 0.65 * exp(-radius * 3.2) + effect.x * 0.16;
    float cs = cos(angle);
    float sn = sin(angle);
    float2 rotated = float2(p.x * cs - p.y * sn, p.x * sn + p.y * cs);
    float2 uv = saturate(rotated / float2(aspect, 1.0) + 0.5);
    float aberration = 2.5 * exp(-radius * 2.0) / effect.y;
    float red = sceneTexture.Sample(sceneSampler, saturate(uv + float2(aberration, 0.0))).r;
    float4 scene = sceneTexture.Sample(sceneSampler, uv) * input.color;
    float blue = sceneTexture.Sample(sceneSampler, saturate(uv - float2(aberration, 0.0))).b;
    scene.rgb = saturate(float3(red, scene.g, blue) *
                         lerp(float3(1.10, 0.87, 1.2), float3(0.8, 1.0, 1.2), saturate(radius)));
    return scene;
}
