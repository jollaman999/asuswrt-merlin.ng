# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the kernel character device drivers.
#

obj-y				+= mem.o random.o
obj-$(CONFIG_TTY_PRINTK)	+= ttyprintk.o
obj-y				+= misc.o
obj-$(CONFIG_ATARI_DSP56K)	+= dsp56k.o
obj-$(CONFIG_VIRTIO_CONSOLE)	+= virtio_console.o
obj-$(CONFIG_RAW_DRIVER)	+= raw.o
obj-$(CONFIG_SGI_SNSC)		+= snsc.o snsc_event.o
obj-$(CONFIG_MSPEC)		+= mspec.o
obj-$(CONFIG_UV_MMTIMER)	+= uv_mmtimer.o
obj-$(CONFIG_IBM_BSR)		+= bsr.o
obj-$(CONFIG_SGI_MBCS)		+= mbcs.o

obj-$(CONFIG_PRINTER)		+= lp.o

obj-$(CONFIG_APM_EMULATION)	+= apm-emulation.o

obj-$(CONFIG_DTLK)		+= dtlk.o
obj-$(CONFIG_APPLICOM)		+= applicom.o
obj-$(CONFIG_SONYPI)		+= sonypi.o
obj-$(CONFIG_RTC)		+= rtc.o
obj-$(CONFIG_HPET)		+= hpet.o
obj-$(CONFIG_EFI_RTC)		+= efirtc.o
obj-$(CONFIG_XILINX_HWICAP)	+= xilinx_hwicap/
ifeq ($(CONFIG_GENERIC_NVRAM),y)
  obj-$(CONFIG_NVRAM)	+= generic_nvram.o
else
  obj-$(CONFIG_NVRAM)	+= nvram.o
endif
obj-$(CONFIG_TOSHIBA)		+= toshiba.o
obj-$(CONFIG_DS1620)		+= ds1620.o
obj-$(CONFIG_HW_RANDOM)		+= hw_random/
obj-$(CONFIG_PPDEV)		+= ppdev.o
obj-$(CONFIG_NWBUTTON)		+= nwbutton.o
obj-$(CONFIG_NWFLASH)		+= nwflash.o
obj-$(CONFIG_SCx200_GPIO)	+= scx200_gpio.o
obj-$(CONFIG_PC8736x_GPIO)	+= pc8736x_gpio.o
obj-$(CONFIG_NSC_GPIO)		+= nsc_gpio.o
obj-$(CONFIG_GPIO_TB0219)	+= tb0219.o
obj-$(CONFIG_TELCLOCK)		+= tlclk.o

obj-$(CONFIG_MWAVE)		+= mwave/
obj-y				+= agp/
obj-$(CONFIG_PCMCIA)		+= pcmcia/

obj-$(CONFIG_HANGCHECK_TIMER)	+= hangcheck-timer.o
obj-$(CONFIG_TCG_TPM)		+= tpm/

obj-$(CONFIG_PS3_FLASH)		+= ps3flash.o

obj-$(CONFIG_JS_RTC)		+= js-rtc.o
js-rtc-y = rtc.o

obj-$(CONFIG_XILLYBUS)		+= xillybus/
obj-$(CONFIG_POWERNV_OP_PANEL)	+= powernv-op-panel.o
obj-$(CONFIG_ADI)		+= adi.o
ifeq ($(MODEL),$(filter $(MODEL),RTAX55 RTAX1800 RTAX58U_V2 RTAX3000N BR63 RTBE58U TUFBE3600 RTBE58U_V2 TUFBE3600_V2 RTBE55))
obj-m				+= rtk/
endif
ifeq ($(MODEL),$(filter $(MODEL),GTBE98 GTBE98_PRO GTBE96 GTBE19000 RTBE92U RTBE95U RTBE82U TUFBE82 RTBE58U_PRO GTBE19000AI GTBE96_AI))
obj-m				+= rtl8372/
endif
ifeq ($(MODEL),$(filter $(MODEL),RTBE82M GSBE18000 GSBE12000 GS7_PRO GT7))
obj-m				+= mxl/
endif
