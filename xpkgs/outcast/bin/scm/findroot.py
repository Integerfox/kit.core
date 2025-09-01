# Short help
def display_summary():
    print("{:<13}{}".format( 'findroot', "Returns the root directory of the current local repository" ))

# DOCOPT command line definition
USAGE = """
Returns the root directory of the current local repository
===============================================================================
usage: evie [common-opts] findroot

Options:
    -h, --help          Display help for this command

    
Notes:
    o The command MUST be run within the local repository.

"""
