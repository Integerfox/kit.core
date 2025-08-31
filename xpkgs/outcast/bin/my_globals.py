""" Global variables """

_package_root = ''

#
def ORC_VERSION():
    return "v2.0.1-alpha"
    
def EVIE_VERSION():
    return ORC_VERSION()
    
# SCM for adopted packages
def OUTCAST_SCM_ADOPTED_TOOL():
    return 'OUTCAST_SCM_ADOPTED_TOOL'
   
# SCM for primary/local package
def OUTCAST_SCM_PRIMARY_TOOL():
    return 'OUTCAST_SCM_PRIMARY_TOOL'

def OVERLAY_PKGS_DIR():
    return 'pkgs.overlaid'

def PACKAGE_INFO_DIR():
    return 'pkg.info'

def PACKAGE_FILE():
    return 'package.json'

def PKG_DIRS_FILE():
    return 'pkg-dirs.lst'

def IGNORE_DIRS_FILE():
    return 'ignore-dirs.lst'

def TEMP_DIR_NAME():
    return '__temp_delete_me__'

def TEMP_DIFF_DIR_NAME():
    return '.__remote_snapshot__'

def PACKAGE_ROOT(set=None):
    global _package_root
    if ( set != None ):
        _package_root = set
    return _package_root

#
#def NQBP_PRJ_DIR( newval=None ):
#    global _NQBP_PRJ_DIR
#    if ( newval != None ):
#        if ( newval.endswith(os.sep) ):
#            newval [:-1]
#        _NQBP_PRJ_DIR = newval
#    return _NQBP_PRJ_DIR

          
    
    
    
    
