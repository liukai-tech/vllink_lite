/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __CORTEX_A_GENERIC_H__
#define __CORTEX_A_GENERIC_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

#define __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#include "hal/driver/driver.h"
#undef  __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/

#define __LITTLE_ENDIAN                 1
#define __BYTE_ORDER                    __LITTLE_ENDIAN

// TODO:
#if __ARM_ARCH != 5
#   error invalid __ARM_ARCH
#endif

#ifndef VSF_ARCH_PRI_NUM
#   define VSF_ARCH_PRI_NUM             32
#endif
#if VSF_ARCH_PRI_NUM > 32
#   error invalid VSF_ARCH_PRI_NUM
#endif

#ifndef VSF_ARCH_MMU
#   define VSF_ARCH_MMU                 ENABLED
#endif

// software interrupt provided by arch
#define VSF_ARCH_SWI_NUM                1

#if VSF_ARCH_PRI_NUM > 0
#   define __VSF_ARCH_PRI(__N, __BIT)                                           \
            VSF_ARCH_PRIO_##__N = (__N),                                        \
            vsf_arch_prio_##__N = (__N),
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define isb()           __asm__ __volatile__ ("" : : : "memory")
#define dsb()           __asm__ __volatile__ ("mcr p15, 0, %0, c7, c10,  4" : : "r" (0) : "memory")
#define dmb()           __asm__ __volatile__ ("" : : : "memory")

/*============================ TYPES =========================================*/

#if VSF_ARCH_PRI_NUM > 0
// arm9 arch_priority is software emulated, simple start from 0 to VSF_ARCH_PRI_NUM - 1
enum vsf_arch_prio_t {
    VSF_ARCH_PRIO_IVALID            = -1,
    vsf_arch_prio_ivalid            = -1,

    REPEAT_MACRO(VSF_ARCH_PRI_NUM, __VSF_ARCH_PRI,VSF_ARCH_PRI_BIT)
    vsf_arch_prio_highest           = VSF_ARCH_PRI_NUM - 1,
};
typedef enum vsf_arch_prio_t vsf_arch_prio_t;
#else
typedef int vsf_arch_prio_t;
#endif

typedef unsigned long long          virtual_addr_t;
typedef unsigned long long          virtual_size_t;
typedef unsigned long long          physical_addr_t;
typedef unsigned long long          physical_size_t;

enum {
    MMU_MAP_TYPE_NCNB               = 0x0,
    MMU_MAP_TYPE_NCB                = 0x1,
    MMU_MAP_TYPE_CNB                = 0x2,
    MMU_MAP_TYPE_CB                 = 0x3,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static ALWAYS_INLINE void vsf_arch_set_stack(uint32_t stack)
{
    __asm__ __volatile__ ("mov SP, %0" ::"r"(stack):);
}

#if VSF_ARCH_MMU == ENABLED
extern void vsf_arch_mmu_map(uint32_t *ttb, virtual_addr_t virt, physical_addr_t phys, physical_size_t size, int type);
extern void vsf_arch_mmu_enable(uint32_t *ttb);
extern void vsf_arch_mmu_refresh(void);
#endif

extern void vsf_cache_sync(void * addr, uint_fast32_t size, bool is_from_device);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */

