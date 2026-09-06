# Projects.Examples.Itc.AsyncCancel {#projects_examples_itc_asynccancel}

\brief Asynchronous message - with cancel semanatics - based Inter-Thread-Communication (ITC).

The directory contains a example application on how to use KIT's asynchronous
ITC messaging AND how to cancel an outstanding asynchronous message.  The example
contains server widget.  The server widget is responsible for generating
N duty cycles of LED flash pattern. The Client widget use asynchronous
ITC message set the number of duty cycles and the flash pattern

## Use cases

The most common use is to use asynchronous ITC messaging is when the client has
asynchronous behavior (i.e. its behavior is inherently a state machine) or when
the client cannot block executing while waiting on event X to happen.

## Details, Constraints, Requirements

- When using asynchronous ITC messages - the message client and server **can**
  be the same thread.  
  - NOTE: In this example the client and server are in **separate** threads.

- Both the client and server **must** executing in event-loop based thread(s)

- A ITC request message class must be created. See [IFlashRequest.h](https://github.com/Integerfox/kit.core/blob/main/projects/examples/Itc/AsyncCancel/IFlashRequest.h).

- A ITC response message class must be created and it references the request class.
  See [IFlashRequest.h](https://github.com/Integerfox/kit.core/blob/main/projects/examples/Itc/AsyncCancel/IFlashRequest.h).
  - The Request and Response messages _share_ the same payload which is defined
    in the Request message class.
  - It is recommended to define the Response class in the same header file as 
    the Request message.

- It is recommended to have a single exit point from the message request
  function - that calls returnToSender() on the message - to ensure that the
  message transaction completes.
  
- The _server_ has **no** knowledge of the synchronous/asynchronous semantics
  of the messaging.  It is the **client's** usage that determine the semantics
  of the ITC message.  This done by which concrete [IReturnHandler](https://github.com/Integerfox/kit.core/blob/main/src/Kit/Itc/IReturnHandler.h)
  instance is used when the ITC message is created.

- Is an example of multiple ITC request nessages being defined in single 
  'Request' class.

- Both the Client and Server widgets use synchronous ITC for initialization and
  shutdown.

**NOTE**: The example also utilizes sending an argument/data via the synchronous
          ITC open message.

## Class Diagram

![Transaction Example Class Diagram](classDiagram_examplesitcsynchronous.png)

## See Also

- @ref Kit::Itc "Kit::Itc namespace documentation"
- @ref projects_examples_itc_asynchronous "Asynchronous ITC example"

## Implementation

- Root source directory: [projects/examples/Itc/AsyncCancel](https://github.com/Integerfox/kit.core/blob/main/projects/examples/Itc/AsyncCancel)
- Build directory: [projects/examples/Itc/AsyncCancel/_0build](https://github.com/Integerfox/kit.core/blob/main/projects/examples/Itc/AsyncCancel/_0build)
- Build Targets:
  - Host: Linux, Windows
  - NUCLEO-F413ZH w/FreeRTOS
