# Short help
def display_summary():
    print("{:<13}{}".format( 'meta', "Retrieves various repository meta information" ))

# DOCOPT command line definition
USAGE="""
Retrieves various repository meta information
===============================================================================
usage: evie [common-opts] meta [options] <repo> <origin> tag [<tfilter>]

Arguments:
    <repo>           Name of the repository to retrieve meta data from.
    <origin>         Path/URL to the repository.
    <tfilter>        Tag filter, e.g. OUTCAST
    
Options:
    -h, --help          Display help for this command

Notes:
    o Example:
        evie -v meta nqbp2 https://github.com/johnttaylor tag OUTCAST
"""