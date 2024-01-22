
#ifndef APP_ANDROID_AUDIO_H
#define	APP_ANDROID_AUDIO_H

#ifdef	__cplusplus
extern "C" {
#endif

void APP_AndroidAudioTasks(void);
void APP_AndroidAudioInitialize(void);
bool APP_AndroidAudioEventHandler( uint8_t address, USB_EVENT event, void *data, uint32_t size );


#ifdef	__cplusplus
}
#endif

#endif	/* APP_ANDROID_AUDIO_H */

