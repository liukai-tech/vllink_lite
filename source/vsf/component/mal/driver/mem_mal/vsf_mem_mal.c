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

/*============================ INCLUDES ======================================*/

#include "../../vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED && VSF_USE_MEM_MAL == ENABLED

#define VSF_MAL_INHERIT
#define VSF_MEM_MAL_IMPLEMENT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static uint_fast32_t __vk_mem_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
static bool __vk_mem_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
dcl_vsf_peda_methods(static, __vk_mem_mal_init)
dcl_vsf_peda_methods(static, __vk_mem_mal_fini)
dcl_vsf_peda_methods(static, __vk_mem_mal_read)
dcl_vsf_peda_methods(static, __vk_mem_mal_write)

/*============================ GLOBAL VARIABLES ==============================*/

const vk_mal_drv_t vk_mem_mal_drv = {
    .blksz          = __vk_mem_mal_blksz,
    .buffer         = __vk_mem_mal_buffer,
    .init           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_mem_mal_init),
    .fini           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_mem_mal_fini),
    .read           = (vsf_peda_evthandler_t)vsf_peda_func(__vk_mem_mal_read),
    .write          = (vsf_peda_evthandler_t)vsf_peda_func(__vk_mem_mal_write),
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t __vk_mem_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op)
{
    vk_mem_mal_t *pthis = (vk_mem_mal_t *)mal;
    return pthis->blksz;
}

static bool __vk_mem_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem)
{
    vk_mem_mal_t *pthis = (vk_mem_mal_t *)mal;
    mem->pchBuffer = &pthis->mem.pchBuffer[addr];
    mem->nSize = size;
    return true;
}

__vsf_component_peda_ifs_entry(__vk_mem_mal_init, vk_mal_init)
{
    vsf_peda_begin();
    vk_mem_mal_t *pthis = (vk_mem_mal_t *)&vsf_this;
    VSF_MAL_ASSERT(pthis != NULL);
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_mem_mal_fini, vk_mal_fini)
{
    vsf_peda_begin();
    vk_mem_mal_t *pthis = (vk_mem_mal_t *)&vsf_this;
    VSF_MAL_ASSERT(pthis != NULL);
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_mem_mal_read, vk_mal_read)
{
    vsf_peda_begin();
    vk_mem_mal_t *pthis = (vk_mem_mal_t *)&vsf_this;
    uint_fast64_t addr;
    uint_fast32_t size;

    VSF_MAL_ASSERT(pthis != NULL);
    addr = vsf_local.addr;
    size = vsf_local.size;
    VSF_MAL_ASSERT((size > 0) && ((addr + size) <= pthis->mem.nSize));

    if (vsf_local.buff != &pthis->mem.pchBuffer[addr]) {
        memcpy(vsf_local.buff, &pthis->mem.pchBuffer[addr], size);
    }
    vsf_eda_return(size);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_mem_mal_write, vk_mal_write)
{
    vsf_peda_begin();
    vk_mem_mal_t *pthis = (vk_mem_mal_t *)&vsf_this;
    uint_fast64_t addr;
    uint_fast32_t size;

    VSF_MAL_ASSERT(pthis != NULL);
    addr = vsf_local.addr;
    size = vsf_local.size;
    VSF_MAL_ASSERT((size > 0) && ((addr + size) <= pthis->mem.nSize));

    if (vsf_local.buff != &pthis->mem.pchBuffer[addr]) {
        memcpy(&pthis->mem.pchBuffer[addr], vsf_local.buff, size);
    }
    vsf_eda_return(size);
    vsf_peda_end();
}

#endif
