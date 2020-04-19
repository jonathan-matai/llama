/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > lltable.h
 Table Class for Logfile
*/

#pragma once

namespace llama
{
    class Table
    {
        friend class Logfile_I;

    public:

        Table(std::string_view title, Color color, std::initializer_list<std::string_view> headers);

        inline void addRow(Color color, std::initializer_list<std::string_view> row);
        inline void addRow(std::initializer_list<std::pair<Color, std::string_view>> row);

    private:

        std::string m_title;
        std::vector<std::vector<std::pair<Color, std::string>>> m_data;
    };
}

#include "lltable.inl"