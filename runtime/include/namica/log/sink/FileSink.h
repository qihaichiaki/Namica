#pragma once

#include "namica/log/sink/TextSink.h"
#include "namica/core/Base.h"
#include "namica/core/Types.h"
#include <fstream>

namespace namica
{

struct FileSinkConfig;

class FileSink final : public TextSink
{
public:
    CLASS_DISABLE_COPY_MOVE(FileSink)

    NAMICA_API static SharedPtr<FileSink> create(FileSinkConfig const& _config);

public:
    NAMICA_API void setFlushOnWrite(Bool _flushOnWrite) noexcept;

    NAMICA_API Bool write(LogRecord const& _record) override;
    NAMICA_API Bool flush() override;

private:
    FileSink(FileSinkConfig const& _config);

private:
    std::ofstream m_ofs;
    Bool m_flushOnWrite;
};

}  // namespace namica