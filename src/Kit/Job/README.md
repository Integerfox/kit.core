# Kit::Job
@brief Namespace description for Kit::Job
@namespace Job::Logging @brief

The 'Job' namespace is provides a framework for asynchronously running a Job. A
Job can be anything - as long as it honors the semantics (e.g. start|stop)
defined in the IJob header file.

- The original use case for the Job framework was to support being able to
  selective run from the TShell console a set of tests used for board checkout,
  end-of-line manufacturing testing, emissions testing, design validation, etc.

  - For this use case, a rough analog for a Job would be launching a console
    command to execute in the background.
