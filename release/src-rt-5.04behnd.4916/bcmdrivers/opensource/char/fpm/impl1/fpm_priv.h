/*
* <:copyright-BRCM:2015:DUAL/GPL:standard
* 
*    Copyright (c) 2015 Broadcom 
*    All Rights Reserved
* 
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed
* to you under the terms of the GNU General Public License version 2
* (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
* with the following added to such license:
* 
*    As a special exception, the copyright holders of this software give
*    you permission to link this software with independent modules, and
*    to copy and distribute the resulting executable under terms of your
*    choice, provided that you also meet, for each linked independent
*    module, the terms and conditions of the license of that module.
*    An independent module is a module which is not derived from this
*    software.  The special exception does not apply to any modifications
*    of the software.
* 
* Not withstanding the above, under no circumstances may you combine
* this software in any way with any other Broadcom software provided
* under a license other than the GPL, without Broadcom's express prior
* written consent.
* 
* :> 
*/

/*****************************************************************************
 *
 * Copyright (c) 2015 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Author: Tim Ross <tross@broadcom.com>
 *****************************************************************************/
#ifndef _FPM_PRIV_H_
#define _FPM_PRIV_H_

#include <linux/types.h>
#include <linux/skbuff.h>
#include <uapi/linux/if_ether.h>
#include "fpm.h"

#if !defined(CONFIG_BCM_FPM_POOL_NUM) || (CONFIG_BCM_FPM_POOL_NUM < 1)
#define FPM_POOL_NUM 1
#else
#define FPM_POOL_NUM CONFIG_BCM_FPM_POOL_NUM
#endif

/* For debugging. */
#define FPM_CACHED 1
#define FPM_FORCE_NO_TOKEN_CHUNKS_BITS	1
#define STOP_UBUS_CAPTURE		0

#define MODULE_NAME	"bcm-fpm"
#define MODULE_VER	"2.0"

/*
 * System-wide all FPM's used for network packets need to have space reserved
 * at the beginning and end of the buffer for use by DOCSIS and WiFi. The FPM
 * driver stores the values below into the FPM hardware scratch register when
 * it initializes the FPM hardware. Other hardware entities (i.e. various
 * IOP's, drivers running under eCos, etc) obtain these values from the FPM
 * scratch register. Other linux entities obtain these values using the
 * fpm_get_hw_info() API.
 */
#define FPM_NET_BUF_HEAD_PAD	(240)
#define WIFI_CNTXT_SPACE	(8)
#define WIFI_SHARED_INFO_SPACE  (64)
#define FPM_NET_BUF_TAIL_PAD	(WIFI_CNTXT_SPACE + sizeof(struct sk_buff) + \
				 WIFI_SHARED_INFO_SPACE)

/* Token operation */
/*
 * Pack to save memory and carefully order members so that they remain
 * aligned on their type-size boundaries for speed.
 */
#define FPM_TOK_OP_BUF_SIZE	(2*ETH_ALEN)
struct __attribute__ ((packed)) fpm_tok_op {
	u32 token;		/* token */
	void *called;		/* driver function called */
	void *caller;		/* driver function caller */
	u32 ts;			/* timestamp of op (jiffies) */
	u32 op_data;		/* if_id/if_sub_id (pkt), header (RPC), ... */
	u8 ref_count;		/* ref count at time of op */
	u8 buf[FPM_TOK_OP_BUF_SIZE];
};

#define FPM_HISTORY_MEM_SIZE	(CONFIG_BCM_FPM_TOKEN_HIST_MEM * 1024)
#define FPM_NUM_HISTORY_ENTRIES \
	(FPM_HISTORY_MEM_SIZE / sizeof(struct fpm_tok_op))

#define FPM_ISR_TIMER_PERIOD	1000	/* msec */
#define FPM_DEV_IRQ_MASK	FPM_POOL_FULL_MASK | \
				FPM_FREE_TOKEN_NO_VALID_MASK |\
				FPM_FREE_TOKEN_INDEX_OUT_OF_RANGE_MASK | \
				FPM_MULTI_TOKEN_NO_VALID_MASK | \
				FPM_MULTI_TOKEN_INDEX_OUT_OF_RANGE_MASK | \
				FPM_POOL_DIS_FREE_MULTI_MASK | \
				FPM_MEMORY_CORRUPT_MASK | \
				FPM_XON_MASK | \
				FPM_XOFF_MASK | \
				FPM_ILLEGAL_ADDRESS_ACCESS_MASK | \
				FPM_ILLEGAL_ALLOC_REQUEST_MASK | \
				FPM_EXPIRED_TOKEN_DET_MASK | \
				FPM_EXPIRED_TOKEN_RECOV_MASK
				
#define FPM_TOKEN_VALID_MASK (0x1 << 31)

#define BcmMemReserveVirtToPhys(vbase, pbase, virt)  \
	(phys_addr_t)((phys_addr_t)(pbase) + (phys_addr_t)((unsigned char*)(virt) - (unsigned char*)(vbase)))

typedef struct FpmCtrl
{
	u32 fpm_ctl;          /* 0x0000 */
#define FPMCTRL_FPM_CTRL_INIT_MEM_SHIFT (4)
#define FPMCTRL_FPM_CTRL_INIT_MEM_MASK (0x1 << FPMCTRL_FPM_CTRL_INIT_MEM_SHIFT)
#define FPMCTRL_FPM_CTRL_INIT_MEM_POOL2_SHIFT (3)
#define FPMCTRL_FPM_CTRL_INIT_MEM_POOL2_MASK (0x1 << FPMCTRL_FPM_CTRL_INIT_MEM_POOL2_SHIFT)
#define FPMCTRL_FPM_CTRL_SOFT_RESET_SHIFT (14)
#define FPMCTRL_FPM_CTRL_SOFT_RESET_MASK (0x1 << FPMCTRL_FPM_CTRL_SOFT_RESET_SHIFT)
#define FPMCTRL_FPM_CTRL_POOL1_ENABLE_SHIFT (16)
#define FPMCTRL_FPM_CTRL_POOL1_ENABLE_MASK (0x1 << FPMCTRL_FPM_CTRL_POOL1_ENABLE_SHIFT)
#define FPMCTRL_FPM_CTRL_POOL2_ENABLE_SHIFT (17)
#define FPMCTRL_FPM_CTRL_POOL2_ENABLE_MASK (0x1 << FPMCTRL_FPM_CTRL_POOL2_ENABLE_SHIFT)
	u32 fpm_cfg1;
	u32 fpm_weight;
#define FPMCTRL_FPM_WEIGHT_DDR0_ALLOC_SHIFT (0)
#define FPMCTRL_FPM_WEIGHT_DDR0_ALLOC_MASK (0xff << FPMCTRL_FPM_WEIGHT_DDR0_ALLOC_SHIFT)
#define FPMCTRL_FPM_WEIGHT_DDR0_FREE_SHIFT (8)
#define FPMCTRL_FPM_WEIGHT_DDR0_FREE_MASK (0xff << FPMCTRL_FPM_WEIGHT_DDR0_FREE_SHIFT)
#define FPMCTRL_FPM_WEIGHT_DDR1_ALLOC_SHIFT (16)
#define FPMCTRL_FPM_WEIGHT_DDR1_ALLOC_MASK (0xff << FPMCTRL_FPM_WEIGHT_DDR1_ALLOC_SHIFT)
#define FPMCTRL_FPM_WEIGHT_DDR1_FREE_SHIFT (24)
#define FPMCTRL_FPM_WEIGHT_DDR1_FREE_MASK (0xff << FPMCTRL_FPM_WEIGHT_DDR1_FREE_SHIFT)
	u32 fpm_bb_cfg;
#define FPMCTRL_FPM_BB_CFG_DDR0 (0x0)
#define FPMCTRL_FPM_BB_CFG_DDR1 (0x1)
#define FPMCTRL_FPM_BB_CFG_DDR_BOTH (0x2)
	u32 pool1_intr_msk;
#define FPM_ALLOC_FIFO_FULL_MASK (0x1 << 0)
#define FPM_FREE_FIFO_FULL_MASK (0x1 << 1)
#define FPM_POOL_FULL_MASK (0x1 << 2)
#define FPM_FREE_TOKEN_NO_VALID_MASK (0x1 << 3)
#define FPM_FREE_TOKEN_INDEX_OUT_OF_RANGE_MASK (0x1 << 4)
#define FPM_MULTI_TOKEN_NO_VALID_MASK (0x1 << 5)
#define FPM_MULTI_TOKEN_INDEX_OUT_OF_RANGE_MASK (0x1 << 6)
#define FPM_POOL_DIS_FREE_MULTI_MASK (0x1 << 7)
#define FPM_MEMORY_CORRUPT_MASK (0x1 << 8)
#define FPM_XOFF_MASK (0x1 << 9)
#define FPM_XON_MASK (0x1 << 10)
#define FPM_ILLEGAL_ADDRESS_ACCESS_MASK (0x1 << 11)
#define FPM_ILLEGAL_ALLOC_REQUEST_MASK (0x1 << 12)
#define FPM_EXPIRED_TOKEN_DET_MASK (0x1 << 13)
#define FPM_EXPIRED_TOKEN_RECOV_MASK (0x1 << 14)
	u32 pool1_intr_sts;
	u32 pool1_stall_msk;
	u32 pool2_intr_msk;
	u32 pool2_intr_sts;
	u32 pool2_stall_msk;
	u32 reserved0[6];
	u32 pool1_cfg1;       /* 0x0040 */
#define FPMCTRL_FP_BUF_SIZE_SHIFT (24)
#define FPMCTRL_FP_BUF_SIZE_256 (0x1 << FPMCTRL_FP_BUF_SIZE_SHIFT)
#define FPMCTRL_FP_BUF_SIZE_512 (0x0 << FPMCTRL_FP_BUF_SIZE_SHIFT)
#define FPMCTRL_FP_BUF_SIZE_MASK (0x7 << FPMCTRL_FP_BUF_SIZE_SHIFT)
	u32 pool1_cfg2;
#define FPMCTRL_FPM_POOL_BASE_ADDRESS_MASK (0xfffffffc)
	u32 pool1_cfg3;
	u32 reserved1;
	u32 pool1_stat1;
#define FPMCTRL_POOL_STAT1_OVRFL_SHIFT (16)
#define FPMCTRL_POOL_STAT1_OVRFL_MASK (0xffff << FPMCTRL_POOL_STAT1_OVRFL_SHIFT)
#define FPMCTRL_POOL_STAT1_UNDRFL_SHIFT (0)
#define FPMCTRL_POOL_STAT1_UNDRFL_MASK (0xffff << FPMCTRL_POOL_STAT1_UNDRFL_SHIFT)
	u32 pool1_stat2;
#define FPMCTRL_POOL_STAT2_POOL_FULL_SHIFT (31)
#define FPMCTRL_POOL_STAT2_POOL_FULL_MASK (0x1 << FPMCTRL_POOL_STAT2_POOL_FULL_SHIFT)
#define FPMCTRL_POOL_STAT2_FREE_FIFO_FULL_SHIFT (29)
#define FPMCTRL_POOL_STAT2_FREE_FIFO_FULL_MASK (0x1 << FPMCTRL_POOL_STAT2_FREE_FIFO_FULL_SHIFT)
#define FPMCTRL_POOL_STAT2_FREE_FIFO_EMPTY_SHIFT (28)
#define FPMCTRL_POOL_STAT2_FREE_FIFO_EMPTY_MASK (0x1 << FPMCTRL_POOL_STAT2_FREE_FIFO_EMPTY_SHIFT)
#define FPMCTRL_POOL_STAT2_ALLOC_FIFO_FULL_SHIFT (27)
#define FPMCTRL_POOL_STAT2_ALLOC_FIFO_FULL_MASK (0x1 << FPMCTRL_POOL_STAT2_ALLOC_FIFO_FULL_SHIFT)
#define FPMCTRL_POOL_STAT2_ALLOC_FIFO_EMPTY_SHIFT (26)
#define FPMCTRL_POOL_STAT2_ALLOC_FIFO_EMPTY_MASK (0x1 << FPMCTRL_POOL_STAT2_ALLOC_FIFO_EMPTY_SHIFT)
#define FPMCTRL_POOL_STAT2_NUM_OF_TOKENS_AVAL_SHIFT (0)
#define FPMCTRL_POOL_STAT2_NUM_OF_TOKENS_AVAL_MASK (0x3ffff << FPMCTRL_POOL_STAT2_NUM_OF_TOKENS_AVAL_SHIFT)
	u32 pool1_stat3;
#define FPMCTRL_POOL_STAT3_NUM_OF_NOT_VALID_TOKEN_FREES_SHIFT (0)
#define FPMCTRL_POOL_STAT3_NUM_OF_NOT_VALID_TOKEN_FREES_MASK (0x3ffff << FPMCTRL_POOL_STAT3_NUM_OF_NOT_VALID_TOKEN_FREES_SHIFT)
	u32 pool1_stat4;
#define FPMCTRL_POOL_STAT4_NUM_OF_NOT_VALID_TOKEN_MULTI_SHIFT (0)
#define FPMCTRL_POOL_STAT4_NUM_OF_NOT_VALID_TOKEN_MULTI_MASK (0x3ffff << FPMCTRL_POOL_STAT4_NUM_OF_NOT_VALID_TOKEN_MULTI_SHIFT)
	u32 pool1_stat5;
#define FPMCTRL_POOL_STAT5_MEM_CORRUPT_STS_RELATED_ALLOC_TOKEN_VALID_SHIFT (31)
#define FPMCTRL_POOL_STAT5_MEM_CORRUPT_STS_RELATED_ALLOC_TOKEN_VALID_MASK (0x1 << FPMCTRL_POOL_STAT5_MEM_CORRUPT_STS_RELATED_ALLOC_TOKEN_VALID_SHIFT)
#define FPMCTRL_POOL_STAT5_MEM_CORRUPT_STS_RELATED_ALLOC_TOKEN_SHIFT (0)
#define FPMCTRL_POOL_STAT5_MEM_CORRUPT_STS_RELATED_ALLOC_TOKEN_MASK (0x7fffffff << FPMCTRL_POOL_STAT5_MEM_CORRUPT_STS_RELATED_ALLOC_TOKEN_SHIFT)
	u32 pool1_stat6;
#define FPMCTRL_POOL_STAT6_INVALID_FREE_TOKEN_VALID_SHIFT (31)
#define FPMCTRL_POOL_STAT6_INVALID_FREE_TOKEN_VALID_MASK (0x1 << FPMCTRL_POOL_STAT6_INVALID_FREE_TOKEN_VALID_SHIFT)
#define FPMCTRL_POOL_STAT6_INVALID_FREE_TOKEN_SHIFT (0)
#define FPMCTRL_POOL_STAT6_INVALID_FREE_TOKEN_MASK (0x7fffffff << FPMCTRL_POOL_STAT6_INVALID_FREE_TOKEN_SHIFT)
	u32 pool1_stat7;
#define FPMCTRL_POOL_STAT7_INVALID_MCAST_TOKEN_VALID_SHIFT (31)
#define FPMCTRL_POOL_STAT7_INVALID_MCAST_TOKEN_VALID_MASK (0x1 << FPMCTRL_POOL_STAT7_INVALID_MCAST_TOKEN_VALID_SHIFT)
#define FPMCTRL_POOL_STAT7_INVALID_MCAST_TOKEN_SHIFT (0)
#define FPMCTRL_POOL_STAT7_INVALID_MCAST_TOKEN_MASK (0x7fffffff << FPMCTRL_POOL_STAT7_INVALID_MCAST_TOKEN_SHIFT)
	u32 pool1_stat8;
#define FPMCTRL_POOL_STAT8_TOKENS_AVAIL_LOW_WTMK_SHIFT (0)
#define FPMCTRL_POOL_STAT8_TOKENS_AVAIL_LOW_WTMK_MASK (0x3ffff << FPMCTRL_POOL_STAT8_TOKENS_AVAIL_LOW_WTMK_SHIFT)
	u32 pool2_stat1;
	u32 pool2_stat2;
	u32 pool2_stat3;
	u32 pool2_stat4;
	u32 pool2_stat5;      /* 0x0080 */
	u32 pool2_stat6;
	u32 pool2_stat7;
	u32 pool2_stat8;
	u32 reserved2[12];
	u32 pool1_xon_xoff_cfg;    /* 0x00c0 */
	u32 reserved3[3];
	u32 fpm_not_empty_cfg;
	u32 reserved4[11];
	u32 mem_ctl;          /* 0x0100 */
	u32 mem_data1;
	u32 mem_data2;
	u32 reserved5[5];
	u32 spare;
	u32 reserved6[3];
	u32 token_recover_ctl;
#define FPMCTRL_TOKEN_RECOVER_CTL_CLR_RECOVERED_TOKEN_COUNT_MASK (0x1 << 6)
#define FPMCTRL_TOKEN_RECOVER_CTL_CLR_EXPIRED_TOKEN_COUNT_MASK (0x1 << 5)
#define FPMCTRL_TOKEN_RECOVER_CTL_FORCE_TOKEN_RECLAIM_MASK (0x1 << 4)
#define FPMCTRL_TOKEN_RECOVER_CTL_TOKEN_RECLAIM_EN_MASK (0x1 << 3)
#define FPMCTRL_TOKEN_RECOVER_CTL_TOKEN_REMARK_EN_MASK (0x1 << 2)
#define FPMCTRL_TOKEN_RECOVER_CTL_SINGLE_PASS_EN_MASK (0x1 << 1)
#define FPMCTRL_TOKEN_RECOVER_CTL_TOKEN_RECOVER_EN_MASK (0x1 << 0)
	u32 short_aging_timer;
	u32 long_aging_timer;
	u32 cache_recycle_timer;
	u32 expired_token_count_pool1;    /* 0x0140 */
	u32 recovered_token_count_pool1;
	u32 expired_token_count_pool2;
	u32 recovered_token_count_pool2;
	u32 token_recover_start_end_pool1;
	u32 token_recover_start_end_pool2;
	u32 reserved7[42];
} FpmCtrl;

typedef struct FpmPoolMgmt
{
	u32 pool1_alloc_dealloc;        /* 0x0000 */
#define FPM_TOKEN_VALID_MASK (0x1 << 31)
#define FPM_TOKEN_POOL_SHIFT (29)
#define FPM_TOKEN_POOL_MASK (0x1 << FPM_TOKEN_POOL_SHIFT)
#define FPM_TOKEN_INDEX_SHIFT (12)
#define FPM_TOKEN_INDEX_WIDTH (17)
#define FPM_TOKEN_INDEX_MASK (0x1ffff << FPM_TOKEN_INDEX_SHIFT)
#define FPM_TOKEN_SIZE_SHIFT (0)
#define FPM_TOKEN_SIZE_MASK (0xfff << FPM_TOKEN_SIZE_SHIFT)
	u32 reserved0;
	u32 pool2_alloc_dealloc;
	u32 reserved1;
	u32 pool3_alloc_dealloc;
	u32 reserved2;
	u32 pool4_alloc_dealloc;
	u32 reserved3;
	u32 spare;                      /* 0x0020 */
	u32 pool_multi;
#define FPM_TOKEN_MULTIINCR_SHIFT (11)
#define FPM_TOKEN_MULTIVAL_MASK (0x7f)
	u32 reserved4[118];
} FpmPoolMgmt;

typedef struct FpmControl
{
	FpmCtrl ctrl;            /* 0x0000 */
	FpmPoolMgmt pool;        /* 0x0200 */
	FpmPoolMgmt pool0;       /* 0x0400 */
	FpmPoolMgmt pool1;       /* 0x0600 */
} FpmControl;

/* Device struct */
struct fpmdev {
	struct platform_device *pdev;

	bool init;				/* init HW or not? */

	phys_addr_t reg_pbase;			/* HW reg phys base addr */
	u32 *reg_vbase;				/* HW reg virt base addr */
	int irq;				/* HW IRQ # */
	phys_addr_t pool_pbase[FPM_POOL_NUM];	/* pool phys base addr */
	u32 *pool_vbase[FPM_POOL_NUM];		/* pool virt base addr */
	u32 pool_size[FPM_POOL_NUM];		/* pool size in bytes */
	u32 pool_alloc_weight[FPM_POOL_NUM];	/* HW pool selection algorithm weight */
	u32 pool_free_weight[FPM_POOL_NUM];	/* HW pool selection algorithm weight */

	/* map from requested buf size to alloc/free reg to use */
	u32 *buf_size_to_alloc_reg_map[8];
	u32 *buf_size_to_alloc_reg_map_pool[FPM_POOL_NUM][8];
	u32 chunk_size;			/* minimum allocation size */
#if defined(FPM_TOKEN_CHUNKS)
	/* tok_idx_to_chunks_map is used to map buffers to tokens */
	u8  tok_idx_to_chunks_map[FPM_MAX_TOKEN_INDEX];
#if !FPM_FORCE_NO_TOKEN_CHUNKS_BITS
	u32 buf_sizes[4];		/* buf sizes for each alloc/free reg */
#endif
#endif

	struct timer_list isr_timer;
	/* FIXME!! Wen! check to see the following workarond is needed for 4908? */
#if 0
	/* tokens preallocated to work around FPM HW bug in chips < 3390B0 */
	u32 prealloc_tok[2][4];
#endif

	bool track_tokens;			/* track token ops */
	bool track_on_err;			/* continue tracking on error */
	s8 *tok_ref_count;			/* token ref count array */
	struct fpm_tok_op *tok_hist_start;	/* token history buffer start ptr */
	struct fpm_tok_op *tok_hist_end;	/* token history buffer end ptr */
	struct fpm_tok_op *tok_hist_head;	/* token history list head ptr */
	struct fpm_tok_op *tok_hist_tail;	/* token history list tail ptr */
	spinlock_t tok_hist_lock;		/* token history list lock */

#if STOP_UBUS_CAPTURE
	phys_addr_t cap_pbase[2];		/* UBUS capture reg phys base addr */
	u32 *cap_vbase[2];			/* UBUS capture reg virt base addr */
	u32 cap_size[2];			/* UBUS capture reg addr space size */
#endif
};

extern struct fpmdev *fpm;

#ifdef CONFIG_BCM_FPM_TOKEN_CHECKING
static inline void fpm_check_token(u32 token)
{
	int status = 0;

	if (unlikely(!(token & FPM_TOKEN_VALID_MASK))) {
		pr_err("Token 0x%08x does not have valid bit set.\n", token);
		status = -EINVAL;
	}
	if (unlikely(!fpm->pool_pbase[FPM_TOKEN_POOL(token)])) {
		pr_err("Token 0x%08x from disabled pool.\n", token);
		status = -EINVAL;
	}
	if (unlikely(FPM_TOKEN_INDEX(token) * fpm->chunk_size >
	    fpm->pool_size[FPM_TOKEN_POOL(token)])) {
		pr_err("Token 0x%08x index out of range.\n", token);
		pr_err("chunk_size: %d, pool: %d, pool_size: %d\n", fpm->chunk_size, FPM_TOKEN_POOL(token), fpm->pool_size[FPM_TOKEN_POOL(token)]);
		status = -EINVAL;
	}
	if (unlikely(FPM_TOKEN_SIZE(token) > fpm->chunk_size * 8)) {
		pr_err("Token 0x%08x size greater than max allowed.\n", token);
		status = -EINVAL;
	}
	if (status)
		dump_stack();
}
#else
#define fpm_check_token(token)
#endif

static inline u32 __fpm_alloc_token(int size)
{
#ifdef CONFIG_BCM_FPM_TOKEN_HIST
	struct fpmdev *fdev = fpm;
	u32 tok_idx;
#endif
	u32 token;
	int idx = (size - 1) / (fpm->chunk_size);

	if (size == 0)
		idx = 0;
	if (unlikely(idx > 7)) {
		pr_err("%s Invalid size %d is too big.\n",
		       __func__, size);
		dump_stack();
		return 0;
	}

	token = fpm_reg_read(fpm->buf_size_to_alloc_reg_map[idx]);
	if (token & FPM_TOKEN_VALID_MASK) {
#ifdef CONFIG_BCM_FPM_TOKEN_HIST
		if (fdev->track_tokens) {
			tok_idx = FPM_TOKEN_INDEX(token);
			tok_idx |= FPM_TOKEN_POOL(token) << FPM_TOKEN_INDEX_WIDTH;
#ifdef CONFIG_BCM_FPM_TOKEN_HIST_CHECKING
			if (fdev->tok_ref_count[tok_idx]) {
				pr_err("Allocation of token (0x%08x) ", token);
				pr_cont("with non-zero ref count (%d).\n",
					fdev->tok_ref_count[tok_idx]);
				fdev->track_tokens = fdev->track_on_err;
			}
#endif
			fdev->tok_ref_count[tok_idx]++;
		}
#endif
		return token;
	}

	return 0;
}

static inline u32 __fpm_alloc_token_pool(int pool, int size)
{
#ifdef CONFIG_BCM_FPM_TOKEN_HIST
	struct fpmdev *fdev = fpm;
	u32 tok_idx;
#endif
	u32 token;
	int idx = (size - 1) / (fpm->chunk_size);

	if (size == 0)
		idx = 0;
	if (unlikely(idx > 7)) {
		pr_err("%s Invalid size %d is too big.\n",
		       __func__, size);
		dump_stack();
		return 0;
	}
	if (pool >= FPM_POOL_NUM) {
		pr_err("%s invalid pool ID %d.\n",
			__func__, pool);
		dump_stack();
		return 0;
	}

	token = fpm_reg_read(fpm->buf_size_to_alloc_reg_map_pool[pool][idx]);
	if (token & FPM_TOKEN_VALID_MASK) {
#ifdef CONFIG_BCM_FPM_TOKEN_HIST
		if (fdev->track_tokens) {
			tok_idx = FPM_TOKEN_INDEX(token);
			tok_idx |= FPM_TOKEN_POOL(token) << FPM_TOKEN_INDEX_WIDTH;
#ifdef CONFIG_BCM_FPM_TOKEN_HIST_CHECKING
			if (fdev->tok_ref_count[tok_idx]) {
				pr_err("Allocation of token (0x%08x) ", token);
				pr_cont("with non-zero ref count (%d).\n",
					fdev->tok_ref_count[tok_idx]);
				fdev->track_tokens = fdev->track_on_err;
			}
#endif
			fdev->tok_ref_count[tok_idx]++;
		}
#endif
		return token;
	}

	return 0;
}

static inline void __fpm_free_token(u32 token)
{
#ifdef CONFIG_BCM_FPM_TOKEN_HIST
	struct fpmdev *fdev = fpm;
	u32 tok_idx;
#endif

	fpm_reg_write(FPM_DEALLOC, token);
#ifdef CONFIG_BCM_FPM_TOKEN_HIST
	if (fdev->track_tokens) {
		tok_idx = FPM_TOKEN_INDEX(token);
		tok_idx |= FPM_TOKEN_POOL(token) << FPM_TOKEN_INDEX_WIDTH;
#ifdef CONFIG_BCM_FPM_TOKEN_HIST_CHECKING
		if (fdev->tok_ref_count[tok_idx] <= 0) {
			pr_err("Free of token (0x%08x) with zero ", token);
			pr_cont("or negative ref count (%d).\n",
				fdev->tok_ref_count[tok_idx]);
			fdev->track_tokens = fdev->track_on_err;
		}
#endif
		fdev->tok_ref_count[tok_idx]--;
	}
#endif
}

static inline u8 *__fpm_token_to_buffer(u32 token)
{
	u8 *buf;

	buf = (u8 *)((uintptr_t)fpm->pool_vbase[FPM_TOKEN_POOL(token)] +
		FPM_TOKEN_INDEX(token) * fpm->chunk_size);
#if defined(FPM_TOKEN_CHUNKS)
	/* need this for buffer to token conversion */
	fpm->tok_idx_to_chunks_map[FPM_TOKEN_INDEX(token)] =
		FPM_TOKEN_CHUNKS(token);
#endif
	return buf;
}

static inline int fpm_buffer_to_pool(u8 *buf)
{
	if ((uintptr_t)buf >= (uintptr_t)fpm->pool_vbase[0] &&
	    (uintptr_t)buf < (uintptr_t)fpm->pool_vbase[0] + fpm->pool_size[0])
		return 0;
#ifdef FPM_TOKEN_POOL_MASK
#if FPM_POOL_NUM > 1
	else if ((uintptr_t)buf >= (uintptr_t)fpm->pool_vbase[1] &&
		 (uintptr_t)buf < (uintptr_t)fpm->pool_vbase[1] + fpm->pool_size[1])
		return 1;
#endif
#endif
	else
		return -1;
}

static inline u32 __fpm_buffer_to_token(u8 *buf, u32 size)
{
	/*
	 * This will calc the index of the chunk_size chunk <= and nearest
	 * to buf. If buf is not within the 1st chunk of the original group
	 * of chunks allocated then the index will not be correct.
	 */
	u32 pool	= fpm_buffer_to_pool(buf);
	u32 token_idx	= ((uintptr_t)buf - (uintptr_t)fpm->pool_vbase[pool]) / fpm->chunk_size;
	u32 token	= FPM_TOKEN_VALID_MASK |
			  (token_idx << FPM_TOKEN_INDEX_SHIFT) |
			  (size & FPM_TOKEN_SIZE_MASK);
#if defined(FPM_TOKEN_POOL_MASK)
	token		|= (pool & FPM_TOKEN_POOL_MASK) << FPM_TOKEN_POOL_SHIFT;
#endif
#if defined(FPM_TOKEN_CHUNKS)
	u32 chunks	= fpm->tok_idx_to_chunks_map[token_idx];
	token		|= (chunks << FPM_TOKEN_CHUNKS_SHIFT);
#endif
	return token;
}

#if defined(FPM_TOKEN_CHUNKS) && !FPM_FORCE_NO_TOKEN_CHUNKS_BITS
/* Without pool ID bits we have no way to get size of entire token buffer. */
static inline u32 fpm_get_buffer_size(u32 token)
{
	return fpm->buf_sizes[FPM_TOKEN_CHUNKS(token)];
}
#endif

static inline u32 __fpm_get_token_size(u32 token)
{
	return FPM_TOKEN_SIZE(token);
}

static inline void fpm_get_sync_start_size(u32 token, u32 head, u32 tail,
					   u32 flags, u8 **start, u32 *size)
					   
{
	*start = (u8 *)__fpm_token_to_buffer(token) + head;
#if defined(FPM_TOKEN_CHUNKS) && !FPM_FORCE_NO_TOKEN_CHUNKS_BITS
	/*
	 * Without pool ID bits we have no way to get size of entire token
	 * buffer, and thus no way to sync the tail.
	 */
	*size = fpm_get_buffer_size(token) - head - tail;
#else
	*size = __fpm_get_token_size(token);
	if (!*size) {
		pr_err("%s: sync with 0 data bytes (head: %d, tail: %d)\n",
		       __func__, head, tail);
		dump_stack();
	}
#endif
	if (flags & FPM_SYNC_HEAD) {
		*start -= head;
		*size += head;
	}
	if (flags & FPM_SYNC_TAIL) {
		*size += tail;
	}
}

#ifdef CONFIG_BCM_FPM_TOKEN_HIST
static inline void fpm_track_token_op(void *called, u32 token, u32 op_data)
{
	struct fpmdev *fdev = fpm;
	struct fpm_tok_op *op = fdev->tok_hist_head;
	unsigned long flags;
	u32 tok_idx;
	u32 len;

	pr_debug("-->\n");

	if (!fdev->track_tokens)
		return;

	op->token = token;
	op->called = called;
	op->caller = __builtin_return_address(0);
	op->ts = jiffies;
	op->op_data = op_data;
	tok_idx = FPM_TOKEN_INDEX(token);
	tok_idx |= FPM_TOKEN_POOL(token) << FPM_TOKEN_INDEX_WIDTH;
	op->ref_count = fdev->tok_ref_count[tok_idx];
	memset(op->buf, 0, FPM_TOK_OP_BUF_SIZE);
	len = min((u32)FPM_TOKEN_SIZE(token), (u32)FPM_TOK_OP_BUF_SIZE);
	memcpy(op->buf, __fpm_token_to_buffer(token) +
	       FPM_NET_BUF_HEAD_PAD, len);

	spin_lock_irqsave(&fdev->tok_hist_lock, flags);
	fdev->tok_hist_head++;
	if (fdev->tok_hist_head >= fdev->tok_hist_end)
		fdev->tok_hist_head = fdev->tok_hist_start;
	if (fdev->tok_hist_head == fdev->tok_hist_tail)
		fdev->tok_hist_tail++;
	if (fdev->tok_hist_tail >= fdev->tok_hist_end)
		fdev->tok_hist_tail = fdev->tok_hist_start;
	spin_unlock_irqrestore(&fdev->tok_hist_lock, flags);

	pr_debug("<--\n");
}
#else
#define fpm_track_token_op(called, token, op_data)
#endif

#endif
