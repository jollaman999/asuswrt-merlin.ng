#***********************************************************************
#
# Makefile
#
#***********************************************************************
include ../common.mak


CFLAGS = -I$(TOP)/openssl/include -I$(TOP)/curl-7.21.7/include -I$(TOP)/smartsync_api
CFLAGS += -I$(SRCBASE) -I$(SRCBASE)/include -I. -I$(TOP)/shared -D_FILE_OFFSET_BITS=64 -DNVRAM_
CFLAGS += $(if $(QCA),$(EXTRACFLAGS))

LDFLAGS = -L$(TOP)/curl-7.21.7/lib/.libs -L$(TOP)/openssl -L$(TOP_PLATFORM)/nvram${BCMEX}${EX7} -L$(TOP)/shared -L$(TOP)/smartsync_api
LIBS = -lpthread -lnvram -lshared -lcrypto -lssl -lcurl -lm -lsmartsync_api
ifeq ($(RTCONFIG_BCM_502L07P2),y)
LDFLAGS += -L$(TOP_PLATFORM)/ethctl_lib
LIBS += -lethctl
endif

ifeq ($(RTCONFIG_TR069),y)
CFLAGS += -I$(TOP)/libssh2/include -I$(TOP)/zlib/include
LDFLAGS += -L$(TOP)/libssh2/src/.libs -lssh2 -L$(TOP)/zlib -lz
endif

ifeq ($(HND_ROUTER),y)
LDFLAGS += -L$(TOP_PLATFORM)/wlcsm$(EXHND) -lwlcsm -lgcc_s
endif

ifeq ($(RTCONFIG_QTN),y)
LDFLAGS += -L$(TOP)/libqcsapi_client
LIBS += -lqcsapi_client
endif

ifeq ($(RTCONFIG_BCMARM),y)
CFLAGS += -I$(SRCBASE)/common/include
endif

ifeq ($(HND_ROUTER_BE_4916), y)
CFLAGS+= -fcommon
endif

ifeq ($(RTCONFIG_TRUSTZONE),y)
LDFLAGS += -L$(TOP)/ateed/ -latee
endif

ifeq ($(BUILD_NAME),$(filter $(BUILD_NAME),RT-BE82M GS-BE18000 GS-BE12000 GS7_PRO GT7))
LDFLAGS += -L$(TOP)/libhostapi -lhostapi
endif

all: google_client 

google_client:google.o function.o list.o cJSON.o mem_pool.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $^ -o $@

.PHONY:clean
clean:
	rm -rf *.o google_client

