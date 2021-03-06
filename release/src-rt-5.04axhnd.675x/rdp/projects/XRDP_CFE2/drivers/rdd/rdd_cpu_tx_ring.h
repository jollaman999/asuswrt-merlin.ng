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
 * cpu_tx_ring.h
 */

#ifndef _CPU_TX_RING_H_
#define _CPU_TX_RING_H_

#if defined(XRDP_MANAGE_SBPM)
int rdd_cpu_tx_new(uint8_t *buffer, uint32_t length, uint8_t tx_port);
#else
int rdd_cpu_tx(uint32_t length, uint16_t bn0, uint16_t bn1, uint8_t bns_num, uint8_t tx_port);
#endif

#endif /* _CPU_TX_RING_H_ */
