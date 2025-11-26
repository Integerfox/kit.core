@brief namespace description for Kit::Itc

@namespace Kit::Itc @brief

The Itc namespace provides classes for message based Inter Thread Communications(ITC) as well as Event Flags.

The ITC message mechanism has the following characteristics:

- The ITC model is a client-server model, where client(s) send messages
  to server(s).

- Messages can be sent asynchronously or synchronously.

- Data flow between clients and server can be uni-directional or
  bi-directional (determined by the application). Because this is
  inter-thread communication, data can be shared via pointers (if
  desired) since Clients and Servers share the same address space.

- Data is shared between clients and server using the concept of a
  payload.  In addition, a convention of ownership is used to
  provide thread-safe  access to the payload.

- A client initially owns the payload, i.e. can access the data
  in the payload without the use of critical sections.

- When a client sends (or posts) a message, there is implicit transfer
  of ownership of the payload.  After sending the message, the client
  is obligated to NOT access the payload contents.

- When a server received a message, the server now owns the payload
  and can access the data in the payload without the use of critical
  sections.

- When a server returns a message, there is implicit transfer of
  ownership of the payload back to the client. After returning the
  message, the server is obligated to NOT access the payload contents.

- NO dynamic memory is used.  Clients are responsible for providing all
  of the memory required for the individual message(s).

- This translates to no hard limits to the number of messages a server
  can receive.  It also means that the application does NOT have to
  worry about overflowing message queues and/or mailboxes.

- Another side effect of the memory paradigm is that there is NO
  broadcast messages.

- 100% type safe. Messages and payloads are handled and dispatched with
  NO typecasting required.
  
