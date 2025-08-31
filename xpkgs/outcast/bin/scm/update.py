# Short help
def display_summary():
    print("{:<13}{}".format( 'update', "Updates a previously mounted SCM Repository" ))
    
# DOCOPT command line definition
USAGE = """
Updates a previously mounted repository
===============================================================================
usage: evie [common-opts] update [options] <dst> <repo> <origin> <id>
       evie [common-opts] update [options] get-success-msg
       evie [common-opts] update [options] get-error-msg

Arguments:
    <dst>            PARENT directory for where the copy-to-be-updated is
                     located.  The directory is specified as a relative path to
                     the root of primary repository.<pkg>            
    <repo>           Name of the repository to update
    <origin>         Path/URL to the repository
    <id>             Label/Tag/Hash/Version of code to be updated
    get-success-msg  Returns a SCM specific message that informs the end user
                     of additional action(s) that may be required when 
                     the command is successful
    get-error-msg    Returns a SCM specific message that informs the end user
                     of additional action(s) that may be required when 
                     the command fails    
Options:
    -b BRANCH        Specifies the source branch in <repo>.  The use/need
                     of this option in dependent on the <repo> SCM type.
                        
Options:
    -h, --help          Display help for this command

    
Notes:
    o The command MUST be run in the root of the primary respostiory.

"""
