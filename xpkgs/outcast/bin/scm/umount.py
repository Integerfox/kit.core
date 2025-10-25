# Short help
def display_summary():
    print("{:<13}{}".format( 'umount', "Removes a previously mounted SCM Repository" ))

# DOCOPT command line definition
USAGE="""
NOTE: This command is the process of BEING DEPRECATED - do NOT use.
NOTE: This command is the process of BEING DEPRECATED - do NOT use.
NOTE: This command is the process of BEING DEPRECATED - do NOT use.
Removes a previously 'mounted' repository
===============================================================================
usage: evie [common-opts] umount [options] <dst> <repo> <origin> <id>
       evie [common-opts] umount [options] get-success-msg
       evie [common-opts] umount [options] get-error-msg

Arguments:
    <dst>            PARENT directory for where the package was mounted.  The
                     directory is specified as a relative path to the root
                     of primary repository.
    <repo>           Name of the repository to unmount
    <origin>         Path/URL to the repository
    <id>             Label/Tag/Hash/Version of code to be unmounted
    get-success-msg  Returns a SCM specific message that informs the end user
                     of additional action(s) that may be required when 
                     the command is successful
    get-error-msg    Returns a SCM specific message that informs the end user
                     of additional action(s) that may be required when 
                     the command fails    
Options:
    -p PKGNAME       Specifies the Package name if different from the <repo> 
                     name
    -b BRANCH        Specifies the source branch in <repo>.  The use/need
                     of this option in dependent on the <repo> SCM type.                        
Options:
    -h, --help          Display help for this command

    
Notes:
    o The command MUST be run in the root of the primary respostiory.
    o This command only applied to repositories previously mounted using
      the 'mount' command.

"""