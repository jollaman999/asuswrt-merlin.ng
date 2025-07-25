##########################################################################
# LPRng - An Extended Print Spooler System
#
# Copyright 1988-2001 Patrick Powell, San Diego, California
#     papowell@lprng.com
# See LICENSE for conditions of use.
#
###########################################################################
# MODULE: Makefile
# Makefile.in,v 1.1 2001/08/21 20:33:17 root Exp
###########################################################################
#

#************** GENERAL CONFIGURATION INFORMATION ************************
# where user commands are installed: lpr, lpq, lprm, lpc.
# Also the SysV emulation commands: lp, lpstat.
# If these paths are /usr/lib, /usr/ucb etc., make sure you
# have backup copies of the system version!
#
#
# These are automatically sedded in by the "configure" script, but you
# may need to change them if you're on a strange version of UNIX.
include ../../.config


PACKAGE=LPRng
VERSION=3.8.22

# where the source directory is
SRC=.
top_srcdir=..
top_builddir=..
#SRCBASE=../../..
#TOP
# which make we are using


# what C compiler to use.
#CC:=mipsel-uclibc-gcc
#CC:=gcc
#CC:=mipsel-linux-gcc

# what command to use to "ranlib" or "lorder" the library.
#RANLIB=mipsel-uclibc-ranlib
#AR=mipsel-uclibc-ar
#RANLIB=mipsel-linux-ranlib
#AR=mipsel-linux-ar

# what C preprocessor to use.
#CPP:=mipsel-uclibc-gcc -E
#CPP:=gcc -E
#CPP:=mipsel-linux-gcc -E
CPP:= $(CC) -E

# how to install stuff.
INSTALLCMD=/usr/bin/install -c
# a shell interpreter that's as close to the POSIX shell as possible.
SHELL_PATH=/bin/sh
SHELL = /bin/sh

# loader for c programs
#LDCC=mipsel-uclibc-gcc
#LDCC=gcc
#LDCC=mipsel-linux-gcc
LDCC=$(CC)

# any libs that need to be used to get these programs to compile:
LIBS=  

# what libtool
LIBTOOL=$(SHELL) $(top_builddir)/libtool

### PATHS ####
# prefix
prefix=/usr/local
# exec prefix
exec_prefix=${prefix}
# for binaries
bindir=${exec_prefix}/bin
# for admin type of binaries
sbindir=${exec_prefix}/sbin
# for the filters
libexecdir=${exec_prefix}/libexec
# for the configuration stuff
sysconfdir=${prefix}/etc
# libraries
libdir=${exec_prefix}/lib

# install options

STRIP= -s 

CFLAGS:= \
	-I${top_builddir} -I. \
	-I${SRC}/include -I${SRC}/common -I$(TOP)/shared -I$(SRCBASE)/include \
	 -g -W -Wall  -Wno-unused  -g -O2 -g -W -Wall  -Wno-unused  -DHAVE_CONFIG_H -DWINDOW_1 -DTEST_WRITE -DNODEBUG -DLPR_with_ASUS ${GLOBAL_OPTIONS} #-DRETURNOK -DUSBONLY

ifeq ($(HND_ROUTER_BE_4916), y)
CFLAGS+= -fcommon
endif

USER_OBJS := 
LDFLAGS   := -L$(TOP)/shared -lshared -L$(TOP_PLATFORM)/nvram${BCMEX}${EX7} -lnvram
ifeq ($(RTCONFIG_BCM_502L07P2),y)
LDFLAGS += -L$(TOP_PLATFORM)/ethctl_lib -lethctl
endif
ifeq ($(HND_ROUTER),y)
LDFLAGS += -L$(TOP_PLATFORM)/wlcsm -lwlcsm -lgcc_s
endif

ifeq ($(RTCONFIG_QTN),y)
LDFLAGS += -L$(TOP)/libqcsapi_client -lqcsapi_client
endif

ifeq ($(RTCONFIG_TRUSTZONE),y)
LDFLAGS += -L$(TOP)/ateed/ -latee
endif

ifeq ($(BUILD_NAME),$(filter $(BUILD_NAME),RT-BE82M GS-BE18000 GS-BE12000 GS7_PRO GT7))
LDFLAGS += -L$(TOP)/libhostapi -lhostapi
endif

FILTER_DIR = ${libexecdir}/filters
LPD_DIR    = ${sbindir}


#********************** End of configure modifiable settings ************


#********************** OPTIONAL DEFINITIONS ****************************
# These are various flags and settings that will be used by the
# compiler to generate code
#

#### ****** TESTING AND SECURITY LOOPHOLE ******************************
# Define G ETENV to allow the LPD_CONFIG environment
#  variable to be used as the name of a configuration file.  In non-testing
#  systems,  this is a security loophole.
#CFLAGS:= $(CFLAGS) -DGETENV=\"1\"

# Set location of configuration, permissions, and printcap file
#
#GNU

# CFLAGS:= $(CFLAGS) -DFORMAT_TEST -Wformat


##### DMalloc Debugging Package
#
#CFLAGS:= $(CFLAGS) -DDMALLOC
#LIBS:= $(LIBS) -ldmalloc



#********************* OVERRIDING CONFIGURES' GUESSES ******************
#
#
# If you get errors similar to the ones below, it may be
# because "configure" guessed wrong. You can override the
# guesses here, by uncommenting the appropriate line...

# "recvfiles.c: unknown struct fsb has no size":
# define STATFS: to use statfs(2) (BSD)
#        STATVFS: to use statvfs(2) (SVR4)
#     others for system specific cases
#
# CFLAGS:= $(CFLAGS) -DMAKE_USE_STATFS=ULTRIX_STATFS
# CFLAGS:= $(CFLAGS) -DMAKE_USE_STATFS=SVR3_STATFS
# CFLAGS:= $(CFLAGS) -DMAKE_USE_STATFS=STATVFS
# CFLAGS:= $(CFLAGS) -DMAKE_USE_STATFS=STATFS

# define MAKE_USE_STTY = 
#    SGTTYB   to use struct sgttyb and <sgtty.h> (BSD)
#    TERMIO  to use struct termio and <termio.h> (old sysV)
#    TERMIOS to use struct termios and <termio.h> (SVR4)
#
# CFLAGS:= $(CFLAGS) -DMAKE_USE_STTY=SGTTYB
# CFLAGS:= $(CFLAGS) -DMAKE_USE_STTY=TERMIO
# CFLAGS:= $(CFLAGS) -DMAKE_USE_STTY=TERMIOS

# a better way to do this is to edit the "ARGH" section of portable.h,
# and add the appropriate lines to the section for your OS, or add a new
# section if one doesn't exist; then you can send me the patches and
# I'll incorporate them into the distribution.

#
# You might discover that the default locking mechanism
# does not work.  Try one of the following to override the
# configured value: fcntl(), lockf(), flock(), and 0 to suppress locking
#
# CFLAGS:= $(CFLAGS) -DLOCK_DEVS=devlock_fcntl
# CFLAGS:= $(CFLAGS) -DLOCK_DEVS=devlock_lockf
# CFLAGS:= $(CFLAGS) -DLOCK_DEVS=devlock_flock
# CFLAGS:= $(CFLAGS) -DLOCK_DEVS=0
#

# SETPROCTITLE - overwrites the program argument information
#   when ps is used, displays status.  Used only by LPD
# if this does not work, define the following
# CFLAGS:= $(CFLAGS) -DNO_SETPROCTITLE


SRCDIRS=${SRC}/common ${SRC}/AUTHENTICATE
INCLUDE=.. ${SRC}/include

VPATH=..:.:${SRC}/include:${SRC}/common:${SRC}

########################################################################

.SUFFIXES: .lo .sh
LINK =  $(LIBTOOL) --mode=link $(LDCC) $(LDFLAGS) -o $@
.c.lo:
	$(LIBTOOL) --mode=compile $(CC) $(CFLAGS) -c $<

#**********************  End of Configurable Part ****************************

TARGET= \
	lprng_certs lprng_index_certs \
	lpd\
	./lpd.conf
#TARGET= \
	lprng_certs lprng_index_certs \
	lpc lpd lpq lpr lprm lpf checkpc lp lpstat \
	lpbanner monitor ./lpd.conf

NONTARGET = sserver sclient

LPD_OBJS= lpd.o
LPR_OBJS= lpr.o
LPQ_OBJS= lpq.o
LPSTAT_OBJS= lpstat.o
LPRM_OBJS= lprm.o
LPC_OBJS= lpc.o
CHECKPC_OBJS = checkpc.o
LPF_OBJS = lpf.o plp_snprintf.o
MONITOR_OBJS= monitor.o getopt.o
LPBANNER_OBJS= lpbanner.o plp_snprintf.o

LIBLPR_OBJS = \
	child.lo controlword.lo  \
	debug.lo errormsg.lo fileopen.lo gethostinfo.lo \
	getopt.lo getprinter.lo getqueue.lo globmatch.lo \
	initialize.lo  \
	linelist.lo linksupport.lo lockfile.lo \
	lpd_control.lo lpd_dispatch.lo lpd_jobs.lo lpd_rcvjob.lo \
	lpd_remove.lo  lpd_status.lo \
	 permission.lo plp_snprintf.lo printjob.lo\
	proctitle.lo      \
	 utilities.lo vars.lo  $(USER_OBJS)
#LIBLPR_OBJS = \
	accounting.lo child.lo controlword.lo copyright.lo \
	debug.lo errormsg.lo fileopen.lo gethostinfo.lo \
	getopt.lo getprinter.lo getqueue.lo globmatch.lo \
	initialize.lo krb5_auth.lo \
	linelist.lo linksupport.lo lockfile.lo \
	lpd_control.lo lpd_dispatch.lo lpd_jobs.lo lpd_logger.lo lpd_rcvjob.lo \
	lpd_remove.lo sendauth.lo lpd_secure.lo lpd_status.lo \
	merge.lo permission.lo plp_snprintf.lo printjob.lo \
	proctitle.lo sendjob.lo  sendmail.lo sendreq.lo ssl_auth.lo \
	stty.lo user_auth.lo utilities.lo vars.lo md5.lo $(USER_OBJS)

ALL_OBJS = $(LIBLPR_OBJS) \
	$(LPD_OBJS) \
	
#ALL_OBJS = $(LIBLPR_OBJS) \
	$(LPR_OBJS) $(LPQ_OBJS) $(LPC_OBJS) $(LPRM_OBJS) $(LPD_OBJS) \
	$(CHECKPC_OBJS) $(LPF_OBJS) $(LPRACCNT_OBJS) $(MONITOR_OBJS) $(LPBANNER_OBJS)

all: $(TARGET)

relink:
	rm -f ${TARGET}

defs.h: Makefile
	cp /dev/null $@
	echo "#define " CHOWN \"/bin/chown\" >>$@
	echo "#define " CHGRP \"/bin/chgrp\" >>$@
	echo "#define " PRINTCAP_PATH \"${sysconfdir}/printcap\" >>$@
	echo "#define " LPD_PRINTCAP_PATH \"${sysconfdir}/lpd/lpd_printcap\" >>$@
	echo "#define " LPD_PERMS_PATH \"${sysconfdir}/lpd/lpd.perms\" >>$@
	echo "#define " LPD_CONF_PATH \"${sysconfdir}/lpd/lpd.conf\" >>$@
	echo "#define " REQUIRE_CONFIGFILES \"1\" >>$@
	echo "#define " FILTER_PATH \"/bin:/usr/bin:/usr/local/bin\" >>$@
	echo "#define " LD_LIBRARY_PATH \"/lib:/usr/lib:/usr/local/lib\" >>$@
	echo "#define " LOCKFILE \"/var/run/lpd\" >>$@
	echo "#define " CLEAR \"/usr/bin/clear\" >>$@
	echo "#define " USERID \"root\" >>$@
	echo "#define " GROUPID \"root\" >>$@
	echo "#define " LOCALEDIR \"${prefix}/share/locale\" >>$@
	echo "#define " DONE_JOBS \"1\" >>$@
	echo "#define " DONE_JOBS_MAX_AGE \"0\" >>$@
	echo "#define " UNIXSOCKETPATH \"/var/run/lprng\" >>$@
	echo "#define " PRUTIL \"/usr/bin/pr\" >>$@
	echo "#define " OPENSSL \"/usr/bin/openssl\" >>$@
	echo "#define " SSL_CA_FILE \"${sysconfdir}/lpd/ssl.ca/ca.crt\" >>$@
	echo "#define " SSL_CA_KEY \"${sysconfdir}/lpd/ssl.ca/ca.key\" >>$@
	echo "#define " SSL_CRL_FILE \"${sysconfdir}/lpd/ssl.crl/ssl.crl\" >>$@
	echo "#define " SSL_CERTS_DIR \"${sysconfdir}/lpd/ssl.certs\" >>$@
	echo "#define " SSL_SERVER_CERT \"${sysconfdir}/lpd/ssl.server/server.crt\" >>$@
	echo "#define " SSL_SERVER_PASSWORD_FILE \"${sysconfdir}/lpd/ssl.server/server.pwd\" >>$@

.PHONY: all ci clean \
	uninstall realclean mostlyclean distclean \
	TAGS info generate

# we want to force defaults to be updated whenever we
# change the options in Makefile or Makefile.bsd
#GNU
vars.o vars.lo: Makefile
#
#BSD vars.o: Makefile.bsd

#
# use RANLIB with no ordering, or lorder/tsort if it is there
# if you do not have either, fall back and just build archive
# This appears to cover all cases for portability
#


liblpr.la: $(LIBLPR_OBJS)
	$(LINK) $(LIBLPR_OBJS) -rpath ${libdir}

lpd: $(LPD_OBJS) liblpr.la
	$(LINK) ${LPD_OBJS} liblpr.la $(LIBS)
#lpr: $(LPR_OBJS) liblpr.la
#	$(LINK) ${LPR_OBJS} liblpr.la $(LIBS)
#lpq: $(LPQ_OBJS) liblpr.la
#	$(LINK) ${LPQ_OBJS} liblpr.la $(LIBS)
#lpstat: $(LPSTAT_OBJS) liblpr.la
#	$(LINK) ${LPSTAT_OBJS} liblpr.la $(LIBS)
#lprm: $(LPRM_OBJS) liblpr.la
#	$(LINK) ${LPRM_OBJS} liblpr.la $(LIBS)
#lpc: $(LPC_OBJS) liblpr.la
#	$(LINK) ${LPC_OBJS} liblpr.la $(LIBS)
#checkpc: $(CHECKPC_OBJS) liblpr.la
#	$(LINK) ${CHECKPC_OBJS} liblpr.la $(LIBS)
#lpf: $(LPF_OBJS) liblpr.la
#	$(LINK) ${LPF_OBJS} $(LIBS)
#lpbanner: $(LPBANNER_OBJS) liblpr.la
#	$(LINK) ${LPBANNER_OBJS} $(LIBS)
#monitor: $(MONITOR_OBJS) liblpr.la
#	$(LINK) ${MONITOR_OBJS} liblpr.la $(LIBS)

authenticate_pgp: ${SRC}/AUTHENTICATE/authenticate_pgp.sh
	rm -f $@; cp ${SRC}/AUTHENTICATE/authenticate_pgp.sh $@; chmod 555 $@

sserver: sserver.o liblpr.la
	$(LINK) sserver liblpr.la $(LIBS)
sclient: sclient.o liblpr.la
	$(LINK) sclient liblpr.la $(LIBS)

cancel:
	rm -f $@; ln -s lprm $@ 
lp: lpr
	rm -f $@; ln -s lpr $@

###########################################################################
# lpd.conf

./lpd.conf: Makefile vars.c
	set -e; \
	rm -f $@ ; \
	echo "# lpd.conf" >/tmp/$@.$$$$ ; \
	echo "# VERSION=$(VERSION)" >>/tmp/$@.$$$$ ; \
	$(CPP) $(CFLAGS) -C ${SRC}/common/vars.c \
	 | /bin/sh ../UTILS/make_lpd_conf >>/tmp/$@.$$$$ ; \
	chmod 644 /tmp/$@.$$$$ ; \
	cp /tmp/$@.$$$$ $@ ; rm /tmp/$@.$$$$

.sh:
	set -e ; \
	rm -f $@ ; \
	sed \
	-e "s,@OPENSSL.,/usr/bin/openssl," \
	-e "s,@SSL_CA_FILE.,${sysconfdir}/lpd/ssl.ca/ca.crt," \
	-e "s,@SSL_CA_KEY.,${sysconfdir}/lpd/ssl.ca/ca.key," \
	-e "s,@SSL_CRL_FILE.,${sysconfdir}/lpd/ssl.crl/ssl.crl," \
	-e "s,@SSL_SERVER_CERT.,${sysconfdir}/lpd/ssl.server/server.crt," \
	-e "s,@SSL_SERVER_PASSWORD_FILE.,${sysconfdir}/lpd/ssl.server/server.pwd," \
	-e "s,@SSL_CERTS_DIR.,${sysconfdir}/lpd/ssl.certs," \
	$< >$@
	chmod 755 $@

lprng_certs: Makefile lprng_certs.sh
lprng_index_certs: Makefile lprng_index_certs.sh

###########################################################################
#

NORM_PERMS=0755
SUID_ROOT_PERMS=04755 -o root

###########################################################################

install: all
	echo "Clients are being installed NORM_PERMS";
	for i in ${DESTDIR}${bindir} ${DESTDIR}${sbindir} \
		$(DESTDIR)$(libdir) ${DESTDIR}${sysconfdir} \
		${DESTDIR}${LPD_DIR} ${DESTDIR}${FILTER_DIR}; do \
	  if [ ! -d $$i ] ; then ${SRC}/mkinstalldirs $$i ; fi; \
	done;

	${LIBTOOL} --mode=install $(INSTALLCMD) liblpr.la $(DESTDIR)$(libdir)/liblpr.la
	${LIBTOOL} --mode=install ${INSTALLCMD} ${STRIP} -m $(NORM_PERMS) lpq ${DESTDIR}${bindir}
	${LIBTOOL} --mode=install ${INSTALLCMD} ${STRIP} -m $(NORM_PERMS) lprm ${DESTDIR}${bindir}
	${LIBTOOL} --mode=install ${INSTALLCMD} ${STRIP} -m $(NORM_PERMS) lpr ${DESTDIR}${bindir}
	${LIBTOOL} --mode=install ${INSTALLCMD} ${STRIP} -m $(NORM_PERMS) lpstat ${DESTDIR}${bindir}
	${LIBTOOL} --mode=install ${INSTALLCMD} ${STRIP} -m $(NORM_PERMS) lpc ${DESTDIR}${sbindir}
	${LIBTOOL} --mode=install ${INSTALLCMD} ${STRIP} -m $(NORM_PERMS) lpd ${DESTDIR}${LPD_DIR}
	${LIBTOOL} --mode=install ${INSTALLCMD} ${STRIP} -m $(NORM_PERMS) lpf ${DESTDIR}${FILTER_DIR}
	${LIBTOOL} --mode=install ${INSTALLCMD} ${STRIP} -m $(NORM_PERMS) lpbanner ${DESTDIR}${FILTER_DIR}
	${LIBTOOL} --mode=install ${INSTALLCMD}    -m $(NORM_PERMS) pclbanner ${DESTDIR}${FILTER_DIR}
	${LIBTOOL} --mode=install ${INSTALLCMD}    -m $(NORM_PERMS) psbanner ${DESTDIR}${FILTER_DIR}
	${LIBTOOL} --mode=install ${INSTALLCMD} ${STRIP} -m $(NORM_PERMS) checkpc ${DESTDIR}${sbindir}
	${LIBTOOL} --mode=install ${INSTALLCMD}     -m $(NORM_PERMS) lprng_certs ${DESTDIR}${sbindir}
	${LIBTOOL} --mode=install ${INSTALLCMD}     -m $(NORM_PERMS) lprng_index_certs ${DESTDIR}${sbindir}
	cd ${DESTDIR}${bindir}; rm -f lp cancel; ln -s lpr lp; ln -s lprm cancel;

uninstall:

###########################################################################


clean::
	-rm -rf *.o *.tmp *core *.a ? ?.bak ?.orig $(TARGET) $(NONTARGET) st[0-9]* \
	tags ./lpd.conf */*.old */*.bak */*.orig .libs *.lo *.la log defs.h

realclean mostlyclean distclean:: clean
	-rm -f Makefile Makefile.bsd pclbanner psbanner

info:
tags TAGS:
	ctags -t -d `ls common/*.c include/*.h | sed -e '/checkpc.c/d' `

###########################################################################

CI=
#CO=-kv
CO=-l

cifiles:
	if [ ! -d RCS ] ; then mkdir RCS ; fi ;
	checkin() { \
		(set +e; rcs -l $$1; exit 0); \
		ci $(CI) -l -u -f -mUpdate -t-Initial $$1; \
	}; \
	for i in *; do \
		if [ -f "$$i" ] ; then \
			case $$i in \
				Makefile*|*install* ) echo $$i; checkin $$i ;; \
				*.in ) echo $$i; checkin $$i ;; \
			esac; \
		fi; \
	done;

cifast ci: cifiles
	checkin() { \
		(set +e; rcs -l $$1; exit 0); \
		ci $(CI) -l -u -f -mUpdate -t-Initial $$1; \
	}; \
	for i in *; do \
		if [ -d "$$i" ] ; then \
			case $$i in \
			RCS ) ;; \
			* ) if [ ! -d $$i/RCS ] ; then mkdir $$i/RCS ; fi ; \
				for j in $$i/*; do \
					if [ -f "$$j" ] ; then \
						checkin $$j; \
					fi; \
				done; \
				;; \
			esac; \
		fi; \
	done;

depend: /tmp/dep /tmp/order
	( sed -n '1,/^##.*GENERATED/p' ${SRC}/Makefile.in; \
		echo ; echo; \
	echo "########### LIBRARY ORDER #########"; \
	cat /tmp/order; \
	echo ; echo; \
	echo "########### DEPENDENCIES #########"; \
	cat /tmp/dep; ) >/tmp/Makefile.in
	mv ${SRC}/Makefile.in ${SRC}/Makefile.in.old
	cp /tmp/Makefile.in ${SRC}/Makefile.in
	( sed -n '1,/^##.*GENERATED/p' ${SRC}/Makefile; \
		echo ; echo; \
	echo "########### LIBRARY ORDER #########"; \
	cat /tmp/order; \
	echo ; echo; \
	echo "########### DEPENDENCIES #########"; \
	cat /tmp/dep; ) >/tmp/Makefile
	mv ${SRC}/Makefile ${SRC}/Makefile.old
	cp /tmp/Makefile ${SRC}/Makefile

/tmp/dep:: Makefile
/tmp/dep:: $(ALL_OBJS)
	CFLAGS="${CFLAGS}" ${SRC}/../UTILS/makeinc $(ALL_OBJS) >$@

# make the order in which library files should be loader
# this requires hand tuning the makefile, but it is easier than
# trying to fix up things with conditional tests
#
/tmp/order:: Makefile
/tmp/order:: $(LIBLPR_OBJS)
	lorder ` echo $(LIBLPR_OBJS) | sed -e 's/\.lo/.o/g'` |tsort >/tmp/raw
	awk '\
		BEGIN { print "LIBLPR_ORDER = \\"; } \
		{ line = line " " $$0;            \
			if( ++i >= 4 ){               \
				print "\t" line "\\";    \
				line = "";               \
				i = 0;                   \
			}                            \
		}                                \
		END {                            \
			if( line != "" ){            \
				print "\t" line;         \
			}                            \
		}' /tmp/raw >/tmp/order
	
	
############# GENERATED ###################################################


########### LIBRARY ORDER #########
LIBLPR_ORDER = \
	 krb5_auth.o initialize.o getopt.o copyright.o\
	 proctitle.o utilities.o user_auth.o md5.o\
	 ssl_auth.o stty.o sendreq.o sendmail.o\
	 sendjob.o sendauth.o printjob.o permission.o\
	 merge.o lpd_status.o lpd_secure.o lpd_remove.o\
	 lpd_rcvjob.o lpd_logger.o lpd_jobs.o lockfile.o\
	 accounting.o lpd_dispatch.o lpd_control.o debug.o\
	 controlword.o linksupport.o linelist.o gethostinfo.o\
	 globmatch.o getqueue.o getprinter.o fileopen.o\
	 errormsg.o child.o vars.o plp_snprintf.o\


########### DEPENDENCIES #########
accounting.o accounting.lo :	config.h accounting.h child.h debug.h errorcodes.h errormsg.h fileopen.h getqueue.h linelist.h linksupport.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
child.o child.lo :	config.h child.h debug.h errormsg.h gethostinfo.h getopt.h getqueue.h linelist.h linksupport.h lp.h plp_snprintf.h portable.h proctitle.h utilities.h utilities.h
controlword.o controlword.lo :	config.h control.h debug.h errormsg.h linelist.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
copyright.o copyright.lo :	config.h copyright.h debug.h errormsg.h license.h linelist.h lp.h patchlevel.h plp_snprintf.h portable.h utilities.h utilities.h
debug.o debug.lo :	config.h child.h debug.h errorcodes.h errormsg.h getopt.h linelist.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
errormsg.o errormsg.lo :	config.h child.h debug.h errorcodes.h errormsg.h getopt.h getqueue.h linelist.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
fileopen.o fileopen.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h linelist.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
gethostinfo.o gethostinfo.lo :	config.h debug.h errormsg.h gethostinfo.h getqueue.h globmatch.h linelist.h linksupport.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
getopt.o getopt.lo :	config.h debug.h errormsg.h linelist.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
getprinter.o getprinter.lo :	config.h child.h debug.h errormsg.h gethostinfo.h getprinter.h getqueue.h linelist.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
getqueue.o getqueue.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h gethostinfo.h getprinter.h getqueue.h globmatch.h linelist.h lockfile.h lp.h merge.h permission.h plp_snprintf.h portable.h utilities.h utilities.h
globmatch.o globmatch.lo :	config.h debug.h errormsg.h linelist.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
initialize.o initialize.lo :	config.h defs.h child.h debug.h errorcodes.h errormsg.h gethostinfo.h getopt.h getqueue.h initialize.h linelist.h lp.h plp_snprintf.h portable.h proctitle.h utilities.h utilities.h
krb5_auth.o krb5_auth.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h gethostinfo.h getqueue.h krb5_auth.h linelist.h linksupport.h lp.h lpd_dispatch.h lpd_secure.h permission.h plp_snprintf.h portable.h utilities.h utilities.h
linelist.o linelist.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h gethostinfo.h getprinter.h getqueue.h globmatch.h linelist.h lp.h lpd_dispatch.h lpd_jobs.h lpd_logger.h plp_snprintf.h portable.h utilities.h utilities.h
linksupport.o linksupport.lo :	config.h debug.h errorcodes.h errormsg.h gethostinfo.h linelist.h linksupport.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
lockfile.o lockfile.lo :	config.h debug.h errormsg.h fileopen.h linelist.h lockfile.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
lpd_control.o lpd_control.lo :	config.h child.h control.h debug.h errormsg.h fileopen.h gethostinfo.h getopt.h getprinter.h getqueue.h globmatch.h linelist.h lp.h lpd_control.h permission.h plp_snprintf.h portable.h proctitle.h utilities.h utilities.h
lpd_dispatch.o lpd_dispatch.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h gethostinfo.h getprinter.h getqueue.h krb5_auth.h linelist.h linksupport.h lp.h lpd_control.h lpd_dispatch.h lpd_rcvjob.h lpd_remove.h lpd_secure.h lpd_status.h permission.h plp_snprintf.h portable.h proctitle.h utilities.h utilities.h
lpd_jobs.o lpd_jobs.lo :	config.h accounting.h child.h debug.h errorcodes.h errormsg.h fileopen.h gethostinfo.h getopt.h getprinter.h getqueue.h linelist.h linksupport.h lockfile.h lp.h lpd_jobs.h lpd_rcvjob.h lpd_remove.h merge.h permission.h plp_snprintf.h portable.h printjob.h proctitle.h sendjob.h sendmail.h stty.h utilities.h utilities.h
lpd_logger.o lpd_logger.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h getopt.h getprinter.h getqueue.h linelist.h linksupport.h lp.h lpd_logger.h plp_snprintf.h portable.h proctitle.h utilities.h utilities.h
lpd_rcvjob.o lpd_rcvjob.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h gethostinfo.h getopt.h getqueue.h linelist.h linksupport.h lockfile.h lp.h lpd_jobs.h lpd_rcvjob.h lpd_remove.h permission.h plp_snprintf.h portable.h proctitle.h utilities.h utilities.h
lpd_remove.o lpd_remove.lo :	config.h child.h debug.h errormsg.h fileopen.h gethostinfo.h getopt.h getprinter.h getqueue.h linelist.h lp.h lpd_remove.h permission.h plp_snprintf.h portable.h proctitle.h sendreq.h utilities.h utilities.h
sendauth.o sendauth.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h gethostinfo.h getqueue.h globmatch.h krb5_auth.h linelist.h linksupport.h lp.h permission.h plp_snprintf.h portable.h sendauth.h sendjob.h user_auth.h utilities.h utilities.h
lpd_secure.o lpd_secure.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h getopt.h getqueue.h globmatch.h krb5_auth.h linelist.h linksupport.h lp.h lpd_dispatch.h lpd_jobs.h lpd_rcvjob.h lpd_secure.h permission.h plp_snprintf.h portable.h proctitle.h user_auth.h utilities.h utilities.h
lpd_status.o lpd_status.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h gethostinfo.h getopt.h getprinter.h getqueue.h globmatch.h linelist.h lockfile.h lp.h lpd_jobs.h lpd_status.h permission.h plp_snprintf.h portable.h proctitle.h sendreq.h utilities.h utilities.h
merge.o merge.lo :	config.h debug.h errormsg.h linelist.h lp.h merge.h plp_snprintf.h portable.h utilities.h utilities.h
permission.o permission.lo :	config.h debug.h errormsg.h fileopen.h gethostinfo.h getqueue.h globmatch.h linelist.h linksupport.h lp.h permission.h plp_snprintf.h portable.h utilities.h utilities.h
plp_snprintf.o plp_snprintf.lo :	config.h config.h
printjob.o printjob.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h getqueue.h linelist.h lp.h plp_snprintf.h portable.h printjob.h utilities.h utilities.h
proctitle.o proctitle.lo :	config.h debug.h errormsg.h linelist.h lp.h plp_snprintf.h portable.h proctitle.h utilities.h utilities.h
sendjob.o sendjob.lo :	config.h accounting.h debug.h errorcodes.h errormsg.h fileopen.h getqueue.h linelist.h linksupport.h lp.h plp_snprintf.h portable.h sendauth.h sendjob.h user_auth.h utilities.h utilities.h
sendmail.o sendmail.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h getqueue.h linelist.h lp.h plp_snprintf.h portable.h sendmail.h utilities.h utilities.h
sendreq.o sendreq.lo :	config.h child.h debug.h errormsg.h fileopen.h getqueue.h linelist.h linksupport.h lp.h plp_snprintf.h portable.h readstatus.h sendauth.h sendreq.h user_auth.h utilities.h utilities.h
ssl_auth.o ssl_auth.lo :	config.h debug.h errorcodes.h errormsg.h fileopen.h getqueue.h linelist.h lp.h lpd_secure.h plp_snprintf.h portable.h ssl_auth.h user_auth.h utilities.h utilities.h
stty.o stty.lo :	config.h debug.h errormsg.h linelist.h lp.h plp_snprintf.h portable.h stty.h utilities.h utilities.h
user_auth.o user_auth.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h getqueue.h krb5_auth.h linelist.h linksupport.h lp.h lpd_dispatch.h lpd_secure.h md5.h permission.h plp_snprintf.h portable.h ssl_auth.h user_auth.h utilities.h utilities.h
utilities.o utilities.lo :	config.h debug.h errorcodes.h errormsg.h getopt.h linelist.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
vars.o vars.lo :	config.h defs.h accounting.h child.h debug.h errormsg.h gethostinfo.h getqueue.h linelist.h lp.h permission.h plp_snprintf.h portable.h printjob.h utilities.h utilities.h
md5.o md5.lo :	md5.h md5.h
lpr.o lpr.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h gethostinfo.h getopt.h getprinter.h getqueue.h initialize.h linelist.h linksupport.h lp.h lpd_jobs.h lpr.h patchlevel.h plp_snprintf.h portable.h printjob.h sendjob.h utilities.h utilities.h
lpq.o lpq.lo :	config.h child.h debug.h errormsg.h getopt.h getprinter.h getqueue.h initialize.h linelist.h linksupport.h lp.h lpq.h patchlevel.h plp_snprintf.h portable.h sendreq.h utilities.h utilities.h
lpc.o lpc.lo :	config.h defs.h child.h control.h debug.h errorcodes.h errormsg.h getopt.h getprinter.h initialize.h linelist.h lp.h lpc.h patchlevel.h plp_snprintf.h portable.h sendreq.h utilities.h utilities.h
lprm.o lprm.lo :	config.h child.h debug.h errormsg.h getopt.h getprinter.h getqueue.h initialize.h linelist.h linksupport.h lp.h lprm.h patchlevel.h plp_snprintf.h portable.h sendreq.h utilities.h utilities.h
lpd.o lpd.lo :	config.h child.h debug.h errorcodes.h errormsg.h fileopen.h getopt.h getqueue.h initialize.h linelist.h linksupport.h lockfile.h lp.h lpd.h lpd_logger.h plp_snprintf.h portable.h proctitle.h utilities.h utilities.h
checkpc.o checkpc.lo :	config.h defs.h checkpc.h child.h debug.h errormsg.h fileopen.h gethostinfo.h getopt.h getprinter.h getqueue.h initialize.h linelist.h linksupport.h lockfile.h lp.h lpd_remove.h patchlevel.h plp_snprintf.h portable.h proctitle.h stty.h utilities.h utilities.h
lpf.o lpf.lo :	config.h portable.h portable.h
plp_snprintf.o plp_snprintf.lo :	config.h config.h
monitor.o monitor.lo :	config.h debug.h errormsg.h getopt.h getqueue.h linelist.h linksupport.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
getopt.o getopt.lo :	config.h debug.h errormsg.h linelist.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
lpbanner.o lpbanner.lo :	config.h debug.h errormsg.h linelist.h lp.h plp_snprintf.h portable.h utilities.h utilities.h
plp_snprintf.o plp_snprintf.lo :	config.h config.h
