/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > lllogfile.h
 Console Log and Lofile Manager
*/

#pragma once

#include "lltable.h"

namespace llama
{
    struct DebugInfo
    {
        const char* file;
        int line;
        const char* function;
    };

    #define LLAMA_DEBUG_INFO { __FILE__, __LINE__, __FUNCTION__ }

    class Logfile_T
    {
    public:

        virtual ~Logfile_T() = default;

        virtual void print(Color color, const char* format, ...) = 0;
        virtual void print(Color color, const DebugInfo& debugInfo, const char* format, ...) = 0;
        virtual void print(const Table& table, bool transpose = false) = 0;
    };

    typedef std::shared_ptr<Logfile_T> Logfile;

    // Creates a llama:Logfile object
    //
    // @name:       The title of the logfile which is shown in the title of the HTML file (set to application name)
    // @path:       The path where the HTML file es being crated
    LLAMA_API Logfile createLogfile(std::string_view name, std::string_view path);



    LLAMA_API Logfile& logfile();
}