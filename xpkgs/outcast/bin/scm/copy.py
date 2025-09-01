# Short help
def display_summary():
    print("{:<13}{}".format( 'copy', "Creates a non-tracked/local copy of a SCM Repository" ))

# DOCOPT command line definition
USAGE = """
Creates a non-tracked/local copy of the specified repository/branch/reference
===============================================================================
usage: evie [common-opts] copy [options] <dst> <repo> <origin> <id>
       evie [common-opts] copy get-success-msg
       evie [common-opts] copy error


Arguments:
    <dst>            Parent directory for where the copy is placed. The
                     directory is specified as a relative path to the root
                     of primary repository.
    <repo>           Name of the repository to copy.
    <origin>         Path/URL to the repository.
    <id>             Label/Tag/Hash/Version of code to be copied.
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
    --force          Forces a true copy/clone to be created.  The default
                     behavior is SCM type specific.
                       
Options:
    -h, --help       Display help for this command.

    
Notes:
    o The command MUST be run in the root of the primary respostiory.
"""
