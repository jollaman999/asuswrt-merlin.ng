#This target is to ensure accidental execution of Makefile as a bash script will not execute commands like rm in unexpected directories and exit gracefully.
.prevent_execution:
	exit 0


include ../../common.mak
include $(TOP)/.config



NVRAM_DIR=$(TOP_PLATFORM)/nvram$(BCMEX)$(EX7)
SHARED_DIR=$(TOP)/shared
SQLITE_DIR=$(TOP)/sqlite
BWDPI_DIR=$(TOP)/bwdpi_source
LIBWEBAPI_DIR=$(TOP)/libwebapi
CFGMNT_DIR=$(TOP)/cfg_mnt

ROUTER_LIBS=-lnvram -lshared -lwebapi 
ROUTER_LDFLAGS=-L$(NVRAM_DIR) -L$(SHARED_DIR) -L$(LIBWEBAPI_DIR)

ifeq ($(RTCONFIG_BWDPI),y)
ROUTER_LIBS += -lsqlite3
ROUTER_LIBS += -lbwdpi
ROUTER_LIBS += -lbwdpi_sql

ROUTER_LDFLAGS += -L$(SQLITE_DIR)/.libs
ROUTER_LDFLAGS += -L$(BWDPI_DIR)/asus
ROUTER_LDFLAGS += -L$(BWDPI_DIR)/asus_sql
endif

ifeq ($(RTCONFIG_CFGSYNC),y)
ROUTER_LIBS += -lcfgmnt
ROUTER_LIBS += -lnmpapi

ROUTER_LDFLAGS += -L$(CFGMNT_DIR)
ROUTER_LDFLAGS += -L$(TOP)/networkmap/api/
endif

LDFLAGS = $(ROUTER_LDFLAGS) $(ROUTER_LIBS) 
ifeq ($(RTCONFIG_BCM_502L07P2),y)
LDFLAGS += -L$(TOP_PLATFORM)/ethctl_lib -lethctl
endif

ifeq ($(HND_ROUTER),y)
LDFLAGS += -L$(TOP_PLATFORM)/wlcsm$(EXHND) -lwlcsm
endif

ifeq ($(RTCONFIG_TRUSTZONE),y)
LDFLAGS += -L$(TOP)/ateed/ -latee
endif

ifeq ($(BUILD_NAME),$(filter $(BUILD_NAME),RT-BE82M))
LDFLAGS += -L$(TOP)/libhostapi -lhostapi
endif

ifeq ($(RTCONFIG_HTTPS),y)
CFLAGS += -I$(TOP)/mssl
CFLAGS += -I$(TOP)/openssl/include
LIBS += -L$(TOP)/mssl -lmssl
LIBS += -L$(TOP)/openssl -lssl -lcrypto -ldl
endif

ifeq ($(RTCONFIG_USB),y)
CFLAGS += -I$(TOP)/libdisk
LIBS += -L$(TOP)/libdisk -ldisk
ifeq ($(RTCONFIG_USB_SMS_MODEM),y)
CFLAGS += -I$(TOP)/smspdu
LIBS += -L$(TOP)/smspdu -lsmspdu
endif
endif

ifeq ($(BUILD_NAME),$(filter $(BUILD_NAME),RT-BE82M GS-BE18000 GS-BE12000 GS7_PRO GT7))
LDFLAGS += -L$(TOP)/libhostapi -lhostapi
endif

#remove @ for no make command prints
DEBUG = 

APP_DIR = .
APP_INCLUDE_DIRS += -I $(APP_DIR)

APP_NAME = awsiot
APP_SRC_FILES = time_util.c log.c api.c awsiot_ipc.c awsiot_mnt.c pwenc.c
APP_SRC_FILES += $(APP_NAME).c 

#CC = gcc

#AWSIOT client directory
AWSIOT_DIR = ..
OPENSSL_DIR = $(TOP)/openssl
JSON_C_DIR = $(TOP)/json-c
CURL_DIR = $(TOP)/curl-7.21.7


IOT_INCLUDE_DIRS += -I $(AWSIOT_DIR)/external_file/awsiot/libraries/standard/coreMQTT/source/include
IOT_INCLUDE_DIRS += -I $(AWSIOT_DIR)/external_file/awsiot/libraries/standard/coreMQTT/source/interface
# IOT_INCLUDE_DIRS += -I $(AWSIOT_DIR)/external_file/awsiot/libraries/standard/coreJSON/source/include
IOT_INCLUDE_DIRS += -I $(AWSIOT_DIR)/external_file/awsiot/libraries/standard/corePKCS11/test/shared-config
IOT_INCLUDE_DIRS += -I $(AWSIOT_DIR)/external_file/awsiot/libraries/standard/backoffAlgorithm/source/include
IOT_INCLUDE_DIRS += -I $(AWSIOT_DIR)/external_file/awsiot/platform/include
IOT_INCLUDE_DIRS += -I $(AWSIOT_DIR)/external_file/awsiot/platform/posix/transport/include


IOT_SRC_FILES += $(shell find $(AWSIOT_DIR)/external_file/awsiot/libraries/standard/coreMQTT/source/ -name 'core_mqtt.c')
IOT_SRC_FILES += $(shell find $(AWSIOT_DIR)/external_file/awsiot/libraries/standard/coreMQTT/source/ -name 'core_mqtt_serializer.c')
IOT_SRC_FILES += $(shell find $(AWSIOT_DIR)/external_file/awsiot/libraries/standard/coreMQTT/source/ -name 'core_mqtt_state.c')

# IOT_SRC_FILES += $(shell find $(AWSIOT_DIR)/external_file/awsiot/libraries/standard/coreJSON/source/ -name '*.c')

IOT_SRC_FILES += $(shell find $(AWSIOT_DIR)/external_file/awsiot/libraries/standard/backoffAlgorithm/source/ -name 'backoff_algorithm.c')
IOT_SRC_FILES += $(shell find $(AWSIOT_DIR)/external_file/awsiot/platform/posix/ -name 'clock_posix.c')
IOT_SRC_FILES += $(shell find $(AWSIOT_DIR)/external_file/awsiot/platform/posix/transport/src/ -name 'openssl_posix.c')
IOT_SRC_FILES += $(shell find $(AWSIOT_DIR)/external_file/awsiot/platform/posix/transport/src/ -name 'sockets_posix.c')


JSON_C_LDFLAGS += -L$(JSON_C_DIR)/.libs/ 
ifeq ($(RTCONFIG_QCA),y)
LIBS += -lm
endif


OPENSSL_LDFLAGS = -L$(OPENSSL_DIR)


#Aggregate LDFLAGS
LDFLAGS += $(OPENSSL_LDFLAGS) $(JSON_C_LDFLAGS) 
LDFLAGS += -L$(CURL_DIR)/lib/.libs 

OPENSSL_LIBS = -lssl -lcrypto -lpthread 
LIBS +=  $(OPENSSL_LIBS) -ljson-c 
# LIBS +=  -ldl -lm 
LIBS +=  -lcurl

ifeq ($(RTCONFIG_TR069),y)
LDFLAGS += -L$(TOP)/libssh2/src/.libs -lssh2
LDFLAGS += -L$(TOP)/zlib -lz
endif

#Aggregate all include and src directories
CFLAGS += -g
CFLAGS += $(APP_INCLUDE_DIRS)
CFLAGS += $(IOT_INCLUDE_DIRS)
CFLAGS += -I$(OPENSSL_DIR)/include
CFLAGS += -I$(NVRAM_DIR)
CFLAGS += -I$(SRCBASE)/include
CFLAGS += -I$(SHARED_DIR) 
CFLAGS += -I$(JSON_C_DIR) 
CFLAGS += -I$(CURL_DIR)/include/curl
CFLAGS += -I$(CURL_DIR)/include
CFLAGS += -I$(LIBWEBAPI_DIR)
CFLAGS += -DDISABLE_LOGGING

ifeq ($(RTCONFIG_BWDPI),y)
CFLAGS += -I$(SQLITE_DIR)	
CFLAGS += -I$(BWDPI_DIR)/include
CFLAGS += -I$(BWDPI_DIR)/asus_include
endif

ifeq ($(RTCONFIG_CFGSYNC),y)
CFLAGS += -I$(SHARED_DIR)/sysdeps/amas
CFLAGS += -I$(CFGMNT_DIR)
CFLAGS += -I$(TOP)/networkmap/api
endif

ifeq ($(RTCONFIG_BCMARM),y)
CFLAGS += -I$(SRCBASE)/shared/bcmwifi/include -DTYPEDEF_FLOAT_T
endif

ifeq ($(HND_ROUTER_BE_4916), y)
CFLAGS+= -fcommon
endif

SRC_FILES += $(APP_SRC_FILES)
SRC_FILES += $(IOT_SRC_FILES)


MAKE_CMD = $(CC) $(SRC_FILES) $(CFLAGS) -o $(APP_NAME) $(LDFLAGS)  $(LIBS)


all:
	$(DEBUG)$(MAKE_CMD)

clean:
	rm -f $(APP_DIR)/$(APP_NAME)
