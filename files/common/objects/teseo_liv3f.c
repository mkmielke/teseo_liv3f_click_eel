#include "teseo_liv3f.h"

#define BUFF_SIZE 100


// global variables
// Private internal storage of driver configuration
static ATMO_TeseoLIV3F_Config_t _ATMO_TeseoLIV3F_PrivConfig;
static ATMO_TeseoLIV3F_LocData_t last_location_data;
volatile int collect_nmea_sentence = 0;
volatile int collecting_identifier = 0;
volatile int test = 0;
char rx_buff[BUFF_SIZE] = {0};
int rx_buff_ind = 0;


// Private function prototypes
void uart5_custom_init( void );
void parse_gpgga_sentence( char *str );


void UART5_IRQHandler( void )
{
  char rx_char;

  if ( READ_BIT( UART5->ISR, USART_ISR_RXNE ) ) // Data was received
  {
    // if rx '$'
    // then collect data until ','
    // if data = GPGGA
    // then collect UTC, lat, lon, fix?, num sats, altitude
    // ALWAYS check for '$'

    rx_char = READ_REG( UART5->RDR );
    if ( rx_char == '$' )
    {
      if ( collect_nmea_sentence != 0 )
      {
        parse_gpgga_sentence( rx_buff );
      }
      collect_nmea_sentence = 1;
      collecting_identifier = 1;
      rx_buff_ind = 0;
      return;
    }
    else if ( collect_nmea_sentence != 0 )
    {
      if ( ( rx_char == ',' ) && ( collecting_identifier != 0 ) )
      {
        collecting_identifier = 0;
        rx_buff[rx_buff_ind] = '\0'; // add null terminator
        if ( strncmp( rx_buff, "GPGGA", BUFF_SIZE ) )
        {
          collect_nmea_sentence = 0;
          return;
        }
        rx_buff_ind = 0; // discard identifier
      }
      else
      {
        rx_buff[rx_buff_ind] = rx_char;
        rx_buff_ind++;
      }
    }
  }
}


// Function definitions

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


float ATMO_TeseoLIV3F_GetLatitude()
{
  return last_location_data.latitude;
}


float ATMO_TeseoLIV3F_GetLongitude()
{
  return last_location_data.longitude;
}


bool ATMO_TeseoLIV3F_PosFix()
{
  return last_location_data.posFix;
}


uint32_t ATMO_TeseoLIV3F_GetNumSatellites()
{
  return last_location_data.num_satellites;
}


float ATMO_TeseoLIV3F_GetAltitude()
{
  return last_location_data.altitude;
}


void uart5_custom_init( void )
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* UART5 clock enable */
  __HAL_RCC_UART5_CLK_ENABLE();

  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  /**UART5 GPIO Configuration
  PD2     ------> UART5_RX
  PC12     ------> UART5_TX
  */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  /* configure uart5 */
  CLEAR_BIT( UART5->CR1, USART_CR1_UE ); // disable uart
  WRITE_REG( UART5->BRR, ( 80e6 / 9600.0 ) + 0.5 ); // round
  SET_BIT( UART5->CR1, USART_CR1_TE | USART_CR1_RE ); // enable TX and RX

  /* Enable Interrupts */
  SET_BIT( UART5->CR1, USART_CR1_RXNEIE ); // read data register not empty
  NVIC_EnableIRQ( UART5_IRQn ); // enable all USART1 interrupts globally
  NVIC_SetPriority( UART5_IRQn, 0 );    // highest priority

  /* Enable USART1 */
  SET_BIT( UART5->CR1, USART_CR1_UE );
}


void parse_gpgga_sentence( char *str )
{
  const char delim[2] = ",";
  char *token;


  // UTC of position
  token = strtok( str, delim );
  if ( token == NULL || strlen( token ) < 6 )
  {
    return;
  }
  last_location_data.hrsUtc = ( token[0] & 0xF ) * 10 + ( token[1] & 0xF );
  last_location_data.minutesUtc = ( token[2] & 0xF ) * 10 + ( token[3] & 0xF );
  last_location_data.secondsUtc = ( token[4] & 0xF ) * 10 + ( token[5] & 0xF );

  // Latitude of position
  token = strtok( NULL, delim );
  if ( token == NULL || strlen( token ) < 4 )
  {
    return;
  }
  last_location_data.latitude = ( token[0] & 0xF ) * 10 + ( token[1] & 0xF ) + ( strtof( token + 2, NULL ) / 60.0 );

  // N or S
  token = strtok( NULL, delim );
  if ( token == NULL || strlen( token ) < 1 )
  {
    return;
  }
  if ( *token == 'S' )
  {
    last_location_data.latitude *= -1;
  }

  // Longitude of position
  token = strtok( NULL, delim );
  if ( token == NULL || strlen( token ) < 4 )
  {
    return;
  }
  last_location_data.longitude = ( token[0] & 0xF ) * 100 + ( token[1] & 0xF ) * 10 + ( token[2] & 0xF ) + ( strtof( token + 3, NULL ) / 60.0 );

  // E or W
  token = strtok( NULL, delim );
  if ( token == NULL || strlen( token ) < 1 )
  {
    return;
  }
  if ( *token == 'W' )
  {
    last_location_data.longitude *= -1;
  }

  // GPS Quality indicator
  token = strtok( NULL, delim );
  if ( token == NULL || strlen( token ) < 1 )
  {
    return;
  }
  if ( *token == '0' )
  {
    last_location_data.posFix = false;
  }
  else
  {
    last_location_data.posFix = true;
  }

  // number of satellites in use
  token = strtok( NULL, delim );
  if ( token == NULL || strlen( token ) < 2 )
  {
    return;
  }
  last_location_data.num_satellites = ( token[0] & 0xF ) * 10 + ( token[1] & 0xF );

  // horizontal dilution of precision
  token = strtok( NULL, delim );
  if ( token == NULL )
  {
    return;
  }

  // Antenna altitude above mean-sea-level
  token = strtok( NULL, delim );
  if ( token == NULL )
  {
    return;
  }
  last_location_data.altitude = strtof( token, NULL );

  // M
}
