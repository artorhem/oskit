#
# Copyright (c) 1996, 1998, 1999, 2000, 2001 University of Utah and the Flux Group.
# All rights reserved.
# 
# This file is part of the Flux OSKit.  The OSKit is free software, also known
# as "open source;" you can redistribute it and/or modify it under the terms
# of the GNU General Public License (GPL), version 2, as published by the Free
# Software Foundation (FSF).  To explore alternate licensing terms, contact
# the University of Utah at csl-dist@cs.utah.edu or +1-801-585-3271.
# 
# The OSKit is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GPL for more details.  You should have
# received a copy of the GPL along with the OSKit; see the file COPYING.  If
# not, write to the FSF, 59 Temple Place #330, Boston, MA 02111-1307, USA.
#
AC_INIT(oskit/config.h.in)
AC_PREREQ(2.12)

# Tell autoconf to make AC_DEFINE put stuff in oskit/config.h instead of
# on the command line via DEFS.
AC_CONFIG_HEADER(oskit/config.h)

AC_SUBST(OSKIT_DEFS)
AC_SUBST(oskit_options)
oskit_options=""

# Find the build, host, and target machines,
# and derive the program prefix and suffix.
#AC_CANONICAL_TARGET
test "$program_prefix" = "NONE" && program_prefix=
AC_SUBST(program_prefix)
test "$program_suffix" = "NONE" && program_suffix=
AC_SUBST(program_suffix)

# Determine the general architecture from the CPU types;
# this determines which architecture-specific subdirectories are used.
host_cpu=x86_64
echo HOST CPU $host_cpu
case $host_cpu in
[	i[3-6]86*|x86*)	host_arch=x86 ; host_impl=pc ;; ]
	hppa*)		host_arch=hppa ; host_impl=ws ;;
	arm*)		host_arch=arm32 ; host_impl=shark ;;
	*)
		AC_MSG_ERROR([Unsupported architecture '$host_cpu'.]) ;;
esac
AC_SUBST(host_arch)

#
# Figure out what host implementation. It defaults above, but allow it
# to be specified.
#
flux_ARG_ENABLE(impl,
[  --enable-impl=<impl>    use <impl> to override default host implementation],
if test $enableval = yes; then enableval=$host_impl; fi
host_impl=$enableval)

#
# Add host and impl defines
#
HOST_ARCH="`echo "$host_arch" | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'`"
HOST_IMPL="`echo "$host_impl" | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'`"
OSKIT_DEFS="$OSKIT_DEFS -DOSKIT_${HOST_ARCH} -DOSKIT_${HOST_ARCH}_${HOST_IMPL}"

#
# Checks for programs
#

AC_CHECK_TOOL(CC,gcc)
AC_CHECK_TOOL(LD,ld)
AC_CHECK_TOOL(AR,ar)
AC_CHECK_TOOL(STRIP,strip)
AC_CHECK_TOOL(OBJCOPY,objcopy,true)
AC_CHECK_TOOL(NM,nm)
# Ranlibbing isn't strictly necessary on ELF but shouldn't hurt.
AC_CHECK_TOOL(RANLIB,ranlib)

# Flask SS needs GNU m4
AC_CHECK_PROGS(M4, gm4 m4, m4)

AC_CACHE_CHECK(if $M4 is gnu m4, flux_cv_gnu_m4,
[cat >m4test <<\EOF
decr(10)
EOF
if AC_TRY_COMMAND([$M4 -s m4test > /dev/null]); then
  flux_cv_gnu_m4=yes
  M4FLAGS="-s"
else
  flux_cv_gnu_m4=no
fi
rm -rf m4test])

AC_SUBST(M4FLAGS)

AC_PROG_INSTALL
# Make sure $INSTALL is valid at any depth in our tree, i.e. make it absoulte.
# This can be needed when no valid install is found and autoconf falls back on
# the supplied install.sh, but we called "configure" with a relative path.
[case "$INSTALL" in
..*)
	INSTALL=`pwd`/$INSTALL
	;;
esac]
AC_PROG_AWK
AC_PROG_YACC
flux_PERLINFO

#
# Compiler/assembler/etc features.  Note that if you change these make sure
# you change the corresponding #undef line in oskit/config.h.in
#
dnl This handy macro is defined in aclocal.m4 (which see).

if test "x$host_arch" = xx86; then
flux_CACHED_COMPILE_CHECK(assembler support for cr4 register,
                          [asm("mov %eax,%cr4");], HAVE_CR4)

flux_CACHED_COMPILE_CHECK(assembler support for debug registers,
			  [asm("mov %eax,%dr7");], HAVE_DEBUG_REGS)

flux_CACHED_COMPILE_CHECK(if compiler groks __attribute__((stdcall)), [
	void __attribute__((stdcall)) foo();
], HAVE_STDCALL)
fi

flux_CACHED_COMPILE_CHECK(for .p2align assembler pseudo-op,
			  [asm(".p2align 3");], HAVE_P2ALIGN)

# Our .code16 code needs to be able to generate 32-bit relocs. XXX ???
# Try to figure out how, and punt .code16 altogether if we give up.
save_LDFLAGS="$LDFLAGS"
LDFLAGS="$LDFLAGS -nostartfiles -nostdlib"
AC_CACHE_CHECK(assembler prefix for 32-bit addresses in .code16 mode,
	       flux_cv_asm_addr32, [dnl
flux_cv_asm_addr32=UNKNOWN
# Order matters here:
# Linux binutils-2.9.1.0.x requires `addr32;' and barfs on `addr32';
# Cygnus/GNU binutils > 2.9.1 requires `addr32' to work right, but
# accepts `addr32;' with a warning and does the wrong thing.
# Both of those will accept the insn with no prefix, but do the
# wrong thing.  binutils-2.9.1 barfs on either prefix, but does the
# right thing with no prefix.  We have no reasonable way to test
# whether our assembler does the right thing, just whether it barfs,
# so we assume it is one of the above and will either barf or dtrt.
for flux_try in 'addr32' 'addr32;' ''; do
	AC_TRY_LINK([#define ADDR32 ${flux_try}
#define STRINGIFY(x) STRINGIFY_1(x)
#define STRINGIFY_1(x) #x
#define ADDR32STR STRINGIFY(ADDR32) " "
],
                       [
/* This tests that we can assemble in 16-bit mode an instruction that
   uses a 32-bit address that will need to be relocated and is more than
   64k away from the instruction.  */
asm(".code16;" ADDR32STR "ljmp %cs:frobozz; .space 65537; frobozz:");],
		       [flux_cv_asm_addr32="$flux_try"; break])
done])
LDFLAGS="${save_LDFLAGS}"

if test "x$flux_cv_asm_addr32" = xUNKNOWN; then
	AC_MSG_WARN(cannot assemble 32-bit addresses--punting .code16)
	flux_cv_HAVE_CODE16=no
else
	# Check to see if gas generates actual 16bit code with .code16
	# or wether it generates 32bit code with prefix bytes.
	# We assemble a ret instruction using .code16 and since ret is
	# one byte long, if symbol is at an offset of 2 bytes then we
	# know we've got an older gas (assuming there's no padding).
	AC_CACHE_CHECK(.code16 generates native 16 bit code,
		       flux_cv_code16_new,
	[cat >code16test.S <<\EOF
/* This checks to see if we've got an older version of .code16 that
 * assembles assuming 32bit code and adds prefix bytes so that it will
 * run in 16 bit mode, or we've got a newer .code16 that generates
 * native 16 bit code.
 */
.code16
	ret
symbol:
EOF
	AC_TRY_COMMAND([$CC -c code16test.S])
	if AC_TRY_COMMAND(nm code16test.o | grep "2.*symbol" > /dev/null); then
		flux_cv_code16_new=no
	else
		flux_cv_code16_new=yes
	fi
	rm -rf code16test*])

	flux_CACHED_COMPILE_CHECK(for .code16gcc assembler directive,
				  [asm(".code16gcc");], HAVE_CODE16GCC,
				  [oskit_options="have_code16gcc $oskit_options"])
	if test "x$flux_cv_HAVE_CODE16GCC" = xno &&
	   test "x$flux_cv_code16_new" = xyes; then
		AC_MSG_WARN(must have .code16gcc if .code16 generates 16 bit code--punting .code16)
		flux_cv_HAVE_CODE16=no
	else

		oskit_options="have_addr32 $oskit_options"
		AC_DEFINE_UNQUOTED(ADDR32, $flux_cv_asm_addr32)
		flux_cv_asm_data32=`echo "$flux_cv_asm_addr32" | sed s/addr/data/`
		AC_DEFINE_UNQUOTED(DATA32, $flux_cv_asm_data32)

		# We don't do these checks until after the dependent addr32 check,
		# because we want to avoid defining HAVE_CODE16 at all if addr32
		# is too broken.  (The addr32 check implicitly checks for .code16 too.)
		flux_CACHED_COMPILE_CHECK(for .code16 assembler directive,
					  [asm(".code16");], HAVE_CODE16,
					  [oskit_options="have_code16 $oskit_options"])

	fi
fi


flux_CACHED_COMPILE_CHECK(for working .bss assembler directive, [
	/* NOTE! Splitting this into two asms can break the check! */
	asm(".data; foo: .bss; .space 16; .data; bar: .space foo-bar+1;.text");
], HAVE_WORKING_BSS)

flux_CACHED_COMPILE_CHECK(if compiler groks __attribute__((packed)) on structs,
[	struct foo {
		int x;
	} __attribute__((packed));
], HAVE_PACKED_STRUCTS)

flux_CACHED_COMPILE_CHECK(if compiler groks __attribute__((pure)), [
	void __attribute__((pure)) foo();
], HAVE_PURE)

flux_CACHED_COMPILE_CHECK(if compiler groks __attribute__((noreturn)), [
	void __attribute__((noreturn)) foo();
], HAVE_NORETURN)

#
# arm32 crt lack hack to make constructors work (see crt/x86/crti.h)
#
case $host_arch in
	arm32)
		flux_cv_HAVE_CONSTRUCTOR=no
		;;
[	*) ]
flux_CACHED_COMPILE_CHECK(if compiler groks __attribute__((constructor)), [
	void foo(void) __attribute__ ((constructor));
], HAVE_CONSTRUCTOR)
		;;
esac

#
# Figure out what oskit_subdirs to build.
#
oskit_modulesfile=$srcdir/modules.${host_arch}.${host_impl}
flux_ARG_ENABLE(modulefile,
[  --enable-modulefile=<f> use <f> as the module description file (defaults to ./modules)],
if test $enableval = yes; then enableval=$srcdir/modules; fi
oskit_modulesfile=$enableval)

if test -f $oskit_modulesfile; then
	### Clean out the tabs, then remove the comments and
	### newlines.  Really, that's what it does.
	oskit_subdirs=`cat $oskit_modulesfile | tr '\t' ' ' | sed -n -e '{s/#.*$//;H;}; ${g;s/\n/ /g;s/  */ /g;p;}'`
	oskit_subdirs=" $oskit_subdirs "
	## AC_MSG_WARN(Using module file $oskit_modulesfile)
	## AC_MSG_WARN(OSKit subdirs is $oskit_subdirs)
else
	AC_MSG_ERROR([OSKit modules file $oskit_modulesfile was not found.])
fi

### Cannot build the netbsd/fs module if the compiler is inadequate.
### (arch/i386/include/segments.h needs packed structs.)
case $oskit_subdirs in
[	*\ netbsd/fs\ *) ]
	if test $flux_cv_HAVE_PACKED_STRUCTS = no; then
		AC_MSG_WARN([Cannot build module netbsd/fs because your compiler does not support packed structs.])
		oskit_subdirs=`echo $oskit_subdirs | sed 's,netbsd/fs,,'`
	fi
	;;
esac

# All the mk*image scripts require the `-format binary' input format ld option.
AC_CACHE_CHECK(if $LD supports -format binary, flux_cv_ld_format_binary,
[cat >conftest.S <<\EOF
.long _binary_conftest_o_start
.long _binary_conftest_o_end
EOF
AC_TRY_COMMAND([$CC $CFLAGS -c conftest.S])
if AC_TRY_COMMAND([$LD -Ttext 100000 -o conftest conftest.o
		       -format binary conftest.o -format default 1>&2 &&
		   test -x conftest]); then
  flux_cv_ld_format_binary=yes
else
  flux_cv_ld_format_binary=no
fi
rm -rf conftest*])

# If they want the multiboot adaptor, check that the '-format binary'
# option is supported by the linker.
case $oskit_subdirs in
[	*\ boot/multiboot\ *) ]
	if test $flux_cv_ld_format_binary = yes; then
		outfiles="$outfiles boot/multiboot/mkmbimage"
		if test ! -z "$PERL"; then
			outfiles="$outfiles boot/multiboot/mkmb2"
		else
			AC_MSG_WARN([Skipping build of mkmb2 because perl was not found.])
		fi
	else
		AC_MSG_WARN([Cannot build multiboot adaptor (boot/multiboot) as '-format binary' is not supported by linker.])
		oskit_subdirs=`echo $oskit_subdirs | sed 's,boot/multiboot,,'`
	fi
	;;
esac

# The boot/linux/mklinuximage script requires the `-oformat binary'
# output format ld option.
AC_CACHE_CHECK(if $LD supports -oformat binary, flux_cv_ld_oformat_binary,
[cat >conftest.S <<\EOF
.long _binary_conftest_o_start
.long _binary_conftest_o_end
EOF
AC_TRY_COMMAND([$CC $CFLAGS -c conftest.S])
if AC_TRY_COMMAND([$LD -Ttext 100000 -o conftest -oformat binary conftest.o
		       -format binary conftest.o -format default 1>&2 &&
		   test -x conftest]); then
  flux_cv_ld_oformat_binary=yes
else
  flux_cv_ld_oformat_binary=no
fi
rm -rf conftest*])

# Make sure the tools can support the Linux boot
# adaptor if it is so desired.
case $oskit_subdirs in
[ 	*\ boot/linux\ *) ]

	if test $flux_cv_ld_oformat_binary = yes &&
	   test $flux_cv_HAVE_CODE16 = yes ; then
		outfiles="$outfiles boot/linux/mklinuximage"
		if test ! -z "$PERL"; then
			outfiles="$outfiles boot/linux/mklinux2"
		else
			AC_MSG_WARN([Skipping build of mklinux2 because perl was not found.])
		fi
	else
		AC_MSG_WARN([Cannot build Linux boot adaptor (boot/linux).])
		oskit_subdirs=`echo $oskit_subdirs | sed 's,boot/linux,,'`
	fi
	;;
esac

AC_SUBST(DOS_LD)
if test "x$DOS_LD" = x; then
	DOS_LD='${LD}'
	flux_dosld="${LD}"
else
	flux_dosld="${DOS_LD}"
fi

# We need the `-oformat msdos' ld output format option to build
# the DOS boot adaptor (dosboot.exe).
AC_CACHE_CHECK(if ${flux_dosld} supports -oformat msdos,
               flux_cv_ld_oformat_msdos,
[echo '_start() { }' >conftest.c
AC_TRY_COMMAND([$CC $CFLAGS -c conftest.c])
if AC_TRY_COMMAND([${flux_dosld} -Ttext 0 -o conftest
			   -oformat msdos conftest.o 1>&2 &&
		   test -x conftest]); then
	flux_cv_ld_oformat_msdos=yes
else
	flux_cv_ld_oformat_msdos=no
fi
rm -rf conftest*])

case $oskit_subdirs in
[ 	*\ boot/dos\ *) ]
	if test $flux_cv_ld_oformat_msdos = yes && test -d $srcdir/boot/dos; then
		outfiles="$outfiles boot/dos/mkdosimage"
	else
		AC_MSG_WARN([Cannot build MS-DOS boot adaptor (boot/dos) as linker does not support '-oformat msdos'.])
		oskit_subdirs=`echo $oskit_subdirs | sed 's,boot/dos,,'`
	fi
	;;
esac

AC_SUBST(AOUT_LD)
AC_CACHE_CHECK(for a.out linker, flux_cv_AOUT_LD, [dnl
if test "x${AOUT_LD}" != x; then
	flux_cv_AOUT_LD="${AOUT_LD}"
else
	echo '_start() { }' >conftest.c
	AC_TRY_COMMAND([$CC $CFLAGS -c conftest.c])
	flux_cv_AOUT_LD=NONE
	for flux_try in i386mach i386bsd i386nbsd i386aout armnbsd armaoutl; do
		if AC_TRY_COMMAND([${LD} -m ${flux_try} -n
					 -Ttext 0 -o conftest
					 conftest.o 1>&2 &&
				   test -x conftest]); then
			flux_cv_AOUT_LD="${LD} -m ${flux_try}"
			break
		fi
	done
	rm -rf conftest*
fi])
test "x${AOUT_LD}" != x || AOUT_LD="${flux_cv_AOUT_LD}"

#
# Check for various -f flags that might need to be given.
# At the moment, we just worry about no-strict-aliasing for gcc 2.95.
#
AC_SUBST(OSKIT_FFLAGS)
AC_CACHE_CHECK(if $CC supports -fno-strict-aliasing, flux_cv_no_strict_aliasing,
[cat >conftest.c <<\EOF
main() {}
EOF
if AC_TRY_COMMAND([$CC -fno-strict-aliasing -c conftest.c]); then
  flux_cv_no_strict_aliasing=yes
else
  flux_cv_no_strict_aliasing=no
fi
rm -rf conftest*])
if test $flux_cv_no_strict_aliasing = yes; then
  OSKIT_FFLAGS="$OSKIT_FFLAGS -fno-strict-aliasing"
fi

case $oskit_subdirs in
[ 	*\ boot/bsd\ *) ]
	if test "x$flux_cv_AOUT_LD" != xNONE && test -d $srcdir/boot/bsd; then
		outfiles="$outfiles boot/bsd/mkbsdimage boot/bsd/unmkbsdimage"
	else
		AC_MSG_WARN([Cannot build BSD boot adaptor (boot/bsd) as linker does not support a.out format.])
		oskit_subdirs=`echo $oskit_subdirs | sed 's,boot/bsd,,'`
	fi
	;;
esac

case $oskit_subdirs in
[ 	*\ boot/ofw\ *) ]
	if test "x$flux_cv_AOUT_LD" != xNONE && test -d $srcdir/boot/ofw; then
		outfiles="$outfiles boot/ofw/mkofwimage"
	else
		AC_MSG_WARN([Cannot build OFW boot adaptor (boot/ofw) as linker does not support a.out format.])
		oskit_subdirs=`echo $oskit_subdirs | sed 's,boot/ofw,,'`
	fi
	;;
esac

### Profiling is still an --enable option (and not a module config file thing)
### because it effects the build of all the other modules.
flux_ARG_ENABLE(profiling,
[  --enable-profiling      build profiled versions of the oskit libraries],
case $oskit_subdirs in
[ 	*\ realtime\ *) ]
		[oskit_options="profiling $oskit_options"]
		[oskit_subdirs="$oskit_subdirs gprof"]
		;;
[	*)]
		AC_MSG_ERROR([Must enable realtime module with profiling])
		;;
esac
)

AC_ARG_ENABLE(knit,
[  --enable-knit           compile for use with Utah Knit tools],
[
if test ["x${enableval}"] != xno; then
changequote(<<, >>)
<<
  oskit_subdirs=`echo $oskit_subdirs | sed 's,examples/[a-z/0-9]* ,,'g`
  oskit_subdirs=`echo $oskit_subdirs | sed 's, boot/net , ,'`
>>
changequote([, ])
  AC_DEFINE(KNIT)
else
  # Do not build Knit-related subdirectories.
  oskit_subdirs=`echo $oskit_subdirs | sed 's,knit/c ,,'`
fi
],
[
  # Do not build Knit-related subdirectories.
  oskit_subdirs=`echo $oskit_subdirs | sed 's,knit/c ,,'`
])

###
### If OSKit-on-UNIX support was requested, make sure it is available.
### Remove any unix examples if support is not available or requested.
###

unix_host=NONE
unix_objformat=NONE

case $oskit_subdirs in
[ 	*\ unix\ *) ]
	## See if we can build on the host
	case $host_os in
		linux*)		unix_host=linux; unix_objformat=elf;;
		freebsd2*)      unix_host=freebsd; unix_objformat=aout;;
		freebsdelf*)	unix_host=freebsd; unix_objformat=elf;;
		freebsd[[34]]*)	unix_host=freebsd; unix_objformat=`objformat`;;
	esac

	if test $unix_host = NONE; then
		AC_MSG_WARN([Unsupported unix host environment '$host_os'; Unixmode support will not be built.])
	elif test $build != $host; then
		AC_MSG_WARN([Unix-mode support cannot be built when cross-compiling])
		unix_host=NONE
	fi

	if test x"$unix_objformat" != x"elf"; then
		AC_MSG_WARN([Unix-mode support must be built with native ELF])
		unix_host=NONE
	fi

	if test $unix_host = NONE; then
		oskit_subdirs=`echo $oskit_subdirs | sed 's, unix,,'`
		oskit_subdirs=`echo $oskit_subdirs | sed 's, examples/unix , ,'`
		oskit_subdirs=`echo $oskit_subdirs | sed 's, examples/unix/extended , ,'`
		oskit_subdirs=`echo $oskit_subdirs | sed 's, examples/unix/threads , ,'`
	fi
	;;

[	*\ examples/unix*) ]
	AC_MSG_WARN([Not building unix-mode examples, because unix support library was not enabled.])
	oskit_subdirs=`echo $oskit_subdirs | sed 's, examples/unix , ,'`
	oskit_subdirs=`echo $oskit_subdirs | sed 's, examples/unix/extended , ,'`
	oskit_subdirs=`echo $oskit_subdirs | sed 's, examples/unix/threads , ,'`
	;;
esac

AC_SUBST(unix_host)
AC_SUBST(unix_objformat)

if test $unix_host != NONE; then
###
### Do some tests used by the unix/ support.
###

NATIVE_DEFS=-D_GNU_SOURCE

AC_CACHE_CHECK(for d_namlen in struct dirent, flux_cv_native_d_namlen, [
AC_TRY_COMPILE([#include <sys/types.h>
#include <dirent.h>],
               [int sz = 1 + ((struct dirent *)0)->d_namlen;],
	       flux_cv_native_d_namlen=yes,
	       flux_cv_native_d_namlen=no)])
AC_CACHE_CHECK(for d_ino in struct dirent, flux_cv_native_d_ino, [
AC_TRY_COMPILE([#include <sys/types.h>
#include <dirent.h>],
               [int sz = 1 + ((struct dirent *)0)->d_ino;],
	       flux_cv_native_d_ino=yes,
	       flux_cv_native_d_ino=no)])
AC_CACHE_CHECK(for d_fileno in struct dirent, flux_cv_native_d_fileno, [
AC_TRY_COMPILE([#include <sys/types.h>
#include <dirent.h>],
               [int sz = 1 + ((struct dirent *)0)->d_fileno;],
	       flux_cv_native_d_fileno=yes,
	       flux_cv_native_d_fileno=no)])
if test $flux_cv_native_d_namlen = yes; then
	NATIVE_DEFS="$NATIVE_DEFS -DNATIVE_HAVE_D_NAMLEN"
fi
if test $flux_cv_native_d_ino = yes; then
	NATIVE_DEFS="$NATIVE_DEFS -DNATIVE_HAVE_D_INO"
elif test $flux_cv_native_d_fileno = yes; then
	NATIVE_DEFS="$NATIVE_DEFS -DNATIVE_HAVE_D_FILENO"
fi

AC_CACHE_CHECK(for st_atimespec in struct stat, flux_cv_native_st_atimespec, [
AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>],
               [struct timespec *ts = &((struct stat *)0)->st_atimespec;],
	       flux_cv_native_st_atimespec=yes,
	       flux_cv_native_st_atimespec=no)])
if test $flux_cv_native_st_atimespec = yes; then
	NATIVE_DEFS="$NATIVE_DEFS -DNATIVE_HAVE_ST_ATIMESPEC"
fi

AC_CACHE_CHECK(for sa_len in struct sockaddr, flux_cv_native_sa_len, [
AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/socket.h>],
               [struct sockaddr sa; int len = sa.sa_len;],
	       flux_cv_native_sa_len=yes,
	       flux_cv_native_sa_len=no)])
if test $flux_cv_native_sa_len = yes; then
	NATIVE_DEFS="$NATIVE_DEFS -DNATIVE_HAVE_SA_LEN"
fi

AC_SUBST(NATIVE_DEFS)
fi # $unix_host != NONE

###
### Test GCC for spec-file support
###

AC_CACHE_CHECK([if $CC supports -specs], flux_cv_gcc_new_specs,
[if ${CC} -specs /dev/null -v >&AC_FD_CC 2>&AC_FD_CC; then
	flux_cv_gcc_new_specs=yes
else
	flux_cv_gcc_new_specs=no
fi])
if test $flux_cv_gcc_new_specs = yes; then
	GCC_NEEDS_FULL_SPECS=no
	NEED_FULL_SPECS='### for older GCC # '
	NO_NEED_FULL_SPECS=
elif test $host != $build; then
	AC_MSG_WARN([cross-compiling gcc too old for ${host_arch}-oskit-gcc, not installing it])
	GCC_NEEDS_FULL_SPECS=broken
	NO_NEED_FULL_SPECS='XXX this is broken XXX'
	NEED_FULL_SPECS='XXX this is broken XXX'
else
	GCC_NEEDS_FULL_SPECS=yes
	NO_NEED_FULL_SPECS='### for newer GCC # '
	NEED_FULL_SPECS=
fi
AC_SUBST(GCC_NEEDS_FULL_SPECS)dnl
AC_SUBST(NEED_FULL_SPECS)dnl
AC_SUBST(NO_NEED_FULL_SPECS)


###
### Only build subdirectories for which we have the source.
### This allows people to download and build partial OSKIT trees easily.
###
new_subdirs=
for d in $oskit_subdirs; do
	if test -d $srcdir/$d; then
		new_subdirs="$new_subdirs $d"
		# This is needed since AC_OUTPUT doesn't do mkdir -p.
		test -d $d || mkdir -p $d
	fi
done
oskit_subdirs="$new_subdirs"

AC_SUBST(oskit_subdirs)

#
# Other command line options.
#

flux_ARG_ENABLE(debug,
[  --enable-debug          internal debugging features and sanity checks],
[OSKIT_DEFS="$OSKIT_DEFS -DDEBUG"]
[oskit_options="debug $oskit_options"])

# Turn asserts off if they want.
AC_ARG_ENABLE(asserts,
[  --disable-asserts       compile out assert() calls],
[if test "$enableval" = "no"; then
	OSKIT_DEFS="$OSKIT_DEFS -DNDEBUG"
	oskit_options="ndebug $oskit_options"
fi])

flux_ARG_ENABLE(linux-bogomips,
[  --enable-linux-bogomips[=bogomips]
                          specify bogomips, don't calibrate (default 300)],
[if test $enableval = yes; then enableval=300; fi]
[OSKIT_DEFS="$OSKIT_DEFS -DLINUX_BOGOMIPS=$enableval"]
[oskit_options="linux-bogomips=$enableval $oskit_options"])

AC_ARG_ENABLE(indirect-osenv,
[  --disable-indirect-osenv libraries do not use COM indirection for osenv])
if test "x$enable_indirect_osenv" != xno; then
	OSKIT_DEFS="$OSKIT_DEFS -DINDIRECT_OSENV=1"
	oskit_options="indirect_osenv $oskit_options"
fi


outfiles="$outfiles Makeconf GNUmakefile unsupported/scripts/gcc-driver-script"
outfiles="$outfiles unsupported/scripts/sym-trace"
for d in $oskit_subdirs; do
	outfiles="$outfiles $d/GNUmakefile"
done

AC_OUTPUT($outfiles)
