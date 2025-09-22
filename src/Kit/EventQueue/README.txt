/** @namespace Kit::EventQueue

The 'EventQueue' namespace is used to provide a 'common' Event Queue type when there
are multiple orthogonal sources of events. For example, the KIT library supports
ITC message and Data Model change notification events.  Neither ITC or the DM
have direct dependencies on each other - and while MOST applications will use
both types of events, there is no requirement to always include both in an
application.

The driving reason to have a 'common' Event Queue type is that to 'send|post'
an event - a reference to a Event Queue is needed.  Typically objects are
provided a reference to their Event Queue in their constructor and the reference
is stored as member variable to the class.  If there was NOT a common Event
Queue - then a class that uses ITC and DM change notifications would be required
to 'cache' two different Event Queue references (i.e. this is the case when using
the CPL library - Cpl::Itc::PostApi vs Cpl::Dm::MailboxServer).

*/  


  