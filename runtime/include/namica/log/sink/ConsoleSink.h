#pragma once

#include "namica/core/Base.h"
#include "namica/core/Types.h"
#include "namica/log/sink/TextSink.h"
#include "namica/log/sink/ConsoleSinkConfig.h"

namespace namica
{

class ConsoleSink final : public TextSink
{
public:
    CLASS_DISABLE_COPY_MOVE(ConsoleSink)
    NAMICA_API static SharedPtr<ConsoleSink> create(ConsoleSinkConfig const& _config);

public:
    NAMICA_API void setColorMode(ConsoleColorMode _colorMode);
    NAMICA_API void setColorTheme(ConsoleColorTheme const& _colorTheme);

    NAMICA_API Bool write(LogRecord const& _record) override;
    NAMICA_API Bool flush() override;

private:
    ConsoleSink(ConsoleSinkConfig const& _config);

private:
    std::ostream* m_os;
    ConsoleColorMode m_colorMode;
    Bool m_colorEnabled;
    UInt8 m_logColorIndex[6];
};

}  // namespace namica