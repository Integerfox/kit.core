# Projects.Examples.Dm.Transaction

The directory contains a example application that illustrate how to use a model
point for a 'request-with-response' transaction.  A transaction consists of a
client triggering/requesting an action from a server.  The server then consumes
the requests, carries out its actions, and then returns a success/fail of the
action.  The transaction is done using a single model point instance.  In addition
the example assumes that there can only be at most one in-flight transaction at
any given time.

The client and server instances can be separate threads or the same thread.

## Details, Constraints, Requirements

- The Data Model's change notification mechanism is used by both the client
  and server widgets
  - This requires both the client and server widgets to execute in an Event
    thread context.
  - Also requires that both the client and server widgets inherit/uses the
    `Kit::Itc::OpenCloseSync` class because registering/canceling DM change
    notifications **must** be one in the same thread - and that thread is
    thread context for which the notification callback executes in.

- The example includes creating a new model point that a data structure.
  - **Note**: in the example, the structure is used to separate the 'input'
    and 'results' data for the transaction.  However, the transaction pattern
    does not require it.  For example - a minimal transaction pattern can be
    done with a `Bool` MP where transition to `true` triggers the server/action,
    and the server returns "success" by setting the MP to invalid and returns
    to "fail" by setting the MP to `false`
