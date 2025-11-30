/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Foo.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/Foo/Logging/Pkg/ClassificationId.h"
#include "Kit/System/Assert.h"
#include "Kit/Logging/Framework/Logger.h"


//------------------------------------------------------------------------------
namespace Foo {
namespace Logging {
namespace App {


////////////////////////////////////////////////
Foo::Foo( Kit::Container::RingBuffer<Kit::Logging::Framework::EntryData_T>& logFifo ) noexcept
{
    Kit::Logging::Framework::initialize( *this, logFifo );
}

bool Foo::isClassificationIdValid( uint8_t classificationId ) noexcept
{
    return ::Foo::Logging::Pkg::ClassificationId::_from_integral_nothrow( classificationId );
}

bool Foo::isPackageIdValid( uint8_t packageId ) noexcept
{
    Kit::Logging::Framework::IPackage& foundPkg = getPackage( packageId );
    return &foundPkg != &m_nullPackage;
}

const char* Foo::classificationIdToString( uint8_t classificationId ) noexcept
{
    return Kit::Type::betterEnumToString<::Foo::Logging::Pkg::ClassificationId, uint8_t>(
        classificationId,
        NULL_CLASSIFICATION_ID_TEXT );
}

Kit::Logging::Framework::IPackage& Foo::getPackage( uint8_t packageId ) noexcept
{
    // Brute force lookup since only three packages are supported (and its a unit test)
    switch ( packageId )
    {
    case Pkg::Package::PACKAGE_ID:
        return m_fooPackage;
    case Kit::Logging::Pkg::Package::PACKAGE_ID:
        return m_kitPackage;
    case PkgZ::Logging::Pkg::Package::PACKAGE_ID:
        return m_pkgzPackage;
    default:
        return m_nullPackage;
    }
}


}  // end namespace
}
}
//------------------------------------------------------------------------------