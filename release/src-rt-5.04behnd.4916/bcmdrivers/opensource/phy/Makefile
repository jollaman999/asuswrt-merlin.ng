CUR_DIR := $(BRCMDRIVERS_DIR)/opensource/phy
DRV_DIR := ../../../shared/opensource/drv

# XFLOW
ifeq ("$(CONFIG_BCM_MACSEC_FIRELIGHT)", "y")
    EXTRA_CFLAGS += -include linux/kernel.h -include linux/types.h -include linux/string.h
    EXTRA_CFLAGS += -I$(CUR_DIR) \
                    -I$(CUR_DIR)/xflow \
                    -I$(CUR_DIR)/xflow/include \
                    -I$(CUR_DIR)/xflow/include/xflow_include
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_68880_),)
    EXTRA_CFLAGS += -I$(CUR_DIR)/xflow/include/soc/68880
    DRV_OBJS += xflow/bcm68880.o
    DRV_OBJS += xflow/bchp_regs_int_68880.o
endif
    DRV_OBJS += xflow/xflow_macsec.o
    DRV_OBJS += xflow/xflow_macsec_common.o
    DRV_OBJS += xflow/xflow_macsec_debug.o
    DRV_OBJS += xflow/xflow_macsec_drv.o
    DRV_OBJS += xflow/xflow_macsec_firelight.o
    DRV_OBJS += xflow/xflow_macsec_int.o
    DRV_OBJS += xflow/xflow_macsec_proc.o
    DRV_OBJS += xflow/xflow_macsec_resource.o
    DRV_OBJS += xflow/xflow_macsec_soc_drv.o
    DRV_OBJS += xflow/xflow_macsec_soc_mem.o
    DRV_OBJS += xflow/xflow_macsec_soc_reg.o
    DRV_OBJS += xflow/xflow_macsec_stats.o
    DRV_OBJS += xflow/xflow_macsec_util.o
endif # XFLOW

# LPORT
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6858_),)
    EXTRA_CFLAGS += -I$(CUR_DIR)/lport -I$(CUR_DIR)/lport/ag
    DRV_OBJS += lport/lport_drv.o
    DRV_OBJS += lport/lport_stats.o
    DRV_OBJS += lport/lport_intr.o
    DRV_OBJS += lport/lport_proc.o
    DRV_OBJS += lport/ag/BCM6858_A0_LPORT_AG.o
    DRV_OBJS += lport/ag/BCM6858_A0LPORT_CTRL_AG.o
    DRV_OBJS += lport/ag/BCM6858_A0LPORT_INTR_AG.o
    DRV_OBJS += lport/ag/BCM6858_A0LPORT_LED_AG.o
    DRV_OBJS += lport/ag/BCM6858_A0LPORT_MAB_AG.o
    DRV_OBJS += lport/ag/BCM6858_A0LPORT_MDIO_AG.o
    DRV_OBJS += lport/ag/BCM6858_A0LPORT_MIB_AG.o
    DRV_OBJS += lport/ag/BCM6858_A0LPORT_RGMII_AG.o
    DRV_OBJS += lport/ag/BCM6858_A0LPORT_SRDS_AG.o
    DRV_OBJS += lport/ag/BCM6858_A0LPORT_XLMAC_AG.o
    DRV_OBJS += lport/ag/BCM6858_A0MIB_CONF_AG.o
    DRV_OBJS += lport/ag/BCM6858_A0XLMAC_CONF_AG.o
    DRV_OBJS += lport/ag/bcm6858_drivers_lport_ag.o
    DRV_OBJS += lport/ag/bcm6858_lport_ctrl_ag.o
    DRV_OBJS += lport/ag/bcm6858_lport_intr_ag.o
    DRV_OBJS += lport/ag/bcm6858_lport_led_ag.o
    DRV_OBJS += lport/ag/bcm6858_lport_mab_ag.o
    DRV_OBJS += lport/ag/bcm6858_lport_mdio_ag.o
    DRV_OBJS += lport/ag/bcm6858_lport_mib_ag.o
    DRV_OBJS += lport/ag/bcm6858_lport_rgmii_ag.o
    DRV_OBJS += lport/ag/bcm6858_lport_srds_ag.o
    DRV_OBJS += lport/ag/bcm6858_lport_xlmac_ag.o
    DRV_OBJS += lport/ag/bcm6858_mib_conf_ag.o
    DRV_OBJS += lport/ag/bcm6858_xlmac_conf_ag.o
endif

# XPORT
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_63158_4912_6813_6888_68880_6837_),)
ifeq ($(strip $(BRCM_CHIP)),63158) 
    XPORT_CHIP=63158B0
endif
ifeq ($(strip $(BRCM_CHIP)),4912) 
    XPORT_CHIP=4912A0
endif
ifeq ($(strip $(BRCM_CHIP)),6813) 
    XPORT_CHIP=6813A0
endif
ifeq ($(strip $(BRCM_CHIP)),6888) 
    XPORT_CHIP=6888A0
endif
ifeq ($(strip $(BRCM_CHIP)),68880) 
    XPORT_CHIP=6888A0
    DRV_OBJS += xport/xport_intr.o
endif
ifeq ($(strip $(BRCM_CHIP)),6837) 
    XPORT_CHIP=6837A0
    DRV_OBJS += xport/xport_intr.o
endif
    DRV_OBJS += xport/xport_drv.o
    DRV_OBJS += xport/xport_stats.o
    DRV_OBJS += xport/xport_proc.o
    DRV_OBJS += $(patsubst %.c, %.o, $(shell cd $(src) && find xport/ag/$(XPORT_CHIP) -type f -name '*.c')) 
    EXTRA_CFLAGS += -I$(CUR_DIR)/xport -I$(CUR_DIR)/xport/ag/$(XPORT_CHIP)
endif

# Merlin mptwo
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6858_6756_),)
    EXTRA_CFLAGS += -DEXCLUDE_STD_HEADERS -DUINT16_MAX=0xFFFF -DUINT32_MAX=0xFFFFFFFF
    EXTRA_CFLAGS += -include linux/kernel.h -include linux/types.h -include linux/string.h
    EXTRA_CFLAGS += -I$(CUR_DIR)/merlin_mptwo/include
    EXTRA_CFLAGS += -I$(CUR_DIR)/merlin_mptwo/src
    DRV_OBJS += merlin_mptwo/serdes_wrapper.o
endif

# Merlin shortfin
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6888_68880_6837_63146_4912_6813_6765_6766_6764_),)
    EXTRA_CFLAGS += -DEXCLUDE_STD_HEADERS -DUINT16_MAX=0xFFFF -DUINT32_MAX=0xFFFFFFFF
    EXTRA_CFLAGS += -include linux/kernel.h -include linux/types.h -include linux/string.h
    EXTRA_CFLAGS += -I$(CUR_DIR)/merlin_shortfin/include
    DRV_OBJS += $(patsubst %.c, %.o, $(shell cd $(src) && find merlin_shortfin/src -type f -name '*.c')) 
    DRV_OBJS += merlin_shortfin/serdes_wrapper.o
endif

# Merlin shasta
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6888_68880_6837_),)
ifeq ($(strip $(CONFIG_BRCM_QEMU)),) 
    EXTRA_CFLAGS += -DEXCLUDE_STD_HEADERS -DUINT16_MAX=0xFFFF -DUINT32_MAX=0xFFFFFFFF
    EXTRA_CFLAGS += -include linux/kernel.h -include linux/types.h -include linux/string.h
    EXTRA_CFLAGS += -I$(CUR_DIR)/merlin_shasta/include
    DRV_OBJS += $(patsubst %.c, %.o, $(shell cd $(src) && find merlin_shasta/src -type f -name '*.c')) 
    DRV_OBJS += merlin_shasta/serdes_wrapper.o
endif	
endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6858_6846_6856_63158_6878_6855_6888_68880_6837_6813_),)
DRV_OBJS += phy_drv_pon.o
EXTRA_CFLAGS += -DPHY_PON
endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6858_6846_6856_6878_6855_6888_68880_6837_),)
    DRV_OBJS += mac_drv.o
    DRV_OBJS += bus_drv.o
    DRV_OBJS += phy_drv.o
    DRV_OBJS += phy_drv_mii.o
    DRV_OBJS += phy_drv_brcm.o
    DRV_OBJS += dt_access.o
    DRV_OBJS += dt_parsing.o
    DRV_OBJS += phy_drv_crossbar.o
    EXTRA_CFLAGS += -DPHY_CROSSBAR

ifeq ($(strip ${BRCM_CHIP}),6858)
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_v1.o
    DRV_OBJS += bus_drv_mdio_v1.o
    DRV_OBJS += mac_drv_lport.o
    DRV_OBJS += phy_drv_egphy.o
    DRV_OBJS += phy_drv_ext1.o
    DRV_OBJS += phy_drv_ext2.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += serdes_access.o
    DRV_OBJS += phy_drv_mptwo.o
    DRV_OBJS += phy_mac_sec.o
    DRV_OBJS += phy_mac_sec_list.o
    DRV_OBJS += sha256.o
    EXTRA_CFLAGS += -DBUS_MDIO_V1
    EXTRA_CFLAGS += -DMAC_LPORT
    EXTRA_CFLAGS += -DPHY_EGPHY -DPHY_EXT1 -DPHY_EXT2 -DPHY_EXT3 -DPHY_MPTWO
    EXTRA_CFLAGS += -DLPORT_INTERRUPTS -DMACSEC_SUPPORT
ifeq ($(strip $(CONFIG_BCM_FTTDP_G9991)),)
    EXTRA_CFLAGS += -DSERDES_LINK_POWER_DOWN -DSERDES_SPEED_DETECT -DSERDES_MODULE_DETECT
endif
endif

ifeq ($(strip ${BRCM_CHIP}),6856)
    DRV_OBJS += unimac_drv_impl1.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_v1.o
    DRV_OBJS += bus_drv_mdio_v1.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += phy_drv_egphy.o
    DRV_OBJS += phy_drv_sgmii_plus2.o
    DRV_OBJS += phy_drv_ext1.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += phy_mac_sec.o
    DRV_OBJS += phy_mac_sec_list.o
    DRV_OBJS += sha256.o
    DRV_OBJS += brcm_rgmii.o
    EXTRA_CFLAGS += -DBUS_MDIO_V1
    EXTRA_CFLAGS += -DBRCM_RGMII
    EXTRA_CFLAGS += -DMAC_UNIMAC
    EXTRA_CFLAGS += -DPHY_EGPHY -DPHY_SGMII -DPHY_EXT1 -DPHY_EXT3 
endif

ifeq ($(strip ${BRCM_CHIP}),6855)
    DRV_OBJS += unimac_drv_impl1.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_v1.o
    DRV_OBJS += bus_drv_mdio_v1.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += phy_drv_egphy.o
    DRV_OBJS += xrdp_led_init.o
    DRV_OBJS += phy_drv_sgmii_plus2.o
    DRV_OBJS += phy_drv_ext1.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += phy_mac_sec.o
    DRV_OBJS += phy_mac_sec_list.o
    DRV_OBJS += sha256.o   
    DRV_OBJS += brcm_rgmii.o
    EXTRA_CFLAGS += -DBUS_MDIO_V1
    EXTRA_CFLAGS += -DBRCM_RGMII
    EXTRA_CFLAGS += -DMAC_UNIMAC -DBRCM_64B_UMAC_MIB -DUMAC_32B_WRAP
    EXTRA_CFLAGS += -DPHY_EGPHY -DPHY_SGMII -DPHY_EXT1 -DPHY_EXT3 -DMACSEC_SUPPORT
    EXTRA_CFLAGS += -DXRDP_LED
endif


ifeq ($(strip ${BRCM_CHIP}),6846)
    DRV_OBJS += unimac_drv_impl1.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_v1.o
    DRV_OBJS += bus_drv_mdio_v1.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += phy_drv_egphy.o
    DRV_OBJS += phy_drv_ext1.o
    DRV_OBJS += brcm_rgmii.o
    EXTRA_CFLAGS += -DBUS_MDIO_V1
    EXTRA_CFLAGS += -DBRCM_RGMII
    EXTRA_CFLAGS += -DMAC_UNIMAC
    EXTRA_CFLAGS += -DPHY_EGPHY -DPHY_EXT1
endif

ifeq ($(strip ${BRCM_CHIP}),6878)
    DRV_OBJS += unimac_drv_impl1.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_v1.o
    DRV_OBJS += bus_drv_mdio_v1.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += phy_drv_egphy.o
    DRV_OBJS += xrdp_led_init.o
    DRV_OBJS += phy_drv_ext1.o
    DRV_OBJS += brcm_rgmii.o
    EXTRA_CFLAGS += -DBUS_MDIO_V1
    EXTRA_CFLAGS += -DBRCM_RGMII
    EXTRA_CFLAGS += -DMAC_UNIMAC
    EXTRA_CFLAGS += -DPHY_EGPHY -DPHY_EXT1
    EXTRA_CFLAGS += -DXRDP_LED
endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6888_68880_6837_),)
    DRV_OBJS += unimac_drv_impl1.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_v1.o
    DRV_OBJS += bus_drv_mdio_v1.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += ephy_led_init.o
ifeq ($(strip $(CONFIG_BRCM_QEMU)),)
    DRV_OBJS += eth_phy_top.o
    DRV_OBJS += mac_drv_xport.o
    DRV_OBJS += phy_drv_egphy.o
    DRV_OBJS += serdes_access.o
    DRV_OBJS += phy_drv_shortfin.o
    DRV_OBJS += phy_drv_shasta.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += phy_drv_wan_ae.o
    DRV_OBJS += mpcs.o
endif
    EXTRA_CFLAGS += -DBUS_MDIO_V1
    EXTRA_CFLAGS += -DMAC_UNIMAC -DBRCM_64B_UMAC_MIB
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6888_68880_),)
    EXTRA_CFLAGS += -DUMAC_32B_WRAP
endif
    EXTRA_CFLAGS += -DEPHY_LED
ifeq ($(strip $(CONFIG_BRCM_QEMU)),)
    EXTRA_CFLAGS += -DMAC_XPORT
    EXTRA_CFLAGS += -DPHY_EGPHY -DPHY_SHORTFIN -DPHY_SHASTA -DPHY_EXT3 -DPHY_WAN_AE
ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_6888_68880_),)
    EXTRA_CFLAGS += -DPHY_XPHY
endif
    EXTRA_CFLAGS += -DXPORT_INTERRUPTS
endif
ifeq ($(strip $(CONFIG_BCM_FTTDP_G9991)),)
    EXTRA_CFLAGS += -DSERDES_LINK_POWER_DOWN -DSERDES_MODULE_DETECT
endif
endif

endif

ifneq ($(findstring _$(strip $(BRCM_CHIP))_,_63138_63148_4908_63158_63178_47622_63146_4912_6813_6756_6765_6766_6764_),)
    DRV_OBJS += mac_drv.o
    DRV_OBJS += bus_drv.o
    DRV_OBJS += phy_drv.o
    DRV_OBJS += phy_drv_mii.o
    DRV_OBJS += phy_drv_brcm.o
    DRV_OBJS += dt_access.o
    DRV_OBJS += dt_parsing.o
    DRV_OBJS += phy_drv_crossbar.o
    DRV_OBJS += brcm_rgmii.o
    EXTRA_CFLAGS += -DBRCM_RGMII
    EXTRA_CFLAGS += -DPHY_CROSSBAR

ifeq ($(strip ${BRCM_CHIP}),63138)
    DRV_OBJS += unimac_drv_impl1.o
    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += mac_drv_sf2.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += phy_drv_dsl_serdes.o
    DRV_OBJS += phy_drv_138class_serdes.o
    DRV_OBJS += phy_drv_dsl_i2c.o
    DRV_OBJS += ephy_led_init_impl2.o
    EXTRA_CFLAGS += -DMAC_UNIMAC -DMAC_SF2 -DPHY_DSL_GPHY -DPHY_M2M -DDSL_DEVICES
    EXTRA_CFLAGS += -DPHY_138CLASS_SERDES -DPHY_138CLASS_SERDES_NO_2P5G
    EXTRA_CFLAGS += -DCRB_5X3_QGPHY3_WORKAROUND
endif

ifeq ($(strip ${BRCM_CHIP}),63148)
    DRV_OBJS += unimac_drv_impl1.o
    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += mac_drv_sf2.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += phy_drv_dsl_serdes.o
    DRV_OBJS += phy_drv_138class_serdes.o
    DRV_OBJS += phy_drv_dsl_i2c.o
    DRV_OBJS += ephy_led_init_impl2.o
    EXTRA_CFLAGS += -DMAC_UNIMAC -DMAC_SF2 -DPHY_DSL_GPHY -DPHY_M2M -DDSL_DEVICES
    EXTRA_CFLAGS += -DPHY_138CLASS_SERDES -DPHY_138CLASS_SERDES_NO_2P5G
endif

ifeq ($(strip ${BRCM_CHIP}),4908)
    DRV_OBJS += unimac_drv_impl1.o
    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += mac_drv_sf2.o
    DRV_OBJS += phy_drv_dsl_serdes.o
    DRV_OBJS += phy_drv_138class_serdes.o
    DRV_OBJS += phy_drv_dsl_i2c.o
    DRV_OBJS += phy_drv_dsl_848xx.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += phy_mac_sec.o
    DRV_OBJS += phy_mac_sec_list.o
    DRV_OBJS += sha256.o
    DRV_OBJS += ephy_led_init.o
    EXTRA_CFLAGS += -DMAC_UNIMAC -DMAC_SF2 -DPHY_DSL_GPHY -DPHY_138CLASS_SERDES -DPHY_M2M -DDSL_DEVICES -DSERDES_OPT_CHECK
    EXTRA_CFLAGS += -DPHY_EXT3 -DMACSEC_SUPPORT
endif

ifeq ($(strip ${BRCM_CHIP}),63158)
    DRV_OBJS += unimac_drv_impl1.o
    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += mac_drv_sf2.o
    DRV_OBJS += mac_drv_xport.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += phy_drv_dsl_serdes.o
    DRV_OBJS += phy_drv_158class_serdes.o
    DRV_OBJS += phy_drv_138class_serdes.o
    DRV_OBJS += phy_drv_dsl_848xx.o
    DRV_OBJS += phy_drv_dsl_i2c.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += phy_mac_sec.o
    DRV_OBJS += phy_mac_sec_list.o
    DRV_OBJS += sha256.o
    DRV_OBJS += ephy_led_init.o

    EXTRA_CFLAGS += -DMAC_UNIMAC -DMAC_SF2 -DPHY_DSL_GPHY -DPHY_M2M -DDSL_DEVICES
    EXTRA_CFLAGS += -DMAC_XPORT
    EXTRA_CFLAGS += -DPHY_158CLASS_SERDES -DPHY_138CLASS_SERDES -DPHY_EXT3 -DMACSEC_SUPPORT
endif

ifeq ($(strip ${BRCM_CHIP}),63178)
    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += mac_drv_sf2.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += ephy_led_init.o

    EXTRA_CFLAGS += -DMAC_SF2 -DPHY_DSL_GPHY -DPHY_M2M -DARCHER_DEVICE -DDSL_DEVICES
    EXTRA_CFLAGS += -I$(BRCMDRIVERS_DIR)/broadcom/include/bcm963xx
endif

ifeq ($(strip ${BRCM_CHIP}),47622)
    DRV_OBJS += unimac_drv_impl1.o

    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += mac_drv_sf2.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += phy_drv_dsl_serdes.o
    DRV_OBJS += phy_drv_138class_serdes.o
    DRV_OBJS += phy_drv_dsl_i2c.o
    DRV_OBJS += phy_drv_dsl_848xx.o
    DRV_OBJS += phy_drv_ext3.o
ifneq ($(findstring $(strip ${MODEL}),RTAX95Q RTAXE95Q),)
    DRV_OBJS += phys/nic_rtl8226b_init.o
    DRV_OBJS += phys/nic_rtl8226.o
    DRV_OBJS += phys/rtl8226.o
endif
    DRV_OBJS += phy_mac_sec.o
    DRV_OBJS += phy_mac_sec_list.o
    DRV_OBJS += sha256.o
    DRV_OBJS += ephy_led_init.o

    EXTRA_CFLAGS += -DMAC_UNIMAC -DMAC_SF2_EXTERNAL -DPHY_DSL_GPHY -DPHY_138CLASS_SERDES -DSERDES_OPT_CHECK
    EXTRA_CFLAGS += -DPHY_EXT3 -DPHY_M2M -DARCHER_DEVICE -DDSL_DEVICES -DMACSEC_SUPPORT

    EXTRA_CFLAGS += -I$(BRCMDRIVERS_DIR)/broadcom/include/bcm963xx
endif

ifeq ($(strip ${BRCM_CHIP}),63146)
    DRV_OBJS += unimac_drv_impl1.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += serdes_access.o
    DRV_OBJS += Serdes146Class/merlin16_shortfin_config.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += phy_drv_146class_serdes.o
    DRV_OBJS += phy_drv_dsl_serdes.o
    DRV_OBJS += phy_drv_dsl_848xx.o
    DRV_OBJS += phy_drv_dsl_i2c.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += ephy_led_init.o

    EXTRA_CFLAGS += -DMAC_UNIMAC -DBRCM_64B_UMAC_MIB -DPHY_DSL_GPHY -DPHY_M2M -DDSL_DEVICES
    EXTRA_CFLAGS += -DPHY_146CLASS_SERDES
    EXTRA_CFLAGS += -DPHY_EXT3
endif

ifeq ($(strip ${BRCM_CHIP}),4912)
    DRV_OBJS += unimac_drv_impl1.o
    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += mac_drv_xport.o
    DRV_OBJS += serdes_access.o
    DRV_OBJS += Serdes146Class/merlin16_shortfin_config.o
    DRV_OBJS += phy_drv_dsl_i2c.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += phy_drv_dsl_serdes.o
    DRV_OBJS += phy_drv_dsl_848xx.o
    DRV_OBJS += phy_drv_146class_serdes.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += phy_mac_sec.o
    DRV_OBJS += phy_mac_sec_list.o
    DRV_OBJS += sha256.o
    DRV_OBJS += ephy_led_init.o

    EXTRA_CFLAGS += -DMAC_UNIMAC -DBRCM_64B_UMAC_MIB -DPHY_DSL_GPHY -DPHY_M2M -DDSL_DEVICES
    EXTRA_CFLAGS += -DPHY_146CLASS_SERDES
    EXTRA_CFLAGS += -DMAC_XPORT -DPHY_EXT3 -DMACSEC_SUPPORT
endif

ifeq ($(strip ${BRCM_CHIP}),6813)
    DRV_OBJS += unimac_drv_impl1.o
    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += eth_phy_top.o
    DRV_OBJS += mac_drv_unimac.o
    DRV_OBJS += mac_drv_xport.o
    DRV_OBJS += serdes_access.o
    DRV_OBJS += Serdes146Class/merlin16_shortfin_config.o
    DRV_OBJS += phy_drv_dsl_i2c.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += phy_drv_dsl_serdes.o
    DRV_OBJS += phy_drv_dsl_848xx.o
    DRV_OBJS += phy_drv_146class_serdes.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += phy_mac_sec.o
    DRV_OBJS += phy_mac_sec_list.o
    DRV_OBJS += sha256.o
    DRV_OBJS += ephy_led_init.o
    DRV_OBJS += phy_drv_wan_ae.o
    DRV_OBJS += mpcs.o

    EXTRA_CFLAGS += -DMAC_UNIMAC -DBRCM_64B_UMAC_MIB -DPHY_DSL_GPHY -DPHY_M2M -DDSL_DEVICES
    EXTRA_CFLAGS += -DPHY_146CLASS_SERDES -DPHY_XPHY
    EXTRA_CFLAGS += -DMAC_XPORT -DPHY_EXT3 -DPHY_WAN_AE -DMACSEC_SUPPORT
endif

ifeq ($(strip ${BRCM_CHIP}),6756)
    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += mac_drv_sf2.o
    DRV_OBJS += serdes_access.o
    DRV_OBJS += Serdes6756Class/merlin28_shortfin_config.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += phy_drv_dsl_serdes.o
    DRV_OBJS += phy_drv_dsl_i2c.o
    DRV_OBJS += phy_drv_dsl_848xx.o
    DRV_OBJS += phy_drv_6756class_serdes.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += phy_mac_sec.o
    DRV_OBJS += phy_mac_sec_list.o
    DRV_OBJS += sha256.o
    DRV_OBJS += ephy_led_init.o

    EXTRA_CFLAGS += -DMAC_SF2_DUAL -DPHY_DSL_GPHY
    EXTRA_CFLAGS += -DPHY_6756CLASS_SERDES -I$(CUR_DIR)/Serdes6756Class -DEXCLUDE_STD_HEADERS
    EXTRA_CFLAGS += -DPHY_EXT3 -DPHY_M2M -DARCHER_DEVICE -DDSL_DEVICES -DMACSEC_SUPPORT
    EXTRA_CFLAGS += -I$(BRCMDRIVERS_DIR)/broadcom/include/bcm963xx
endif

ifeq ($(strip ${BRCM_CHIP}),6765)
    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += mac_drv_sf2.o
    DRV_OBJS += serdes_access.o
    DRV_OBJS += Serdes146Class/merlin16_shortfin_config.o
    DRV_OBJS += eth_phy_top.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += phy_drv_dsl_serdes.o
    DRV_OBJS += phy_drv_dsl_i2c.o
    DRV_OBJS += phy_drv_dsl_848xx.o
    DRV_OBJS += phy_drv_146class_serdes.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += phy_mac_sec.o
    DRV_OBJS += phy_mac_sec_list.o
    DRV_OBJS += sha256.o
    DRV_OBJS += ephy_led_init.o

    EXTRA_CFLAGS += -DMAC_SF2_DUAL -DPHY_DSL_GPHY -DPHY_SHARED_REF_CLK 
    EXTRA_CFLAGS += -DPHY_146CLASS_SERDES -DPHY_XPHY -DRESCAL_BY_XPHY
    EXTRA_CFLAGS += -DPHY_EXT3 -DPHY_M2M -DARCHER_DEVICE -DDSL_DEVICES -DMACSEC_SUPPORT
    EXTRA_CFLAGS += -I$(BRCMDRIVERS_DIR)/broadcom/include/bcm963xx
endif

ifeq ($(strip ${BRCM_CHIP}),6766)
    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += mac_drv_sf2.o
    DRV_OBJS += serdes_access.o
    DRV_OBJS += Serdes146Class/merlin16_shortfin_config.o
    DRV_OBJS += eth_phy_top.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += phy_drv_dsl_serdes.o
    DRV_OBJS += phy_drv_dsl_i2c.o
    DRV_OBJS += phy_drv_dsl_848xx.o
    DRV_OBJS += phy_drv_146class_serdes.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += phy_mac_sec.o
    DRV_OBJS += phy_mac_sec_list.o
    DRV_OBJS += sha256.o
    DRV_OBJS += ephy_led_init.o

    EXTRA_CFLAGS += -DMAC_SF2_DUAL -DPHY_DSL_GPHY -DPHY_SHARED_REF_CLK 
    EXTRA_CFLAGS += -DPHY_146CLASS_SERDES
    EXTRA_CFLAGS += -DPHY_EXT3 -DPHY_M2M -DARCHER_DEVICE -DDSL_DEVICES -DMACSEC_SUPPORT
    EXTRA_CFLAGS += -I$(BRCMDRIVERS_DIR)/broadcom/include/bcm963xx

    ifeq ($(BUILD_NAME), $(filter $(BUILD_NAME), GT7))
    EXTRA_CFLAGS += -DCONFIG_GT7
    endif
endif

ifeq ($(strip ${BRCM_CHIP}),6764)
    DRV_OBJS += bus_drv_sf2.o
    DRV_OBJS += mdio_drv_common.o
    DRV_OBJS += mdio_drv_sf2.o
    DRV_OBJS += mac_drv_sf2.o
    DRV_OBJS += serdes_access.o
    DRV_OBJS += Serdes146Class/merlin16_shortfin_config.o
    DRV_OBJS += eth_phy_top.o
    DRV_OBJS += phy_drv_dsl_phy.o
    DRV_OBJS += phy_drv_dsl_serdes.o
    DRV_OBJS += phy_drv_dsl_i2c.o
    DRV_OBJS += phy_drv_dsl_848xx.o
    DRV_OBJS += phy_drv_146class_serdes.o
    DRV_OBJS += phy_drv_ext3.o
    DRV_OBJS += phy_mac_sec.o
    DRV_OBJS += phy_mac_sec_list.o
    DRV_OBJS += sha256.o
    DRV_OBJS += ephy_led_init.o

    EXTRA_CFLAGS += -DMAC_SF2_DUAL -DPHY_DSL_GPHY -DPHY_SHARED_REF_CLK 
    EXTRA_CFLAGS += -DPHY_146CLASS_SERDES
    EXTRA_CFLAGS += -DPHY_EXT3 -DPHY_M2M -DARCHER_DEVICE -DDSL_DEVICES -DMACSEC_SUPPORT
    EXTRA_CFLAGS += -I$(BRCMDRIVERS_DIR)/broadcom/include/bcm963xx
endif

endif


EXTRA_CFLAGS += -Werror
EXTRA_CFLAGS += -I$(CUR_DIR)
EXTRA_CFLAGS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)/pmc
EXTRA_CFLAGS += -I$(BRCMDRIVERS_DIR)/opensource/net/enet/shared
EXTRA_CFLAGS += $(INC_RDP_FLAGS)
EXTRA_CFLAGS += -DPHY_PROC_FS -DPHY_LINK_CHANGE_NOTIFY

ifeq (${BRCM_ENDIAN_FLAGS},-l)
EXTRA_CFLAGS += -D_BYTE_ORDER_LITTLE_ENDIAN_
endif

obj-y += $(DRV_OBJS)


ifneq ("$(CONFIG_BCM_SELECTIVE_PHYS)", "y")
    CONFIG_BCM_PHY_MAKO_A0=y
    CONFIG_BCM_PHY_ORCA_A0=y
    CONFIG_BCM_PHY_ORCA_B0=y
    CONFIG_BCM_PHY_BLACKFIN_A0=y
    CONFIG_BCM_PHY_BLACKFIN_B0=y
    CONFIG_BCM_PHY_SHORTFIN_B0=y
    CONFIG_BCM_PHY_LONGFIN_A0=y
    CONFIG_BCM_PHY_LONGFIN_B0=y
    CONFIG_BCM_PHY_XPHY=y
    CONFIG_BCM_PHY_KAUAI_A0=y
    CONFIG_BCM_PHY_LANAI_A0=y
    CONFIG_BCM_PHY_NIIHAU_A0=y
endif

ifeq ("$(CONFIG_BCM_PHY_MAKO_A0)", "y")
    EXTRA_CFLAGS += -DCONFIG_BCM_PHY_MAKO_A0
    PHY_FIRMWARE += firmware/mako_a0_firmware.bin
endif

ifeq ("$(CONFIG_BCM_PHY_ORCA_A0)", "y")
    EXTRA_CFLAGS += -DCONFIG_BCM_PHY_ORCA_A0
    PHY_FIRMWARE += firmware/orca_a0_firmware.bin
endif

ifeq ("$(CONFIG_BCM_PHY_ORCA_B0)", "y")
    EXTRA_CFLAGS += -DCONFIG_BCM_PHY_ORCA_B0
    PHY_FIRMWARE += firmware/orca_b0_firmware.bin
endif

ifeq ("$(CONFIG_BCM_PHY_BLACKFIN_A0)", "y")
    EXTRA_CFLAGS += -DCONFIG_BCM_PHY_BLACKFIN_A0
    PHY_FIRMWARE += firmware/blackfin_a0_firmware.bin
endif

ifeq ("$(CONFIG_BCM_PHY_BLACKFIN_B0)", "y")
    EXTRA_CFLAGS += -DCONFIG_BCM_PHY_BLACKFIN_B0
    PHY_FIRMWARE += firmware/blackfin_b0_firmware.bin
endif

ifeq ("$(CONFIG_BCM_PHY_SHORTFIN_B0)", "y")
    EXTRA_CFLAGS += -DCONFIG_BCM_PHY_SHORTFIN_B0
    PHY_FIRMWARE += firmware/shortfin_b0_firmware.bin
endif

ifeq ("$(CONFIG_BCM_PHY_KAUAI_A0)", "y")
    EXTRA_CFLAGS += -DCONFIG_BCM_PHY_KAUAI_A0
    PHY_FIRMWARE += firmware/kauai_a0_firmware.bin
endif

ifeq ("$(CONFIG_BCM_PHY_LANAI_A0)", "y")
    EXTRA_CFLAGS += -DCONFIG_BCM_PHY_LANAI_A0
    PHY_FIRMWARE += firmware/lanai_a0_firmware.bin
endif

ifeq ("$(CONFIG_BCM_PHY_NIIHAU_A0)", "y")
    EXTRA_CFLAGS += -DCONFIG_BCM_PHY_NIIHAU_A0
    PHY_FIRMWARE += firmware/niihau_a0_firmware.bin
endif

ifneq ($(findstring _y_,_$(CONFIG_BCM_PHY_LONGFIN_A0)_$(CONFIG_BCM_PHY_LONGFIN_B0)_),)
    EXTRA_CFLAGS += -DCONFIG_BCM_PHY_LONGFIN_A0 -DCONFIG_BCM_PHY_LONGFIN_B0
    PHY_FIRMWARE += firmware/longfin_a0_firmware.bin
endif

ifeq ("$(CONFIG_BCM_PHY_XPHY)", "y")
    EXTRA_CFLAGS += -DCONFIG_BCM_PHY_XPHY
        PHY_FIRMWARE += firmware/xphy_firmware.bin
    endif


-include $(TOPDIR)/Rules.make

INSTALL_DONE = $(src)/.done_install.o
INSTALL_PATH=$(INSTALL_DIR)/etc/fw
INSTALL_SRC = $(addprefix $(src)/, $(PHY_FIRMWARE))

clean-files := $(notdir $(INSTALL_DONE))

$(INSTALL_PATH):
	mkdir -p $(INSTALL_PATH)

$(INSTALL_DONE): $(INSTALL_SRC) | $(INSTALL_PATH)
	$(if $(strip $(INSTALL_SRC)), cp -f $^ $(INSTALL_PATH),)
	$(Q)touch $@

$(obj)/phy_drv_ext3.o: $(INSTALL_DONE)
