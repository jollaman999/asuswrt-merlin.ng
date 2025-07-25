include $(TOP)/.config
OPENSSL_DIR	= $(TOP)/openssl
NVRAM_DIR	= $(TOP_PLATFORM)/nvram$(BCMEX)$(EX7)
JSON_C_DIR	= $(TOP)/json-c
SHARED_DIR	= $(TOP)/shared
ifeq ($(RTCONFIG_LLDPD_1_0_11),y)
LLDPD_DIR   = $(TOP)/lldpd-1.0.11
else
LLDPD_DIR   = $(TOP)/lldpd-0.9.8
endif
CFLAGS		= -I$(OPENSSL_DIR)/include -I$(SHARED_DIR) -I$(NVRAM_DIR) -I$(JSON_C_DIR) -I$(SRCBASE)/include -DUSE_GET_TLV_SUPPORT_MAC -I$(LLDPD_DIR)/src/lib -DUSE_LLDP_CTRL -std=gnu99
ifeq ($(RTCONFIG_BCMARM),y)
CFLAGS		+= -I$(SRCBASE)/shared/bcmwifi/include -DTYPEDEF_FLOAT_T
endif
LDFLAGS		= -L$(OPENSSL_DIR) -L$(NVRAM_DIR) -L$(SHARED_DIR) -L$(JSON_C_DIR)/.libs -L$(LLDPD_DIR)/src/lib/.libs -L$(STAGEDIR)/usr/lib/ -ljansson
LIBS		= -ljson-c -lcrypto -lnvram -lshared -llldpctl
ifeq ($(RTCONFIG_BCM_502L07P2),y)
LDFLAGS += -L$(TOP_PLATFORM)/ethctl_lib
LIBS += -lethctl
endif
LIBS += -ldl -lm -pthread

ifeq ($(HND_ROUTER),y)
CFLAGS += -DHND_ROUTER
LDFLAGS += -L$(TOP_PLATFORM)/wlcsm -lwlcsm
endif

ifeq ($(RPAC92),y)
CFLAGS += -DRPAC92
endif

ifeq ($(RTCONFIG_TRUSTZONE),y)
LDFLAGS += -L$(TOP)/ateed/ -latee
endif

ifeq ($(BUILD_NAME),$(filter $(BUILD_NAME),RT-BE82M GS-BE18000 GS-BE12000 GS7_PRO GT7))
LDFLAGS += -L$(TOP)/libhostapi -lhostapi
endif

OUTPUT_FILE	= libamas-utils
SRC1		= encrypt
SRC1_MAKE	= $(CC) -fPIC $(CFLAGS) -c $(SRC1).c -o $(SRC1).o

SRC2		= amas-utils
SRC2_MAKE	= $(CC) -fPIC $(CFLAGS) -c $(SRC2).c -o $(SRC2).o

LINK		= $(CC) -shared -o $(OUTPUT_FILE).so $(SRC2).o $(SRC1).o $(LDFLAGS) $(LIBS)

SRC3		= amas-utils-cli
SRC3_MAKE	= $(CC) $(CFLAGS) $(SRC3).c -o $(SRC3) $(LDFLAGS) -L. -lamas-utils $(LIBS)

ifeq ($(wildcard $(SRCBASE)/router/amas-utils/*.c),)
all:
	-cp -f prebuild/$(BUILD_NAME)/amas-utils.h ./
	-cp -f prebuild/$(BUILD_NAME)/libamas-utils.so ./
	-cp -f prebuild/$(BUILD_NAME)/amas-utils-cli ./
else
all:	lib cli
endif

lib:
	@echo make $(OUTPUT_FILE) ...
	$(SRC1_MAKE)
	$(SRC2_MAKE)
	$(LINK)
	@ls -l $(OUTPUT_FILE).so

cli:
	@echo make $(SRC3)
	$(SRC3_MAKE)
	@ls -l $(SRC3)

clean:
	@echo clean ...
	@rm -rf *.o
	@rm -rf $(OUTPUT_FILE).so
	@rm -rf $(SRC3)

