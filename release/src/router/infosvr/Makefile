#
# milli_httpd Makefile
#
# Copyright (C) 2001 Broadcom Corporation
#
# $Id: Makefile,v 1.1.1.1 2008/07/21 09:17:45 james26_jang Exp $
#
#CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include -Wall -I. -Wall -Dlinux $(GLOBAL_OPTIONS)
 
#CFLAGS	+= -g -DDEBUG
#CFLAGS	+= -s -O2 -DNO_PARALLEL
#LDFLAGS	+= -L$(TOP)/nvram -L$(TOP)/shared -L$(TOP)/netconf -lnvram -lshared -lnetconf

#vpath %.c $(TOP)/shared

include ../common.mak

CFLAGS  += -I. -I$(TOP)/shared  -I$(SRCBASE)/include -Wall
ifeq ($(RTCONFIG_AMAS),y)
CFLAGS += -I$(TOP)/openssl/include
CFLAGS += -I$(TOP)/json-c
endif
CFLAGS += $(PRIVATE_EXTRACFLAGS)
ifeq ($(RTCONFIG_BCMWL6),y)
CFLAGS  += -I$(SRCBASE)/shared/bcmwifi/include
endif
CFLAGS  += -Dlinux -DNO_PARALLEL -DPRNINFO
LDFLAGS = -L$(TOP)/shared -lshared -L$(TOP_PLATFORM)/nvram${BCMEX}$(EX7) -lnvram 
ifeq ($(RTCONFIG_BCM_502L07P2),y)
LDFLAGS += -L$(TOP_PLATFORM)/ethctl_lib -lethctl
endif
ifeq ($(HND_ROUTER),y)
LDFLAGS += -L$(TOP_PLATFORM)/wlcsm$(EXHND) -lwlcsm
endif

ifeq ($(RTCONFIG_BCMARM),y)
CFLAGS += -I$(SRCBASE)/common/include
LDFLAGS += -lgcc_s
endif

ifeq ($(RTCONFIG_AMAS),y)
LDFLAGS += -L$(TOP)/openssl -lcrypto -lssl
LDFLAGS += -L$(TOP)/json-c/.libs -ljson-c
ifeq ($(RTCONFIG_LLDPD_1_0_11),y)
LLDPD_DIR = $(TOP)/lldpd-1.0.11
else
LLDPD_DIR = $(TOP)/lldpd-0.9.8
endif
CFLAGS += -I$(TOP)/amas-utils -I$(TOP)/shared/sysdeps/amas -pthread
LDFLAGS += -L$(TOP)/amas-utils -lamas-utils -lpthread
LDFLAGS += -L$(LLDPD_DIR)/src/lib/.libs -llldpctl -L$(STAGEDIR)/usr/lib/ -ljansson
endif

ifeq ($(RTCONFIG_QTN),y)
LDFLAGS += -L$(TOP)/libqcsapi_client -lqcsapi_client
endif

ifeq ($(HND_ROUTER_AX), y)
CFLAGS += -I$(SRCBASE)/../components/proto/include -I$(SRCBASE)/../components/wlioctl/include
endif

ifeq ($(BUILD_NAME),$(filter $(BUILD_NAME),RT-BE82M GS-BE18000 GS-BE12000 GS7_PRO GT7))
LDFLAGS += -L$(TOP)/libhostapi -lhostapi
endif

all: infosvr

clean:
	rm -f *.o *~ infosvr

install: all
# Neo
#	install -d $(INSTALLDIR)/usr/sbin
#	install httpd $(INSTALLDIR)/usr/sbin
#	$(STRIP) $(INSTALLDIR)/usr/sbin/httpd

	install -d $(INSTALLDIR)/usr/sbin
	$(STRIP) infosvr
	install infosvr $(INSTALLDIR)/usr/sbin


infosvr: ./infosvr.o ./common.o ./packet.o ./storage.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

ifneq ($(wildcard ./prebuild/interface.o),)
interface.o:
	@-cp -f ./prebuild/interface.o .
endif
