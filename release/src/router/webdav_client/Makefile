#***********************************************************************
#
# Makefile
#
#***********************************************************************
include ../common.mak

#C = $(CC)
#TOP = $(TOP)
CFLAGS += -I$(TOP)/neon/src -I$(TOP)/libxml2/include -I$(TOP)/openssl/include
CFLAGS += -I$(SRCBASE) -I$(SRCBASE)/include -I. -I$(TOP)/shared -D_FILE_OFFSET_BITS=64 -DNVRAM_
LDFLAGS = -L$(TOP)/neon/src/.libs -L$(TOP)/libxml2/.libs -L$(TOP_PLATFORM)/nvram${BCMEX}${EX7} -L$(TOP)/shared -L$(TOP)/zlib -L$(TOP)/openssl
ifeq ($(RTCONFIG_BCM_502L07P2),y)
LDFLAGS += -L$(TOP_PLATFORM)/ethctl_lib -lethctl
endif

# TBD...theose conflict vars should be treated as extern or static
ifeq ($(HND_ROUTER),y)
CFLAGS := $(shell echo $(CFLAGS)| sed 's/-fno-common//g')
LDFLAGS += -L$(TOP_PLATFORM)/wlcsm$(EXHND) -lwlcsm
endif

ifeq ($(HND_ROUTER_BE_4916), y)
CFLAGS+= -fcommon
endif

ifeq ($(RTCONFIG_TRUSTZONE),y)
LDFLAGS += -L$(TOP)/ateed/ -latee
endif

LIBS = -lpthread -lnvram -lneon -lxml2 -lshared -lz -lcrypto -lssl

ifeq ($(RTCONFIG_QTN),y)
LDFLAGS += -L$(TOP)/libqcsapi_client
LIBS += -lqcsapi_client
endif

ifeq ($(BUILD_NAME),$(filter $(BUILD_NAME),RT-BE82M GS-BE18000 GS-BE12000 GS7_PRO GT7))
LDFLAGS += -L$(TOP)/libhostapi -lhostapi
endif

ifeq ($(wildcard $(TOP)/webdav_client/*.c),)
all:
	-cp -f $(TOP)/webdav_client/prebuild/$(BUILD_NAME)/webdav_client webdav_client
else
all: webdav_client
endif

webdav_client: webdav.o base.o list.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS)  $^ -o $@

.PHONY: clean
clean:
	-rm -rf *.o webdav_client

