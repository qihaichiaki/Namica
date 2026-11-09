#pragma once

#include "namica/log/sink/TextSinkConfig.h"
#include "namica/core/Types.h"

#include <filesystem>

namespace namica
{

enum class FileOpenMode
{
    Append,    // 追加
    Truncate,  // 覆盖
};

struct FileSinkConfig : TextSinkConfig
{
    std::filesystem::path filePath{};
    FileOpenMode openMode{FileOpenMode::Append};
    Bool flushOnWrite{True};

    FileSinkConfig() = default;
    FileSinkConfig(LogLevel _minLevel,
                   String const& _logFmt,
                   String const& _dateFmt,
                   std::filesystem::path _logPath,
                   FileOpenMode _openMode,
                   Bool _flushOnWrite)
        : TextSinkConfig{_minLevel, _logFmt, _dateFmt},
          filePath{_logPath},
          openMode{_openMode},
          flushOnWrite{_flushOnWrite}
    {
    }
};

}  // namespace namica