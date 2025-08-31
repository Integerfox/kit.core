#!/usr/bin/python3
"""
Orc is a Outcast tool for implementing a Mono-Consuming-Poly strategy
===============================================================================
usage: orc [options] <command> [<args>...]
       orc [options]

Options:
    --scm TOOL           Selects the SCM tool (overrides the environment 
                         OUTCAST_SCM_ADOPTED_TOOL setting) with respect to 
                         adopted packages. The default SCM tool is: git. 
    --primary-scm TOOL   Selects the SCM tool (overrides the environment 
                         OUTCAST_SCM_PRIMARY_TOOL setting) with respect to 
                         primary repository. The default SCM tool is: git. 
    -q                   Suppresses Warning messages.
    -v                   Be verbose. 
    --where              Only display the path to where 'orc' is located.
    -h, --help           Display help for common options/usage.
    

Type 'orc help <command>' for help on a specific command.
"""

import sys
import os
from subprocess import call

from docopt.docopt import docopt
from my_globals import ORC_VERSION
from my_globals import OUTCAST_SCM_ADOPTED_TOOL
from my_globals import OUTCAST_SCM_PRIMARY_TOOL
from my_globals import PACKAGE_ROOT
import utils




#------------------------------------------------------------------------------
def load_command( name ):
    try:
        command_module = __import__("commands.{}".format(name), fromlist=["commands"])
    except ImportError as e:
        exit("{} is not a Orc command. Use 'orc help' for list of commands [{}]".format(name, e) )

    return command_module
        
        
#------------------------------------------------------------------------------
def display_command_list():
    import pkgutil
    import commands
    p = commands
    
    print( ' ' )
    print( "Type 'orc help <command>' for more details. Type 'orc --help' for base usage." )
    print( "-------------------------------------------------------------------------------" )
    
    # Display the list of commands
    for importer, modname, ispkg in pkgutil.iter_modules(p.__path__):
        if ( not ispkg ):
            cmd = load_command( modname )
            cmd.display_summary()

    print( ' ' )

#------------------------------------------------------------------------------
# MAIN
if __name__ == '__main__':
    # Parse command line
    args = docopt(__doc__, version=ORC_VERSION(), options_first=True )

    # Determine which 'adopted' SCM tool to use
    scm = os.environ.get( OUTCAST_SCM_ADOPTED_TOOL() )
    if ( scm == None ):
        scm = 'git'
    if ( args['--scm'] ):
        scm = args['--scm']    
    args['--scm'] = scm

    # Determine which 'primary' SCM tool to use
    scm = os.environ.get( OUTCAST_SCM_PRIMARY_TOOL() )
    if ( scm == None ):
        scm = 'git'
    if ( args['--primary-scm'] ):
        scm = args['--primary-scm']    
    args['--primary-scm'] = scm


    # Trap the special where option
    if ( args['--where'] ):
        print(__file__)
        exit(0)
 
    # Trap help on a specific command
    if ( args['<command>'] == 'help' ):

        # Display list of commands if none specified
        if ( args['<args>'] == [] ):
            display_command_list()
        
        # Display command specific help
        else:
            load_command( args['<args>'][0] ).run( args, ['--help'] )


    # Run the command (if it exists)
    else:
        # Set quite & verbose modes
        utils.set_quite_mode( args['-q'] )
        utils.set_verbose_mode( args['-v'] )
    
        # Set the current working directory to the root directory
        repo_root = utils.find_root( args['--primary-scm'], args['-v'] )
        utils.push_dir( repo_root )
        PACKAGE_ROOT( repo_root )

        # run the command
        load_command( args['<command>'] ).run( args, [args['<command>']] + args['<args>'] )


    
