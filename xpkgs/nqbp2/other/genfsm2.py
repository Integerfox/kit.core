#!/usr/bin/python3
#=============================================================================
# This script is used to run the 'StateSmith' Finite State Machine Code Generator
# utility and modifies the output to take advantage of the KIT library.  The
# script expects that the input to the StateSmith sets certain markers in the
# auto generated code.  This script has option to generate a Plant UML template
# that provides the required configuration customization.
#
# https://github.com/StateSmith/StateSmith
# https://plantuml.com/
#
#=============================================================================


#
import sys
import os


# Make sure the environment is properly set
NQBP_BIN = os.environ.get('NQBP_BIN')
if NQBP_BIN is None:
    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
sys.path.append( NQBP_BIN )

#
from nqbplib.docopt.docopt import docopt

# 
usage = """ 
genfsm2 - Generates source code from PlantUML/StateSmith FSM Diagrams
===============================================================================
Usage: genfsm2 [options] <fsmname>
       genfsm2 [options] new <fsmname>
       genfsm2 [options]

Arguments:
  <fsmname>        The name of the State Machine diagram. Is also used as the
                   base file name for the Plant UML file.  Note: The name IS
                   case sensitive.
      
  new              Creates a new Plant UML file with the specified base file
                   name.  The new file contains the required configuration State
                   Smith configuration.
                   
Options:
  -l               Lists the supported OSALs.
  -t OSAL          Target OSAL. Supported OSALs are: CPL, KIT. [Default: KIT]
  --nosim          When specified, HTML simulation of the State Machine diagram
                   will not be generated
  -v               Be verbose.
  -h, --help       Display command help.
        
EXAMPLES:
    genfsm2 -v Bob
        Generates the FSM code from the Plant UML diagram 'Bob.puml' using 
        the KIT OSAL with verbose output.

    genfsm2 -t CPL myfsm
        Generates the FSM code from the Plant UML diagram 'myfsm.puml' using 
        the CPL OSAL.
    
    genfsm2 new myfsm
        Creates a new Plant UML file named 'myfsm.puml' with the required State
        Smith configuration template.         
NOTES:
    o Requires that the StateSmith `ss.cli` utility is installed and available
      in the command path. https://github.com/StateSmith/StateSmith
"""
#
import subprocess
import re
import sys

VERSION = "0.0.1"

MARKER_COPYRIGHT       = "// <MARKER:COPYRIGHT>" 
MARKER_HEADER_GUARD    = "<MARKER:INCLUDE_GUARD_LABEL>"
MARKER_CUSTOM_INCLUDE  = "<MARKER:HEADER_INCLUDES>"
MARKER_EVENT_QUEUE     = "<MARKER:PARENT_EVENT_QUEUE:"
MARKER_ACTION_METHODS  = "// <MARKER:ACTIONS>"
MARKER_GUARD_METHODS   = "// <MARKER:GUARDS>"
MARKER_OTHERS          = "// <MARKER:OTHER>"


#-------------------------------------------------------------------------------
symbols         = { "que_depth": "0", 
                    "has_event_queue": False,
                  }
supported_osals = [ "CPL", "KIT" ]
osal            = None

cpl_copyright_header = """* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2025  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice."""

kit_copyright_header = """ * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
"""
copyright_headers = {
    "CPL": cpl_copyright_header,
    "KIT": kit_copyright_header
}
trace_include = {
    "CPL": "Cpl/System/Trace.h",
    "KIT": "Kit/System/Trace.h"
}
fatalerror_include = {
    "CPL": "Cpl/System/FatalError.h",
    "KIT": "Kit/System/FatalError.h"
}
ringbuffer_include = {
    "CPL": "Cpl/Container/RingBuffer.h", 
    "KIT": "Kit/Container/RingBuffer.h"
}
ringbuffer_class = {
    "CPL": "Cpl::Container::RingBuffer<EventId>",
    "KIT": "Kit::Container::RingBuffer<EventId>"
}
ringbuffer_constructor = {
    "CPL": "m_eventQueue( max_event_queue_depth + 1, m_eventQueueMemory )",
    "KIT": "m_eventQueue( m_eventQueueMemory, max_event_queue_depth + 1 )"
}
fatalerror_logf = {
    "CPL": "Cpl::System::FatalError::logf(",
    "KIT": "Kit::System::FatalError::logf( Kit::System::Shutdown::eFSM_EVENT_OVERFLOW,"
}
trace_msg = {
    "CPL": "CPL_SYSTEM_TRACE_MSG( SECT_, (",
    "KIT": "KIT_SYSTEM_TRACE_MSG( SECT_,"
}
trace_msg_suffix = {
    "CPL": ")",
    "KIT": ""
}

#------------------------------------------------------------------------------
# Parse command line
def run( argv, copyright=None ):
    global symbols, osal

    # Process command line args...
    args  = docopt(usage, version=VERSION)
    symbols["verbose"]   = args['-v']
    
    # Validate target OSAL
    osal = args['-t'].strip()
    if osal not in supported_osals:
        exit(f"ERROR: Unsupported OSAL ({osal}) specified. Use 'genfsm2 osal' to see supported OSALs.")

    # Diagram file name
    fsmname = args['<fsmname>']
    if fsmname.endswith( ".puml" ):  # Strip any .puml extension
        fsmname = fsmname[:-5]
    fsmdiag = fsmname + ".puml"
    symbols["fsm_name"]    =  fsmname
    symbols["fsm_diagram"] =  fsmdiag
    symbols["fsm_header"]  =  fsmname + ".h"
    symbols["fsm_cpp"]     =  fsmname + ".cpp"


    # Create new Plant UML file
    if args['new']:
        if os.path.exists( fsmdiag ):
            exit(f"ERROR: File already exists: {fsmdiag}")
        generate_plantuml_file()
        print(f"Created new Plant UML file: {fsmdiag}")
        exit(0)
    
    # Run the StateSmith code generator
    cmd = "ss.cli run --lang Cpp --no-csx --rebuild"
    if args['--nosim']:
        cmd += " --no-sim-gen"
    if args['-v']:
        cmd += " -v"
    cmd += f" {fsmdiag}"
    if args['-v']:
        print(f"Running StateSmith command: {cmd}")
    p = subprocess.Popen( cmd, shell=True )
    p.communicate()
    if p.returncode != 0:
        exit("ERROR: StateSmith encountered an error or failed to run." )

    # Set copyright header (either explicit or based on OSAL). NOTE: This is a work-in-progress, i.e. how is explicit header provided?
    if copyright != None:
        symbols["copyright_header"] = copyright
    else:
        symbols["copyright_header"] = copyright_headers[osal]

    # Prescan the generated files for 'magic values' (e.g. the namespace)
    # Returns a dictionary with the found symbols/values
    prescan_generated_header()
    prescan_generated_cpp()

    # Derive addtional symbols
    derive_symbols()

    # Modify the generated code to take advantage of the KIT library
    update_generated_header()

    # Modify the generated code to take advantage of the KIT library
    update_generated_cpp()

#------------------------------------------------------------------------------
# Prescan the generated header file for 'magic values' (e.g. the namespace)
# Returns a dictionary with the found symbols/values
def prescan_generated_header():
    global symbols
    namespace   = ""
    with open( symbols["fsm_header"] ) as inf:
        for line in inf:
            line = line.lstrip()

            # Capture namespace
            if line.startswith( "namespace"):
                tokens    = line.split()
                namespace = tokens[1].strip()
                # Remove trailing '{' if present
                namespace = namespace.replace('{','').strip()
                symbols['nested_namespace'] = namespace
                if args['-v']:
                    print(f"Found namespace: {namespace}")
    
            # Is using a Event Queue parent class
            if MARKER_EVENT_QUEUE in line:
                depth_start = line.find( MARKER_EVENT_QUEUE ) + len( MARKER_EVENT_QUEUE )
                depth_end   = line.find( ">", depth_start )
                que_depth   = line[ depth_start : depth_end ].strip()
                symbols['que_depth'] = que_depth
                if args['-v']:
                    print(f"Found Event Queue with depth: {que_depth}")

# Prescan the generated header file for 'magic values' (e.g. the namespace)
# Returns a dictionary with the found symbols/values
def prescan_generated_cpp():
    global symbols
    action_methods = set()
    guard_methods = set()
    
    with open( symbols["fsm_cpp"] ) as inf:
        lines = inf.readlines()
        
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped_line = line.strip()
        
        # Look for guard methods in uml comments
        # Pattern: "// uml: [guard_name()] / { ActionB(); } TransitionTo(State2)"
        if stripped_line.startswith("// uml:") and "[" in line:
            # Extract all guard methods from square brackets
            guard_matches = re.findall(r'\[([a-zA-Z_][a-zA-Z0-9_]*)\s*\(\s*\)\]', line)
            for guard in guard_matches:
                guard_methods.add(guard)
        
        # Look for action methods in uml comments with curly braces
        # Pattern: "// uml: / { ActionA(); } TransitionTo(State1)"
        # or "// uml: enter / { ActionH();\nActionB(); }"
        if stripped_line.startswith("// uml:") and "{" in line:
            # Extract content between curly braces
            brace_matches = re.findall(r'\{([^}]+)\}', line)
            for action_block in brace_matches:
                # Replace literal \n with actual newline for splitting
                action_block = action_block.replace('\\n', '\n')
                # Find all function calls in the action block
                func_matches = re.findall(r'([a-zA-Z_][a-zA-Z0-9_]*)\s*\(\s*\)\s*;?', action_block)
                for func in func_matches:
                    action_methods.add(func)
        
        # Look for action methods in transition action comments or execute action comments
        # Pattern: "// Step 2: Transition action: `ActionName();`."
        # or "// Step 1: execute action `ActionH();\nActionB();`"
        if "Transition action:" in line or "execute action" in line:
            # Extract content between backticks
            action_matches = re.findall(r'`([^`]+)`', line)
            for action_block in action_matches:
                # Replace literal \n with actual newline for splitting
                action_block = action_block.replace('\\n', '\n')
                # Find all function calls in the action block
                func_matches = re.findall(r'([a-zA-Z_][a-zA-Z0-9_]*)\s*\(\s*\)\s*;?', action_block)
                for func in func_matches:
                    action_methods.add(func)
        
        i += 1
    
    # Convert sets to sorted lists
    action_methods = sorted(list(action_methods))
    guard_methods = sorted(list(guard_methods))
    
    # Add to symbols dictionary
    symbols['action_methods'] = action_methods
    symbols['guard_methods'] = guard_methods
    
    if args['-v']:
        print(f"Found action methods: {action_methods}")
        print(f"Found guard methods: {guard_methods}")


def derive_symbols():
    global symbols
    
    # Convert the nested namespace to a list of namespaces in hierarchically order
    # Note: There may only be a single namespace
    namespaces = symbols.get('nested_namespace', '').split('::')
    symbols['namespace_list']  = namespaces
    symbols['namespace_count'] = len( namespaces )

    # Derive Header guard - consists of the all upper case with the namespace(s) concatenated with the FSM name and then trailing _H_
    symbols['include_guard_label'] = "_".join( [ n.upper() for n in namespaces ] + [ symbols['fsm_name'].upper(), "H" ] ) + "_"

    # Determine if there is parent event queue class. Note: the dictionary is initialized with default KVP for que_depth(0) and has_event_queue (false)
    if symbols["que_depth"] == '0':
        symbols['ringbuffer_include'] = "\n"
        symbols['trace_include']      = "\n"
        symbols['fatalerror_include'] = "\n"
    else:
        symbols['has_event_queue'] = True
        symbols['ringbuffer_include'] = ringbuffer_include[osal]
        symbols['trace_include']      = trace_include[osal]
        symbols['fatalerror_include'] = fatalerror_include[osal]
    

def update_generated_header():
    # File names 
    srcheader = symbols["fsm_header"]
    dstheader = srcheader + '.new'

    # Copy the header file one line at a time, making modifications as needed
    with open( srcheader ) as inf:
        with open( dstheader, "w") as outf:  
            skip_next          = False
            found_other_marker = False
            namespace_found    = False
            prev_line          = ""
            for line in inf:
                stripped_line = line.strip()
                stripped_prev = prev_line.strip()
                
                # Skip next line when needed
                if skip_next:
                    prev_line = line
                    skip_next = False
                    continue
                
                # Insert copyright header
                if MARKER_COPYRIGHT in line:
                    outf.write( "/*\n" )
                    for cline in symbols["copyright_header"].splitlines():
                        outf.write( f"{cline}\n" )
                    outf.write( "*/\n\n" )
                    continue

                # Replace include guard label
                if MARKER_HEADER_GUARD in line:
                    line = line.replace(MARKER_HEADER_GUARD, symbols['include_guard_label'])

                # Update the include statements
                if MARKER_CUSTOM_INCLUDE in line:
                    if symbols['has_event_queue']:
                        line = f'#include "{symbols["ringbuffer_include"]}"\n'
                    else:
                        continue

                # Rename the start() method
                if stripped_line.startswith( "void start()" ):
                    line = line.replace( "start()", "startFsm()" )

                # Patch namespace if needed
                if line.startswith( "namespace "):
                    namespace_found = True
                    if symbols['namespace_count'] > 1:
                        line = ""
                        for ns in symbols['namespace_list']:
                            ns_pattern = f'namespace {ns} {{\n'
                            line = line + ns_pattern
                        skip_next = True

                # Strip over the Base Class marker (it is only used to specify the event queue depth)
                if MARKER_EVENT_QUEUE in line:
                    line = f"class {symbols['fsm_name']}\n"
                    
                # Add a virtual destructor 
                if stripped_line.startswith( "// State machine constructor." ):
                    line2 = f"    /// Virtual destructor\n"
                    outf.write( line2 )
                    line2 = f"    virtual ~{symbols['fsm_name']}() {{}}\n"
                    outf.write( line2 )
                    outf.write( '\n')

                # Patch the constructor to initialize the event queue (when used)
                if stripped_line.startswith( f"{symbols['fsm_name']}()" ):
                    if symbols['has_event_queue']:
                        line = f'    {symbols["fsm_name"]}() : {ringbuffer_constructor[osal]}, m_processingFsmEvent(false)\n'

                # Add the ACTION methods
                if line.startswith( MARKER_ACTION_METHODS ):
                    found_other_marker = True
                    update_header_action_block( outf )
                    continue

                # Add the GUARD methods
                if line.startswith( MARKER_GUARD_METHODS ):
                    found_other_marker = True
                    update_header_guard_block( outf )
                    continue

                # Add the OTHER code
                if line.startswith( MARKER_OTHERS ):
                    found_other_marker = True
                    update_header_other_block( outf )
                    continue

                # Patch the end-namespace
                if found_other_marker:
                    if stripped_line == "}":
                        line = f"{int(symbols['namespace_count'])*'}'}\n"
                
                # Fix non-doxygen comment styles
                if stripped_line.startswith( "//" ) and not stripped_line.startswith( "///" ) and not stripped_line.startswith("// -" ) and not stripped_line.startswith("// #" ):
                    line = line.replace( "//", "///", 1 )

                # Trap uncomment enum lines
                if stripped_line.startswith( "enum" ) and not stripped_prev.startswith("///" ):
                    outf.write( "    /// Enum definition\n" )

                # Trap uncomment void functions lines
                if stripped_line.startswith( "void" ) and not stripped_prev.startswith("///" ):
                    outf.write( "    /// State Machine function\n" )

                # Output the (possibly modified) line
                prev_line = line
                outf.write( line.rstrip() + "\n" )

    # Replace the original file with the modified file
    os.remove( srcheader )
    os.rename( dstheader, srcheader )

def update_generated_cpp():
    # File names 
    srccpp = symbols["fsm_cpp"]
    dstcpp = srccpp + '.new'

    # Copy the header file one line at a time, making modifications as needed
    with open( srccpp ) as inf:
        with open( dstcpp, "w") as outf:  
            skip_next          = False
            for line in inf:
                stripped_line = line.strip()

                # Skip next line when needed
                if skip_next:
                    skip_next = False
                    continue
                
                # Insert copyright header
                if MARKER_COPYRIGHT in line:
                    outf.write( "/*\n" )
                    for cline in symbols["copyright_header"].splitlines():
                        outf.write( f"{cline}\n" )
                    outf.write( "*/\n\n" )
                    continue

                # Update the include statements
                if MARKER_CUSTOM_INCLUDE in line:
                    if symbols['has_event_queue']:
                        line = f'#include "{symbols["trace_include"]}"\n'
                        outf.write( line )
                        line = f'#include "{symbols["fatalerror_include"]}"\n'
                    else:
                        continue

                # Patch namespace if needed
                if line.startswith( "namespace "):
                    if symbols['namespace_count'] > 1:
                        line = ""
                        for ns in symbols['namespace_list']:
                            ns_pattern = f'namespace {ns} {{\n'
                            line = line + ns_pattern
                        skip_next = True

                # Rename the original dispatch event method to fsmDispatchEvent
                if stripped_line.startswith( f"void {symbols['fsm_name']}::dispatchEvent(EventId eventId)" ) and symbols['has_event_queue']:
                    line = line.replace( "dispatchEvent", "fsmDispatchEvent" )

                # Rename the start() method
                if stripped_line.startswith( f"void {symbols['fsm_name']}::start()" ):
                    line = line.replace( "start()", "startFsm()" )

                # Patch the end-namespace
                if line.rstrip() == "}":
                    # add event queue
                    if symbols['has_event_queue']:
                        outf.write( "\n" )
                        outf.write( "    void " + symbols['fsm_name'] + "::dispatchEvent( EventId msg )\n" )
                        outf.write( "    {\n" )
                        outf.write( f"        static constexpr const char* tsection = \"{symbols["nested_namespace"]}\";\n" )
                        outf.write( "\n" )
                        outf.write( "        // Queue my event\n" )
                        outf.write( "        if ( !m_eventQueue.add( msg ) )\n" )
                        outf.write( "        {\n" )
                        outf.write( f"            Kit::System::FatalError::logf( Kit::System::Shutdown::eFSM_EVENT_OVERFLOW, \"%s(" + symbols["fsm_name"] + "): - Buffer Overflow!\", tsection );\n" )
                        outf.write( "        }\n" )
                        outf.write( "\n" )
                        outf.write( "        // Protect against in-thread 'feedback loops' that can potentially generate events\n" )
                        outf.write( "        if ( !m_processingFsmEvent )\n" )
                        outf.write( "        {\n" )
                        outf.write( "            m_processingFsmEvent = true;\n" )
                        outf.write( "            while ( m_eventQueue.remove( msg ) )\n" )
                        outf.write( "            {\n" )
                        outf.write( "                KIT_SYSTEM_TRACE_MSG( tsection, \"EVENT:= %s, current state=%s ...\", eventIdToString( msg ), stateIdToString(stateId) );\n" )
                        outf.write( "                fsmDispatchEvent( msg );\n" )
                        outf.write( "                KIT_SYSTEM_TRACE_MSG( tsection, \"-->Completed: end state=%s\", stateIdToString(stateId) );\n" )
                        outf.write( "            }\n" )
                        outf.write( "\n" )
                        outf.write( "            m_processingFsmEvent = false;\n" )
                        outf.write( "        }\n" )
                        outf.write( "    }\n" )
                        outf.write( "\n" )

                    # Update the end-namespace
                    line = f"{int(symbols['namespace_count'])*'}'}\n"

                outf.write( line.rstrip() + "\n" )

    # Replace the original file with the modified file
    os.remove( srccpp )
    os.rename( dstcpp, srccpp )


def update_header_other_block( outf ):
    outf.write( f"\n" )
    outf.write( f"public:\n" )
    outf.write( f"    /// The State Machine name\n" )
    outf.write( f'    static constexpr const char* fsmName = "{symbols["fsm_name"]}";\n' )
    outf.write( f"\n" )
    outf.write( f"protected:\n" )
    
    # Add the event queue members  (when needed)
    if symbols['has_event_queue']:
        que_depth = symbols['que_depth']
        outf.write( f"    /// Depth of the Event Queue\n" )
        outf.write( f"    static constexpr unsigned max_event_queue_depth = {que_depth};\n" )
        outf.write( f"\n" )
        outf.write( f"    /// Event Queue\n" )
        outf.write( f"    {ringbuffer_class[osal]} m_eventQueue;\n" )
        outf.write( f"\n" )
        outf.write( f"    /// Track event processing in progress state\n" )
        outf.write( f"    bool m_processingFsmEvent;\n" )
        outf.write( f"\n" )
        outf.write( f"    /// Memory for the Event Queue\n" )
        outf.write( f"    EventId m_eventQueueMemory[max_event_queue_depth + 1];\n" )
        outf.write( f"\n" )
        outf.write( f"    /// The 'real' dispatch event method that is generated by StateSmith\n" )
        outf.write( f"    void fsmDispatchEvent(EventId eventId);\n" )
        outf.write( f"\n" )
    
def update_header_action_block( outf ):
    action_methods = symbols.get('action_methods', [])
    if len( action_methods ) == 0:
        return
    outf.write( f"protected:\n" )
    for method in action_methods:
        outf.write( f"    /// Action Method\n" )
        outf.write( f"    virtual void {method}() noexcept = 0;\n" )
        outf.write( f"\n" )
    outf.write( f"\n" )

def update_header_guard_block( outf ):
    guard_methods = symbols.get('guard_methods', [])
    if len( guard_methods ) == 0:
        return
    outf.write( f"protected:\n" )
    for method in guard_methods:
        outf.write( f"    /// Guard Method\n" )
        outf.write( f"    virtual bool {method}() noexcept = 0;\n" )
    outf.write( f"\n" )

def generate_plantuml_file():
    fname   = symbols["fsm_diagram"]
    fsmname = symbols["fsm_name"]
    with open( fname, "w") as outf:
        outf.write( f"' The name of the FSM diagram should match the filename ({fname}).\n" )
        outf.write( f"@startuml {fsmname}\n" )
        outf.write( "\n" )
        outf.write( "' Recommended PlantUML settings for StateSmith FSM diagrams.\n" )
        outf.write( "hide empty description\n" )
        outf.write( "'scale 1.25\n" )
        outf.write( "\n" )
        outf.write( "' Note: StateSmith treats state names and events as case insensitive.\n" )
        outf.write( "' More info: https://github.com/StateSmith/StateSmith/wiki/PlantUML\n" )
        outf.write( "'\n" )
        outf.write( "' CHEAT SHEET for PlantUML arrow syntax:\n" )
        outf.write( "' -down-> or -->\n" )
        outf.write( "' -right-> or -> (default arrow)\n" )
        outf.write( "' -left->\n" )
        outf.write( "' -up->\n" )
        outf.write( "\n" )
        outf.write( "\n" )
        outf.write( "' ADD YOUR STATES and TRANSITIONS HERE\n" )
        outf.write( "state Idle\n" )
        outf.write( "[*] -down-> Idle : / Initialize();\n" )
        outf.write( "\n" )
        outf.write( "\n" )
        outf.write( "' //////////////////////// StateSmith config ////////////////////////\n" )
        outf.write( "' The below special comment block sets the StateSmith configuration.\n" )
        outf.write( "' More info: https://github.com/StateSmith/StateSmith/issues/335\n" )
        outf.write( "'\n" )
        outf.write( "' The following config is used to enable the NQBP genfsm2.py script.  Only edit\n" )
        outf.write( "' the FSM settings. Do NOT edit/remove the GENFSM2 settings unless you\n" )
        outf.write( "' know what you are doing.\n" )
        outf.write( "\n" )
        outf.write( "'///////////////////////////////////////////////////////////////////\n" )
        outf.write( "' FSM Settings.\n" )
        outf.write( "'  NameSpace -->a single or nested namespace is allowed, e.g.\n" )
        outf.write( "'    Examples:\n" )
        outf.write( "'      # Single namespace\n" )
        outf.write( "'      RenderConfig.Cpp.NameSpace = \"Foo\"\n" )
        outf.write( "\n" )
        outf.write( "'      # Nested namespaces\n" )
        outf.write( "'      RenderConfig.Cpp.NameSpace = \"Foo::Bar\"\n" )
        outf.write( "'\n" )
        outf.write( "'  BaseClassCode -->This configuration option is 'hijacked' to specify the\n" )
        outf.write( "'                   depth of the event queue. If no event queue is needed, \n" )
        outf.write( "'                   comment the line out.\n" )
        outf.write( "'    Examples:\n" )
        outf.write( "'      # No event queue ()\n" )
        outf.write( "'      #RenderConfig.Cpp.ClassName = \"<MARKER:PARENT_EVENT_QUEUE:4>\"\n" )
        outf.write( "'\n" )
        outf.write( "'      # Event queue with depth of 4\n" )
        outf.write( "'      RenderConfig.Cpp.ClassName = \"<MARKER:PARENT_EVENT_QUEUE:4>\"\n" )
        outf.write( "\n" )
        outf.write( "/'! $CONFIG : toml\n" )
        outf.write( "RenderConfig.Cpp.NameSpace = \"Foo::Bar\"\n" )
        outf.write( "#RenderConfig.Cpp.BaseClassCode  = \"<MARKER:PARENT_EVENT_QUEUE:4>\"\n" )
        outf.write( "'/\n" )
        outf.write( "\n" )
        outf.write( "\n" )
        outf.write( "'///////////////////////////////////////////////////////////////////\n" )
        outf.write( "' GENFSM2 Settings.\n" )
        outf.write( "'\n" )
        outf.write( "\n" )
        outf.write( "/'! $CONFIG : toml\n" )
        outf.write( "\n" )
        outf.write( "RenderConfig.Cpp.IncludeGuardLabel = \"<MARKER:INCLUDE_GUARD_LABEL>\"\n" )
        outf.write( "RenderConfig.Cpp.HFileTopPostIncludeGuard = \"// <MARKER:COPYRIGHT>\"\n" )
        outf.write( "RenderConfig.Cpp.HFileExtension = \".h\"\n" )
        outf.write( "RenderConfig.Cpp.HFileIncludes = \"<MARKER:HEADER_INCLUDES>\"\n" )
        outf.write( 'RenderConfig.Cpp.CFileTop = \"// <MARKER:COPYRIGHT>\"\n' )
        outf.write( "RenderConfig.Cpp.CFileIncludes = \"<MARKER:HEADER_INCLUDES>\"\n" )
        outf.write( 'RenderConfig.Cpp.ClassCode = """\n' )
        outf.write( "// ------------- Start of Injected ClassCode ----------------------------------\n" )
        outf.write( "// <MARKER:ACTIONS>\n" )
        outf.write( "// <MARKER:GUARDS>\n" )
        outf.write( "// <MARKER:OTHER>\n" )
        outf.write( "// ------------- End of Injected ClassCode ----------------------------------\n" )
        outf.write( '"""\n' )
        outf.write( "\n" )
        outf.write( "# More Cpp settings are available. See docs.\n" )
        outf.write( "\n" )
        outf.write( "[SmRunnerSettings]\n" )
        outf.write( "outputCodeGenTimestamp = true\n" )
        outf.write( "\n" )
        outf.write( "'/\n" )
        outf.write( "@enduml\n" )

#------------------------------------------------------------------------------
# MAIN
if __name__ == '__main__':
    # Process command line args...
    args  = docopt( usage, version=VERSION )
    
    # Validate target OSAL
    osal = args['-t'].strip()
    if osal not in supported_osals:
        exit(f"ERROR: Unsupported OSAL ({osal}) specified. Use 'genfsm2 osal' to see supported OSALs.")

    # List supported OSALs
    if args['-l']:
        print("Supported OSALs:")
        for osal in supported_osals:
            print(f"{osal}")
        exit(0)

    run( args )