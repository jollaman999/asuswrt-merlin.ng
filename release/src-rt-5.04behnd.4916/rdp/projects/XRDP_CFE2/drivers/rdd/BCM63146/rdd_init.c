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

* :>
*/

#include "rdd.h"
#include "rdd_defs.h"
#include "rdd_init.h"

#include "XRDP_AG.h"
#include "rdd_map_auto.h"
#include "rdp_common.h"
#include "rdd_runner_proj_defs.h"
#include "xrdp_drv_rnr_regs_ag.h"

extern int reg_id[32];


int rdd_init(void)
{
    return 0;
}

void rdd_exit(void)
{
}

void rdp_rnr_write_context(void *__to, void *__from, unsigned int __n);

static void rdd_global_registers_init(uint32_t core_index, uint32_t local_regs[NUM_OF_MAIN_RUNNER_THREADS][NUM_OF_LOCAL_REGS], uint32_t last_thread)
{
     uint32_t i;

    /********** common to all runners **********/
    /* in this project we don't have really global, so will set all registers that should be global for all threads */
    for (i = 0; i <= last_thread; ++i)
    {
        local_regs[i][1] = 1; /* CONST_1 is 1 */
    }
}

static void image_0_context_set(uint32_t core_index)
{
    static uint32_t local_regs[NUM_OF_MAIN_RUNNER_THREADS][NUM_OF_LOCAL_REGS];
    uint32_t *sram_context;

    rdd_global_registers_init(core_index, local_regs, core_index);
    sram_context = (uint32_t *)RUNNER_CORE_CONTEXT_ADDRESS(core_index);

    /* read the local registers from the Context memory - maybe it was initialized by the ACE compiler */
    MREAD_BLK_32(local_regs, sram_context, sizeof(local_regs));

    /* CPU_RX: thread 0 */
    local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[0]]  = ADDRESS_OF(image_0, direct_processing_wakeup_request);
    local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[9]]  = IMAGE_0_RX_FLOW_TABLE_ADDRESS << 16 | IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER;
    local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[10]] = BB_ID_DISPATCHER_REORDER;
    local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[11]] = 0;
    local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[16]] = IMAGE_0_DIRECT_PROCESSING_PD_TABLE_ADDRESS;
    local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[17]] = IMAGE_0_SRAM_PD_FIFO_ADDRESS;
    local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[18]] = IMAGE_0_SRAM_PD_FIFO_ADDRESS + IMAGE_0_SRAM_PD_FIFO_BYTE_SIZE - sizeof(CPU_RX_DESCRIPTOR_STRUCT);

    /* CPU_TX: thread 1 */
    local_regs[IMAGE_0_CFE_CORE_CPU_TX_THREAD_NUMBER][reg_id[0]] = ADDRESS_OF(image_0, cpu_tx_wakeup_request);
    local_regs[IMAGE_0_CFE_CORE_CPU_TX_THREAD_NUMBER][reg_id[8]] = IMAGE_0_BBH_TX_RING_TABLE_ADDRESS;
    local_regs[IMAGE_0_CFE_CORE_CPU_TX_THREAD_NUMBER][reg_id[9]] = IMAGE_0_BBH_TX_BB_DESTINATION_TABLE_ADDRESS;

    MWRITE_32(sram_context, local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[0]]);
    MWRITE_32(sram_context + (9), local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[9]]);
    MWRITE_32(sram_context + (10), local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[10]]);
    MWRITE_32(sram_context + (11), local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[11]]);
    MWRITE_32(sram_context + (16), local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[16]]);
    MWRITE_32(sram_context + (17), local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[17]]);
    MWRITE_32(sram_context + (18), local_regs[IMAGE_0_CFE_CORE_CPU_RX_THREAD_NUMBER][reg_id[18]]);

    MWRITE_32(sram_context + (IMAGE_0_CFE_CORE_CPU_TX_THREAD_NUMBER * 32), local_regs[IMAGE_0_CFE_CORE_CPU_TX_THREAD_NUMBER][reg_id[0]]);
    MWRITE_32(sram_context + ((IMAGE_0_CFE_CORE_CPU_TX_THREAD_NUMBER * 32) + 8), local_regs[IMAGE_0_CFE_CORE_CPU_TX_THREAD_NUMBER][reg_id[8]]);
    MWRITE_32(sram_context + ((IMAGE_0_CFE_CORE_CPU_TX_THREAD_NUMBER * 32) + 9), local_regs[IMAGE_0_CFE_CORE_CPU_TX_THREAD_NUMBER][reg_id[9]]);
}

static void rdd_local_registers_init(void)
{
    image_0_context_set(0);
}

int rdd_data_structures_init(rdd_init_params_t *init_params)
{
    bdmf_error_t rc = BDMF_ERR_OK;
    CPU_RX_LAST_READ_INDEX_STRUCT *last_read_idx_ptr = RDD_CPU_RX_LAST_READ_INDEX_PTR(0);
    uint32_t flow_map[] = {BB_ID_RX_BBH_0, BB_ID_RX_BBH_1, BB_ID_RX_BBH_2,
                           BB_ID_RX_BBH_3, BB_ID_RX_BBH_4, BB_ID_RX_BBH_5,
                           BB_ID_RX_BBH_6, BB_ID_RX_BBH_7};
    uint32_t num_of_queues = sizeof(flow_map)/sizeof(uint32_t);
    int i;

    rdd_local_registers_init();
    MWRITE_16(last_read_idx_ptr, (IMAGE_0_SRAM_PD_FIFO_ADDRESS +
                                  IMAGE_0_SRAM_PD_FIFO_BYTE_SIZE -
                                  sizeof(CPU_RX_DESCRIPTOR_STRUCT)));
    for (i = 0; i < num_of_queues; i++)
        rdd_rx_flow_cfg((256 + flow_map[i]), 0, i, 0);

    return rc;
}


