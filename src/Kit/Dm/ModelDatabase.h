#ifndef KIT_DM_MODELDATABASE_H_
#define KIT_DM_MODELDATABASE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Dm/IModelDatabase.h"
#include "Kit/Container/SList.h"


///
namespace Kit {
///
namespace Dm {


/** This concrete class implements a simple Model Database.

    Note: The model point instances are contained in SList. This is to reduce
          the RAM overhead on each Model Point instance (e.g. a MapItem has 32
          bytes of overhead vs. 8 bytes for Item).  This means that looking up
          a model point by name is not efficient.  However, the typically use
          case for the look-up function is when reading/writing model point
          via the TShell command shell the performance impact should not be
          noticeable.

          The SList is sorted alphabetically on the first call to getFirstByName().

    Note: All of the methods are thread safe.  However, since the Model Database
          instances are typically created statically and statically allocated
          Model point self-register there is race condition on when the
          Model Database instance's Mutex is created/initialize.  As a result
          mutex is dynamically created/initialized on its first usage.
  */
class ModelDatabase : public IModelDatabase
{
public:
    /// Constructor
    ModelDatabase() noexcept;

    /** This is a special constructor for when the Model Database is
        statically declared (i.e. it is initialized as part of
        C++ startup BEFORE main() is executed.  C/C++ does NOT guarantee
        the order of static initializers.  Since the Model Database contains
        a list of model points - and one or more of those model points can
        be statically created/initialized - we need to do something to ensure
        my internal list is properly initialized for this scenario - hence this
        constructor.
     */
    ModelDatabase( const char* ignoreThisParameter_usedToCreateAUniqueConstructor ) noexcept;

public:
    /// See Kit::Dm::IModelDatabase
    IModelPoint* lookupModelPoint( const char* modelPointName ) noexcept override;

    /// See Kit::Dm::IModelDatabase
    IModelPoint* getFirstByName() noexcept override;

    /// See Kit::Dm::IModelDatabase
    IModelPoint* getNextByName( IModelPoint& currentModelPoint ) noexcept override;

    /// See Kit::Dm::IModelDatabase
    bool fromJSON( const char*         src,
                   Kit::Text::IString* errorMsg          = 0,
                   IModelPoint**       retMp             = 0,
                   uint16_t*           retSequenceNumber = 0 ) noexcept override;

public:
    /// See Kit::Dm::IModelDatabase
    void insert_( IModelPoint& mpToAdd ) noexcept override;

    /// See Kit::Dm::IModelDatabase
    Kit::System::Mutex& getMutex_() noexcept override;

protected:
    /// Helper method to sort the model point list alphabetical
    virtual void sortList() noexcept;

    /// Helper method to find a point by name
    virtual IModelPoint* find( const char* name ) noexcept;

protected:
    /// Map to the store the Model Points
    Kit::Container::SList<IModelPoint> m_list;

    /// Mutex for making the Database thread safe
    Kit::System::Mutex m_lock;

    /// Keep track if the point list has been sorted
    bool m_listSorted;

private:
    /// Prevent access to the copy constructor -->Model Databases can not be copied!
    ModelDatabase( const ModelDatabase& m );

    /// Prevent access to the assignment operator -->Model Databases can not be copied!
    const ModelDatabase& operator=( const ModelDatabase& m );
};


}       // end namespaces
}
#endif  // end header latch
