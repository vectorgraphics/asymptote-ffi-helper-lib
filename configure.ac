AC_INIT(asyffi-helper-lib, 0.0.0-SNAPSHOT)

AC_LANG(C++)
AC_PROG_CXX

default_asyffi_url=https://raw.githubusercontent.com/vectorgraphics/asymptote/52de09fa50a5f0c35a6d5f2ed50413f80b8ef730/asyffi.h

AC_ARG_WITH(
    asyffi-header-url, [AS_HELP_STRING(URL to asyffi.h)],[
        asyffi_header_url=$withval
    ],[
        asyffi_header_url=$default_asyffi_url
    ])
AC_ARG_WITH(asyffi-header-location, [AS_HELP_STRING()], [
    asyffi_header_file=$withval
])

wgetrc_args=
AC_ARG_WITH(
    wgetrc-file,
    [AS_HELP_STRING(
        wgetrc file to use when downloading header file.
        This option is inert if with-asyffi-header-location is given
        )],
        [
        wgetrc_args=--config=$withval
        ]
)

mkdir -p asyffi_header
rm -rf asyffi_header/asyffi.h

AS_IF([test "x$asyffi_header_file" = "x"],[
    # file not given, download from web
    AC_MSG_NOTICE([
        Downloading asyffi.h from $default_asyffi_url.
        If the download fails try giving wget additional options through --with-wgetrc-file
    ])
    wget $wgetrc_args -O asyffi_header/asyffi.h $asyffi_header_url
],[
    # file given, copy from web
    AC_MSG_NOTICE([Using asyffi.h from $asyffi_header_file])
    cp $asyffi_header_file asyffi_header/asyffi.h
])

# doxygen
AC_CHECK_TOOL(DOXYGEN,doxygen,[])
AS_IF([test "x$DOXYGEN" = "x"], [AC_MSG_NOTICE([Doxygen not found. Building of documentation will not be available.])])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
