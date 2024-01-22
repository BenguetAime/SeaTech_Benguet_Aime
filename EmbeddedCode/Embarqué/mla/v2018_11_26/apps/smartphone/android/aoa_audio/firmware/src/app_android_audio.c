
#include <stdbool.h>
#include <stdint.h>

#include <usb.h>
#include <usb_host_android.h>
#include <usb_host_audio_v1.h>

#include <system.h>
#include <leds.h>
#include <adc.h>


/***********************************************************
Type definitions, enumerations, and constants
***********************************************************/

typedef struct
{
    USB_AUDIO_V1_DEVICE_ID* audioDeviceID;
    bool interfaceSet;
} AUDIO_ACCESSORY_EXAMPLE_VARS;

typedef struct __attribute__((packed))
{
    union __attribute__((packed))
    {
        struct __attribute__((packed))
        {
            unsigned play_pause         :1;
            unsigned scan_next          :1;
            unsigned scan_previous      :1;
            unsigned volume_increment   :1;
            unsigned volume_decrement   :1;
            unsigned                    :3;
        } bits;
        unsigned char value;
    } controls;
} HID_REPORT;

/***********************************************************
Local prototypes
***********************************************************/
static int8_t VolumeControl(uint8_t currentVolume, uint8_t potReading);

/***********************************************************
Local variables
***********************************************************/
static void* device_handle = NULL;
static bool device_attached = false;
static bool registered = false;
static AUDIO_ACCESSORY_EXAMPLE_VARS audioAccessoryVars =
{
    NULL,
    false
};

//static char manufacturer[] = "Microchip Technology Inc.";
//static char model[] = "Basic Accessory Demo";
static char description[] = DEMO_BOARD_NAME_STRING;
static char version[] = "2.0";
static char uri[] = "http://www.microchip.com/android";
static char serial[] = "N/A";

static ANDROID_ACCESSORY_INFORMATION myDeviceInfo =
{
    //Pass in NULL for the manufacturer and model string if you don't want
    //  an app to pop up when the accessory is plugged in.
    NULL,       //manufacturer,
    0,          //sizeof(manufacturer),

    NULL,       //model,
    0,          //sizeof(model),

    description,
    sizeof(description),

    version,
    sizeof(version),

    uri,
    sizeof(uri),

    serial,
    sizeof(serial),

    //ANDROID_AUDIO_MODE__NONE
    ANDROID_AUDIO_MODE__44K_16B_PCM
};

static char ReportDescriptor[] = {
    0x05, 0x0c,                    // USAGE_PAGE (Consumer Devices)
    0x09, 0x01,                    // USAGE (Consumer Control)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0xcd,                    //   USAGE (Play/Pause)
    0x09, 0xb5,                    //   USAGE (Scan Next Track)
    0x09, 0xb6,                    //   USAGE (Scan Previous Track)
    0x09, 0xe9,                    //   USAGE (Volume Increment)
    0x09, 0xea,                    //   USAGE (Volume Decrement)
    0x09, 0x00,                    //   USAGE (Unassigned)
    0x09, 0x00,                    //   USAGE (Unassigned)
    0x09, 0x00,                    //   USAGE (Unassigned)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x81, 0x42,                    //   INPUT (Data,Ary,Var,Null)
    0xc0                           // END_COLLECTION
};

static HID_REPORT report;
static HID_REPORT oldReport;

static uint8_t requestStatus = USB_SUCCESS;
static ISOCHRONOUS_DATA audioData;
static uint8_t device_address = 0;
static bool reportComplete = true;

static uint8_t currentVolume = 8;
static int8_t volumeChange = 0;

void APP_AndroidAudioInitialize()
{
    memset(&report, 0, sizeof(report));
    memset(&oldReport, 0, sizeof(oldReport));

    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);

    BUTTON_Enable(BUTTON_ANDROID_AUDIO_DEMO_PREVIOUS);
    BUTTON_Enable(BUTTON_ANDROID_AUDIO_DEMO_NEXT);
    BUTTON_Enable(BUTTON_ANDROID_AUDIO_DEMO_PLAY_PAUSE);

    AndroidAppStart(&myDeviceInfo);
}

void APP_AndroidAudioTasks()
{   
    //If the device isn't attached yet,
    if( registered == false )
    {
        reportComplete = true;
        report.controls.value = 0;

        //Don't need to do anything yet.
        return;
    }

    if( reportComplete == true )
    {
        if( BUTTON_IsPressed(BUTTON_ANDROID_AUDIO_DEMO_PREVIOUS) == true )
        {
            report.controls.bits.scan_previous = 1;
        }
        else
        {
            report.controls.bits.scan_previous = 0;
        }

        if( BUTTON_IsPressed(BUTTON_ANDROID_AUDIO_DEMO_NEXT) == true )
        {
            report.controls.bits.scan_next = 1;
        }
        else
        {
            report.controls.bits.scan_next = 0;
        }

        if( BUTTON_IsPressed(BUTTON_ANDROID_AUDIO_DEMO_PLAY_PAUSE) == true )
        {
            report.controls.bits.play_pause = 1;
        }
        else
        {
            report.controls.bits.play_pause = 0;
        }

        volumeChange = VolumeControl(currentVolume, (ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER)/8) );

        report.controls.bits.volume_increment = 0;
        report.controls.bits.volume_decrement = 0;

        switch(volumeChange)
        {
            case 1:
                if(currentVolume < 16)
                {
                    report.controls.bits.volume_increment = 1;
                    currentVolume++;
                }
                break;

            case -1:
                if(currentVolume > 0)
                {
                    report.controls.bits.volume_decrement = 1;
                    currentVolume--;
                }
                break;

            default:
                break;
        }

        if( memcmp(&report, &oldReport, sizeof(report)) != 0 )
        {
            reportComplete = false;

            if(AndroidAppHIDSendEvent(device_address, 1, (uint8_t*)&report, sizeof(HID_REPORT)) != USB_SUCCESS)
            {
                reportComplete = true;
                USBHostClearEndpointErrors(device_address, 0);
            }
        }
    }
}


/***************************************************************************
  Function:
    char VolumeControl(uint8_t currentVolume, uint8_t potReading)

  Summary:
    Determines if the volume needs to be set higher or lower based on the
    current pot value and the current setting.

  Description:
    Determines if the volume needs to be set higher or lower based on the
    current pot value and the current setting.

  Precondition:
    None

  Parameters:
    uint8_t currentVolume - the current volume setting 0-15
    uint8_t potReading - the current pot reading (0-127)

  Return Values:
    char - 1, 0, or -1 based on if the volume should be turned up, stay the
    same, or be turned down.

  Remarks:
    None
***************************************************************************/
static int8_t VolumeControl(uint8_t currentVolume, uint8_t potReading)
{
    /*************************************************************************
     We are using a hysteresis to prevent glitching.  The equations in the
     formulas below are creating that hysteresis.  Here are a few example
     points run through the below logic:

     currentVolume potReading result
     ------------- ---------- ------
     0             1          0
     0             4          0
     0             5          1
     1             4          0
     1             3          0
     1             2          -1

     currentVolume +trip -trip
     ------------- ----- -----
     0             5     N/A
     1             13    2
     2             21    10
     3             29    18
     4             37    26
     5             45    34
     6             53    42
     7             61    50
     8             69    58
     9             77    66
     10            85    74
     11            93    82
     12            101   90
     13            109   98
     14            117   106
     15            125   114
     16            133   122

     *************************************************************************/

    if(potReading >= (((currentVolume + 1) * 8) - 3) )
    {
        return 1;
    }
    else if (currentVolume == 0)
    {
        return 0;
    }
    else if (potReading <= (((currentVolume - 1) * 8) + 2) )
    {
        return -1;
    }

    return 0;
}


bool APP_AndroidAudioEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size )
{
    switch( (int)event )
    {
        case EVENT_VBUS_RELEASE_POWER:
        case EVENT_HUB_ATTACH:
        case EVENT_UNSUPPORTED_DEVICE:
        case EVENT_CANNOT_ENUMERATE:
        case EVENT_CLIENT_INIT_ERROR:
        case EVENT_OUT_OF_MEMORY:
        case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
        case EVENT_DETACH:                   // USB cable has been detached (data: uint8_t, address of device)
        case EVENT_ANDROID_DETACH:
            device_attached = false;
            registered = false;
            return true;
            break;

        case EVENT_AUDIO_ATTACH:
            device_address = address;
            audioAccessoryVars.audioDeviceID = data;

            requestStatus = USBHostAudioV1SetInterfaceFullBandwidth(audioAccessoryVars.audioDeviceID->deviceAddress);

            if( requestStatus != USB_SUCCESS )
            {
            }
            return true;

        case EVENT_AUDIO_INTERFACE_SET:
            if(USBHostIsochronousBuffersCreate(&audioData, 2, 1023) == false)
            {
            }

            USBHostIsochronousBuffersReset(&audioData, 2);

            requestStatus = USBHostAudioV1ReceiveAudioData(audioAccessoryVars.audioDeviceID->deviceAddress, &audioData );

            if(requestStatus != USB_SUCCESS)
            {
            }
            return true;

        case EVENT_AUDIO_DETACH:
            USBHostIsochronousBuffersDestroy(&audioData, 2);
            memset(&audioAccessoryVars, 0, sizeof(audioAccessoryVars));
            return true;

        case EVENT_ANDROID_HID_REGISTRATION_COMPLETE:
            registered = true;
            return true;

        case EVENT_ANDROID_HID_SEND_EVENT_COMPLETE:
            memcpy(&oldReport, &report, sizeof(report));

            reportComplete = true;
            return true;

        // Android Specific events
        case EVENT_ANDROID_ATTACH:
            device_address = address;
            device_handle = data;
            device_attached = true;
            AndroidAppHIDRegister(device_address, 1, (uint8_t*)ReportDescriptor, sizeof(ReportDescriptor));
            return true;

        default :
            break;
    }
    return false;
}