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

#ifndef __VSF_THREAD_H__
#define __VSF_THREAD_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED && VSF_USE_KERNEL == ENABLED
#include <setjmp.h>
#include "../vsf_eda.h"


/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__VSF_THREAD_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef __VSF_THREAD_CLASS_IMPLEMENT
#elif   defined(__VSF_THREAD_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef __VSF_THREAD_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#ifndef VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE
#   define VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE	    1
#endif
#ifndef VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE
#   define VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE    0
#endif

#define __VSF_THREAD_STACK_SAFE_SIZE(__STACK)                                   \
            (   (   (   ((__STACK) + VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE - 1) \
                    /   VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE)                  \
                * VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE)                        \
            + VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE)


/*============================ MACROFIED FUNCTIONS ===========================*/

#define __declare_vsf_thread(__NAME)                                            \
            typedef struct __NAME __NAME;                                       \
            typedef struct thread_cb_##__NAME##_t thread_cb_##__NAME##_t;
#define declare_vsf_thread(__NAME)      __declare_vsf_thread(__NAME)
#define declare_vsf_thread_ex(__NAME)   __declare_vsf_thread(__NAME)

#define dcl_vsf_thread(__NAME)          declare_vsf_thread(__NAME)
#define dcl_vsf_thread_ex(__NAME)       declare_vsf_thread(__NAME)

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __def_vsf_thread(__NAME, __STACK, ...)                               \
            struct thread_cb_##__NAME##_t {                                     \
                implement(vsf_thread_cb_t)                                      \
                __VA_ARGS__                                                     \
                uint32_t canary;                                                \
                uint64_t stack_arr[(__VSF_THREAD_STACK_SAFE_SIZE(__STACK)+7)/8];\
            };                                                                  \
            struct __NAME {                                                     \
                implement(vsf_thread_t)                                         \
                implement_ex(thread_cb_##__NAME##_t, param)                     \
            }ALIGN(8);                                                          \
            extern void vsf_thread_##__NAME##_start(struct __NAME *task,        \
                                                    vsf_prio_t priority);       \
            extern void vsf_thread_##__NAME##_entry(                            \
                            struct thread_cb_##__NAME##_t *ptTthis);  
                                                    
#   define __implement_vsf_thread(__NAME)                                       \
            void vsf_thread_##__NAME##_entry(                                   \
                        struct thread_cb_##__NAME##_t *ptTthis);                \
            void vsf_thread_##__NAME##_start( struct __NAME *task,              \
                                                vsf_prio_t priority)            \
            {                                                                   \
                VSF_KERNEL_ASSERT(NULL != task);                                \
                thread_cb_##__NAME##_t *this_ptr =  &(task->param);                \
                this_ptr->use_as__vsf_thread_cb_t.entry = (vsf_thread_entry_t *)   \
                                    &vsf_thread_##__NAME##_entry;               \
                this_ptr->use_as__vsf_thread_cb_t.stack = task->param.stack_arr;   \
                this_ptr->use_as__vsf_thread_cb_t.stack_size =                     \
                    sizeof(task->param.stack_arr);                              \
                this_ptr->canary = 0xDEADBEEF;                                     \
                vk_thread_start(   &(task->use_as__vsf_thread_t),              \
                                    &(task->param.use_as__vsf_thread_cb_t),     \
                                    priority);                                  \
            }                                                                   \
            void vsf_thread_##__NAME##_entry(                                   \
                        struct thread_cb_##__NAME##_t *ptThis)
                        
#   define __vsf_eda_call_thread_prepare__(__NAME, __THREAD_CB)                 \
            do {                                                                \
                thread_cb_##__NAME##_t *this_ptr = (__THREAD_CB);                  \
                const vsf_thread_prepare_cfg_t cfg = {                          \
                    .entry = (vsf_thread_entry_t *)                             \
                                    &vsf_thread_##__NAME##_entry,               \
                    .stack = (__THREAD_CB)->stack_arr,                          \
                    .stack_size = sizeof((__THREAD_CB)->stack_arr),             \
                };                                                              \
                vk_eda_call_thread_prepare(&(this_ptr->use_as__vsf_thread_cb_t),\
                                              (vsf_thread_prepare_cfg_t *)&cfg);\
            } while(0)

#   define vsf_eda_call_thread_prepare(__NAME, __THREAD_CB)                     \
                __vsf_eda_call_thread_prepare__(__NAME, __THREAD_CB)

#   define vsf_eda_call_thread(__THREAD_CB)                                     \
                vk_eda_call_thread(&((__THREAD_CB)->use_as__vsf_thread_cb_t))
     

#   define __def_vsf_thread_ex(__NAME, ...)                                     \
            struct thread_cb_##__NAME##_t {                                     \
                implement(vsf_thread_cb_t)                                      \
                __VA_ARGS__                                                     \
            };                                                                  \
            struct __NAME {                                                     \
                implement(vsf_thread_t)                                         \
                implement_ex(thread_cb_##__NAME##_t, param)                     \
            };                                                                  \
            extern void vsf_thread_##__NAME##_start( struct __NAME *task,       \
                                                vsf_prio_t priority,            \
                                                void *stack,                    \
                                                uint_fast32_t size);            \
            extern void vsf_thread_##__NAME##_entry(                            \
                            struct thread_cb_##__NAME##_t *ptTthis);            


#   define __implement_vsf_thread_ex(__NAME)                                    \
            void vsf_thread_##__NAME##_entry(                                   \
                        struct thread_cb_##__NAME##_t *ptTthis);                \
            void vsf_thread_##__NAME##_start(   struct __NAME *task,            \
                                                vsf_prio_t priority,            \
                                                void *stack,                    \
                                                uint_fast32_t size)             \
            {                                                                   \
                VSF_KERNEL_ASSERT(NULL != task && 0 != size && NULL != stack);  \
                thread_cb_##__NAME##_t *this_ptr =  &(task->param);                \
                this_ptr->use_as__vsf_thread_cb_t.entry = (vsf_thread_entry_t *)   \
                                    &vsf_thread_##__NAME##_entry;               \
                this_ptr->use_as__vsf_thread_cb_t.stack = stack;                   \
                this_ptr->use_as__vsf_thread_cb_t.stack_size = size;               \
                vk_thread_start(   &(task->use_as__vsf_thread_t),              \
                                    &(task->param.use_as__vsf_thread_cb_t),     \
                                    priority);                                  \
            }                                                                   \
            void vsf_thread_##__NAME##_entry(                                   \
                        struct thread_cb_##__NAME##_t *ptThis)

#   define __vsf_eda_call_thread_prepare_ex__(  __NAME,                         \
                                                __THREAD_CB,                    \
                                                __STACK,                        \
                                                __SIZE)                         \
            do {                                                                \
                VSF_KERNEL_ASSERT((NULL != (__STACK)) && (0 != (__SIZE)))       \
                thread_cb_##__NAME##_t *this_ptr = (__THREAD_CB);                  \
                const vsf_thread_prepare_cfg_t cfg = {                          \
                    .entry = (vsf_thread_entry_t *)                             \
                                    &vsf_thread_##__NAME##_entry,               \
                    .stack = (__STACK),                                         \
                    .stack_size = (__SIZE),                                     \
                };                                                              \
                vk_eda_call_thread_prepare(&(this_ptr->use_as__vsf_thread_cb_t),\
                                              (vsf_thread_prepare_cfg_t *)&cfg);\
            } while(0)


#   define vsf_eda_call_thread_prepare_ex(  __NAME,                             \
                                            __THREAD_CB,                        \
                                            __STACK,                            \
                                            __SIZE)                             \
                __vsf_eda_call_thread_prepare_ex__( __NAME,                     \
                                                    (__THREAD_CB),              \
                                                    (__STACK),                  \
                                                    (__SIZE))

#   define vsf_eda_call_thread_ex(__THREAD_CB)                                  \
                vk_eda_call_thread(&((__THREAD_CB)->use_as__vsf_thread_cb_t))

#else
#   define __def_vsf_thread(__NAME, __STACK, ...)                               \
            struct thread_cb_##__NAME##_t {                                     \
                implement(vsf_thread_t)                                         \
                __VA_ARGS__                                                     \
            };                                                                  \
            struct __NAME {                                                     \
                uint64_t stack_arr[(__VSF_THREAD_STACK_SAFE_SIZE(__STACK)+7)/8];\
                implement_ex(thread_cb_##__NAME##_t, param);                    \
            }ALIGN(8);                                                          \
            extern void vsf_thread_##__NAME##_start(struct __NAME *task,        \
                                                    vsf_prio_t priority);       \
            extern void vsf_thread_##__NAME##_entry(                            \
                        struct thread_cb_##__NAME##_t *ptTthis); 

#   define __implement_vsf_thread(__NAME)                                       \
            void vsf_thread_##__NAME##_entry(                                   \
                        struct thread_cb_##__NAME##_t *ptTthis);                \
            void vsf_thread_##__NAME##_start( struct __NAME *task,              \
                                                vsf_prio_t priority)            \
            {                                                                   \
                VSF_KERNEL_ASSERT(NULL != task);                                \
                vsf_thread_t *this_ptr =                                           \
                    &(task->param.use_as__vsf_thread_t);                        \
                this_ptr->entry = (vsf_thread_entry_t *)                           \
                                    &vsf_thread_##__NAME##_entry;               \
                this_ptr->stack = task->stack_arr;                                 \
                this_ptr->stack_size = sizeof(task->stack_arr);                    \
                vk_thread_start(this_ptr, priority);                              \
            }                                                                   \
            void vsf_thread_##__NAME##_entry(                                   \
                        struct thread_cb_##__NAME##_t *ptThis)

#   define __def_vsf_thread_ex(__NAME, ...)                                     \
            struct thread_cb_##__NAME##_t {                                     \
                implement(vsf_thread_t)                                         \
                __VA_ARGS__                                                     \
            };                                                                  \
            struct __NAME {                                                     \
                implement_ex(thread_cb_##__NAME##_t, param);                    \
            }ALIGN(8);                                                          \
            extern void vsf_thread_##__NAME##_start(struct __NAME *task,        \
                                                    vsf_prio_t priority);       \
            extern void vsf_thread_##__NAME##_entry(                            \
                        struct thread_cb_##__NAME##_t *ptThis);                 
                                                    

#   define __implement_vsf_thread_ex(__NAME)                                    \
            void vsf_thread_##__NAME##_entry(                                   \
                        struct thread_cb_##__NAME##_t *ptThis);                 \
            void vsf_thread_##__NAME##_start(   struct __NAME *task,            \
                                                vsf_prio_t priority,            \
                                                void *stack,                    \
                                                uint_fast32_t size)             \
            {                                                                   \
                VSF_KERNEL_ASSERT(NULL != task && 0 != size && NULL != stack);  \
                vsf_thread_t *this_ptr =                                           \
                    &(task->param.use_as__vsf_thread_t);                        \
                this_ptr->entry = (vsf_thread_entry_t *)                           \
                                    &vsf_thread_##__NAME##_entry;               \
                this_ptr->stack = stack;                                           \
                this_ptr->stack_size = size;                                       \
                vk_thread_start(this_ptr, priority);                              \
            }                                                                   \
            void vsf_thread_##__NAME##_entry(                                   \
                        struct thread_cb_##__NAME##_t *ptThis)

#endif


#define def_vsf_thread(__NAME, __STACK, ...)                                    \
            __def_vsf_thread(__NAME, __STACK, __VA_ARGS__)
            
#define define_vsf_thread(__NAME, __STACK, ...)                                 \
            def_vsf_thread(__NAME, __STACK, __VA_ARGS__)

#define def_vsf_thread_ex(__NAME, ...)                                          \
            __def_vsf_thread_ex(__NAME, __VA_ARGS__)

#define define_vsf_thread_ex(__NAME, ...)                                       \
            def_vsf_thread_ex(__NAME, __VA_ARGS__)
            
#define end_def_vsf_thread(...)
#define end_define_vsf_thread(...)

#define implement_vsf_thread(__NAME)        __implement_vsf_thread(__NAME)
#define implement_vsf_thread_ex(__NAME)     __implement_vsf_thread_ex(__NAME)

#define imp_vsf_thread(__NAME)              implement_vsf_thread(__NAME)
#define imp_vsf_thread_ex(__NAME)           implement_vsf_thread_ex(__NAME)

#define __init_vsf_thread(__NAME, __TASK, __PRI)                                \
            vsf_thread_##__NAME##_start((__TASK), (__PRI))

#define __init_vsf_thread_ex(__NAME, __TASK, __PRI, __STACK, __SIZE)            \
            vsf_thread_##__NAME##_start((__TASK), (__PRI), (__STACK), (__SIZE))

#define init_vsf_thread(__NAME, __TASK, __PRI)                                  \
            __init_vsf_thread(__NAME, (__TASK), (__PRI))

#define init_vsf_thread_ex(__NAME, __TASK, __PRI, __STACK, __SIZE)              \
            __init_vsf_thread_ex(__NAME, (__TASK), (__PRI), (__STACK), (__SIZE))

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __vsf_thread(__NAME)      thread_cb_##__NAME##_t
#   define vsf_thread(__NAME)        __vsf_thread(__NAME) 

#endif

#define vsf_thread_wfm      vsf_thread_wait_for_msg
#define vsf_thread_wfe      vsf_thread_wait_for_evt

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
#   define vsf_thread_wfem     vsf_thread_wait_for_evt_msg
#endif


#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __vsf_thread_call_sub(__NAME, __TARGET, ...)                         \
            vk_thread_call_eda( (uintptr_t)(__NAME),                            \
                                (uintptr_t)(__TARGET),                          \
                                (0, ##__VA_ARGS__),                             \
                                0,                                              \
                                0)


#   define vsf_thread_call_sub(__NAME, __TARGET, ...)                           \
            __vsf_thread_call_sub(__NAME, (__TARGET), (0, ##__VA_ARGS__))


#   define vsf_thread_call_pt(__NAME, __TARGET, ...)                            \
            (__TARGET)->tState = 0;                                             \
            vsf_thread_call_sub(vsf_pt_func(__NAME), (__TARGET), (0, ##__VA_ARGS__))

#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED

#   define vsf_thread_call_fsm(__NAME, __TARGET, ...)                           \
                vk_thread_call_fsm(  (vsf_fsm_entry_t)(__NAME),                 \
                                        (uintptr_t)(__TARGET), (0, ##__VA_ARGS__))
        
#   define vsf_thread_call_task(__NAME, __TARGET, ...)                          \
                vsf_thread_call_fsm(vsf_task_func(__NAME), __TARGET, (0, ##__VA_ARGS__))
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define vsf_thread_delay_ms(__ms)            vsf_thread_delay(vsf_systimer_ms_to_tick(__ms))
#   define vsf_thread_delay_us(__us)            vsf_thread_delay(vsf_systimer_us_to_tick(__us))
#endif

/*============================ TYPES =========================================*/

declare_class(vsf_thread_t)

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
declare_class(vsf_thread_cb_t)

typedef void vsf_thread_entry_t(vsf_thread_cb_t *thread);

def_class( vsf_thread_t, 
    which(
    #   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        implement(vsf_teda_t)
    #   else
        implement(vsf_eda_t)
    #   endif
    )
)
end_def_class(vsf_thread_t)
//! \name thread
//! @{
def_class(vsf_thread_cb_t,

    public_member(
        // you can add public member here
        vsf_thread_entry_t  *entry;
        uint16_t            stack_size;
        uint64_t            *stack;                 //!< stack must be 8byte aligned
    )

    private_member(
        jmp_buf         *pos;
        jmp_buf         *ret;
    )
)
end_def_class(vsf_thread_cb_t)
//! @}

typedef struct {
    vsf_thread_entry_t  *entry; 
    void                *stack;
    uint_fast32_t       stack_size;
    //vsf_prio_t      priority;         //!< TODO do we need this??
}vsf_thread_prepare_cfg_t;


#else


typedef void vsf_thread_entry_t(vsf_thread_t *thread);

//! \name thread
//! @{
def_class(vsf_thread_t,
    which(
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        implement(vsf_teda_t)
#else
        implement(vsf_eda_t)
#endif
    )

    public_member(
        // you can add public member here
        vsf_thread_entry_t  *entry;
        uint16_t            stack_size;
        uint64_t            *stack;                 //!< stack must be 8byte aligned
    ),

    private_member(
        jmp_buf         *pos;
        jmp_buf         *ret;
    )
)
end_def_class(vsf_thread_t)
//! @}
#endif
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
SECTION("text.vsf.kernel.vk_eda_call_thread_prepare")
extern vsf_err_t vk_eda_call_thread_prepare( vsf_thread_cb_t *thread_cb,
                                                vsf_thread_prepare_cfg_t *cfg);

SECTION("text.vsf.kernel.vk_eda_call_thread")
extern vsf_err_t vk_eda_call_thread(vsf_thread_cb_t *thread_cb);

SECTION("text.vsf.kernel.vk_thread_call_fsm")
extern 
fsm_rt_t vk_thread_call_fsm(vsf_fsm_entry_t eda_handler, uintptr_t param, size_t local_size);

SECTION("text.vsf.kernel.vk_thread_call_eda")
extern 
vsf_err_t vk_thread_call_eda(   uintptr_t eda_handler, 
                                uintptr_t param, 
                                size_t local_size,
                                size_t local_buff_size,
                                uintptr_t local_buff);

SECTION("text.vsf.kernel.vsf_thread_call_thread")
extern vsf_err_t vk_thread_call_thread(  vsf_thread_cb_t *thread_cb,
                                            vsf_thread_prepare_cfg_t *cfg);

#endif


SECTION("text.vsf.kernel.vsf_thread")
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
extern vsf_err_t vk_thread_start(  vsf_thread_t *thread, 
                                    vsf_thread_cb_t *thread_cb, 
                                    vsf_prio_t priority);
#else
extern vsf_err_t vk_thread_start(vsf_thread_t *ptThis, vsf_prio_t tPriority);
#endif

SECTION("text.vsf.kernel.vsf_thread_exit")
extern void vsf_thread_exit(void);

SECTION("text.vsf.kernel.vsf_thread_get_cur")
extern vsf_thread_t *vsf_thread_get_cur(void);

SECTION("text.vsf.kernel.vsf_thread_ret")
extern void vsf_thread_ret(void);

SECTION("text.vsf.kernel.vsf_thread_wait")
extern vsf_evt_t vsf_thread_wait(void);

SECTION("text.vsf.kernel.vsf_thread_wfe")
extern void vsf_thread_wait_for_evt(vsf_evt_t evt);

SECTION("text.vsf.kernel.vsf_thread_sendevt")
extern void vsf_thread_sendevt(vsf_thread_t *thread, vsf_evt_t evt);

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
SECTION("text.vsf.kernel.vsf_thread_wait_for_evt_msg")
extern uintptr_t vsf_thread_wait_for_evt_msg(vsf_evt_t evt);
#endif

SECTION("text.vsf.kernel.vsf_thread_wait_for_evt_msg")
extern uintptr_t vsf_thread_wait_for_msg(void);

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
SECTION("text.vsf.kernel.vsf_thread_delay")
extern void vsf_thread_delay(uint_fast32_t tick);
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
SECTION("text.vsf.kernel.vsf_thread_mutex")
extern vsf_sync_reason_t vsf_thread_mutex_enter(vsf_mutex_t *mtx, int_fast32_t timeout);

SECTION("text.vsf.kernel.vsf_thread_mutex")
extern vsf_err_t vsf_thread_mutex_leave(vsf_mutex_t *mtx);

SECTION("text.vsf.kernel.vsf_thread_sem_post")
extern vsf_err_t vsf_thread_sem_post(vsf_sem_t *sem);

SECTION("text.vsf.kernel.__vsf_thread_wait_for_sync")
extern vsf_sync_reason_t vsf_thread_sem_pend(vsf_sem_t *sem, int_fast32_t timeout);

SECTION("text.vsf.kernel.__vsf_thread_wait_for_sync")
extern vsf_sync_reason_t vsf_thread_trig_pend(vsf_trig_t *trig, int_fast32_t timeout);

#   if VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT == ENABLED
SECTION("text.vsf.kernel.vsf_thread_bmpevt_pend")
extern vsf_sync_reason_t vsf_thread_bmpevt_pend(
                    vsf_bmpevt_t *bmpevt,
                    vsf_bmpevt_pender_t *pender,
                    int_fast32_t timeout);
#   endif
#endif

#ifdef __cplusplus
}
#endif

#endif
#endif
