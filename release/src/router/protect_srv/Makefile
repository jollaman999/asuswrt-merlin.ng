include ./Makefile.config

ifeq ($(ASUSWRT_SDK),y)
include $(SRCBASE)/router/.config
MAKE = make
CFLAGS = -Wall -O2 -g  $(EXTRACFLAGS)
CFLAGS += -I$(TOP)/shared -I$(SRCBASE)/include -I$(SRCBASE)/common/include
CFLAGS += -I$(SRCBASE) -I$(SRCBASE)/include -I. -I$(TOP)/protect_srv/include/
CFLAGS += -DASUSWRT_SDK

ifeq ($(RTCONFIG_NOTIFICATION_CENTER),y)
CFLAGS  += -I$(TOP)/sqlite
CFLAGS  += -I$(TOP)/nt_center/lib
LDFLAGS += -L$(TOP)/nt_center/lib/ -lnt
LDFLAGS += -L$(TOP)/sqlite/.libs -lsqlite3
LDFLAGS += -lpthread #Depend on sqlite
endif

ifeq ($(RTCONFIG_QTN),y)
CFLAGS += -I$(TOP)/libqcsapi_client -I$(TOP)/httpd -I./sysdeps/qtn
LDFLAGS += -L$(TOP)/libqcsapi_client -lqcsapi_client
endif

ifeq ($(CONFIG_KERNEL_PTHREAD_ONE),y)
CFLAGS += -DCONFIG_LINUX3X_OR_4X
endif
ifeq ($(HND_ROUTER),y)
CFLAGS += -DNON_P_THREAD
endif

ifeq ($(RPAC92),y)
CFLAGS += -DRPAC92
endif

ifeq ($(RTCONFIG_BCMARM),y)
CFLAGS += -I$(SRCBASE)/shared/bcmwifi/include -DTYPEDEF_FLOAT_T
endif

ifeq ($(RTCONFIG_TRUSTZONE),y)
LDFLAGS += -L$(TOP)/ateed/ -latee
endif

ifeq ($(BUILD_NAME),$(filter $(BUILD_NAME),RT-BE82M GS-BE18000 GS-BE12000 GS7_PRO GT7))
LDFLAGS += -L$(TOP)/libhostapi -lhostapi
endif

LDFLAGS += -L$(TOP_PLATFORM)/nvram$(BCMEX)${EX7} ${EXTRA_NV_LDFLAGS} -lnvram
LDFLAGS += -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib -lshared
ifeq ($(RTCONFIG_BCM_502L07P2),y)
LDFLAGS += -L$(TOP_PLATFORM)/ethctl_lib -lethctl
endif
LDFLAGS += -L$(TOP)/protect_srv/lib/ -lptcsrv

LIBS = -lpthread -lgcc_s

MDIRS = lib

ifeq ($(wildcard $(SRCBASE)/router/protect_srv/*.c),)
all: 
	-cp -f prebuild/$(BUILD_NAME)/protect_srv protect_srv
	-cp -f prebuild/$(BUILD_NAME)/Send_Event2ptcsrv Send_Event2ptcsrv
	-cp -f lib/prebuild/$(BUILD_NAME)/libptcsrv.so lib
	-cp -f prebuild/$(BUILD_NAME)/req_ptcsrv req_ptcsrv
else
all: ptcsrvdir Send_Event2ptcsrv protect_srv req_ptcsrv
endif

protect_srv: protect_srv.o
Send_Event2ptcsrv: Send_Event2ptcsrv.o
req_ptcsrv: req_ptcsrv.o

ptcsrvdir:
	set -e; for d in ${MDIRS}; do $(MAKE) -C $${d}; done
protect_srv:
	@$(CC) ${CFLAGS} $< -o $@ ${LDFLAGS} $(LIBS)
	@$(STRIP) $@

Send_Event2ptcsrv:
	@$(CC) ${CFLAGS} $< -o $@ ${LDFLAGS} $(LIBS)
	@$(STRIP) $@

req_ptcsrv:
	@$(CC) ${CFLAGS} $< -o $@ ${LDFLAGS} $(LIBS)
	@$(STRIP) $@

else # DSL_ASUSWRT_SDK 

CC = mips-linux-uclibc-gcc
MAKE = make 

CFLAGS = -Wall -O2 -g 
CFLAGS += $(TC_CFLAGS) -I.
CFLAGS += -I$(APP_PROTECTION_SERVER_DIR)/include/
CFLAGS += -I. -I$(APP_SHARED_DIR) -I$(APP_TCAPILIB_DIR) $(TC_CFLAGS)

LDFLAGS = -L$(APP_SHARED_DIR) -lshared -L$(APP_TCAPILIB_DIR) -ltcapi
LDFLAGS += -L$(APP_PROTECTION_SERVER_DIR)/lib/ -lptcsrv
LDFLAGS += -lpthread

ifneq ($(strip $(RTCONFIG_NOTIFICATION_CENTER)),)
CFLAGS += -I$(APP_NOTIFICATION_CENTER_DIR) -I$(APP_NOTIFICATION_CENTER_DIR)/lib/
CFLAGS += -I$(APP_SQLITE_DIR)/
LDFLAGS += -L$(APP_NOTIFICATION_CENTER_DIR)/lib -lnt -lm
LDFLAGS += -L$(APP_SQLITE_DIR)/.libs -lsqlite3
endif
LIBS =

MDIRS+= lib

all: ptcsrvdir Send_Event2ptcsrv protect_srv

protect_srv: protect_srv.o
Send_Event2ptcsrv: Send_Event2ptcsrv.o
req_ptcsrv: req_ptcsrv.o

ptcsrvdir:
	set -e; for d in ${MDIRS}; do $(MAKE) -C $${d}; done

protect_srv:
	@${CC} ${CFLAGS} $< -o $@ ${LDFLAGS}
Send_Event2ptcsrv:
	@${CC} ${CFLAGS} $< -o $@ ${LDFLAGS}
req_ptcsrv:
	@${CC} ${CFLAGS} $< -o $@ ${LDFLAGS}

endif

.PHONY : all clean
clean:
	set -e; for d in ${MDIRS}; do $(MAKE) -C $${d} clean; done
	rm -rf *.o protect_srv Send_Event2ptcsrv

