# Short help
def display_summary():
    print("{:<13}{}".format( 'rm', "Removes a previously copied SCM Repository" ))

# DOCOPT command line definition
USAGE="""
Removes a previously 'copied' repository
===============================================================================
usage: evie [common-opts] rm [options] <dst> <repo> <origin> <id>
       evie [common-opts] rm [options] get-success-msg
       evie [common-opts] rm [options] get-error-msg

Arguments:
    <dst>            PARENT directory for where the package was copied.  The
                     directory is specified as a relative path to the root
                     of primary repository.
    <repo>           Name of the repository to remove
    <origin>         Path/URL to the repository
    <id>             Label/Tag/Hash/Version of code to be remove
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
      the 'copy' command.

"""