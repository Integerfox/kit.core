# Short help
def display_summary():
    print("{:<13}{}".format( 'mount', "Creates a semi-tracked/local copy of a SCM Repository" ))

# DOCOPT command line definition
USAGE = """
Creates a semi-tracked/local copy of the specified repository/branch/reference
===============================================================================
usage: evie [common-opts] mount [options] <dst> <repo> <origin> <id>
       evie [common-opts] mount [options] get-success-msg
       evie [common-opts] mount [options] get-error-msg

Arguments:
    <dst>            PARENT directory for where the copy is placed.  The
                     directory is specified as a relative path to the root
                     of primary repository.
    <repo>           Name of the repository to mount
    <origin>         Path/URL to the repository
    <id>             Label/Tag/Hash/Version of code to be mounted

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
    --noro           Do NOT mark the package as read-only in the file system
    -h, --help       Display help for this command

    
Notes:
    o The command MUST be run in the root of the primary respostiory.
    o The 'mount' command is different from the 'copy' in that creates
      semi-tracked clone of the repository which can be updated directly from
      the source <repo> at a later date (think git subtrees)

"""
