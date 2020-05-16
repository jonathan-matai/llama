/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llshader.h
 Contains the Shader Class
*/

#pragma once

namespace llama
{
    using Renderer = std::shared_ptr<class Renderer_T>;

    class Shader_T
    {
    public:

        virtual ~Shader_T() = default;

    };

    using Shader = std::shared_ptr<Shader_T>;
    
    LLAMA_API Shader createShader(Renderer renderer, std::string_view descriptorFilePath);
}
