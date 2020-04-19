#include "llcore.h"

#include <cstdarg>
#include <locale>

namespace llama
{
    class Logfile_I : public Logfile_T
    {
    public:

        Logfile_I(std::string_view name, std::string_view path);
        ~Logfile_I() override;

    private:

        void print(Color color, const char* format, ...) override;
        void print(Color color, const DebugInfo& debugInfo, const char* format, ...) override;
        void print(const Table& table, bool transpose) override;

        FILE* m_fileHandle;
        std::mutex m_printMutex;
    };
}

llama::Logfile llama::createLogfile(std::string_view name, std::string_view path)
{
    return std::make_shared<Logfile_I>(name, path);
}

llama::Logfile& llama::logfile()
{
    static Logfile s_logfile = createLogfile("Llama Program", "log.html");

    return s_logfile;
}

llama::Logfile_I::Logfile_I(std::string_view name, std::string_view path)
{
    std::locale::global(std::locale("en_us.UTF8"));

    m_fileHandle = fopen(std::string(path).c_str(), "w");

    // Log error, if logfile couldn't be opened
    if (m_fileHandle == nullptr)
    {
        printf("\x1b[38;2;255;255;0m[%s] Couldn't open or create logfile at path\"%s\"\x1b[0m\n",
            Timestamp().getString("%X", TimeAccuracy::MILLISECONDS).c_str(), std::string(name).c_str());
        return;
    }

    std::string applicationName(name);

    fprintf(m_fileHandle,
        "<style>body{background-color:#222;color:#eee;font-family:'Cascadia Code','Consolas',monospace;}th,td{padding-right:50px;text-align:left;}</style><head>\
<meta charset=\"utf-8\"><title>%s (Logfile)</title></head><body><h1>%s (Logfile)</h1>\
<table><tr><td><b>Operating System</b></td><td>%s</td></tr>\
<tr><td><b>Configuration</b></td><td>%s</td></tr>\
<tr><td><b>Date</b></td><td>%s</td></tr></table><hr>",
applicationName.c_str(), applicationName.c_str(), LLAMA_OS, LLAMA_CONFIG, Timestamp().getString("%F %T", TimeAccuracy::MILLISECONDS).c_str());

    fflush(m_fileHandle);
}

llama::Logfile_I::~Logfile_I()
{
    fprintf(m_fileHandle, "<hr></body>");
    fflush(m_fileHandle);
    fclose(m_fileHandle);
}

void llama::Logfile_I::print(Color color, const char* format, ...)
{
    char buffer[4096];
    va_list list;
    va_start(list, format);
    vsnprintf(buffer, 4096, format, list);
    va_end(list);

    std::string time = Timestamp().getString("%T", TimeAccuracy::MILLISECONDS);

    if (m_fileHandle)
    {
        fprintf(m_fileHandle, "<p style=\"color:#%02x%02x%02x;\">[%s] %s</p>", color.r, color.g, color.b, time.c_str(), buffer);
        fflush(m_fileHandle);
    }

    printf("\x1b[38;2;%u;%u;%um[%s] %s\x1b[0m\n\n", color.r, color.g, color.b, time.c_str(), buffer);
}

void llama::Logfile_I::print(Color color, const DebugInfo& debugInfo, const char* format, ...)
{
    char buffer[4096];
    va_list list;
    va_start(list, format);
    vsnprintf(buffer, 4096, format, list);
    va_end(list);

    std::string time = Timestamp().getString("%T", TimeAccuracy::MILLISECONDS);
    std::string file = std::filesystem::path(debugInfo.file).filename().string();

    if (m_fileHandle)
    {
        fprintf(m_fileHandle, "<p style=\"color:#%02x%02x%02x;\">[%s] %s:%d (%s) : %s</p>",
            color.r, color.g, color.b, time.c_str(), file.c_str(), debugInfo.line, debugInfo.function, buffer);
        fflush(m_fileHandle);
    }

    printf("\x1b[38;2;%u;%u;%um[%s] %s:%d (%s) : %s\x1b[0m\n\n",
        color.r, color.g, color.b, time.c_str(), file.c_str(), debugInfo.line, debugInfo.function, buffer);
}

void llama::Logfile_I::print(const Table& table, bool transpose)
{
    std::lock_guard lock(m_printMutex);

    std::string time = Timestamp().getString("%T", TimeAccuracy::MILLISECONDS);;
    Color color = table.m_data[0][0].first;

    fprintf(m_fileHandle, "<p style=\"color:#%02x%02x%02x;\">[%s] %s:</p><div><table style=\"font-size: 15; display: inline-block\"><tr>",
        color.r, color.g, color.b, time.c_str(), table.m_title.c_str());

    printf("\x1b[38;2;%u;%u;%um[%s] %s:\x1b[0m\n",
        color.r, color.g, color.b, time.c_str(), table.m_title.c_str());

    if (!transpose)
    {
        std::vector<size_t> columns(table.m_data.begin()->size());

        for (size_t i = 0; i < columns.size(); ++i) // Loop through columns
            for (const auto& j : table.m_data) // Loop through rows
            {
                size_t stringLength = llama::stringLength(j[i].second);
                if (stringLength > columns[i])
                    columns[i] = stringLength;
            }


        for (size_t i = 0; i < table.m_data[0].size(); ++i) // Loop through first row
        {
            Color c = table.m_data[0][i].first;
            fprintf(m_fileHandle, "<th style=\"background-color: #%02x%02x%02x; color: #222\">%s</th>", c.r, c.g, c.b, table.m_data[0][i].second.c_str());
            printf("\x1b[30;48;2;%u;%u;%um%-*s    \x1b[0m ",
                c.r, c.g, c.b, static_cast<int>(columns[i]), table.m_data[0][i].second.c_str());
        }

        fprintf(m_fileHandle, "</tr>");
        printf("\n");

        for (auto i = table.m_data.begin() + 1; i != table.m_data.end(); ++i) // Loop through all rows except first one
        {
            fprintf(m_fileHandle, "<tr>");

            for (size_t j = 0; j < i->size(); ++j)
            {
                Color c = (*i)[j].first;
                fprintf(m_fileHandle, "<td style=\"color:#%02x%02x%02x;\">%s</td>", c.r, c.g, c.b, (*i)[j].second.c_str());
                printf("\x1b[38;2;%u;%u;%um%-*s    \x1b[0m ", c.r, c.g, c.b, static_cast<int>(columns[j]), (*i)[j].second.c_str());
            }

            fprintf(m_fileHandle, "</tr>");
            printf("\n");
        }
    }
    else
    {
        std::vector<size_t> rows(table.m_data.size());

        for (size_t i = 0; i < rows.size(); ++i) // Loop through rows
            for (const auto& j : table.m_data[i]) // Loop through columns
            {
                size_t stringLength = llama::stringLength(j.second);
                if (stringLength > rows[i])
                    rows[i] = stringLength;
            }

        for (size_t i = 0; i < table.m_data.size(); ++i) // Loop through first column
        {
            Color c = table.m_data[i][0].first;
            fprintf(m_fileHandle, "<th style=\"background-color: #%02x%02x%02x; color: #222\">%s</th>", c.r, c.g, c.b, table.m_data[i][0].second.c_str());
            printf("\x1b[30;48;2;%u;%u;%um%-*s    \x1b[0m ",
                c.r, c.g, c.b, static_cast<int>(rows[i]), table.m_data[i][0].second.c_str());
        }

        fprintf(m_fileHandle, "</tr>");
        printf("\n");

        for (size_t i = 1; i < table.m_data[0].size(); ++i) // Loop through all columns except first one
        {
            fprintf(m_fileHandle, "<tr>");

            for (size_t j = 0; j < table.m_data.size(); ++j) // Loop through all rows of current column
            {
                Color c = table.m_data[j][i].first;
                fprintf(m_fileHandle, "<td style=\"color:#%02x%02x%02x;\">%s</td>", c.r, c.g, c.b, table.m_data[j][i].second.c_str());
                printf("\x1b[38;2;%u;%u;%um%-*s    \x1b[0m ", c.r, c.g, c.b, static_cast<int>(rows[j]), table.m_data[j][i].second.c_str());
            }

            fprintf(m_fileHandle, "</tr>");
            printf("\n");
        }
    }

    fprintf(m_fileHandle, "</table></div><br>");
    fflush(m_fileHandle);
    printf("\n");
}
