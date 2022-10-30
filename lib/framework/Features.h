#ifndef Features_h
#define Features_h

// modified by Proddy

#define FT_ENABLED(feature) feature

// project feature on by default
#ifndef FT_PROJECT
#define FT_PROJECT 0
#endif

// security feature on by default
#ifndef FT_SECURITY
#define FT_SECURITY 1
#endif

// mqtt feature on by default
#ifndef FT_MQTT
#define FT_MQTT 0
#endif

// ntp feature on by default
#ifndef FT_NTP
#define FT_NTP 1
#endif

// ota feature on by default
#ifndef FT_OTA
#define FT_OTA 1
#endif

// upload firmware/file feature on by default
#ifndef FT_UPLOAD_FIRMWARE
#define FT_UPLOAD_FIRMWARE 1
#endif


#endif
