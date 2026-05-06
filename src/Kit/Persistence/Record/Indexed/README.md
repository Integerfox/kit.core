# Kit::Persistence::Record::Indexed
@brief namespace description for Kit::Persistence::Record::Indexed
@namespace Kit::Persistence::Record @brief

The 'Indexed' namespace extends the basic Persistent::Record interface with the
ability for a single Record to contain N entries, where an entry content is
defined by a Kit::Persistence::IPayload instance.  An example usage for Indexed
entries is to persistently store log entries.

- A single IChunk handler is used for writing all N entries.

- For a given IEntry Record instance, all entries must be the same, fixed size.

- When reading/traversing the list of entries and an corrupt entry is detected,
  the traversal logic will skip up to M consecutive corrupt entries before
  declaring a failure.

- The individual entries are logically stored in a Ring Buffer.  This means that
  once the Ring Buffer is full (i.e. all of the space available to the IMedia
  instance has been written to) - older entries are over written when there is
  request to write a new entry.  
  
  - A 'head record' (with its own chunk handler) is used to persistently store
    the 'head pointer' (aka the offset of the newest entry in the Ring Buffer).

    - NOTE: The 'head record' is wholly managed by the IEntry instance, i.e.
          the head record instance should NOT be directly added to the Record
          Server's list of records to manage.

    - It is recommended to use a `Mirrored` IChunk handler for the head record
      to provide protection against corrupting the head record if power fails
      during an update.

  - There is **no method** for erasing entries. There is no need for the
    application to 'manually' reset or clear the entries to 'make room' or 'free
    of space'. The 'latest' N entries are always guaranteed to be stored.  

    - If the application needs to 'erase' the entries - it must do it **outside**
      of the `Indexed` interfaces (e.g delete the underlying file when using a
      `IMedia::FileAdapter`)

    - There is a 'logical erase' (i.e. the `resetHead()` method) that can be
      used to *abandon* all of the entries.  This method is cosmetic in that
      no entries are actually erased/removed.

- A 64 bit 'timestamp' is associated with each entry.  The timestamp value is a
  free running counter that is used to determine the **relative** age between
  entries.

  - The larger an timestamp value is, the newer the entry is.

- On-startup - only the "head record" is loaded into the RAM
