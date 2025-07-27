# How to use VSCode to debug a running process - that is running on Linux - for debugging  
This should be simple process, but Linux requires that you have root permission
in order to attach GDB to a running procces. Below is one solution where the only
GDB is executing with root permissions (instead of VSCode).  The solution leverages
the debug configurations that are created by the `nqbpy.py` script for a specific
application/unit-test.

## Attach to a running process
1. Make sure `gdbserver` is install on your system
   * `sudo apt install gdbserver`
2. Get the PID of the application you want to debug
     * `ps -ef | grep myapplication-name`
3. Launch gdbserver using the applicaiton PID from step 2.
     *	`sudo gdbserver :1234 --attach <PID>`
4. Select the debug configuration "**NQBP Attach...**" from Debug tool window.
   * To create the debug configurations for your project, run the following
      in the project's build directory:
      * `nqbpy.py --vsgdb`
      
5. Launch/Start the selected debug configuration

## Detaching the debugger in VS Code without killing the process
Pressing the STOP button in GDB will terminate the running the process.  To stop 
your debugging session without terminating the application - use the steps below

1. In VSCode - pause the application
2. In the Debug console window, issue the following command
   * `-exec detach`

The above steps will detach VSCode from the application without exiting the application, **but** it also terminates the gdbserver session, i.e. you will to relaunch the gdbserver when startinga new debug session.
