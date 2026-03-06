/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "ArduinoHelpers.h"
#include "Kit/System/Trace.h"
#include "Kit/Text/DString.h"

using namespace Kit::Json;

static void dump( Kit::Text::IString& dst, const JsonVariant& variant, int nesting );
static void dump( Kit::Text::IString& dst, bool value, int nesting );
static void dump( Kit::Text::IString& dst, long value, int nesting );
static void dump( Kit::Text::IString& dst, double value, int nesting );
static void dump( Kit::Text::IString& dst, const char* str, int nesting );
static void dump( Kit::Text::IString& dst, const JsonObject& obj, int nesting );
static void dump( Kit::Text::IString& dst, const JsonArray& arr, int nesting );
static void indent( Kit::Text::IString& dst, int nesting );

//////////////////////////////////
void Kit::Json::dumpToTrace( const char* traceSection, const JsonVariant& srcObjToDump ) noexcept
{
    Kit::Text::DString buf( "", 2 * 2048, 512 );
    dump( buf, srcObjToDump, 0 );
    KIT_SYSTEM_TRACE_MSG( traceSection, "\n%s", buf.getString() );
}


void dump( Kit::Text::IString& dst, const JsonVariant& variant, int nesting )
{
    if ( variant.is<bool>() )
    {
        dump( dst, variant.as<bool>(), nesting );
    }
    else if ( variant.is<long>() )
    {
        dump( dst, variant.as<long>(), nesting );
    }
    else if ( variant.is<double>() )
    {
        dump( dst, variant.as<double>(), nesting );
    }
    else if ( variant.is<const char*>() )
    {
        dump( dst, variant.as<const char*>(), nesting );
    }
    else if ( variant.is<JsonObject>() )
    {
        dump( dst, variant.as<JsonObject>(), nesting );
    }
    else if ( variant.is<JsonArray>() )
    {
        dump( dst, variant.as<JsonArray>(), nesting );
    }
    else
    {
        indent( dst, nesting );
        dst += "Undefined";
    }
}

void dump( Kit::Text::IString& dst, bool value, int nesting )
{
    indent( dst, nesting );
    dst += "Bool: ";
    dst += value ? "true" : "false";
    dst += "\n";
}

void dump( Kit::Text::IString& dst, long value, int nesting )
{
    indent( dst, nesting );
    dst += "Integer: ";
    dst += value;
    dst += "\n";
}

void dump( Kit::Text::IString& dst, double value, int nesting )
{
    indent( dst, nesting );
    dst += "Float: ";
    dst.formatAppend( "%g", value );
    dst += "\n";
}

void dump( Kit::Text::IString& dst, const char* str, int nesting )
{
    indent( dst, nesting );
    dst += "IString: ";
    dst += str;
    dst += "\n";
}

void dump( Kit::Text::IString& dst, const JsonObject& obj, int nesting )
{
    indent( dst, nesting );
    dst += "Object (size = ";
    dst += obj.size();
    dst += "):\n";

    // Iterate though all key-value pairs
    for ( JsonPair kvp : obj )
    {
        // Print the key
        indent( dst, nesting + 1 );
        dst += "[\"";
        dst += kvp.key().c_str();
        dst += "\"]\n";

        // Print the value
        dump( dst, kvp.value(), nesting + 2 );  // <- RECURSION
    }
}

void dump( Kit::Text::IString& dst, const JsonArray& arr, int nesting )
{
    indent( dst, nesting );
    dst += "Array (size = ";
    dst += arr.size();
    dst += ")\n";

    int index = 0;
    // Iterate though all elements
    for ( JsonVariant value : arr )
    {
        // Print the index
        indent( dst, nesting + 1 );
        dst += "[";
        dst += index;
        dst += "]\n";

        // Print the value
        dump( dst, value, nesting + 2 );  // <- RECURSION

        index++;
    }
}

void indent( Kit::Text::IString& dst, int nesting )
{
    for ( int i = 0; i < nesting; i++ )
        dst += "  ";
}
