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

#if defined(USING_UDP_SERVER) 

void m2m_ota_handle_events(t_m2mOtaEventType eventCode, t_m2mOtaEventData *p_eventData)
{
    switch (eventCode)
    {
        case M2M_OTA_STATUS_EVENT:
            // data in p_eventData->otaUpdateStatus
            break;
    }
}

#endif // USING_UDP_SERVER
