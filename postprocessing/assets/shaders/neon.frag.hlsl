cbuffer EffectParams : register(b0, space3) { float4 effect; };
Texture2D sceneTexture : register(t0, space2);
SamplerState sceneSampler : register(s0, space2);
struct PSInput { float4 color : COLOR0; float2 uv : TEXCOORD0; };

// Weighted RGB brightness approximates how bright a pixel looks.
float luminance(float3 color) { return dot(color, float3(0.299, 0.587, 0.114)); }

float4 main(PSInput input) : SV_Target
{
    // Brightness differences between neighbors reveal image edges.
    float2 pixel = 1.5 / effect.yz;
    float4 scene = sceneTexture.Sample(sceneSampler, input.uv) * input.color;
    float left = luminance(sceneTexture.Sample(sceneSampler, input.uv - float2(pixel.x, 0.0)).rgb);
    float right = luminance(sceneTexture.Sample(sceneSampler, input.uv + float2(pixel.x, 0.0)).rgb);
    float up = luminance(sceneTexture.Sample(sceneSampler, input.uv - float2(0.0, pixel.y)).rgb);
    float down = luminance(sceneTexture.Sample(sceneSampler, input.uv + float2(0.0, pixel.y)).rgb);
    float2 gradient = float2(right - left, down - up);
    float edge = saturate(length(gradient) * 2.3);
    // Wider samples create a softer halo around those edges.
    float2 wide = 4.0 / effect.yz;
    float haloX = luminance(sceneTexture.Sample(sceneSampler, input.uv + float2(wide.x, 0.0)).rgb) -
                  luminance(sceneTexture.Sample(sceneSampler, input.uv - float2(wide.x, 0.0)).rgb);
    float haloY = luminance(sceneTexture.Sample(sceneSampler, input.uv + float2(0.0, wide.y)).rgb) -
                  luminance(sceneTexture.Sample(sceneSampler, input.uv - float2(0.0, wide.y)).rgb);
    float halo = saturate(length(float2(haloX, haloY)) * 1.2);
    float3 hue = lerp(float3(0.04, 0.94, 1.0), float3(1.0, 0.13, 0.68),
                      saturate(gradient.x * 2.0 + 0.5));
    scene.rgb = saturate(scene.rgb * 0.17 + hue * (edge * 1.35 + halo * 0.65));
    return scene;
}
