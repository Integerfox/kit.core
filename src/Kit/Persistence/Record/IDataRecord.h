#ifndef KIT_PERSISTENCE_RECORD_IDATARECORD_H
#define KIT_PERSISTENCE_RECORD_IDATARECORD_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/IRecord.h"
#include "Kit/Persistence/Record/IPayload.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {


/** This abstract extends the IRecord interface to include semantics for an
    unmanaged 'data store', i.e. combines the IPayload interface with the
    IRecord interface.

    The data is only read/written from Persistent storage 'on demand' from
    the application.

    NOTE: This interface/class is NOT THREAD SAFE and should only be 'used' from
          the Record Server's thread.
*/
class IDataRecord : public IRecord, public IPayload
{
public:
    /** This method is used by to write record's data to persistent storage.
        This method calls the IPayload's getData() method for the data to write
        to persistent storage.

        The 'index' offset can be used to 'index into' the IMedia instead
        of starting at offset zero.  This argument should only be used when
        multiple 'entries' are being stored in a single Record.

        The method returns true on success; else false if an error occurred.

        NOTE: This method is NOT THREAD SAFE and should only be called from the
              Record Server's thread.
     */
    virtual bool writeToMedia( Size_T index = 0 ) noexcept = 0;

    /** This method is used by the application to read the DataRecord's data
        from persistent storage. Note: This method calls the DataRecord's putData()
        method with the data read from persistent storage.

        The 'index' offset can be used to 'index into' the IMedia instead
        of starting at offset zero.  This argument should only be used when
        multiple 'entries' are being stored in a single Record.

        The method returns true on success; else false if an error occurred

        NOTE: This method is NOT THREAD SAFE and should only be called from the
              Record Server's thread.
     */
    virtual bool readFromMedia( Size_T index = 0 ) noexcept = 0;

public:
    /** This method is responsible for updating the 'data store' in RAM to
        default values.  This method is called when there is NO valid data when
        reading the record's data from persistence storage.

        The method returns true if the 'data store' is in an acceptable state
        and the end of the call; else if there is error or there is no acceptable
        state of the 'data store' then false is returned
     */
    virtual bool processNoValidData() noexcept = 0;
};

}  // end namespaces
}
}
#endif  // end header latch
