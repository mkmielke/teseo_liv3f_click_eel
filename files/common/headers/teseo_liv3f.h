#ifndef ATMO_TESEO_LIV3F_H_
#define ATMO_TESEO_LIV3F_H_

/******************************************************************************/
/*                                  Includes                                  */
/******************************************************************************/
//#include "../gpio/gpio.h"


typedef enum {
    ATMO_TeseoLIV3F_Status_Success              = 0x00u,  // Common - Operation was successful
    ATMO_TeseoLIV3F_Status_Fail                 = 0x01u,  // Common - Operation failed
    ATMO_TeseoLIV3F_Status_Initialized          = 0x02u,  // Common - Peripheral already initialized
    ATMO_TeseoLIV3F_Status_Invalid              = 0x03u,  // Common - Invalid operation or result
    ATMO_TeseoLIV3F_Status_NotSupported         = 0x04u,  // Common - Feature not supported by platform
} ATMO_TeseoLIV3F_Status_t;

typedef struct 
{
    ATMO_GPIO_Device_Pin_t wupPin;
    ATMO_GPIO_Device_Pin_t rstPin;
    ATMO_DriverInstanceHandle_t gpioDriverInstance;
    ATMO_DriverInstanceHandle_t i2cDriverInstance;
} ATMO_TeseoLIV3F_Config_t;

typedef struct
{
	uint32_t hrsUtc;
	uint32_t minutesUtc;
	uint32_t secondsUtc;
	float latitude;
	float longitude;
	float altitude;
	ATMO_BOOL_t posFix;
	uint32_t numSatellites;
	uint64_t expiration_time;
} ATMO_TeseoLIV3F_LocData_t;



/******************************************************************************/
/*                         Public Function Prototypes                         */
/******************************************************************************/
ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_Init(ATMO_TeseoLIV3F_Config_t *config);
ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_SetConfiguration(const ATMO_TeseoLIV3F_Config_t *config);
ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_GetLocation( void );
ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_GetLatitude( float * latitude );
ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_GetLongitude( float * longitude );
ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_CheckFix( bool * posFix );
ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_GetNumSatellites( uint8_t * numSatellites );
ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_GetAltitude( float * altitude );


#endif

