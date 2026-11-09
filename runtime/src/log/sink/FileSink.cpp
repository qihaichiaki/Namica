#include "namica/log/sink/FileSink.h"
#include "namica/log/sink/FileSinkConfig.h"

#include <stdexcept>

namespace namica
{

SharedPtr<FileSink> FileSink::create(FileSinkConfig const& _config)
{
    return SharedPtr<FileSink>{new FileSink{_config}};
}

FileSink::FileSink(FileSinkConfig const& _config)
    : TextSink{_config}, m_ofs{}, m_flushOnWrite{_config.flushOnWrite}
{
    // 输入日志文件路径不为空
    if (_config.filePath.empty())
    {
        throw std::invalid_argument{"Log file path must not be empty."};
    }

    auto openMode{std::ios::out | std::ios::binary};
    switch (_config.openMode)
    {
        case FileOpenMode::Append:
            openMode |= std::ios::app;
            break;

        case FileOpenMode::Truncate:
            openMode |= std::ios::trunc;
            break;

        default:
            throw std::invalid_argument{"Invalid log file open mode."};
    }

    // 查看是否需要创建目录
    auto parentPath{_config.filePath.parent_path()};
    if (!parentPath.empty() && !std::filesystem::exists(parentPath))
    {
        std::filesystem::create_directories(parentPath);
    }

    this->m_ofs.open(_config.filePath, openMode);
    if (!this->m_ofs.is_open() || this->m_ofs.fail())
    {
        // 异常状态
        throw std::filesystem::filesystem_error{"Failed to open log file",
                                                _config.filePath,
                                                std::make_error_code(std::io_errc::stream)};
    }
}

void FileSink::setFlushOnWrite(Bool _flushOnWrite) noexcept
{
    this->m_flushOnWrite = _flushOnWrite;
}

Bool FileSink::write(LogRecord const& _record)
{
    // 已经失败时不继续格式化，也不擅自清除错误状态。
    if (!this->m_ofs.is_open() || this->m_ofs.fail())
    {
        return False;
    }

    this->m_ofs << formatRecord(_record) << "\n";
    if (this->m_ofs.fail())
    {
        return False;
    }

    return this->m_flushOnWrite ? this->flush() : True;
}

Bool FileSink::flush()
{
    if (!this->m_ofs.is_open())
    {
        return False;
    }

    this->m_ofs.flush();
    return !(this->m_ofs.fail());
}

}  // namespace namica