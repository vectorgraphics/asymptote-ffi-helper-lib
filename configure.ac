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
    wgetrc_file,
    [AS_HELP_STRING(
        wgetrc file to use when downloading header file.
        This option is inert if with-asyffi-header-location is given
        )],
        [
        wgetrc_args=--config=$withval
        ]
)

mkdir -p asyffi_header

AS_IF([test "x$asyffi_header_file" = "x"],[
    # file not given, download from web
    wget -l $wgetrc_args $default_asyffi_url -O asyffi_header/asyffi.h
],[
    # file given, copy from web
    AC_MSG_NOTICE([Using asyffi.h from $asyffi_header_file])
    cp $asyffi_header_file asyffi_header/asyffi.h
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
