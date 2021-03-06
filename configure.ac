dnl Process this file with autoconf to produce a configure script.

AC_INIT(mstflint, 4.6.0, adrianc@mellanox.co.il)

AC_DEFINE_UNQUOTED([PROJECT], ["mstflint"], [Define the project name.])
AC_SUBST([PROJECT])

AC_DEFINE_UNQUOTED([VERSION], ["4.6.0"], [Define the project version.])
AC_SUBST([VERSION])

AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([README])
AM_INIT_AUTOMAKE([foreign])

dnl Checks for programs
AC_PROG_CC
AC_PROG_CXX
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_LIBTOOL
AC_CONFIG_HEADERS( config.h )

dnl Checks for headers
AC_CHECK_HEADER(termios.h,[CXXFLAGS="${CXXFLAGS} -DHAVE_TERMIOS_H"])
TOOLS_CRYPTO=""

AC_MSG_CHECKING(--enable-dc argument)
AC_ARG_ENABLE(dc,
		    [  --disable-dc            Disable mstflint "dc" command. Eliminates zlib dependency],
		    [enable_dc=$enableval],
		    [enable_dc="yes"])
AC_MSG_RESULT($enable_dc)
if test "$enable_dc" = "yes"; then
  AC_CHECK_HEADER(zlib.h,,AC_MSG_ERROR([cannot find zlib.h . Use --disable-dc to remove this dependaency]))
  AC_CHECK_LIB(z, uncompress,, AC_MSG_ERROR([cannot find zlib uncompress() function. Use --disable-dc to remove this dependaency]))
fi

# XML2
AC_MSG_CHECKING(--enable-xml2 argument)
AC_ARG_ENABLE(xml2,
            [  --enable-xml2           Enable mstflint libxml2 dependant features],
            [enable_xml2="yes"],
            [enable_xml2="no"])
AC_MSG_RESULT($enable_xml2)
if test "x$enable_xml2" = "xyes"; then
    # XML2 checks
    if test ! -f /usr/include/libxml2/libxml/xpath.h; then
        AC_MSG_NOTICE([checking for libxml2... no])
        AC_MSG_ERROR([libxml2: xpath.h is not found in the system PATH. make sure libxml2 headres are installed.])
    else
        AC_MSG_NOTICE([checking for libxml2... yes])
        XML2_INC_DIR=/usr/include/libxml2/
    fi
    AC_CHECK_LIB(xml2, xmlDocDumpFormatMemoryEnc,, AC_MSG_ERROR([cannot find xml2 xmlDocDumpFormatMemoryEnc() function. Try re-installing the library...]))
    AC_SUBST(XML2_INC_DIR)
fi
AM_CONDITIONAL(DISABLE_XML2, [test  "x$enable_xml2" = "xno"])
AC_SUBST(DISABLE_XML2)

AM_CONDITIONAL(ENABLE_DC, [test  "x$enable_dc" = "xyes"])
AC_SUBST(ENABLE_DC)

AC_MSG_CHECKING(--enable-inband argument)
AC_ARG_ENABLE(inband,
		    [  --disable-inband        Disable inabnd access. Prevents FW updtes for Mellanox SwitchX and ConnectIB devices. Eliminates infiniband/mad.h dependency],
		    [enable_inband=$enableval],
		    [enable_inband="yes"])
AC_MSG_RESULT($enable_inband)
if test "x$enable_inband" = "xyes"; then
  AC_CHECK_HEADER(infiniband/mad.h,,AC_MSG_ERROR([cannot find infiniband/mad.h . Use --disable-inband to remove this dependaency]))
fi

AM_CONDITIONAL(ENABLE_INBAND, [test  "x$enable_inband" = "xyes"])

AC_MSG_CHECKING(--enable-cs argument)
AC_ARG_ENABLE(cs,
            [  --enable-cs             Enable mstflint "checksum" command, dependant of openssl library],
            [enable_cs="yes"],
            [enable_cs="no"])
AC_MSG_RESULT($enable_cs)
if test "x$enable_cs" = "xyes"; then
  AC_CHECK_HEADER(openssl/md5.h,,AC_MSG_ERROR([cannot find openssl/md5.h . remove --enable-openssl to remove this dependaency or install openssl]))
  TOOLS_CRYPTO="tools_crypto"
fi
AC_SUBST(TOOLS_CRYPTO)
AM_CONDITIONAL(ENABLE_OPENSSL, [test  "x$enable_cs" = "xyes"])

AC_ARG_ENABLE([static_libstdcpp],
    AS_HELP_STRING([--enable-static-libstdcpp], [Enable link static to libstdc++]))

AS_IF([test "x$enable_static_libstdcpp" = "xyes"], [
  CXXFLAGS="$CXXFLAGS -static-libstdc++"
])

AC_ARG_ENABLE([dynamic_ld],
    AS_HELP_STRING([--enable-dynamic-ld], [Enable dynamic link to ld64.so]))

AS_IF([test "x$enable_dynamic_ld" = "xyes"], [
  LDFLAGS="$LDFLAGS -Wl,--dynamic-linker=/lib64/ld64.so.2"
])

CFLAGS="$CFLAGS -Werror -DMST_UL"
CXXFLAGS="$CXXFLAGS -Werror -DMST_UL"

AC_CONFIG_FILES( mstflint.spec )

AS_IF([test "x$enable_cs" = "xyes"], [
        AC_CONFIG_FILES(tools_crypto/Makefile)
    ])

AC_OUTPUT( Makefile common/Makefile mft_utils/Makefile mtcr_ul/Makefile ext_libs/Makefile ext_libs/sqlite/Makefile ext_libs/muparser/Makefile dev_mgt/Makefile tools_layouts/Makefile reg_access/Makefile \
        cmdif/Makefile tools_res_mgmt/Makefile mlxconfig/Makefile mlxconfig/mlxconfig_dbs/Makefile mflash/Makefile mlxfwops/Makefile mlxfwops/lib/Makefile cmdparser/Makefile flint/Makefile \
 		small_utils/Makefile mstdump/Makefile mstdump/crd_lib/Makefile mstdump/crd_main/Makefile mstdump/mstdump_dbs/Makefile )
