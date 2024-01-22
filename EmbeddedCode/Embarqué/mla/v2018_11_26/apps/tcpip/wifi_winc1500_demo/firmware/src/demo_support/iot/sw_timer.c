/*******************************************************************************
  File Name:
    sw_timer.c

  Summary:
    SW Timer component for the IoT(Internet of things) service.
    
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*==============================================================================
Copyright 2016 Microchip Technology Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "winc1500_api.h"
#include "demo_config.h"

#if defined(USING_SUPPORT_IOT)
#include "sw_timer.h"

#define Assert ssert_sw_timer

/** Tick count of timer. */
static uint32_t sw_timer_tick = 0;

static void ssert_sw_timer(void *status)
{
    if (status == NULL)
    {
        printf("Assert happen!\r\n");
        while (1)
            ;
    }
}
/**
 * \brief TCC callback of SW timer.
 *
 * This function performs to the increasing the tick count.
 *
 * \param[in] module Instance of the TCC.
 */
void sw_timer_tcc_callback(void)
{
    sw_timer_tick++;
}

void sw_timer_get_config_defaults(struct sw_timer_config *const config)
{
    Assert(config);

    config->accuracy = 100;
    config->tcc_dev = 0;
    config->tcc_callback_channel = 0;
}

void sw_timer_init(struct sw_timer_module *const module_inst, struct sw_timer_config *const config)
{
    Assert(module_inst);

    module_inst->accuracy = config->accuracy;
    // ToDo: 
    //tcc_register_callback(tcc_module, sw_timer_tcc_callback, config->tcc_callback_channel + TCC_CALLBACK_CHANNEL_0);
}

void sw_timer_enable(struct sw_timer_module *const module_inst)
{
    Assert(module_inst);
    // ToDo, add code of pic24
}

void sw_timer_disable(struct sw_timer_module *const module_inst)
{
    Assert(module_inst);
    // ToDo, add code of pic24
}

int sw_timer_register_callback(struct sw_timer_module *const module_inst,
        sw_timer_callback_t callback, void *context, uint32_t period)
{
    int index;
    struct sw_timer_handle *handler;

    Assert(module_inst);

    for (index = 0; index < CONF_SW_TIMER_COUNT; index++) 
    {
        if (module_inst->handler[index].used == 0) 
        {
            handler = &module_inst->handler[index];
            handler->callback = callback;
            handler->callback_enable = 0;
            handler->context = context;
            handler->period = period / module_inst->accuracy;
            handler->used = 1;
            return index;
        }
    }
    return -1;
}

void sw_timer_unregister_callback(struct sw_timer_module *const module_inst, int timer_id)
{
    struct sw_timer_handle *handler;

    Assert(module_inst);
    Assert((void *)(timer_id >= 0 && timer_id < CONF_SW_TIMER_COUNT));

    handler = &module_inst->handler[timer_id];

    handler->used = 0;
}

void sw_timer_enable_callback(struct sw_timer_module *const module_inst, int timer_id, uint32_t delay)
{
    struct sw_timer_handle *handler;

    Assert(module_inst);
    Assert((void *)(timer_id >= 0 && timer_id < CONF_SW_TIMER_COUNT));

    handler = &module_inst->handler[timer_id];

    handler->callback_enable = 1;
    handler->expire_time = sw_timer_tick + (delay / module_inst->accuracy);
}

void sw_timer_disable_callback(struct sw_timer_module *const module_inst, int timer_id)
{
    struct sw_timer_handle *handler;

    Assert(module_inst);
    Assert((void *)(timer_id >= 0 && timer_id < CONF_SW_TIMER_COUNT));

    handler = &module_inst->handler[timer_id];

    handler->callback_enable = 0;
}

void sw_timer_task(struct sw_timer_module *const module_inst)
{
    int index;
    struct sw_timer_handle *handler;

    Assert(module_inst);

    for (index = 0; index < CONF_SW_TIMER_COUNT; index++) 
    {
        if (module_inst->handler[index].used && module_inst->handler[index].callback_enable) 
        {
            handler = &module_inst->handler[index];
            if ((int)(handler->expire_time - sw_timer_tick) < 0 && handler->busy == 0) 
            {
                /* Enter critical section. */
                handler->busy = 1;
                /* Timer was expired. */
                if (handler->period > 0) 
                {
                    handler->expire_time = sw_timer_tick + handler->period;
                } 
                else 
                {
                    /* One shot. */
                    handler->callback_enable = 0;
                }
                /* Call callback function. */
                handler->callback(module_inst, index, handler->context, handler->period);
                /* Leave critical section. */
                handler->busy = 0;
            }
        }
    }
}

#endif