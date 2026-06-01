/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/

#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Json/ArduinoHelpers.h"
#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "Kit/Io/IOutput.h"
#include "Kit/Text/FString.h"
#include <limits>


namespace
{
class OutputSink : public Kit::Io::IOutput
{
public:
    bool write( const void* buffer, Kit::Io::ByteCount_T maxBytes, Kit::Io::ByteCount_T& bytesWritten ) noexcept override
    {
        bytesWritten = maxBytes;
        if ( buffer == nullptr || maxBytes <= 0 )
        {
            return true;
        }

        const char* src = static_cast<const char*>( buffer );
        for ( Kit::Io::ByteCount_T i = 0; i < maxBytes; ++i )
        {
            m_contents += src[i];
        }

        return true;
    }

    void flush() noexcept override
    {
    }

    void close() noexcept override
    {
    }

    bool isEos() override
    {
        return false;
    }

    void clear() noexcept
    {
        m_contents.clear();
    }

    const char* get() const noexcept
    {
        return m_contents;
    }

private:
    Kit::Text::FString<1024> m_contents;
};

class TraceRedirectGuard
{
public:
    explicit TraceRedirectGuard( Kit::Io::IOutput& output ) noexcept
    {
        KIT_SYSTEM_TRACE_REDIRECT( output );
    }

    ~TraceRedirectGuard() noexcept
    {
        KIT_SYSTEM_TRACE_REVERT();
    }
};
}


TEST_CASE( "ArduinoHelpers-dumpToTrace-prints-wide-integers-as-integers" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::System::initialize();
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "json-test" );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eINFO );

    OutputSink sink;
    TraceRedirectGuard redirect( sink );

    DynamicJsonDocument doc( 256 );
    doc["unsignedValue"] = (std::numeric_limits<unsigned long long>::max)();
    doc["signedValue"]   = (std::numeric_limits<long long>::max)();

    sink.clear();
    Kit::Json::dumpToTrace( "json-test", doc.as<JsonVariant>() );

    REQUIRE( strstr( sink.get(), "[\"unsignedValue\"]" ) != nullptr );
    REQUIRE( strstr( sink.get(), "[\"signedValue\"]" ) != nullptr );
    REQUIRE( strstr( sink.get(), "Float:" ) == nullptr );
    REQUIRE( strstr( sink.get(), "Integer: 18446744073709551615" ) != nullptr );
    REQUIRE( strstr( sink.get(), "Integer: 9223372036854775807" ) != nullptr );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}