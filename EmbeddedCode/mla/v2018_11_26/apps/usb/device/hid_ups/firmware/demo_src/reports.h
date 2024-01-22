/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

#ifndef REPORTS_H
#define REPORTS_H

typedef enum HIDReportID
{
    //Power Summary Section
    //ID requests
    POWER_SUMMARY_POWER_SUMMARY_ID          = 0x01,

    //String requests
    POWER_SUMMARY_I_NAME                    = 0x10,
    POWER_SUMMARY_I_PRODUCT                 = 0x11,
    POWER_SUMMARY_I_SERIAL_NUMBER           = 0x12,
    POWER_SUMMARY_I_DEVICE_CHEMISTRY        = 0x13,
    POWER_SUMMARY_I_OEM_INFORMATION         = 0x14,
    POWER_SUMMARY_I_MANUFACTURER_NAME       = 0x15,
    POWER_SUMMARY_I_MANUFACTURER            = 0x16,

    //Voltage Information
    POWER_SUMMARY_VOLTAGE_INFO              = 0x20,

    //Current Information
    POWER_SUMMARY_CURRENT_INFO              = 0x30,

    //Status Information
    POWER_SUMMARY_STATUS_INFO               = 0x40,

    //Capacity Information
    POWER_SUMMARY_CAPACITY_MODE             = 0x50,
    POWER_SUMMARY_DESIGN_CAPACITY           = 0x51,
    POWER_SUMMARY_REMAINING_CAPACITY        = 0x52,
    POWER_SUMMARY_FULL_CHARGE_CAPACITY      = 0x53,
    POWER_SUMMARY_REMAINING_CAPACITY_LIMIT  = 0x54,
    POWER_SUMMARY_WARNING_CAPACITY_LIMIT    = 0x55,
    POWER_SUMMARY_CAPACITY_GRANULARITY_1    = 0x56,
    POWER_SUMMARY_CAPACITY_GRANULARITY_2    = 0x57,

    //Time Information
    POWER_SUMMARY_RUN_TIME_TO_EMPTY         = 0x60,

    NO_MORE_REPORTS = 0xFF
} HID_REPORT_ID;

#endif
