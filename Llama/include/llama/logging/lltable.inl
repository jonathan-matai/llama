/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > lltable.inl
 Implementation for Table Class for Logfile
*/

#pragma once

llama::Table::Table(std::string_view title, Color color, std::initializer_list<std::string_view> headers) :
    m_title(title)
{
    m_data.push_back(std::vector<std::pair<Color, std::string>>(headers.size()));

    auto a = headers.begin();
    auto b = m_data.rbegin()->begin();

    for (; a != headers.end(); ++a, ++b)
        *b = std::pair(color, *a);
}

inline void llama::Table::addRow(Color color, std::initializer_list<std::string_view> row)
{
    m_data.push_back(std::vector<std::pair<Color, std::string>>(row.size()));

    auto a = row.begin();
    auto b = m_data.rbegin()->begin();

    for (; a != row.end(); ++a, ++b)
        *b = std::pair(color, *a);
}

inline void llama::Table::addRow(std::initializer_list<std::pair<Color, std::string_view>> row)
{
    m_data.push_back(std::vector<std::pair<Color, std::string>>(row.size()));

    auto a = row.begin();
    auto b = m_data.rbegin()->begin();

    for (; a != row.end(); ++a, ++b)
        *b = std::pair(a->first, a->second);
}
