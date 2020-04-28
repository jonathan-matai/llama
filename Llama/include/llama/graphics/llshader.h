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

#include "llwindowcontext.h"

namespace llama
{
    class Shader_T
    {
    public:

        virtual ~Shader_T() = default;

    };

    typedef std::shared_ptr<Shader_T> Shader;
    
    LLAMA_API Shader createShader(WindowContext context, std::string_view vertexShaderSpv, std::string_view fragmentShaderSpv);
}
