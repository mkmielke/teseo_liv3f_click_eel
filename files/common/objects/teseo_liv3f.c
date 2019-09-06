/******************************************************************************/
/*                                  Includes                                  */
/******************************************************************************/
#include "../app_src/atmosphere_platform.h"
#include "teseo_liv3f.h"


/******************************************************************************/
/*                                  Defines                                   */
/******************************************************************************/
#define GPGLL_MSG_LEN 22
#define I2C_RX_BUFF_LEN 101

#define LOCATION_EXP_DELTA 1000 // location data expires after 1 second


typedef struct {
    ATMO_TeseoLIV3F_Config_t config;
    unsigned int locRxAbilityHandle;
    unsigned int latRxAbilityHandle;
    unsigned int longRxAbilityHandle;
    ATMO_BOOL_t latRxAbilityHandleSet;
    ATMO_BOOL_t longRxAbilityHandleSet;
    ATMO_BOOL_t locRxAbilityHandleSet;
    ATMO_BOOL_t configured;
} ATMO_TeseoLIV3F_Priv_Config;


/******************************************************************************/
/*                              Global Variables                              */
/******************************************************************************/
static ATMO_TeseoLIV3F_Priv_Config _ATMO_TeseoLIV3F_config;

//static ATMO_BOOL_t lastLocDataReceived = false;
static ATMO_TeseoLIV3F_LocData_t lastLocData;
/*
// Private internal storage of driver configuration
static ATMO_TeseoLIV3F_Config_t _ATMO_TeseoLIV3F_PrivConfig;
static ATMO_TeseoLIV3F_LocData_t last_location_data;
volatile int collect_nmea_sentence = 0;
volatile int collecting_identifier = 0;
volatile int test = 0;
char rx_buff[BUFF_SIZE] = {0};
int rx_buff_ind = 0;
*/

static const uint16_t teseo_slave_addr = 0x3A;
static const char *gpgga_msg = "$PSTMNMEAREQUEST,2,0\r\n";

char rx_buff[I2C_RX_BUFF_LEN + 1];


/******************************************************************************/
/*                        Private Function Prototypes                         */
/******************************************************************************/
ATMO_BOOL_t ATMO_TeseoLIV3F_ParseMessage( char* str, ATMO_TeseoLIV3F_LocData_t *msg );


/******************************************************************************/
/*                        Private Function Definitions                        */
/******************************************************************************/
/*static void ATMO_TeseoLIV3F_Pulse()
{
    if(!_ATMO_TeseoLIV3F_config.configured)
    {
        return;
    }
	ATMO_GPIO_SetPinState(_ATMO_TeseoLIV3F_config.config.gpioDriverInstance, _ATMO_TeseoLIV3F_config.config.powerPin, ATMO_GPIO_PinState_Low);
	ATMO_PLATFORM_DelayMilliseconds(200);
	ATMO_GPIO_SetPinState(_ATMO_TeseoLIV3F_config.config.gpioDriverInstance, _ATMO_TeseoLIV3F_config.config.powerPin, ATMO_GPIO_PinState_High);
	ATMO_PLATFORM_DelayMilliseconds(3000);
	ATMO_GPIO_SetPinState(_ATMO_TeseoLIV3F_config.config.gpioDriverInstance, _ATMO_TeseoLIV3F_config.config.powerPin, ATMO_GPIO_PinState_Low);
	ATMO_PLATFORM_DelayMilliseconds(200);
}


void ATMO_TeseoLIV3F_FactoryReset()
{
    if(!_ATMO_TeseoLIV3F_config.configured)
    {
        return;
    }
	uint32_t dummy = 0;
	ATMO_UART_WriteBlocking(_ATMO_TeseoLIV3F_config.config.uartDriverInstance, "$PSRF101,0,0,0,000,0,0,12,8*1C\r\n", strlen("$PSRF101,0,0,0,000,0,0,12,8*1C\r\n"), &dummy, 100);
}


void ATMO_TeseoLIV3F_DisableOutput()
{
    if(!_ATMO_TeseoLIV3F_config.configured)
    {
        return;
    }
	uint32_t dummy = 0;
	ATMO_UART_WriteBlocking(_ATMO_TeseoLIV3F_config.config.uartDriverInstance, "$PSRF103,02,00,00,01*26\r\n", strlen("$PSRF103,02,00,00,01*26\r\n"), &dummy, 100);
	ATMO_UART_WriteBlocking(_ATMO_TeseoLIV3F_config.config.uartDriverInstance, "$PSRF103,03,00,00,01*27\r\n", strlen("$PSRF103,03,00,00,01*27\r\n"), &dummy, 100);
	ATMO_UART_WriteBlocking(_ATMO_TeseoLIV3F_config.config.uartDriverInstance, "$PSRF103,04,00,00,01*20\r\n", strlen("$PSRF103,04,00,00,01*20\r\n"), &dummy, 100);
	ATMO_UART_WriteBlocking(_ATMO_TeseoLIV3F_config.config.uartDriverInstance, "$PSRF103,00,00,00,01*24\r\n", strlen("$PSRF103,00,00,00,01*24\r\n"), &dummy, 100);
}


uint8_t ATMO_TeseoLIV3F_NMEAChecksum(const char *str)
{
	while(*str == '$')
	{
		str++;
	}

	uint8_t checksum = 0;

	while(*str != '*')
	{
		checksum ^= *str;
		str++;
	}

	return checksum;
}


void ATMO_TeseoLIV3F_SetReportRate(int reportRateHz)
{
	uint32_t dummy = 0;
	char cmdStr[128];
	sprintf(cmdStr, "$PSRF103,00,00,%d,01*", reportRateHz);
	uint8_t checksum = ATMO_TeseoLIV3F_NMEAChecksum(cmdStr);
	sprintf(cmdStr + strlen(cmdStr), "%02X\r\n", checksum);
	ATMO_UART_WriteBlocking(_ATMO_TeseoLIV3F_config.config.uartDriverInstance, cmdStr, strlen(cmdStr), &dummy, 100);
}
*/


void ATMO_TeseoLIV3F_RxCb(void *data)
{
	char str[256];
	ATMO_GetString((ATMO_Value_t *)data, str, 256);

	ATMO_PLATFORM_DebugPrint("Str: %s\r\n", str);

    // Try to get location info
    ATMO_TeseoLIV3F_LocData_t locData;
    if(ATMO_TeseoLIV3F_ParseMessage(str, &locData))
    {
        // lastLocDataReceived = true;
        memcpy(&lastLocData, &locData, sizeof(locData));

        if(_ATMO_TeseoLIV3F_config.locRxAbilityHandleSet)
        {
            ATMO_Value_t val;
            ATMO_CreateValueBinary(&val, &locData, sizeof(locData));
            ATMO_AddAbilityExecute(_ATMO_TeseoLIV3F_config.locRxAbilityHandle, &val);
        }
        if(_ATMO_TeseoLIV3F_config.longRxAbilityHandleSet)
        {
            ATMO_Value_t val;
            ATMO_CreateValueFloat(&val, locData.longitude);
            ATMO_AddAbilityExecute(_ATMO_TeseoLIV3F_config.longRxAbilityHandle, &val);
        }
        if(_ATMO_TeseoLIV3F_config.latRxAbilityHandleSet)
        {
            ATMO_Value_t val;
            ATMO_CreateValueFloat(&val, locData.latitude);
            ATMO_AddAbilityExecute(_ATMO_TeseoLIV3F_config.latRxAbilityHandle, &val);
        }
    }
}



ATMO_BOOL_t ATMO_TeseoLIV3F_ParseMessage( char* str, ATMO_TeseoLIV3F_LocData_t *msg )
{
	const char delim[] = "\xFF" ",";
	char *token;

	// Should be $GPGGA
	token = strtok( str, delim );
	if( token == NULL || strlen( token ) < 6 || strncmp( token, "$GPGGA", 6 ) )
	{
		return false;
	}

	// UTC of position
	token = strtok( NULL, delim );
	if ( token == NULL || strlen( token ) < 6 )
	{
		return false;
	}
	msg->hrsUtc = ( token[0] & 0xF ) * 10 + ( token[1] & 0xF );
	msg->minutesUtc = ( token[2] & 0xF ) * 10 + ( token[3] & 0xF );
	msg->secondsUtc = ( token[4] & 0xF ) * 10 + ( token[5] & 0xF );

	// Latitude of position
	token = strtok( NULL, delim );
	if ( token == NULL || strlen( token ) < 4 )
	{
		return false;
	}
	msg->latitude = ( token[0] & 0xF ) * 10 + ( token[1] & 0xF ) + ( strtof( token + 2, NULL ) / 60.0 );

	// N or S
	token = strtok( NULL, delim );
	if ( token == NULL || strlen( token ) < 1 )
	{
		return false;
	}
	if ( *token == 'S' )
	{
		msg->latitude *= -1;
	}

	// Longitude of position
	token = strtok( NULL, delim );
	if ( token == NULL || strlen( token ) < 4 )
	{
		return false;
	}
	msg->longitude = ( token[0] & 0xF ) * 100 + ( token[1] & 0xF ) * 10 + ( token[2] & 0xF ) + ( strtof( token + 3, NULL ) / 60.0 );

	// E or W
	token = strtok( NULL, delim );
	if ( token == NULL || strlen( token ) < 1 )
	{
		return false;
	}
	if ( *token == 'W' )
	{
		msg->longitude *= -1;
	}

	// GPS Quality indicator
	token = strtok( NULL, delim );
	if ( token == NULL || strlen( token ) < 1 )
	{
		return false;
	}
	if ( *token == '0' )
	{
		msg->posFix = false;
	}
	else
	{
		msg->posFix = true;
	}

	// number of satellites in use
	token = strtok( NULL, delim );
	if ( token == NULL || strlen( token ) < 2 )
	{
		return false;
	}
	msg->numSatellites = ( token[0] & 0xF ) * 10 + ( token[1] & 0xF );

	// horizontal dilution of precision (throw away)
	token = strtok( NULL, delim );
	if ( token == NULL )
	{
		return false;
	}

	// Antenna altitude above mean-sea-level
	token = strtok( NULL, delim );
	if ( token == NULL )
	{
		return false;
	}
	msg->altitude = strtof( token, NULL );

	// M

	return true;
}


/******************************************************************************/
/*                        Public Function Definitions                         */
/******************************************************************************/
ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_Init(ATMO_TeseoLIV3F_Config_t *config)
{
	memset(&_ATMO_TeseoLIV3F_config, 0, sizeof(_ATMO_TeseoLIV3F_config));

    // Did the user supply a configuration?
    if( config )
    {
        ATMO_TeseoLIV3F_SetConfiguration(config);
    }
    else
    {
    	_ATMO_TeseoLIV3F_config.configured = false;
    }

    return ATMO_TeseoLIV3F_Status_Success;
}


ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_SetConfiguration(const ATMO_TeseoLIV3F_Config_t *config)
{
	ATMO_GPIO_Config_t gpioConfig;

    if( config == NULL )
    {
        return ATMO_TeseoLIV3F_Status_Fail;
    }

    memcpy( &_ATMO_TeseoLIV3F_config.config, config, sizeof(ATMO_TeseoLIV3F_Config_t) );
    _ATMO_TeseoLIV3F_config.configured = true;

    // Initialize I2C
    // already done in app_src/atmosphere_platform.c

    // Set PC0 and PC1 as floating input
    gpioConfig.pinMode = ATMO_GPIO_PinMode_Input_HighImpedance;
    ATMO_GPIO_SetPinConfiguration( config->gpioDriverInstance, PC0, &gpioConfig );
    ATMO_GPIO_SetPinConfiguration( config->gpioDriverInstance, PC1, &gpioConfig );

    // Reset GPS
	gpioConfig.initialState = ATMO_GPIO_PinState_Low;
	gpioConfig.pinMode = ATMO_GPIO_PinMode_Output_PushPull;
	ATMO_GPIO_SetPinConfiguration(config->gpioDriverInstance, config->rstPin, &gpioConfig);

	// At least 100ms
	ATMO_PLATFORM_DelayMilliseconds(300);
	ATMO_GPIO_SetPinState(config->gpioDriverInstance, config->rstPin, ATMO_GPIO_PinState_High);

	gpioConfig.initialState = ATMO_GPIO_PinState_High;
	ATMO_GPIO_SetPinConfiguration(config->gpioDriverInstance, config->wupPin, &gpioConfig);
/*
    // Reset GPS
    ATMO_TeseoLIV3F_Pulse();
	ATMO_PLATFORM_DelayMilliseconds(100);

	while(ATMO_GPIO_Read(config->gpioDriverInstance, config->wupPin) != ATMO_GPIO_PinState_High)
	{
		ATMO_TeseoLIV3F_Pulse();
	}

    ATMO_TeseoLIV3F_DisableOutput();
    ATMO_TeseoLIV3F_SetReportRate(config->reportFreq);

    ATMO_PLATFORM_DebugPrint("GPS Initialized\r\n");*/

    return ATMO_TeseoLIV3F_Status_Success;
}


ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_GetLocation( void )
{
	ATMO_TeseoLIV3F_LocData_t locData;

	ATMO_I2C_MasterWrite( _ATMO_TeseoLIV3F_config.config.i2cDriverInstance, teseo_slave_addr, NULL, 0, (uint8_t*)gpgga_msg, GPGLL_MSG_LEN, 0xFFFFFFFF );

	ATMO_PLATFORM_DelayMilliseconds( 50 );
	ATMO_I2C_MasterRead( _ATMO_TeseoLIV3F_config.config.i2cDriverInstance, teseo_slave_addr,  NULL, 0, (uint8_t*)rx_buff, I2C_RX_BUFF_LEN, 0xFFFFFFFF );

	ATMO_TeseoLIV3F_ParseMessage( rx_buff, &locData );
	locData.expiration_time = ATMO_PLATFORM_UptimeMs() + LOCATION_EXP_DELTA;

	memcpy( &lastLocData, &locData, sizeof(locData) );

/*
    if(!lastLocDataReceived)
    {
        return ATMO_TeseoLIV3F_Status_Fail;
    }

    memcpy(locData, &lastLocData, sizeof(lastLocData));
*/
    return ATMO_TeseoLIV3F_Status_Success;
}

/*
ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_Init( ATMO_TeseoLIV3F_Config_t *config )
{
  if ( config == NULL )
  {
    return ATMO_TeseoLIV3F_Status_Fail;
  }

  memcpy( &_ATMO_TeseoLIV3F_PrivConfig, config, sizeof( ATMO_TeseoLIV3F_Config_t ) );

  // uart5 init
  uart5_custom_init();

  return ATMO_TeseoLIV3F_Status_Success;
}
*/

ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_GetLatitude( float * latitude )
{
	// Check if last location has expired
	if ( lastLocData.expiration_time < ATMO_PLATFORM_UptimeMs() )
	{
		// Update location data
		ATMO_TeseoLIV3F_GetLocation();
	}

	*latitude = lastLocData.latitude;

	return ATMO_TeseoLIV3F_Status_Success;
}


ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_GetLongitude( float * longitude )
{
	// Check if last location has expired
	if ( lastLocData.expiration_time < ATMO_PLATFORM_UptimeMs() )
	{
		// Update location data
		ATMO_TeseoLIV3F_GetLocation();
	}

	*longitude = lastLocData.longitude;

	return ATMO_TeseoLIV3F_Status_Success;
}


ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_CheckFix( bool * posFix )
{
	// Check if last location has expired
	if ( lastLocData.expiration_time < ATMO_PLATFORM_UptimeMs() )
	{
		// Update location data
		ATMO_TeseoLIV3F_GetLocation();
	}

	*posFix = lastLocData.posFix;

	return ATMO_TeseoLIV3F_Status_Success;
}


ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_GetNumSatellites( uint8_t * numSatellites )
{
	// Check if last location has expired
	if ( lastLocData.expiration_time < ATMO_PLATFORM_UptimeMs() )
	{
		// Update location data
		ATMO_TeseoLIV3F_GetLocation();
	}

	*numSatellites = lastLocData.numSatellites;

	return ATMO_TeseoLIV3F_Status_Success;
}


ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_GetAltitude( float * altitude )
{
	// Check if last location has expired
	if ( lastLocData.expiration_time < ATMO_PLATFORM_UptimeMs() )
	{
		// Update location data
		ATMO_TeseoLIV3F_GetLocation();
	}

	*altitude = lastLocData.altitude;

	return ATMO_TeseoLIV3F_Status_Success;
}



