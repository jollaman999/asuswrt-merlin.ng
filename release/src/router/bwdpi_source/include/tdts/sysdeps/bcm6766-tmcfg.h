/*
 * \copyright
 * Copyright 2014 Trend Micro Incorporated
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

/*
 * Automatically generated make config: don't edit
 * Date: Tue Jan 14 15:09:23 2025
 */
#ifndef __TMCFG__AUTOCONF_OUTPUT_H_
#define __TMCFG__AUTOCONF_OUTPUT_H_

#define TMCFG_BRAND "asus"
#define TMCFG_MODEL "bt10"

/*
 * Target device information
 */
#define TMCFG_USER_SPACE 0 // n
#define TMCFG_KERN_SPACE 1 // y
#define TMCFG_ARCH_X86 0 // n
#define TMCFG_ARCH_X86_64 0 // n
#define TMCFG_ARCH_MIPS 0 // n
#define TMCFG_ARCH_ARM 1 // y
#define TMCFG_ARCH_ARM_64 0 // n
#define TMCFG_ARCH_POWERPC 0 // n
#define TMCFG_CPU_BIG_ENDIAN 0 // n
#define TMCFG_CPU_LITTLE_ENDIAN 1 // y
#define TMCFG_KERN_DIR "/opt/ASUS/asuswrt_GPL_BT10_3.0.0.6.102.37576/release/src-rt-5.04behnd.4916/kernel/linux-4.19"
#define TMCFG_KERN_ARCH "arm"
#define TMCFG_CPU_32BITS 1 // y
#define TMCFG_CPU_64BITS 0 // n
#define TMCFG_LOCAL_ATOMIC_OP_LOCK 0 // n

/*
 * Toolchain (TC) configurations
 */

/*
 * Target device toolchain (cross utilities)
 */
#define TMCFG_TC_PFX "/opt/ASUS/toolchains/crosstools-arm-gcc-10.3-linux-4.19-glibc-2.32-binutils-2.36.1/bin/arm-buildroot-linux-gnueabi-"
#define TMCFG_TC_BIT_FIELD_ORDER_LITTLE_ENDIAN 1 // y
#define TMCFG_TC_BIT_FIELD_ORDER_BIG_ENDIAN 0 // n
#define TMCFG_TC_CC "$(TMCFG_TC_PFX)gcc"
#define TMCFG_TC_AR "$(TMCFG_TC_PFX)ar"
#define TMCFG_TC_RANLIB "$(TMCFG_TC_PFX)ranlib"
#define TMCFG_TC_STRIP "$(TMCFG_TC_PFX)strip"
#define TMCFG_TC_OBJDUMP "$(TMCFG_TC_PFX)objdump"

/*
 * (CAUTION) Shared CC flags. Do not change if u are not sure.
 */
#define TMCFG_TC_EXTRA_CFLAGS "-fsigned-char -Wno-incompatible-pointer-types"
#define TMCFG_TC_EXTRA_LDFLAGS ""
#define TMCFG_TC_RUN_STRIP 1 // y

/*
 * Local host system toolchain
 */
#define TMCFG_HOST_TC_PFX ""
#define TMCFG_HOST_TC_CC "$(TMCFG_HOST_TC_PFX)gcc"
#define TMCFG_HOST_TC_STRIP "$(TMCFG_HOST_TC_PFX)strip"
#define TMCFG_HOST_TC_RUN_STRIP 1 // y

/*
 * Debug
 */
#define TMCFG_DBG_VERBOSE_CC_MSG 1 // y
#define TMCFG_DBG_HIT_RATE_TEST 0 // n

/*
 * Engine
 */
#define TMCFG_E_EXTRA_CFLAGS "-I/opt/ASUS/asuswrt_GPL_BT10_3.0.0.6.102.37576/release/src-rt-5.04behnd.4916/kernel/bcmkernel/include -I/opt/ASUS/asuswrt_GPL_BT10_3.0.0.6.102.37576/release/src-rt-5.04behnd.4916/bcmdrivers"
#define TMCFG_E_KMOD_NAME "tdts"
#define TMCFG_E_KMOD_IOCTL_DEV_NAME "idp"
#define TMCFG_E_KMOD_IOCTL_DEV_MAJ 190
#define TMCFG_E_KMOD_IOCTL_DEV_MIN 0
#define TMCFG_E_KMOD_IOCTL_DEV_MAGIC 190

/*
 * ******
 */
#define TMCFG_E_MAJ_VER 2
#define TMCFG_E_MID_VER 0
#define TMCFG_E_MIN_VER 3
#define TMCFG_E_LOCAL_VER "03e1542"

/*
 * Engine core
 */
#define TMCFG_E_CORE_EXTRA_CFLAGS ""
#define TMCFG_E_CORE_USE_KBUILD 1 // y
#define TMCFG_E_CORE_DPI_METHOD_SW 1 // y
#define TMCFG_E_CORE_IPS 1 // y
#define TMCFG_E_CORE_APPID 1 // y
#define TMCFG_E_CORE_DEVID 1 // y
#define TMCFG_E_CORE_DNS_DECODE 1 // y
#define TMCFG_E_CORE_APP_SET_BY_DNS 1 // y
#define TMCFG_E_CORE_SIP_DECODE 1 // y
#define TMCFG_E_CORE_STREAMING_DECODE 1 // y
#define TMCFG_E_CORE_METADATA_EXTRACT 1 // y
#define TMCFG_E_CORE_METADATA_MAX_HANDLER 128
#define TMCFG_E_CORE_TLS_DECODE 1 // y
#define TMCFG_E_CORE_TLS_DECODE_MAX_SESS 1

/*
 * UPnP decoder
 */
#define TMCFG_E_CORE_UPNP_DECODE 0 // n

/*
 * Anomaly detection
 */
#define TMCFG_E_CORE_ADP 0 // n
#define TMCFG_E_CORE_PORT_SCAN_DETECTION 1 // y
#define TMCFG_E_CORE_PORT_SCAN_DETECTION_HDB_NUM 2048
#define TMCFG_E_CORE_PORT_SCAN_DETECTION_HDB_HASH_NUM 32
#define TMCFG_E_CORE_IP_SWEEP_DETECTION 0 // n
#define TMCFG_E_CORE_TCP_CHECKSUM 0 // n
#define TMCFG_E_CORE_RULE_FORMAT_NK 0 // n
#define TMCFG_E_CORE_RULE_FORMAT_FM 1 // y

/*
 * Rule Limits
 */
#define TMCFG_E_CORE_CONFIG_POLICY_NUM 12000
#define TMCFG_E_CORE_CONFIG_CONTENT_NUM 26100
#define TMCFG_E_CORE_CONFIG_TOTAL_CONTENT_LENGTH 290000
#define TMCFG_E_CORE_RULE_FORMAT_V2 0 // n
#define TMCFG_E_CORE_RULE_BINDING 1 // y
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_CHIPSET_PROVIDER 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL 1 // y
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_OPERATOR 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_TELCO 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL_COMPANY_ASUS 1 // y
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL_COMPANY_ELECOM 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL_COMPANY_NEC 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL_COMPANY_UMEDIA 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL_COMPANY_TRENDMICRO 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL_COMPANY_NETGEAR 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL_COMPANY_TPLINK 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL_COMPANY_BUFFALO 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL_COMPANY_BELKIN 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL_COMPANY_LENOVO 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL_COMPANY_UBIQUITI 0 // n
#define TMCFG_E_CORE_RULE_BINDING_CATEGORY_RETAIL_COMPANY_RUCKUS 0 // n
#define TMCFG_E_CORE_IP6 1 // y
#define TMCFG_E_CORE_TCP_STREAM_REASM 0 // n
#define TMCFG_E_CORE_FINE_GRAIN_TCP_LOCK 0 // n
#define TMCFG_E_CORE_CONFIG_TCP_CHAIN_LAYOUT 3
#define TMCFG_E_CORE_CONFIG_TCP_CONN_NUM 8
#define TMCFG_E_CORE_CONFIG_TCP_HASH_SIZE 1
#define TMCFG_E_CORE_CONFIG_TCP_TURBO_MODE_ENABLED 1 // y
#define TMCFG_E_CORE_CONFIG_TCP_PACKET_INSPECTION_DEPTH 20
#define TMCFG_E_CORE_CONFIG_TCP_BYTE_INSPECTION_DEPTH 30000
#define TMCFG_E_CORE_CONFIG_TCP_SG_PACKET_INSPECTION_DEPTH 50
#define TMCFG_E_CORE_CONFIG_UDP_TURBO_MODE_ENABLED 1 // y
#define TMCFG_E_CORE_CONFIG_UDP_PACKET_INSPECTION_DEPTH 7
#define TMCFG_E_CORE_CONFIG_UDP_BYTE_INSPECTION_DEPTH 10000
#define TMCFG_E_CORE_CONFIG_UDP_SG_PACKET_INSPECTION_DEPTH 50
#define TMCFG_E_CORE_UDP_FLOW_TRACKING 1 // y
#define TMCFG_E_CORE_CONFIG_UDP_FLOW_NUM 4
#define TMCFG_E_CORE_CONFIG_UDP_HASH_SIZE 1
#define TMCFG_E_CORE_MULTI_DIR_MATCH 0 // n
#define TMCFG_E_CORE_PORT_BASE_MATCH_OPTM 1 // y
#define TMCFG_E_CORE_ANOMALY_CHECK 0 // n
#define TMCFG_E_CORE_KERN_CONN_TRACK 0 // n
#define TMCFG_E_CORE_THREAD_MAX 64
#define TMCFG_E_CORE_PACKET_MAX 64
#define TMCFG_E_CORE_IP_DEFRAG 0 // n
#define TMCFG_E_CORE_DPI_MODEL_PARAM 0 // n
#define TMCFG_E_CORE_BNFA_SPARSE_MATRIX 1 // y
#define TMCFG_E_CORE_AC_COMP_WSP_MEMORY 3
#define TMCFG_E_CORE_AC_STATE_TYPE_UINT 1 // y
#define TMCFG_E_CORE_AC_STATE_NUM 64
#define TMCFG_E_CORE_DYNAMIC_SIGNATURE_TABLE 1 // y
#define TMCFG_E_CORE_DYNAMIC_MEMORY_ALLOC 0 // n
#define TMCFG_E_CORE_CONNECTION_DYNAMIC_ALLOC 0 // n
#define TMCFG_E_CORE_CONFIG_HTTP_DECODER_NUM 1
#define TMCFG_E_CORE_CONFIG_HTTP_URI_LENGTH 2048
#define TMCFG_E_CORE_CONFIG_HTTP_REPLY_DECODE 0 // n
#define TMCFG_E_CORE_CONFIG_HTTP_PTN_MATCH_OPTM 0 // n
#define TMCFG_E_CORE_CONFIG_SG_DECODER_NUM 1
#define TMCFG_E_CORE_CONFIG_SG_CAND_NUM 1
#define TMCFG_E_CORE_CONFIG_SG_CAND_HASH_SIZE 512
#define TMCFG_E_CORE_CONFIG_MAX_NR_PORT_BITMAP 1024
#define TMCFG_E_CORE_PER_CPU_VARIABLES 0 // n
#define TMCFG_E_CORE_CONFIG_EMBEDDED_PLATFORM 1 // y

/*
 * appid
 */
#define TMCFG_E_CORE_APPID_MAX_NAME_LEN 64
#define TMCFG_E_CORE_APPID_MAX_CAT_NUM 48
#define TMCFG_E_CORE_APPID_MAX_APP_NUM 1024
#define TMCFG_E_CORE_APPID_MAX_BEH_NUM 12

/*
 * DevID
 */
#define TMCFG_E_CORE_DEVID_UA 1 // y
#define TMCFG_E_CORE_DEVID_COLLECT_UN 1 // y

/*
 * Engine shell
 */
#define TMCFG_E_SHELL_EXTRA_CFLAGS "-I/opt/ASUS/asuswrt_GPL_BT10_3.0.0.6.102.37576/release/src-rt-5.04behnd.4916/kernel/bcmkernel/include -I/opt/ASUS/asuswrt_GPL_BT10_3.0.0.6.102.37576/release/src-rt-5.04behnd.4916/bcmdrivers"
#define TMCFG_E_SHELL_ATIMER 1 // y
#define TMCFG_E_SHELL_DEPRECATED_PROCFS 1 // y

/*
 * Accompany applications or modules
 */

/*
 * Kernel
 */
#define TMCFG_APP_K_EXTRA_CFLAGS ""
#define TMCFG_APP_K_TDTS_NFFW 0 // n
#define TMCFG_APP_K_TDTS_NFFW_INTERNAL 0 // n
#define TMCFG_APP_K_TEMPLATE 0 // n

/*
 * Userland
 */

/*
 * Userspace toolchain
 */
#define TMCFG_APP_U_TC_PFX "/opt/ASUS/toolchains/crosstools-arm-gcc-10.3-linux-4.19-glibc-2.32-binutils-2.36.1/bin/arm-buildroot-linux-gnueabi-"
#define TMCFG_APP_U_TC_CC "$(TMCFG_APP_U_TC_PFX)gcc"
#define TMCFG_APP_U_TC_AR "$(TMCFG_APP_U_TC_PFX)ar"
#define TMCFG_APP_U_TC_LD "$(TMCFG_APP_U_TC_PFX)ld"
#define TMCFG_APP_U_TC_RANLIB "$(TMCFG_APP_U_TC_PFX)ranlib"
#define TMCFG_APP_U_TC_STRIP "$(TMCFG_APP_U_TC_PFX)strip"
#define TMCFG_APP_U_TC_OBJDUMP "$(TMCFG_APP_U_TC_PFX)objdump"
#define TMCFG_APP_U_EXTRA_CFLAGS ""
#define TMCFG_APP_U_EXTRA_LDFLAGS ""
#define TMCFG_APP_U_KA_API 1 // y
#define TMCFG_APP_U_TDTS_RULE_AGENT 1 // y
#define TMCFG_APP_U_TDTS_RULE_AGENT_SAMPLE 0 // n
#define TMCFG_APP_U_LIBTDTS_CTRL 0 // n
#define TMCFG_APP_U_TDTS_RULE_AGENT_LC 0 // n
#define TMCFG_APP_U_UDB_SAMPLE 0 // n
#define TMCFG_APP_U_UDB_QOSD_BRCM 0 // n
#define TMCFG_APP_U_TEMPLATE 0 // n

#endif // EOF

