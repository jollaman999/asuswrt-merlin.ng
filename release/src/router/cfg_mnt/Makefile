include ../common.mak

ifeq ($(KPATH_37_OR_ABOVE),y)
CFLAGS += -I$(KDIR)/include
else
CFLAGS += -I$(LINUX_INC_DIR)/include
endif

CFLAGS += $(if $(KPATH_37_OR_ABOVE),$(if $(and $(RTCONFIG_QCA),$(RTCONFIG_MUSL_LIBC)),,-I$(KDIR)/include),-I$(LINUX_INC_DIR)/include)
CFLAGS += -Wall $(if $(RTCONFIG_DEBUG),-g,-Os) $(EXTRACFLAGS) -ffunction-sections -fdata-sections
CFLAGS += -I. -I$(TOP)/shared -I$(SRCBASE) -I$(SRCBASE)/include -I./include -I$(TOP)/networkmap -I$(TOP)/networkmap/api
CFLAGS += -I./sysdeps
CFLAGS += -I$(TOP)/openssl/include
CFLAGS += -I$(TOP)/json-c

LDFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections
LDFLAGS += -L$(TOP_PLATFORM)/nvram${BCMEX}${EX7} -lnvram -L$(TOP)/shared -lshared
ifeq ($(RTCONFIG_BCM_502L07P2),y)
LDFLAGS += -L$(TOP_PLATFORM)/ethctl_lib -lethctl
endif
LDFLAGS += -L$(TOP)/openssl -lcrypto -lssl
LDFLAGS += -lpthread
LDFLAGS += -L$(TOP)/json-c/.libs -ljson-c
LDFLAGS += -lm
LDFLAGS += -L./ -lcfgmnt
LDFLAGS += -L$(TOP)/networkmap/api/ -lnmpapi

LDFLAGS_LIB += -L$(TOP_PLATFORM)/nvram${BCMEX}${EX7} -lnvram -L$(TOP)/shared -lshared -L$(TOP)/json-c/.libs -ljson-c
LDFLAGS_LIB += -L$(TOP)/networkmap/api/ -lnmpapi

ifeq ($(RTCONFIG_DMALLOC),y)
CFLAGS += -DDMALLOC -DDMALLOC_FUNC_CHECK -I$(STAGEDIR)/usr/include
LDFLAGS += -L$(STAGEDIR)/usr/lib -ldmalloc
endif

ifeq ($(RTCONFIG_AMAS),y)
ifeq ($(RTCONFIG_LLDPD_1_0_11),y)
LLDPD_DIR = $(TOP)/lldpd-1.0.11
else
LLDPD_DIR = $(TOP)/lldpd-0.9.8
endif
CFLAGS += -I$(TOP)/amas-utils -I$(TOP)/shared/sysdeps/amas
LDFLAGS += -L$(TOP)/amas-utils -lamas-utils
LDFLAGS += -L$(LLDPD_DIR)/src/lib/.libs -llldpctl -L$(STAGEDIR)/usr/lib/ -ljansson
ifeq ($(RTCONFIG_AVBLCHAN),y)
CFLAGS += -DAVBLCHAN
endif
CFLAGS += -DFREE_MEMORY_DOWNLOAD
endif

ifeq ($(HND_ROUTER),y)
CFLAGS += -DPTHREAD_STACK_SIZE_256K -DPTHREAD_EXIT
LDFLAGS += -L$(TOP_PLATFORM)/wlcsm -lwlcsm
else ifeq ($(RTCONFIG_QCA),y)
CFLAGS += -DPTHREAD_STACK_SIZE_256K -DPTHREAD_EXIT
endif

ifeq ($(HND_ROUTER_BE_4916),y)
CFLAGS += -fcommon
endif

ifeq ($(RTCONFIG_HND_ROUTER_AX),y)
CFLAGS +=-I$(SRCBASE)/../components/wlioctl/include -I$(SRCBASE)/../components/proto/include -I$(SRCBASE)/wl/sys
endif

ifeq ($(RTCONFIG_NOTIFICATION_CENTER),y)
CFLAGS  += -I$(TOP)/sqlite
CFLAGS  += -I$(TOP)/nt_center/lib
CFLAGS  += -I$(TOP)/nt_center/actMail
CFLAGS  += -I$(TOP)/wlc_nt
LDFLAGS += -L$(TOP)/sqlite/.libs -lsqlite3 -lpthread -L$(TOP)/nt_center/lib -lnt
LDFLAGS += -L$(TOP)/wlc_nt -lwlc_nt_client
endif

ifeq ($(RTCONFIG_DHDAP), y)
CFLAGS += -DDEBUG_WL
endif

ifeq ($(RTCONFIG_INTEL),y)
CFLAGS += -fPIC
endif
ifeq ($(RTCONFIG_BCMARM),y)
CFLAGS += -I$(SRCBASE)/shared/bcmwifi/include -DTYPEDEF_FLOAT_T
CFLAGS += -I$(SRCBASE)/common/include
else ifeq ($(RTCONFIG_RALINK),y)
CFLAGS += -I$(TOP)/shared/sysdeps/ralink -I$(TOP)/wireless_tools$(WTOOL_SUFFIX)
CFLAGS += -fPIC
else ifeq ($(RTCONFIG_QCA),y)
CFLAGS += -fPIC
else ifeq ($(RTCONFIG_REALTEK),y)
CFLAGS += -fPIC
endif

ifeq ($(RTCONFIG_SW_HW_AUTH),y)
CFLAGS  += -I$(TOP)/sw-hw-auth
endif

ifeq ($(RTCONFIG_LIBASUSLOG),y)
CFLAGS += -I$(TOP)/libasuslog
LDFLAGS += -L$(TOP)/libasuslog -lasuslog
endif

ifeq ($(RTCONFIG_RALINK),y)
COMMON_OBJS += ralink.o
vpath %.c sysdeps/ralink
else ifeq ($(RTCONFIG_QCA),y)
CFLAGS += -I$(TOP)/shared/sysdeps/qca -I$(TOP)/wireless_tools
COMMON_OBJS += qca.o
vpath %.c sysdeps/qca
else ifeq ($(RTCONFIG_REALTEK),y)
COMMON_OBJS += realtek.o
vpath %.c sysdeps/realtek
else ifeq ($(RTCONFIG_INTEL),y)
COMMON_OBJS += intel.o
vpath %.c sysdeps/intel
else ifeq ($(RTCONFIG_LANTIQ),y)
COMMON_OBJS += lantiq.o
CFLAGS += -fPIC
vpath %.c sysdeps/lantiq
else
COMMON_OBJS += broadcom.o
vpath %.c sysdeps/broadcom
endif

ifeq ($(RTCONFIG_AMAS),y)
COMMON_OBJS += chmgmt.o
endif
COMMON_OBJS += encrypt.o linklist.o
COMMON_OBJS += cfg_crc.o cfg_udp.o cfg_dencrypt.o cfg_common.o cfg_ipc.o cfg_wevent.o cfg_upgrade.o cfg_clientlist.o cfg_sched.o cfg_slavelist.o
COMMON_OBJS += cfg_capability.o cfg_action.o cfg_eventnotify.o cfg_bandindex.o
COMMON_OBJS += cfg_ctrllist.o cfg_param.o
ifeq ($(RTCONFIG_DWB),y)
COMMON_OBJS += cfg_dwb.o
endif
COMMON_OBJS += cfg_chanspec.o
COMMON_OBJS += cfg_parammgnt.o
ifeq ($(RTCONFIG_MULTILAN_CFG), y)
COMMON_OBJS += cfg_mtlan.o
COMMON_OBJS += cfg_mlo.o
endif
ifeq ($(RTCONFIG_AMAS_CENTRAL_CONTROL),y)
COMMON_OBJS += cfg_centralcontrol.o
endif
SERVER_OBJS += $(COMMON_OBJS) cfg_mnt.o
ifeq ($(RTCONFIG_AMAS_CENTRAL_OPTMZ),y)
SERVER_OBJS += cfg_optimization.o
endif
ifeq ($(RTCONFIG_AMAS_CENTRAL_ADS),y)
SERVER_OBJS += cfg_ads.o
endif
CLIENT_OBJS += $(COMMON_OBJS) cfg_client.o
REPORTSTATUS_OBJS += cfg_common.o cfg_reportstatus.o

LIBSHARED_OBJS += cfg_lib.o
ifeq ($(RTCONFIG_MULTILAN_CFG),y)
LIBSHARED_OBJS += cfg_mtlan.o
endif
ifeq ($(RTCONFIG_AMAS_UPLOAD_FILE),y)
CLIENT_OBJS += cfg_uploadfile.o
endif

ifeq ($(RTCONFIG_WIFI_SON),y)
CFLAGS += -DREPORT_PAP_INFO
endif
ifeq ($(RTCONFIG_AMAS),y)
COMMON_OBJS += cfg_roaming.o
CFLAGS += -DLEGACY_ROAMING
COMMON_OBJS += cfg_onboarding.o cfg_ethevent.o
CFLAGS += -DONBOARDING -DRSSI_LIST
ifeq ($(RTCONFIG_VIF_ONBOARDING),y)
CFLAGS += -DONBOARDING_VIA_VIF
endif
# for wireless channel sync
CFLAGS += -DSYNC_WCHANNEL
# for roaming info for sta
#COMMON_OBJS += cfg_roaminginfo.o
#CFLAGS += -DROAMING_INFO
# for master detection
#CFLAGS += -DMASTER_DET
# for radar detected
COMMON_OBJS += cfg_radardet.o
CFLAGS += -DRADAR_DET
COMMON_OBJS += cfg_conndiag.o
CFLAGS += -DCONN_DIAG
ifeq ($(RTCONFIG_STA_AP_BAND_BIND),y)
# for sta bind on AP
CFLAGS += -DSTA_BIND_AP
endif
ifeq ($(RTCONFIG_AMAS_CENTRAL_CONTROL),y)
CFLAGS += -DUPDATE_COMMON_CONFIG
endif
endif

ifeq ($(RTCONFIG_ROUTERBOOST),y)
CFLAGS += -DROUTERBOOST
COMMON_OBJS += cfg_routerboost.o
endif

ifeq ($(RTCONFIG_AMASDB),y)
CFLAGS += -DDUAL_BAND_DETECTION
COMMON_OBJS += cfg_dualbandlist.o
endif
# for sending re join event
#ifeq ($(HND_ROUTER_AX),y)
#CFLAGS += -DNOTIFY_RE_JOIN_EVENT
#endif

ifeq ($(RTCONFIG_TCODE),y)
ifeq ($(RTCONFIG_CFGSYNC_LOCSYNC),y)
COMMON_OBJS += cfg_loclist.o
endif
endif

ifeq ($(or $(RTCONFIG_HAS_5G_2),$(RTCONFIG_HAS_6G_2)), y)
CFLAGS += -DSUPPORT_TRI_BAND
else ifdef $(filter $(BUILD_NAME), RT-AC5300 GT-AC5300 RT-AC92U RT-AX92U GT-AX11000 RT-AX95Q XT8PRO BT12 BT10 BQ16 BQ16_PRO BM68 XT8_V2 RT-AXE95Q ET8PRO ET8_V2 RP-AC92 GT-AXE11000 GT-AX11000PRO RT-BE92U GS-BE18000 GS-BE12000 GS7_PRO GT7)
CFLAGS += -DSUPPORT_TRI_BAND
endif

ifeq ($(RTCONFIG_MASTER_DET),y)
CFLAGS += -DMASTER_DET
endif

ifeq ($(RTCONFIG_PRELINK),y)
CFLAGS += -DPRELINK
COMMON_OBJS += cfg_prelink.o
endif

ifeq ($(RTCONFIG_MSSID_PRELINK),y)
CFLAGS += -DMSSID_PRELINK
COMMON_OBJS += cfg_prelink.o
endif

ifeq ($(RTCONFIG_NBR_RPT),y)
COMMON_OBJS += cfg_nbr_rpt.o
endif

ifeq ($(RTCONFIG_LANTIQ),y)
# LDFLAGS += -L$(TOP)/fapi_wlan_common-1.0.0.1 -lfapiwlancommon -ldl -L$(TOP)/libhelper-1.4.0.2 -lhelper -lrt
LDFLAGS += -L$(TOP)/fapi_wlan_common-05.04.00.131 -lfapiwlancommon -ldl -L$(TOP)/libhelper-1.4.0.2 -lhelper -L$(TOP)/libfapi-0.1 -lfapi
LDFLAGS += -L$(TOP)/wlc_nt -lwlc_nt_client
endif

ifeq ($(RTCONFIG_LANTIQ),y)
CFLAGS += -I$(TOP)/shared/sysdeps/lantiq -I$(TOP)/wireless_tools -I$(TOP)/fapi_wlan_common-05.04.00.131/include -I$(TOP)/libhelper-1.4.0.2/include -DPACKAGE_ID=\"fapi_wlan_common\" -DLOGGING_ID="fapi_wlan_common"
CFLAGS  += -I$(TOP)/wlc_nt
endif

ifeq ($(RTCONFIG_BCMBSD),y)
CFLAGS += -DSMART_CONNECT
endif

ifeq ($(or $(HND_ROUTER_AX),$(RTCONFIG_WIFI_QCN5024_QCN5054)),y)
CFLAGS += -DSUPPORT_11AX
endif

ifeq ($(RTCONFIG_QCA_PLC2),y)
CFLAGS += -DPLC_STATUS
endif

ifeq ($(RTCONFIG_TRUSTZONE),y)
CFLAGS += -I$(TOP)/ateed/
LDFLAGS += -L$(TOP)/ateed/ -latee
endif

ifeq ($(or $(RTCONFIG_SMARTHAUL),$(RTCONFIG_BRCM_SMARTHAUL)),y)
CFLAGS += -DSMARTHAUL
COMMON_OBJS += cfg_smarthaul.o
endif

ifeq ($(RTCONFIG_BCM_AFC),y)
CFLAGS += -DAFC_ENABLED
SERVER_OBJS += cfg_afc.o
endif

ifeq ($(BUILD_NAME),$(filter $(BUILD_NAME),RT-BE82M GS-BE18000 GS-BE12000 GS7_PRO GT7))
LDFLAGS += -L$(TOP)/libhostapi -lhostapi
endif

ifeq ($(wildcard $(SRCBASE)/router/cfg_mnt/*.c),)
all:
	-cp -f prebuild/$(BUILD_NAME)/cfg_server cfg_server
	-cp -f prebuild/$(BUILD_NAME)/cfg_client cfg_client
	-cp -f prebuild/$(BUILD_NAME)/cfg_reportstatus cfg_reportstatus
	-cp -f prebuild/$(BUILD_NAME)/libcfgmnt.so libcfgmnt.so
else
all: libcfgmnt.so cfg_server cfg_client cfg_reportstatus
endif

cfg_server: $(SERVER_OBJS)
	echo " [cfg_mnt] CC $@"
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

cfg_client: $(CLIENT_OBJS)
	echo " [cfg_mnt] CC $@"
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

cfg_reportstatus: $(REPORTSTATUS_OBJS)
	echo " [cfg_mnt] CC $@"
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

libcfgmnt.so: $(LIBSHARED_OBJS)
	echo " [libcfgmnt] CC $@"
	$(CC) -fPIC -shared $(CFLAGS) -o $@ $^ $(LDFLAGS_LIB)

install: all
	@echo "[cfg_mnt] Installing..."
	@install -D cfg_server $(INSTALLDIR)/usr/sbin/cfg_server
	@install -D cfg_client $(INSTALLDIR)/usr/sbin/cfg_client
	@install -D cfg_reportstatus $(INSTALLDIR)/usr/sbin/cfg_reportstatus
	@install -D libcfgmnt.so $(INSTALLDIR)/usr/lib/libcfgmnt.so
	@install -D gencfgcert.sh $(INSTALLDIR)/usr/sbin/gencfgcert.sh
	$(if $(RTCONFIG_DEBUG),,@$(STRIP) $(INSTALLDIR)/usr/sbin/cfg_server)
	$(if $(RTCONFIG_DEBUG),,@$(STRIP) $(INSTALLDIR)/usr/sbin/cfg_client)
	$(if $(RTCONFIG_DEBUG),,@$(STRIP) $(INSTALLDIR)/usr/sbin/cfg_reportstatus)
	$(if $(RTCONFIG_DEBUG),,@$(STRIP) $(INSTALLDIR)/usr/lib/libcfgmnt.so)
	@chmod 0755 $(INSTALLDIR)/usr/sbin/cfg_server
	@chmod 0755 $(INSTALLDIR)/usr/sbin/cfg_client
	@chmod 0755 $(INSTALLDIR)/usr/sbin/cfg_reportstatus
	@chmod 0500 $(INSTALLDIR)/usr/sbin/gencfgcert.sh

clean:
	rm -f cfg_server cfg_client cfg_reportstatus *.so *.o .*.depend
