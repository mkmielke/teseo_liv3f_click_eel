#ifndef ATMO_TESEO_LIV3F_H_
#define ATMO_TESEO_LIV3F_H_


#include "../atmo/core.h"
#include "../app_src/atmosphere_platform.h"


typedef struct 
{
  int myproperty;
} ATMO_TeseoLIV3F_Config_t;

typedef struct
{
  uint32_t hrsUtc;
  uint32_t minutesUtc;
  uint32_t secondsUtc;
  float latitude;
  float longitude;
  bool posFix;
  uint32_t num_satellites;
  float altitude;
}ATMO_TeseoLIV3F_LocData_t;

typedef enum
{
  ATMO_TeseoLIV3F_Status_Success = 0,
  ATMO_TeseoLIV3F_Status_Fail
} ATMO_TeseoLIV3F_Status_t;


ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_Init( ATMO_TeseoLIV3F_Config_t *config );
float ATMO_TeseoLIV3F_GetLatitude();
float ATMO_TeseoLIV3F_GetLongitude();
bool ATMO_TeseoLIV3F_PosFix();
uint32_t ATMO_TeseoLIV3F_GetNumSatellites();
float ATMO_TeseoLIV3F_GetAltitude();

#endif

