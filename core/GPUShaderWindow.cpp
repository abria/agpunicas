// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria
// Released under the BSD License
// ----------------------------------------------------------------

#ifdef WITH_SHADERS

#include "GPUShaderWindow.h"
#include "Scene.h"
#include <algorithm>
#include <cstring>
#include <stdexcept>

using namespace agp;

GPUShaderWindow::GPUShaderWindow(const std::string& title, int width, int height)
    : Window(title, width, height), _device(nullptr), _sceneTarget(nullptr),
      _effectTarget(nullptr), _cpuResult(nullptr), _targetWidth(0), _targetHeight(0), _startTime(0)
{
}

GPUShaderWindow::~GPUShaderWindow()
{
    for (auto& pair : _shaders)
    {
        SDL_DestroyGPURenderState(pair.second.state);
        SDL_ReleaseGPUShader(_device, pair.second.shader);
    }
    SDL_DestroyTexture(_effectTarget);
    SDL_DestroyTexture(_sceneTarget);
    SDL_DestroyTexture(_cpuResult);
}

void GPUShaderWindow::initRenderer()
{
    _renderer = SDL_CreateGPURenderer(nullptr, _window);
    if (!_renderer)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "SDL_GPU unavailable (%s); shaders disabled", SDL_GetError());
        Window::initRenderer();
        return;
    }

    _device = SDL_GetGPURendererDevice(_renderer);
    if (!_device || !SDL_SetRenderVSync(_renderer, 1) ||
        !SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND))
        throw std::runtime_error(SDL_GetError());

    _startTime = SDL_GetTicksNS();
    resize(_width, _height);
}

void GPUShaderWindow::loadShader(const std::string& name, const std::string& assetPath)
{
    if (!_device)
        return;

    if (_shaders.count(name))
        return;

    struct ShaderFormat
    {
        SDL_GPUShaderFormat format;
        const char* extension;
        const char* entrypoint;
        bool text;
    };
    const ShaderFormat formats[] = {
        { SDL_GPU_SHADERFORMAT_SPIRV, ".spv", "main", false },
        { SDL_GPU_SHADERFORMAT_DXIL, ".dxil", "main", false },
        { SDL_GPU_SHADERFORMAT_MSL, ".msl", "main0", true }
    };

    const char* basePath = SDL_GetBasePath();
    if (!basePath)
        throw std::runtime_error(SDL_GetError());

    SDL_GPUShaderFormat supported = SDL_GetGPUShaderFormats(_device);
    for (const auto& candidate : formats)
    {
        if (!(supported & candidate.format))
            continue;

        std::string path = std::string(basePath) + assetPath + candidate.extension;
        size_t size = 0;
        void* file = SDL_LoadFile(path.c_str(), &size);
        if (!file)
            continue;

        std::vector<Uint8> code(size + (candidate.text ? 1 : 0));
        std::memcpy(code.data(), file, size);
        SDL_free(file);
        if (candidate.text)
            code[size] = 0;

        SDL_GPUShaderCreateInfo info{};
        info.code = code.data();
        info.code_size = code.size();
        info.entrypoint = candidate.entrypoint;
        info.format = candidate.format;
        info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
        info.num_samplers = 1;
        info.num_uniform_buffers = 1;

        SDL_GPUShader* shader = SDL_CreateGPUShader(_device, &info);
        if (!shader)
            throw std::runtime_error("Cannot load shader " + path + ": " + SDL_GetError());

        SDL_GPURenderStateCreateInfo stateInfo{};
        stateInfo.fragment_shader = shader;
        SDL_GPURenderState* state = SDL_CreateGPURenderState(_renderer, &stateInfo);
        if (!state)
        {
            std::string error = SDL_GetError();
            SDL_ReleaseGPUShader(_device, shader);
            throw std::runtime_error("Cannot create shader state " + path + ": " + error);
        }

        _shaders.emplace(name, ShaderPass{ shader, state });
        return;
    }

    throw std::runtime_error("No compatible shader asset found for " + assetPath);
}

void GPUShaderWindow::applyShader(const std::string& name)
{
    if (!_device)
        return;

    if (!_shaders.count(name))
        throw std::invalid_argument("Shader not loaded: " + name);
    if (!shaderActive(name))
        _activeShaders.push_back(name);
}

void GPUShaderWindow::removeShader(const std::string& name)
{
    _activeShaders.erase(std::remove(_activeShaders.begin(), _activeShaders.end(), name),
                         _activeShaders.end());
}

bool GPUShaderWindow::shaderActive(const std::string& name) const
{
    return std::find(_activeShaders.begin(), _activeShaders.end(), name) != _activeShaders.end();
}

void GPUShaderWindow::ensureTargets()
{
    int width = 0;
    int height = 0;
    if (!SDL_GetCurrentRenderOutputSize(_renderer, &width, &height))
        throw std::runtime_error(SDL_GetError());
    if (width == _targetWidth && height == _targetHeight)
        return;

    SDL_DestroyTexture(_effectTarget);
    SDL_DestroyTexture(_sceneTarget);
    SDL_DestroyTexture(_cpuResult);
    _effectTarget = nullptr;
    _sceneTarget = nullptr;
    _cpuResult = nullptr;

    _sceneTarget = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_TARGET, width, height);
    _effectTarget = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888,
                                      SDL_TEXTUREACCESS_TARGET, width, height);
    _cpuResult = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!_sceneTarget || !_effectTarget || !_cpuResult ||
        !SDL_SetTextureBlendMode(_sceneTarget, SDL_BLENDMODE_NONE) ||
        !SDL_SetTextureBlendMode(_effectTarget, SDL_BLENDMODE_NONE) ||
        !SDL_SetTextureBlendMode(_cpuResult, SDL_BLENDMODE_NONE))
        throw std::runtime_error(SDL_GetError());

    _targetWidth = width;
    _targetHeight = height;
}

void GPUShaderWindow::render(const std::vector<Scene*>& scenes)
{
    if (_activeShaders.empty() && !_cpuShader)
    {
        Window::render(scenes);
        return;
    }

    ensureTargets();
    if (!SDL_SetRenderTarget(_renderer, _sceneTarget) ||
        !SDL_SetRenderClipRect(_renderer, nullptr) ||
        !SDL_SetRenderViewport(_renderer, nullptr))
        throw std::runtime_error(SDL_GetError());

    SDL_SetRenderDrawColor(_renderer, _color.r, _color.g, _color.b, 255);
    SDL_RenderClear(_renderer);
    for (auto scene : scenes)
        scene->render();

    SDL_Texture* source = _sceneTarget;
    if (_cpuShader)
    {
        SDL_Surface* pixels = SDL_RenderReadPixels(_renderer, nullptr);
        if (!pixels)
            throw std::runtime_error(SDL_GetError());
        if (pixels->format != SDL_PIXELFORMAT_RGBA8888)
        {
            SDL_Surface* converted = SDL_ConvertSurface(pixels, SDL_PIXELFORMAT_RGBA8888);
            SDL_DestroySurface(pixels);
            pixels = converted;
            if (!pixels)
                throw std::runtime_error(SDL_GetError());
        }

        _cpuShader(static_cast<Uint32*>(pixels->pixels), pixels->w, pixels->h, pixels->pitch);
        bool updated = SDL_UpdateTexture(_cpuResult, nullptr, pixels->pixels, pixels->pitch);
        SDL_DestroySurface(pixels);
        if (!updated)
            throw std::runtime_error(SDL_GetError());
        source = _cpuResult;
    }

    if (_activeShaders.empty())
    {
        if (!SDL_SetRenderTarget(_renderer, nullptr) ||
            !SDL_SetRenderClipRect(_renderer, nullptr) ||
            !SDL_SetRenderViewport(_renderer, nullptr) ||
            !SDL_RenderTexture(_renderer, source, nullptr, nullptr) ||
            !SDL_RenderPresent(_renderer))
            throw std::runtime_error(SDL_GetError());
        return;
    }

    for (size_t i = 0; i < _activeShaders.size(); ++i)
    {
        bool last = i + 1 == _activeShaders.size();
        SDL_Texture* target = last ? nullptr :
            (source == _sceneTarget ? _effectTarget : _sceneTarget);
        if (!SDL_SetRenderTarget(_renderer, target) ||
            !SDL_SetRenderClipRect(_renderer, nullptr) ||
            !SDL_SetRenderViewport(_renderer, nullptr))
            throw std::runtime_error(SDL_GetError());

        SDL_SetRenderDrawColor(_renderer, _color.r, _color.g, _color.b, 255);
        SDL_RenderClear(_renderer);

        ShaderPass& pass = _shaders.at(_activeShaders[i]);
        float uniforms[4] = {
            float(SDL_GetTicksNS() - _startTime) / 1000000000.0f,
            float(_targetWidth), float(_targetHeight), 1.0f
        };
        if (!SDL_SetGPURenderStateFragmentUniforms(pass.state, 0, uniforms, sizeof(uniforms)) ||
            !SDL_SetGPURenderState(_renderer, pass.state))
            throw std::runtime_error(SDL_GetError());

        bool rendered = SDL_RenderTexture(_renderer, source, nullptr, nullptr);
        SDL_SetGPURenderState(_renderer, nullptr);
        if (!rendered)
            throw std::runtime_error(SDL_GetError());
        source = target;
    }

    if (!SDL_RenderPresent(_renderer))
        throw std::runtime_error(SDL_GetError());
}

#endif
