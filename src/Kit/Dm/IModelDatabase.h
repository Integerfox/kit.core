#ifndef KIT_DM_IMODELDATABASE_H_
#define KIT_DM_IMODELDATABASE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/Text/IString.h"
#include "Kit/System/Mutex.h"
#include "Kit/Json/Arduino.h"

/** This symbol defines the size, in bytes, of a single/global JSON document
    buffer that is used for the toJSON() and fromJSON() operations. Only one
    instance of this buffer is allocated.
*/
#ifndef OPTION_KIT_DM_MODEL_DATABASE_MAX_CAPACITY_JSON_DOC
#define OPTION_KIT_DM_MODEL_DATABASE_MAX_CAPACITY_JSON_DOC ( 1024 * 2 )
#endif


///
namespace Kit {
///
namespace Dm {

/// Forward reference to a Model point -->used to avoid circular dependencies
class IModelPoint;

/** This class defines the basic operations that can be performed on a Model
    Base.  A Model Database is a collection of instantiated Model Points.

    All methods in this class are thread Safe unless explicitly documented
    otherwise.
 */
class IModelDatabase
{
public:
    /** This method looks-up the model point instance by name and returns a
        pointer to the instance.  If the model point name cannot be found, the
        method returns nullptr.

        NOTE: The Model Point returned is a generic Model Point in that any
              operations that require the concrete data type of the Model
              Point ARE NOT available.  It is 'okay' for the caller (IF AND ONLY
              IF the caller 'knows' what the concrete type is) to down cast the
              returned pointer to a type specific child class instance of the
              Model Point.

        NOTE: There is no 'add' method.  This is because Model Points self
              register with their assigned Model Database when the Model Points
              are created.
     */
    virtual IModelPoint* lookupModelPoint( const char* modelPointName ) noexcept = 0;


public:
    /** This method returns a pointer to the first Model Point in the Database.
        The model points are traversed in order by model point name.  If there
        are no Model Points in the Database, the method returns nullptr.
     */
    virtual IModelPoint* getFirstByName() noexcept = 0;

    /** This method returns the next (in sorted order) Model Point in the
        Database. If the current model point is the last Model Point in the
        Database the method returns nullptr.
     */
    virtual IModelPoint* getNextByName( IModelPoint& currentModelPoint ) noexcept = 0;


public:
    /** This method attempts to convert the null terminated JSON formatted 'src'
        string to its binary format and copies the result to the Model Point's
        internal data. The expected format of the JSON string is specific to
        the concrete leaf class.

        The method optional returns - via 'retMp' - a pointer to the Model Point
        identified by 'mpname'.  If the method false return, then 'retMp' has
        no meaning.

        The method optional returns - via 'retSequenceNumber' - the Model Point's
        sequence number after the conversion. If the method false return, then
        'retSequenceNumber' has no meaning.

        If the conversion is successful true is returned. If the contents of
        the 'src' is not a valid JSON object and/or not parse-able, OR the Point
        does not support a full/complete conversion from Text to binary, OR
        the conversion fails, OR the specified model point name does not exist
        then the method returns false.  When the conversion fails, the optional
        'errorMsg' argument is updated with a plain text error message.


        The general input format:
        \code

        { name="<mpname>", locked:true|false }              // Locks/unlocks the MP
        { name="<mpname>", valid:false }                    // Invalidates the MP
        { name="<mpname>", valid:false, locked=true }       // Invalidates the MP and locks the MP
        { name="<mpname>", val:<value> }                    // Writes a new (valid) value to the MP
        { name="<mpname>", val:<value>, locked=true }       // Writes a new (valid) value to the MP and locks the MP


        \endcode
     */
    virtual bool fromJSON( const char*         src,
                           Kit::Text::IString* errorMsg          = nullptr,
                           IModelPoint**       retMp             = nullptr,
                           uint16_t*           retSequenceNumber = nullptr ) noexcept = 0;

public:
    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Kit::Dm namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method inserts a new Model Point into the Model Database.
     */
    virtual void insert_( IModelPoint& mpToAdd ) noexcept = 0;

    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Kit::Dm namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method returns the database's mutex.  This mutex is used for ALL
        critical sections with respect to Model Point operations.  Note: Each
        MP database can have it own mutex
     */
    virtual Kit::System::Mutex& getMutex_() noexcept = 0;

public:
    /** This method has 'PACKAGE Scope' in that is should only be called by
        other classes in the Kit::Dm namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        This method provides a single global mutex for critical sections across
        ALL Model Database instances. The method is used to protect global Model
        Database data (e.g.the global parse buffer).
     */
    static Kit::System::Mutex& getGlobalMutex_() noexcept;

    /** This variable has 'PACKAGE Scope' in that is should only be called by
        other classes in the Kit::Dm namespace.  It is ONLY public to avoid
        the tight coupling of C++ friend mechanism.

        Global/single instance of a JSON document. Model Point's need to have
        acquired the global lock before using this buffer
     */
    static StaticJsonDocument<OPTION_KIT_DM_MODEL_DATABASE_MAX_CAPACITY_JSON_DOC> g_doc_;

public:
    /// Virtual destructor to make the compiler happy
    virtual ~IModelDatabase() {}
};


}  // end namespaces
}
#endif  // end header latch
