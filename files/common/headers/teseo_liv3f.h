#ifndef ATMO_TESEO_LIV3F_H_
#define ATMO_TESEO_LIV3F_H_


#include "../atmo/core.h"
#include "../app_src/atmosphere_platform.h"


typedef struct 
{
  int myproperty;
} ATMO_TeseoLIV3F_Config_t;

typedef enum
{
  ATMO_TeseoLIV3F_Status_Success = 0,
  ATMO_TeseoLIV3F_Status_Fail
} ATMO_TeseoLIV3F_Status_t;


ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_Init( ATMO_TeseoLIV3F_Config_t *config );
ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_Test();


#endif

