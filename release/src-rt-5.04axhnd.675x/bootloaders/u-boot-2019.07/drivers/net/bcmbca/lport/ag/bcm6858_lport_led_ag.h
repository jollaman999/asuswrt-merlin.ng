// SPDX-License-Identifier: GPL-2.0+
/*
   Copyright (c) 2015 Broadcom Corporation
   All Rights Reserved

    
*/

#ifndef _BCM6858_LPORT_LED_AG_H_
#define _BCM6858_LPORT_LED_AG_H_

#include "access_macros.h"
#include "bcmtypes.h"
typedef struct
{
    uint8_t lnk_ovrd_en;
    uint8_t spd_ovrd_en;
    uint8_t lnk_status_ovrd;
    uint8_t led_spd_ovrd;
    uint8_t act_led_pol_sel;
    uint8_t spdlnk_led2_act_pol_sel;
    uint8_t spdlnk_led1_act_pol_sel;
    uint8_t spdlnk_led0_act_pol_sel;
    uint8_t act_led_act_sel;
    uint8_t spdlnk_led2_act_sel;
    uint8_t spdlnk_led1_act_sel;
    uint8_t spdlnk_led0_act_sel;
    uint8_t tx_act_en;
    uint8_t rx_act_en;
} lport_led_cntrl;

typedef struct
{
    uint8_t rsvd_sel_spd_encode_2;
    uint8_t rsvd_sel_spd_encode_1;
    uint8_t sel_10g_encode;
    uint8_t sel_2500m_encode;
    uint8_t sel_1000m_encode;
    uint8_t sel_100m_encode;
    uint8_t sel_10m_encode;
    uint8_t sel_no_link_encode;
} lport_led_link_and_speed_encoding_sel;

typedef struct
{
    uint8_t rsvd_spd_encode_2;
    uint8_t rsvd_spd_encode_1;
    uint8_t m10g_encode;
    uint8_t m2500_encode;
    uint8_t m1000_encode;
    uint8_t m100_encode;
    uint8_t m10_encode;
    uint8_t no_link_encode;
} lport_led_link_and_speed_encoding;

int ag_drv_lport_led_cntrl_set(uint8_t led_id, const lport_led_cntrl *cntrl);
int ag_drv_lport_led_cntrl_get(uint8_t led_id, lport_led_cntrl *cntrl);
int ag_drv_lport_led_link_and_speed_encoding_sel_set(uint8_t led_id, const lport_led_link_and_speed_encoding_sel *link_and_speed_encoding_sel);
int ag_drv_lport_led_link_and_speed_encoding_sel_get(uint8_t led_id, lport_led_link_and_speed_encoding_sel *link_and_speed_encoding_sel);
int ag_drv_lport_led_link_and_speed_encoding_set(uint8_t led_id, const lport_led_link_and_speed_encoding *link_and_speed_encoding);
int ag_drv_lport_led_link_and_speed_encoding_get(uint8_t led_id, lport_led_link_and_speed_encoding *link_and_speed_encoding);
int ag_drv_lport_led_aggregate_led_cntrl_set(uint8_t led_id, uint8_t lnk_pol_sel, uint8_t act_pol_sel, uint8_t act_sel, uint16_t port_en);
int ag_drv_lport_led_aggregate_led_cntrl_get(uint8_t led_id, uint8_t *lnk_pol_sel, uint8_t *act_pol_sel, uint8_t *act_sel, uint16_t *port_en);
int ag_drv_lport_led_aggregate_led_blink_rate_cntrl_set(uint8_t led_id, uint16_t led_on_time, uint16_t led_off_time);
int ag_drv_lport_led_aggregate_led_blink_rate_cntrl_get(uint8_t led_id, uint16_t *led_on_time, uint16_t *led_off_time);

#ifdef USE_BDMF_SHELL
bdmfmon_handle_t ag_drv_lport_led_cli_init(bdmfmon_handle_t driver_dir);
#endif


#endif

