/*
   <:copyright-BRCM:2015:DUAL/GPL:standard
   
      Copyright (c) 2015 Broadcom 
      All Rights Reserved
   
   Unless you and Broadcom execute a separate written software license
   agreement governing use of this software, this software is licensed
   to you under the terms of the GNU General Public License version 2
   (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
   with the following added to such license:
   
      As a special exception, the copyright holders of this software give
      you permission to link this software with independent modules, and
      to copy and distribute the resulting executable under terms of your
      choice, provided that you also meet, for each linked independent
      module, the terms and conditions of the license of that module.
      An independent module is a module which is not derived from this
      software.  The special exception does not apply to any modifications
      of the software.
   
   Not withstanding the above, under no circumstances may you combine
   this software in any way with any other Broadcom software provided
   under a license other than the GPL, without Broadcom's express prior
   written consent.
   
   :>
 */

/*
 *  Created on: Nov/2015
 *      Author: dima.mamut@broadcom.com
 */

#ifndef _BCMENET_PROC_H_
#define _BCMENET_PROC_H_

#ifdef ENET_PROC
#ifdef ENET_DEBUG_RX_CPU_TRAFFIC
extern uint32_t g_debug_mode;
#ifdef CONFIG_ARM64
extern uint64_t g_debug_mode_pckt_rx;
#else
extern uint32_t g_debug_mode_pckt_rx;
#endif
extern struct timeval g_start_time;
extern struct timeval g_end_time;
#endif

int enet_proc_init(void);
void enet_proc_exit(void);
#else
static inline void enet_proc_exit(void)
{
}

static inline int enet_proc_init(void)
{
    return 0;
}
#endif
#endif
