# Kit::Persistence::Record
@brief namespace description for Kit::Persistence::Record
@namespace Kit::Persistence::Record @brief

The 'Record' namespace provides a basic records based paradigm for storing
non-volatile data.  The sub-system has has the following features:

- Persistent data is organizing and manage in terms of individual records.  
  - A 'Record' is the unit of atomic read/write operations from/to persistent storage.
  - The Application is responsible for defining the data content the record(s).
  - It is the responsibility of the concrete Record instances to initiate updates
    to the persistent media.

- On start-up the Records are read and the concrete Record instances process
  the stored data.
  - Note:  Start-up in the only time Records are read from persistent storage.

- The provided `IChunk` implementations use a CRC mechanism for detecting corrupted
  data in the persistent media.
  - The CRCs are only validated on start-up.
  - Each `IChunk` implementation can have different CRC mechanisms including
    not having a CRC at all (though this is not recommended)

- Record instances are responsible for defaulting their data (and subsequently
  initiating an update to the persistent media) when the stored data has been
  detected as corrupt (i.e. bad CRC)

- The sub-system is independent of the physical persistent storage media.

- A Record Server can process an unlimited number of records.  It is also okay
  to have more than one Record Server instance.

- It is recommended that the Record Server execute in a dedicated thread if
  possible.  This provides the application the flexible to have writing to
  persistence storage as a *background task*.

  - Multiple Record Server instances can share a single thread.
