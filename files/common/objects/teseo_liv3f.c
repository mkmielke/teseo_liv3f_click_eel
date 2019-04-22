#include "teseo_liv3f.h"


// Private internal storage of driver configuration
static ATMO_TeseoLIV3F_Config_t _ATMO_TeseoLIV3F_PrivConfig;


// Private function prototypes
void uart5_custom_init( void );


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
}


ATMO_TeseoLIV3F_Status_t ATMO_TeseoLIV3F_Test()
{
  char buff[20] = "Hello World\r\n";

  UART5->TDR = '!';

  return ATMO_TeseoLIV3F_Status_Success;
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
  UART5->BRR = ( 80e6 / 115200 ) + 0.5; // round
  SET_BIT( UART5->CR1, USART_CR1_TE | USART_CR1_RE | USART_CR1_UE );
}

