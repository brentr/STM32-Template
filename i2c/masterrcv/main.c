/**
  ******************************************************************************
  * @file    I2C/I2C_TwoBoards/MasterReceiverInterrupt/main.c
  * @author  MCD Application Team
  * @version V1.1.1
  * @date    13-April-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "main.h"

/** @addtogroup STM32L1xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup MasterReceiverInterrupt
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TXBUFFERSIZE   (countof(TxBuffer) - 1)
#define RXBUFFERSIZE   TXBUFFERSIZE
/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))  
/* Private variables ---------------------------------------------------------*/
I2C_InitTypeDef  I2C_InitStructure;
uint8_t TxBuffer[] = "Communication between two I2C using Interrupt Example: \
                      Slave Transmitter to Master Receiver";

#if defined (I2C_MASTER)
__IO uint8_t Rx_Idx = 0x00;
uint8_t RxBuffer [RXBUFFERSIZE];
__IO uint8_t NumberOfByteToReceive = 0x00;
__IO uint32_t TimeOut = 0x0;
#endif

#if defined (I2C_SLAVE)
uint8_t NbrOfDataToSend = TXBUFFERSIZE;
__IO uint8_t Tx_Idx = 0x00;
#endif /* I2C_SLAVE */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void I2C_Config(void);
static void SysTickConfig(void);

#if defined (I2C_MASTER)
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
static void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLength);
static void delay(__IO uint32_t nCount);
static void TimeOut_UserCallback(void);
#endif /* I2C_MASTER */

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32l1xx_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32l1xx.c file
     */

  /* I2C configuration ------------------------------------------------------*/
  I2C_Config();

  /* Initialize LEDs mounted on STM32L152-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  
/* SysTick configuration ---------------------------------------------------*/
  SysTickConfig();
  
/*************************************Master Code******************************/
#if defined (I2C_MASTER)
  /* I2C De-initialize */
  I2C_DeInit(I2Cx);

  /*!< I2C Struct Initialize */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DUTYCYCLE;
  I2C_InitStructure.I2C_OwnAddress1 = 0xA0;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;

#ifndef I2C_10BITS_ADDRESS
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
#else
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
#endif /* I2C_10BITS_ADDRESS */
  
  /*!< I2C Initialize */
  I2C_Init(I2Cx, &I2C_InitStructure);

  /* Enable Error Interrupt */
  I2C_ITConfig(I2Cx, I2C_IT_ERR , ENABLE);

  /* I2C ENABLE */
  I2C_Cmd(I2Cx, ENABLE);
  
  while (1)
  {
    NumberOfByteToReceive = RXBUFFERSIZE;
    Rx_Idx = 0x00;
    /* Clear the RxBuffer */
    Fill_Buffer(RxBuffer, RXBUFFERSIZE);
    
    /* Enable Event Interrupts */
    I2C_ITConfig(I2Cx, I2C_IT_EVT , ENABLE);
    
    /* Enable Acknowledge */
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    /* Generate the Start condition */
    I2C_GenerateSTART(I2Cx, ENABLE);
    /* Data transfer is performed in the I2C interrupt routine */
    /* Wait until end of data transfer or time out*/
    TimeOut = USER_TIMEOUT;
    while ((Rx_Idx < RXBUFFERSIZE)&&(TimeOut != 0x00))
    {}
    if(TimeOut == 0)
    {
      TimeOut_UserCallback();
    }
    
    if (Buffercmp(TxBuffer, RxBuffer, RXBUFFERSIZE) == PASSED)
    {
       /* LED2, LED3 and LED4 Toggle */
       STM_EVAL_LEDToggle(LED2);
    }
    else 
    { 
      /* ED2, LED3 and LED4 On */
       STM_EVAL_LEDOff(LED2);
     }
     delay(10);
  }
#endif /* I2C_MASTER */

/**********************************Slave Code**********************************/
#if defined (I2C_SLAVE)
  /*I2Cx DeInitialize*/
  I2C_DeInit(I2Cx);

  /*!< I2C Struct Initialize */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DUTYCYCLE;
  I2C_InitStructure.I2C_OwnAddress1 = SLAVE_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;

#ifndef I2C_10BITS_ADDRESS
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
#else
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
#endif /* I2C_10BITS_ADDRESS */
  I2C_Init(I2Cx, &I2C_InitStructure);

  /* Enable Error Interrupt */
  I2C_ITConfig(I2Cx, I2C_IT_ERR , ENABLE);

  /* I2C ENABLE */
  I2C_Cmd(I2Cx, ENABLE);

  /* Infinite Loop */
  while (1)
  {
    Tx_Idx = 0x00;
    
    /* Enable I2C event interrupt */
    I2C_ITConfig(I2Cx, I2C_IT_EVT, ENABLE);
    
    /* Wait until end of data transfer */
    while (Tx_Idx < TXBUFFERSIZE)
    {
    } 

    /* LED2, LED3 and LED4 Toggle */
    STM_EVAL_LEDToggle(LED2);
  }
#endif /* I2C_SLAVE */
}

/**
  * @brief  Enables the I2C Clock and configures the different GPIO ports.
  * @param  None
  * @retval None
  */
static void I2C_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  /*!< I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(I2Cx_CLK, ENABLE);

  /*!< SDA GPIO clock enable */
  RCC_AHBPeriphClockCmd(I2Cx_SDA_GPIO_CLK, ENABLE);
 
  /*!< SCL GPIO clock enable */
  RCC_AHBPeriphClockCmd(I2Cx_SCL_GPIO_CLK, ENABLE);
  
  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_SOURCE, GPIO_AF_I2C1);

  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_SOURCE, GPIO_AF_I2C1);
  
  /*!< Configure I2C SCL pin */
  GPIO_InitStructure.GPIO_Pin = I2Cx_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure I2C SDA pin */
  GPIO_InitStructure.GPIO_Pin = I2Cx_SDA_PIN;
  GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStructure);

  /* Configure the Priority Group to 1 bit */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Configure the I2C event priority */
  NVIC_InitStructure.NVIC_IRQChannel = I2Cx_EV_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure I2C error interrupt to have the higher priority */
  NVIC_InitStructure.NVIC_IRQChannel = I2Cx_ER_IRQn;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configure a SysTick Base time to 10 ms.
  * @param  None
  * @retval None
  */
static void SysTickConfig(void)
{
  /* Setup SysTick Timer for 10ms interrupts  */
  if (SysTick_Config(SystemCoreClock / 100))
  {
    /* Capture error */
    while (1);
  }

  /* Configure the SysTick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0);
}

#if defined (I2C_MASTER)
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = (34000 * nCount); index != 0; index--)
  {
  }
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
static void TimeOut_UserCallback(void)
{
  /* User can add his own implementation to manage TimeOut Communication failure */
  /* Block communication and all processes */
  while (1)
  {   
  }
}
/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}

/**
  * @brief  Fills buffer.
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferLength: size of the buffer to fill
  * @retval None
  */
static void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLength)
{
  uint16_t index = 0;

  /* Put in global buffer same values */
  for (index = 0; index < BufferLength; index++ )
  {
    pBuffer[index] = 0x00;
  }
}
#endif /* I2C_MASTER */

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
