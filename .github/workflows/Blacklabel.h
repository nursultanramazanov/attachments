name: Rust

on: <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
        <meta http-equiv="content-type" content="text/html; charset=utf-8"/>
        <title>DuneNvme test system Release Notes</title>
        <meta name="generator" content="LibreOffice 6.3.6.2 (Linux)"/>
        <meta name="created" content="00:00:00"/>
        <meta name="changedby" content="Terry Barnaby"/>
        <meta name="changed" content="2020-07-18T07:19:54.309394236"/>
        <meta name="created" content="00:00:00">
        <style type="text/css">
                h3.cjk { font-family: "Noto Sans CJK SC" }
                h3.ctl { font-family: "Lohit Devanagari" }
        </style>
</head>
<body lang="en-GB" dir="ltr"><h1 align="center">DuneNvme test system
Release Notes</h1>
<h3 class="western">Release 1.0.1: <font face="Liberation Serif, serif"><font size="4" style="font-size: 14pt"><b>2020-0</b></font></font><font face="Liberation Serif, serif"><font size="4" style="font-size: 14pt"><b>7</b></font></font><font face="Liberation Serif, serif"><font size="4" style="font-size: 14pt"><b>-1</b></font></font><font face="Liberation Serif, serif"><font size="4" style="font-size: 14pt"><b>8</b></font></font></h3>
<p>Minor bugfix release to aid debugging.</p>
<ol>
        <li><p>The bfpga_driver’s Makefile now finds the correct PCI
        device when loading the bfpga Linux driver.</p>
        <li><p>The bfpga driver’s udev file, bfpga.rules, now has the
        correct device name.</p>
        <li><p>Fix to the test code for correct bfpga device name string.</p>
        <li><p>Added NVMe PCIe phy_status and link_status signals to the
        NvmeStorageUnit’s status register (bits 30 and 31).</p>
</ol>
<h3 class="western">Release 1.0.0: <font face="Liberation Serif, serif"><font size="4" style="font-size: 14pt"><b>2020-06-16</b></font></font></h3>
<p>First release of Dune NvmeStorage system. This includes builds for
the AB17-M2FMC and Opsero OP47 NVMe boards.</p>
<p>NvmeStorage Utilisation:</p>
<table width="580" cellpadding="0" cellspacing="0" style="page-break-inside: avoid">
        <col width="78"/>

        <col width="79"/>

        <col width="75"/>

        <col width="37"/>

        <col width="40"/>

        <col width="64"/>

        <col width="64"/>

        <col width="49"/>

        <col width="95"/>

        <tr valign="top">
                <th width="78" style="border: none; padding: 0cm"><p>Total LUTs</p>
                </th>
                <th width="79" style="border: none; padding: 0cm"><p>Logic LUTs</p>
                </th>
                <th width="75" style="border: none; padding: 0cm"><p>LUTRAMs</p>
                </th>
                <th width="37" style="border: none; padding: 0cm"><p>SRLs</p>
                </th>
                <th width="40" style="border: none; padding: 0cm"><p>FFs</p>
                </th>
                <th width="64" style="border: none; padding: 0cm"><p>RAMB36</p>
                </th>
                <th width="64" style="border: none; padding: 0cm"><p>RAMB18</p>
                </th>
                <th width="49" style="border: none; padding: 0cm"><p>URAM</p>
                </th>
                <th width="95" style="border: none; padding: 0cm"><p>DSP48 Blocks</p>
                </th>
        </tr>
        <tr valign="top">
                <td width="78" style="border: none; padding: 0cm" sdval="10625" sdnum="2057;"><p>
                        10625</p>
                </td>
                <td width="79" style="border: none; padding: 0cm" sdval="8729" sdnum="2057;"><p>
                        8729</p>
                </td>
                <td width="75" style="border: none; padding: 0cm" sdval="1888" sdnum="2057;"><p>
                        1888</p>
                </td>
                <td width="37" style="border: none; padding: 0cm" sdval="8" sdnum="2057;"><p>
                        8</p>
                </td>
                <td width="40" style="border: none; padding: 0cm" sdval="13773" sdnum="2057;"><p>
                        13773</p>
                </td>
                <td width="64" style="border: none; padding: 0cm" sdval="24" sdnum="2057;"><p>
                        24</p>
                </td>
                <td width="64" style="border: none; padding: 0cm" sdval="26" sdnum="2057;"><p>
                        26</p>
                </td>
                <td width="49" style="border: none; padding: 0cm" sdval="0" sdnum="2057;"><p>
                        0</p>
                </td>
                <td width="95" style="border: none; padding: 0cm" sdval="0" sdnum="2057;"><p>
                        0</p>
                </td>
        </tr>
</table>
<p><br/>
<br/>

</p>
</body>
</html>
  push: 

/* Includes ------------------------------------------------------------------*/
#include "stm8s_conf.h"
#include "stm8s.h"


/* Private functions ---------------------------------------------------------*/
#define I2C_ADR 0x70 //i2c slave adress
#define I2C_SPEED 400000 //speed i2c
#define ClkFreqMHz 16
#define DeadTime   0xFF  //deadtime for transistor close
#define igniteWait  3     // frequenfy*wait delay

void Delay(uint32_t t) {
  if (t==0){t=1;}
    while(--t)
    {
    if (GPIO_ReadInputPin(GPIOD,GPIO_PIN_3))
    {
      enableInterrupts();
    }
    }
}


uint16_t i;
uint16_t ICValue1 =0, ICValue2 =0,freq;

void SendLed (uint16_t num);

void main(void)
{
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);      //16MH
  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);      //16MH
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);


  //--------------GPIO---------------

  GPIO_Init(GPIOD,GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6,GPIO_MODE_OUT_PP_HIGH_SLOW );
  GPIO_Init(GPIOA,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,GPIO_MODE_OUT_PP_HIGH_SLOW);
  GPIO_Init(GPIOC,GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7,GPIO_MODE_OUT_PP_HIGH_SLOW);
  GPIO_Init(GPIOD,GPIO_PIN_3,GPIO_MODE_IN_PU_IT );
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_ONLY);
  enableInterrupts();
  //--------------------------------

   // ------------ TIM2 -------------------
  TIM2_DeInit();
  TIM2_TimeBaseInit(TIM2_PRESCALER_4096 ,50000); 
  TIM2_Cmd(ENABLE);
    // -------------------------------------



  uint8_t mod=0,gotov=0;
  uint16_t buf[10];
  uint32_t b;
  /* Infinite loop */
  while (1)
  {
    //--------------LED-------------------
   //i++; 
   //i=(i>9999)?0:i;
    Delay(20);
    mod++;
    if ((freq!=buf[3])){
      mod=0;
    for(i = 0;i< 3;i++)
    {
      buf[i]=buf[i+1];
    } 
    buf[3]=freq;
    b=0;
    for(i = 0;i<4;i++)
    {
      b= buf[i]+b;
    } 
    b=(b==0)?937500:b;
    i=937500/b;
    }
    GPIO_WriteHigh(GPIOC, GPIO_PIN_4);
    i=(i>10000)?9999:i;
   if (i>999)
   {
      SendLed((i/1000)%10);
      GPIO_WriteLow(GPIOC, GPIO_PIN_7);
      Delay(40);
      GPIO_WriteHigh(GPIOC, GPIO_PIN_7);
   }
   if (i>99)
   {
      SendLed((i/100)%10);
      GPIO_WriteLow(GPIOC, GPIO_PIN_6);
      Delay(40);
      GPIO_WriteHigh(GPIOC, GPIO_PIN_6);
   }
   if (i>9)
   {

      SendLed((i/10)%10);
      GPIO_WriteLow(GPIOC, GPIO_PIN_3);
      Delay(40);
      GPIO_WriteHigh(GPIOC, GPIO_PIN_3);
   }
    SendLed(i%10);
    GPIO_WriteLow(GPIOC, GPIO_PIN_4);

   //-----------------------------



  }

}


INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
   if (!GPIO_ReadInputPin(GPIOD,GPIO_PIN_3))
  {
    ICValue1=  TIM2_GetCounter();
    if (ICValue1>ICValue2){
      freq=ICValue1-ICValue2;
      ICValue2=ICValue1;
    } 
    if (ICValue1<ICValue2){
    freq=ICValue1+50000-ICValue2;
    ICValue2=ICValue1;     
    }
    disableInterrupts();
  }
}

 INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14)
 {
   if (TIM2_GetITStatus(TIM2_IT_CC2)==SET){
    TIM2_ClearITPendingBit(TIM2_IT_CC2);
    TIM2_ClearFlag(TIM2_FLAG_CC2);
   }
 }

void SendLed (uint16_t num)
{

  GPIO_WriteLow(GPIOA, GPIO_PIN_3|GPIO_PIN_1|GPIO_PIN_2);
  GPIO_WriteLow(GPIOD,  GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_2|GPIO_PIN_3);

  switch (num){ 
      case 0:
        GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
        GPIO_WriteHigh(GPIOA, GPIO_PIN_1|GPIO_PIN_2);
        GPIO_WriteHigh(GPIOD, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_2);
    break;
      case 1:
        GPIO_WriteHigh(GPIOA, GPIO_PIN_2);
        GPIO_WriteHigh(GPIOD, GPIO_PIN_4);
    break;
      case 2:
        GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
        GPIO_WriteHigh(GPIOA, GPIO_PIN_2);
        GPIO_WriteHigh(GPIOD,  GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6);
    break;
      case 3:
        GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
        GPIO_WriteHigh(GPIOA, GPIO_PIN_2);
        GPIO_WriteHigh(GPIOD,  GPIO_PIN_4|GPIO_PIN_2|GPIO_PIN_6);
    break;
      case 4:
        GPIO_WriteHigh(GPIOA, GPIO_PIN_1|GPIO_PIN_2);
        GPIO_WriteHigh(GPIOD,  GPIO_PIN_4|GPIO_PIN_6);
    break;
      case 5:
        GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
        GPIO_WriteHigh(GPIOA, GPIO_PIN_1);
        GPIO_WriteHigh(GPIOD,  GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_2);
    break;
      case 6:
        GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
        GPIO_WriteHigh(GPIOA, GPIO_PIN_1);
        GPIO_WriteHigh(GPIOD,  GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_2);
    break;
      case 7:
        GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
        GPIO_WriteHigh(GPIOA, GPIO_PIN_2);
        GPIO_WriteHigh(GPIOD,  GPIO_PIN_4);
    break;
      case 8:
        GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
        GPIO_WriteHigh(GPIOA, GPIO_PIN_1|GPIO_PIN_2);
        GPIO_WriteHigh(GPIOD,  GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_2);
    break;
      case 9:
        GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
        GPIO_WriteHigh(GPIOA, GPIO_PIN_1|GPIO_PIN_2);
        GPIO_WriteHigh(GPIOD,  GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_2);
    break;


  }

}





#ifdef USE_FULL_ASSERT


void assert_failed(u8* file, u32 line)
{ 

  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
    branches: [ "main" ]
  pull_request: /** 
  @page exampletemplate Project Example Template 

  @verbatim 
  ******************** (C) COPYRIGHT 2014 STMicroelectronics ******************* 
  * @file    readme.txt  
  * @author  MCD Application Team 
  * @version V2.3.0 
  * @date    16-June-2017 
  * @brief   This sub-directory contains all the user-modifiable files  
  *          needed to create a new project linked with the STM8S/A   
  *          Standard Peripheral Library. 
  ****************************************************************************** 
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
   @endverbatim 

@par Project Example Template Description  

This example is used as a template that can be used as reference to build any new  
firmware development for all STM8S and STM8A product devices using the STM8S/A  
Standard Peripherals Library. 


@par Directory contents  

This folder contains standard template projects for EWSTM8, STVD with Cosmic,  
STVD with raisonance and RIDE toolchains that compile the needed STM8S/A Standard 
Peripheral's drivers plus all the user-modifiable files that are necessary to  
create a new project.  

  - Project/Template/stm8s_conf.h    Library Configuration file 

  - Project/Template/stm8s_it.c:     Source file containing the interrupt handlers (the  
                                     function bodies are empty in this template). 

  - Project/Template/stm8s_it.h      Header file including all interrupt handlers prototypes. 

  - Project/Template/main.c          Main program 

 @note 
  - High-Density STM8A devices are the STM8AF52xx STM8AF6269/8x/Ax, 
    STM8AF51xx, and STM8AF6169/7x/8x/9x/Ax microcontrollers where the Flash memory 
    density ranges between 32 to 128 Kbytes 
  - Medium-Density STM8A devices are the STM8AF622x/4x, STM8AF6266/68, 
    STM8AF612x/4x, and STM8AF6166/68 microcontrollers where the Flash memory  
    density ranges between 8 to 32 Kbytes 
  - High-Density STM8S devices are the STM8S207xx, STM8S007 and STM8S208xx microcontrollers 
    where the Flash memory density ranges between 32 to 128 Kbytes. 
  - Medium-Density STM8S devices are the STM8S105x and STM8S005 microcontrollers 
    where the Flash memory density ranges between 16 to 32-Kbytes. 
  - Low-Density STM8S devices are the STM8S103xx, STM8S003, STM8S001 and STM8S903xx microcontrollers 
    where the Flash density is 8 Kbytes. 

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3> 
 */ 
    branches: [ "main" ]

env: /**
  ******************************************************************************
  * @file    stm8s_conf.h
  * @author  MCD Application Team
  * @version V2.3.0
  * @date    16-June-2017
  * @brief   This file is used to configure the Library.
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8S_CONF_H
#define __STM8S_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* Uncomment the line below to enable peripheral header file inclusion */
#if defined(STM8S105) || defined(STM8S005) || defined(STM8S103) || defined(STM8S003) ||\
    defined(STM8S001) || defined(STM8S903) || defined (STM8AF626x) || defined (STM8AF622x)
#include "stm8s_adc1.h" 
#endif /* (STM8S105) ||(STM8S103) || (STM8S001) || (STM8S903) || (STM8AF626x) || (STM8AF622x) */
#if defined(STM8S208) || defined(STM8S207) || defined(STM8S007) || defined (STM8AF52Ax) ||\
    defined (STM8AF62Ax)
 #include "stm8s_adc2.h"
#endif /* (STM8S208) || (STM8S207) || (STM8AF62Ax) || (STM8AF52Ax) */
#include "stm8s_awu.h"
#include "stm8s_beep.h"
#if defined (STM8S208) || defined (STM8AF52Ax)
 #include "stm8s_can.h"
#endif /* (STM8S208) || (STM8AF52Ax) */
#include "stm8s_clk.h"
#include "stm8s_exti.h"
#include "stm8s_flash.h"
#include "stm8s_gpio.h"
#include "stm8s_i2c.h"
#include "stm8s_itc.h"
#include "stm8s_iwdg.h"
#include "stm8s_rst.h"
#include "stm8s_spi.h"
#include "stm8s_tim1.h"
#if !defined(STM8S903) || !defined(STM8AF622x)
 #include "stm8s_tim2.h"
#endif /* (STM8S903) || (STM8AF622x) */
#if defined(STM8S208) || defined(STM8S207) || defined(STM8S007) ||defined(STM8S105) ||\
    defined(STM8S005) ||  defined (STM8AF52Ax) || defined (STM8AF62Ax) || defined (STM8AF626x)
 #include "stm8s_tim3.h"
#endif /* (STM8S208) ||defined(STM8S207) || defined(STM8S007) ||defined(STM8S105) */ 
#if !defined(STM8S903) || !defined(STM8AF622x)
 #include "stm8s_tim4.h"
#endif /* (STM8S903) || (STM8AF622x) */
#if defined(STM8S903) || defined(STM8AF622x)
 #include "stm8s_tim5.h"
 #include "stm8s_tim6.h"
#endif  /* (STM8S903) || (STM8AF622x) */
#if defined(STM8S208) ||defined(STM8S207) || defined(STM8S007) ||defined(STM8S103) ||\
    defined(STM8S003) ||defined(STM8S001) || defined(STM8S903) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
 #include "stm8s_uart1.h"
#endif /* (STM8S208) || (STM8S207) || (STM8S103) || (STM8S001) || (STM8S903) || (STM8AF52Ax) || (STM8AF62Ax) */
#if defined(STM8S105) || defined(STM8S005) ||  defined (STM8AF626x)
 #include "stm8s_uart2.h"
#endif /* (STM8S105) || (STM8AF626x) */
#if defined(STM8S208) ||defined(STM8S207) || defined(STM8S007) || defined (STM8AF52Ax) ||\
    defined (STM8AF62Ax)
 #include "stm8s_uart3.h"
#endif /* STM8S208 || STM8S207 || STM8AF52Ax || STM8AF62Ax */
#if defined(STM8AF622x)
 #include "stm8s_uart4.h"
#endif /* (STM8AF622x) */      
#include "stm8s_wwdg.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the
   Standard Peripheral Library drivers code */
#define USE_FULL_ASSERT    (1) 

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed.
  *   If expr is true, it returns no value.
  * @retval : None
  */
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
void assert_failed(uint8_t* file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif /* __STM8S_CONF_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
  CARGO_TERM_COLOR: /**
  ******************************************************************************
  * @file    stm8s_it.c
  * @author  MCD Application Team
  * @version V2.3.0
  * @date    16-June-2017
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all peripherals interrupt service 
  *          routine.
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
#include "stm8s_it.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

#ifdef _COSMIC_
/**
  * @brief Dummy Interrupt routine
  * @par Parameters:
  * None
  * @retval
  * None
*/
INTERRUPT_HANDLER(NonHandledInterrupt, 25)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif /*_COSMIC_*/

/**
  * @brief TRAP Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Top Level Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TLI_IRQHandler, 0)

{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Auto Wake Up Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(AWU_IRQHandler, 1)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Clock Controller Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(CLK_IRQHandler, 2)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External Interrupt PORTA Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External Interrupt PORTB Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External Interrupt PORTC Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
{
  if (GPIO_ReadInputPin(GPIOC,GPIO_PIN_6))
  {
    //Breizinheim();
  }
}

/**
  * @brief External Interrupt PORTD Interrupt routine.
  * @param  None
  * @retval None
  */


/**
  * @brief External Interrupt PORTE Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

#if defined (STM8S903) || defined (STM8AF622x) 
/**
  * @brief External Interrupt PORTF Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(EXTI_PORTF_IRQHandler, 8)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }
#endif /* (STM8S903) || (STM8AF622x) */

#if defined (STM8S208) || defined (STM8AF52Ax)
/**
  * @brief CAN RX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(CAN_RX_IRQHandler, 8)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }

/**
  * @brief CAN TX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(CAN_TX_IRQHandler, 9)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }
#endif /* (STM8S208) || (STM8AF52Ax) */

/**
  * @brief SPI Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SPI_IRQHandler, 10)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Timer1 Update/Overflow/Trigger/Break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{

  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Timer1 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_CAP_COM_IRQHandler, 12)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

#if defined (STM8S903) || defined (STM8AF622x)
/**
  * @brief Timer5 Update/Overflow/Break/Trigger Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM5_UPD_OVF_BRK_TRG_IRQHandler, 13)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }

/**
  * @brief Timer5 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM5_CAP_COM_IRQHandler, 14)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }

#else /* (STM8S208) || (STM8S207) || (STM8S105) || (STM8S103) || (STM8AF62Ax) || (STM8AF52Ax) || (STM8AF626x) */
/**
  * @brief Timer2 Update/Overflow/Break Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
 {




 }

/**
  * @brief Timer2 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */

#endif /* (STM8S903) || (STM8AF622x) */

#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S105) || \
    defined(STM8S005) ||  defined (STM8AF62Ax) || defined (STM8AF52Ax) || defined (STM8AF626x)
/**
  * @brief Timer3 Update/Overflow/Break Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }

/**
  * @brief Timer3 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM3_CAP_COM_IRQHandler, 16)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }
#endif /* (STM8S208) || (STM8S207) || (STM8S105) || (STM8AF62Ax) || (STM8AF52Ax) || (STM8AF626x) */

#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S103) || \
    defined (STM8S003) || defined(STM8S001) || defined (STM8AF62Ax) || defined (STM8AF52Ax) || defined (STM8S903)
/**
  * @brief UART1 TX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }

/**
  * @brief UART1 RX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }
#endif /* (STM8S208) || (STM8S207) || (STM8S103) || (STM8S001) || (STM8S903) || (STM8AF62Ax) || (STM8AF52Ax) */

#if defined(STM8AF622x)
/**
  * @brief UART4 TX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART4_TX_IRQHandler, 17)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }

/**
  * @brief UART4 RX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART4_RX_IRQHandler, 18)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }
#endif /* (STM8AF622x) */

/**
  * @brief I2C Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(I2C_IRQHandler, 19)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

#if defined(STM8S105) || defined(STM8S005) ||  defined (STM8AF626x)
/**
  * @brief UART2 TX interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART2_TX_IRQHandler, 20)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }

/**
  * @brief UART2 RX interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART2_RX_IRQHandler, 21)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }
#endif /* (STM8S105) || (STM8AF626x) */

#if defined(STM8S207) || defined(STM8S007) || defined(STM8S208) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
/**
  * @brief UART3 TX interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART3_TX_IRQHandler, 20)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }

/**
  * @brief UART3 RX interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART3_RX_IRQHandler, 21)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }
#endif /* (STM8S208) || (STM8S207) || (STM8AF52Ax) || (STM8AF62Ax) */

#if defined(STM8S207) || defined(STM8S007) || defined(STM8S208) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
/**
  * @brief ADC2 interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(ADC2_IRQHandler, 22)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }
#else /* STM8S105 or STM8S103 or STM8S903 or STM8AF626x or STM8AF622x */
/**
  * @brief ADC1 interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
 INTERRUPT_HANDLER(ADC1_IRQHandler, 22)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }
#endif /* (STM8S208) || (STM8S207) || (STM8AF52Ax) || (STM8AF62Ax) */

#if defined (STM8S903) || defined (STM8AF622x)
/**
  * @brief Timer6 Update/Overflow/Trigger Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM6_UPD_OVF_TRG_IRQHandler, 23)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }
#else /* STM8S208 or STM8S207 or STM8S105 or STM8S103 or STM8AF52Ax or STM8AF62Ax or STM8AF626x */
/**
  * @brief Timer4 Update/Overflow Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
 {

  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }
#endif /* (STM8S903) || (STM8AF622x)*/

/**
  * @brief Eeprom EEC Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EEPROM_EEC_IRQHandler, 24)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

jobs: /**
  ******************************************************************************
  * @file    stm8s_it.h
  * @author  MCD Application Team
  * @version V2.3.0
  * @date    16-June-2017
  * @brief   This file contains the headers of the interrupt handlers
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8S_IT_H
#define __STM8S_IT_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#ifdef _COSMIC_
 void _stext(void); /* RESET startup routine */
 INTERRUPT void NonHandledInterrupt(void);
#endif /* _COSMIC_ */

#ifndef _RAISONANCE_
 INTERRUPT void TRAP_IRQHandler(void); /* TRAP */
 INTERRUPT void TLI_IRQHandler(void); /* TLI */
 INTERRUPT void AWU_IRQHandler(void); /* AWU */
 INTERRUPT void CLK_IRQHandler(void); /* CLOCK */
 INTERRUPT void EXTI_PORTA_IRQHandler(void); /* EXTI PORTA */
 INTERRUPT void EXTI_PORTB_IRQHandler(void); /* EXTI PORTB */
 INTERRUPT void EXTI_PORTC_IRQHandler(void); /* EXTI PORTC */
 INTERRUPT void EXTI_PORTD_IRQHandler(void); /* EXTI PORTD */
 INTERRUPT void EXTI_PORTE_IRQHandler(void); /* EXTI PORTE */

#if defined(STM8S903) || defined(STM8AF622x)
 INTERRUPT void EXTI_PORTF_IRQHandler(void); /* EXTI PORTF */
#endif /* (STM8S903) || (STM8AF622x) */

#if defined (STM8S208) || defined (STM8AF52Ax)
 INTERRUPT void CAN_RX_IRQHandler(void); /* CAN RX */
 INTERRUPT void CAN_TX_IRQHandler(void); /* CAN TX/ER/SC */
#endif /* (STM8S208) || (STM8AF52Ax) */

 INTERRUPT void SPI_IRQHandler(void); /* SPI */
 INTERRUPT void TIM1_CAP_COM_IRQHandler(void); /* TIM1 CAP/COM */
 INTERRUPT void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void); /* TIM1 UPD/OVF/TRG/BRK */

#if defined(STM8S903) || defined(STM8AF622x)
 INTERRUPT void TIM5_UPD_OVF_BRK_TRG_IRQHandler(void); /* TIM5 UPD/OVF/BRK/TRG */
 INTERRUPT void TIM5_CAP_COM_IRQHandler(void); /* TIM5 CAP/COM */
#else /* (STM8S208) || (STM8S207) || (STM8S105) || (STM8S103) || (STM8AF52Ax) || (STM8AF62Ax) || (STM8A626x) */
 INTERRUPT void TIM2_UPD_OVF_BRK_IRQHandler(void); /* TIM2 UPD/OVF/BRK */
 INTERRUPT void TIM2_CAP_COM_IRQHandler(void); /* TIM2 CAP/COM */
#endif /* (STM8S903) || (STM8AF622x) */

#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S105) || \
    defined(STM8S005) ||  defined (STM8AF52Ax) || defined (STM8AF62Ax) || defined (STM8AF626x)
 INTERRUPT void TIM3_UPD_OVF_BRK_IRQHandler(void); /* TIM3 UPD/OVF/BRK */
 INTERRUPT void TIM3_CAP_COM_IRQHandler(void); /* TIM3 CAP/COM */
#endif /* (STM8S208) || (STM8S207) || (STM8S105) || (STM8AF52Ax) || (STM8AF62Ax) || (STM8A626x) */

#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S103) || \
    defined (STM8S001) || defined(STM8S003) ||  defined (STM8AF52Ax) || defined (STM8AF62Ax) || defined (STM8S903)
 INTERRUPT void UART1_TX_IRQHandler(void); /* UART1 TX */
 INTERRUPT void UART1_RX_IRQHandler(void); /* UART1 RX */
#endif /* (STM8S208) || (STM8S207) || (STM8S903) || (STM8S103) || (STM8S001) || (STM8AF52Ax) || (STM8AF62Ax) */

#if defined (STM8AF622x)
 INTERRUPT void UART4_TX_IRQHandler(void); /* UART4 TX */
 INTERRUPT void UART4_RX_IRQHandler(void); /* UART4 RX */
#endif /* (STM8AF622x) */

 INTERRUPT void I2C_IRQHandler(void); /* I2C */

#if defined(STM8S105) || defined(STM8S005) ||  defined (STM8AF626x)
 INTERRUPT void UART2_RX_IRQHandler(void); /* UART2 RX */
 INTERRUPT void UART2_TX_IRQHandler(void); /* UART2 TX */
#endif /* (STM8S105) || (STM8AF626x) */

#if defined(STM8S207) || defined(STM8S007) || defined(STM8S208) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
 INTERRUPT void UART3_RX_IRQHandler(void); /* UART3 RX */
 INTERRUPT void UART3_TX_IRQHandler(void); /* UART3 TX */
#endif /* (STM8S207) || (STM8S208) || (STM8AF62Ax) || (STM8AF52Ax) */

#if defined(STM8S207) || defined(STM8S007) || defined(STM8S208) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
 INTERRUPT void ADC2_IRQHandler(void); /* ADC2 */
#else /* (STM8S105) || (STM8S103) || (STM8S903) || (STM8AF622x) */
 INTERRUPT void ADC1_IRQHandler(void); /* ADC1 */
#endif /* (STM8S207) || (STM8S208) || (STM8AF62Ax) || (STM8AF52Ax) */

#if defined(STM8S903) || defined(STM8AF622x)
 INTERRUPT void TIM6_UPD_OVF_TRG_IRQHandler(void); /* TIM6 UPD/OVF/TRG */
#else /* (STM8S208) || (STM8S207) || (STM8S105) || (STM8S103) || (STM8AF62Ax) || (STM8AF52Ax) || (STM8AF626x) */
 INTERRUPT void TIM4_UPD_OVF_IRQHandler(void); /* TIM4 UPD/OVF */
#endif /* (STM8S903) || (STM8AF622x) */
 INTERRUPT void EEPROM_EEC_IRQHandler(void); /* EEPROM ECC CORRECTION */
#endif /* _RAISONANCE_ */

#endif /* __STM8S_IT_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
  build: ################################################################################
#        Makefile to build Dune NvmeStorage documents
#                Beam Ltd,        2020-02-18
################################################################################
#
include ../Config.mk

DateNow = $(shell date -I)

.PHONY:        config release

all:        config docs

config: .configured

.configured: ../Config.mk
        sed -i "s/PROJECT_NUMBER.*/PROJECT_NUMBER\t\t= ${VERSION}/g" fpga_doxygen.dox
        sed -i "s/PROJECT_NUMBER.*/PROJECT_NUMBER\t\t= ${VERSION}/g" host_doxygen.dox
        sed -i "s/\\\version.*/\\\version ${VERSION}/g" fpga_overview.dox
        sed -i "s/\\\date.*/\\\date ${DateNow}/g" fpga_overview.dox
        sed -i "s/\\\version.*/\\\version ${VERSION}/g" host_overview.dox
        sed -i "s/\\\date.*/\\\date ${DateNow}/g" host_overview.dox
        touch .configured

install:
        @if [ "${DOCDIR}" != "" ]; then \
                mkdir -p ${DOCDIR}/host ${DOCDIR}/fpga; \
                cp -a ../doc/host/html ${DOCDIR}/host; \
                cp -a ../doc/fpga/html ${DOCDIR}/fpga; \
                cp -a ../doc/*.pdf ${DOCDIR}; \
                cp -a ../doc/*.html ${DOCDIR}; \
        fi;

clean:

distclean: clean
        rm -fr ../doc/host ../doc/fpga
        rm -f .configured

release:

docs:
        doxygen host_doxygen.dox
        doxygen fpga_doxygen.dox
        soffice --headless --convert-to pdf --outdir ../doc Readme.odt
        soffice --headless --convert-to pdf --outdir ../doc DuneNvmeStorageManual.odt
        soffice --headless --convert-to pdf --outdir ../doc DuneNvmeStorageDesign.odt
        soffice --headless --convert-to pdf --outdir ../doc DuneNvmeStorageProject.odt
        soffice --headless --convert-to pdf --outdir ../doc DuneNvmeStorageTestSoftware.odt
        cp DuneNvme-releasenotes.html ../doc

docspdf: docs
        (cd ../doc/host/latex; make)
        cp ../doc/host/latex/refman.pdf ../doc/${PROJECT}_host.pdf
        (cd ../doc/fpga/latex; make)
        cp ../doc/fpga/latex/refman.pdf ../doc/${PROJECT}_fpga.pdf

    runs-on: /* Beam doxygen CSS overrides for WebSys */
ul {
        overflow: visible;        /* Reverted this: Fixed: list item bullets overlap floating elements*/
}

a:hover {
        text-decoration: none;
}

    steps: # Doxyfile 1.8.15

PROJECT_NAME           = "DuneNvme"
PROJECT_NUMBER                = 1.0.0
PROJECT_BRIEF          = "This is a simple NVMe test environment that allows experimentation with the low level PCIe NVMe interfaces as available on a Xilinx FPGA environment."
PROJECT_LOGO           =
INPUT                  = fpga_overview.dox ../src
OUTPUT_DIRECTORY       = ../doc/fpga
OPTIMIZE_OUTPUT_VHDL   = YES
HTML_EXTRA_STYLESHEET  = beam_doxygen.css

#---------------------------------------------------------------------------
# Project related configuration options
#---------------------------------------------------------------------------
DOXYFILE_ENCODING      = UTF-8
CREATE_SUBDIRS         = NO
ALLOW_UNICODE_NAMES    = NO
OUTPUT_LANGUAGE        = English
OUTPUT_TEXT_DIRECTION  = None
BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
ABBREVIATE_BRIEF       = "The $name class" \
                         "The $name widget" \
                         "The $name file" \
                         is \
                         provides \
                         specifies \
                         contains \
                         represents \
                         a \
                         an \
                         the

ALWAYS_DETAILED_SEC    = NO
INLINE_INHERITED_MEMB  = NO
FULL_PATH_NAMES        = YES
STRIP_FROM_PATH        =
STRIP_FROM_INC_PATH    =
SHORT_NAMES            = NO
JAVADOC_AUTOBRIEF      = NO
QT_AUTOBRIEF           = NO
MULTILINE_CPP_IS_BRIEF = NO
INHERIT_DOCS           = YES
SEPARATE_MEMBER_PAGES  = NO
TAB_SIZE               = 4
ALIASES                =
TCL_SUBST              =
OPTIMIZE_OUTPUT_FOR_C  = NO
OPTIMIZE_OUTPUT_JAVA   = NO
OPTIMIZE_FOR_FORTRAN   = NO
#OPTIMIZE_OUTPUT_VHDL   = NO
OPTIMIZE_OUTPUT_SLICE  = NO
EXTENSION_MAPPING      =
MARKDOWN_SUPPORT       = YES
TOC_INCLUDE_HEADINGS   = 0
AUTOLINK_SUPPORT       = YES
BUILTIN_STL_SUPPORT    = NO
CPP_CLI_SUPPORT        = NO
SIP_SUPPORT            = NO
IDL_PROPERTY_SUPPORT   = YES
DISTRIBUTE_GROUP_DOC   = NO
GROUP_NESTED_COMPOUNDS = NO
SUBGROUPING            = YES
INLINE_GROUPED_CLASSES = NO
INLINE_SIMPLE_STRUCTS  = NO
TYPEDEF_HIDES_STRUCT   = NO
LOOKUP_CACHE_SIZE      = 0

#---------------------------------------------------------------------------
# Build related configuration options
#---------------------------------------------------------------------------
EXTRACT_ALL            = NO
EXTRACT_PRIVATE        = NO
EXTRACT_PACKAGE        = NO
EXTRACT_STATIC         = NO
EXTRACT_LOCAL_CLASSES  = YES
EXTRACT_LOCAL_METHODS  = NO
EXTRACT_ANON_NSPACES   = NO
HIDE_UNDOC_MEMBERS     = NO
HIDE_UNDOC_CLASSES     = NO
HIDE_FRIEND_COMPOUNDS  = NO
HIDE_IN_BODY_DOCS      = NO
INTERNAL_DOCS          = NO
CASE_SENSE_NAMES       = YES
HIDE_SCOPE_NAMES       = NO
HIDE_COMPOUND_REFERENCE= NO
SHOW_INCLUDE_FILES     = YES
SHOW_GROUPED_MEMB_INC  = NO
FORCE_LOCAL_INCLUDES   = NO
INLINE_INFO            = YES
SORT_MEMBER_DOCS       = YES
SORT_BRIEF_DOCS        = NO
SORT_MEMBERS_CTORS_1ST = NO
SORT_GROUP_NAMES       = NO
SORT_BY_SCOPE_NAME     = NO
STRICT_PROTO_MATCHING  = NO
GENERATE_TODOLIST      = YES
GENERATE_TESTLIST      = YES
GENERATE_BUGLIST       = YES
GENERATE_DEPRECATEDLIST= YES
ENABLED_SECTIONS       =
MAX_INITIALIZER_LINES  = 30
SHOW_USED_FILES        = YES
SHOW_FILES             = YES
SHOW_NAMESPACES        = YES
FILE_VERSION_FILTER    =
LAYOUT_FILE            =
CITE_BIB_FILES         =
QUIET                  = NO
WARNINGS               = YES
WARN_IF_UNDOCUMENTED   = YES
WARN_IF_DOC_ERROR      = YES
WARN_NO_PARAMDOC       = NO
WARN_AS_ERROR          = NO
WARN_FORMAT            = "$file:$line: $text"
WARN_LOGFILE           =

#---------------------------------------------------------------------------
# Configuration options related to the input files
#---------------------------------------------------------------------------
#INPUT                  = ../src
INPUT_ENCODING         = UTF-8
FILE_PATTERNS          = *.c \
                         *.cc \
                         *.cxx \
                         *.cpp \
                         *.c++ \
                         *.java \
                         *.ii \
                         *.ixx \
                         *.ipp \
                         *.i++ \
                         *.inl \
                         *.idl \
                         *.ddl \
                         *.odl \
                         *.h \
                         *.hh \
                         *.hxx \
                         *.hpp \
                         *.h++ \
                         *.cs \
                         *.d \
                         *.php \
                         *.php4 \
                         *.php5 \
                         *.phtml \
                         *.inc \
                         *.m \
                         *.markdown \
                         *.md \
                         *.mm \
                         *.dox \
                         *.py \
                         *.pyw \
                         *.f90 \
                         *.f95 \
                         *.f03 \
                         *.f08 \
                         *.f \
                         *.for \
                         *.tcl \
                         *.vhd \
                         *.vhdl \
                         *.ucf \
                         *.qsf \
                         *.ice

RECURSIVE              = YES
EXCLUDE                =
EXCLUDE_SYMLINKS       = NO
EXCLUDE_PATTERNS       =
EXCLUDE_SYMBOLS        =
EXAMPLE_PATH           =
EXAMPLE_PATTERNS       = *
EXAMPLE_RECURSIVE      = NO
IMAGE_PATH             =
INPUT_FILTER           =
FILTER_PATTERNS        =
FILTER_SOURCE_FILES    = NO
FILTER_SOURCE_PATTERNS =
USE_MDFILE_AS_MAINPAGE =

#---------------------------------------------------------------------------
# Configuration options related to source browsing
#---------------------------------------------------------------------------
SOURCE_BROWSER         = NO
INLINE_SOURCES         = NO
STRIP_CODE_COMMENTS    = YES
REFERENCED_BY_RELATION = NO
REFERENCES_RELATION    = NO
REFERENCES_LINK_SOURCE = YES
SOURCE_TOOLTIPS        = YES
USE_HTAGS              = NO
VERBATIM_HEADERS       = YES

#---------------------------------------------------------------------------
# Configuration options related to the alphabetical class index
#---------------------------------------------------------------------------
ALPHABETICAL_INDEX     = YES
COLS_IN_ALPHA_INDEX    = 5
IGNORE_PREFIX          =

#---------------------------------------------------------------------------
# Configuration options related to the HTML output
#---------------------------------------------------------------------------
GENERATE_HTML          = YES
HTML_OUTPUT            = html
HTML_FILE_EXTENSION    = .html
HTML_HEADER            =
HTML_FOOTER            =
HTML_STYLESHEET        =
#HTML_EXTRA_STYLESHEET  =
HTML_EXTRA_FILES       =
HTML_COLORSTYLE_HUE    = 220
HTML_COLORSTYLE_SAT    = 100
HTML_COLORSTYLE_GAMMA  = 80
HTML_TIMESTAMP         = NO
HTML_DYNAMIC_MENUS     = YES
HTML_DYNAMIC_SECTIONS  = NO
HTML_INDEX_NUM_ENTRIES = 100
GENERATE_DOCSET        = NO
DOCSET_FEEDNAME        = "Doxygen generated docs"
DOCSET_BUNDLE_ID       = org.doxygen.Project
DOCSET_PUBLISHER_ID    = org.doxygen.Publisher
DOCSET_PUBLISHER_NAME  = Publisher
GENERATE_HTMLHELP      = NO
CHM_FILE               =
HHC_LOCATION           =
GENERATE_CHI           = NO
CHM_INDEX_ENCODING     =
BINARY_TOC             = NO
TOC_EXPAND             = NO
GENERATE_QHP           = NO
QCH_FILE               =
QHP_NAMESPACE          = org.doxygen.Project
QHP_VIRTUAL_FOLDER     = doc
QHP_CUST_FILTER_NAME   =
QHP_CUST_FILTER_ATTRS  =
QHP_SECT_FILTER_ATTRS  =
QHG_LOCATION           =
GENERATE_ECLIPSEHELP   = NO
ECLIPSE_DOC_ID         = org.doxygen.Project
DISABLE_INDEX          = NO
GENERATE_TREEVIEW      = NO
ENUM_VALUES_PER_LINE   = 4
TREEVIEW_WIDTH         = 250
EXT_LINKS_IN_WINDOW    = NO
FORMULA_FONTSIZE       = 10
FORMULA_TRANSPARENT    = YES
USE_MATHJAX            = NO
MATHJAX_FORMAT         = HTML-CSS
MATHJAX_RELPATH        = https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5/
MATHJAX_EXTENSIONS     =
MATHJAX_CODEFILE       =
SEARCHENGINE           = YES
SERVER_BASED_SEARCH    = NO
EXTERNAL_SEARCH        = NO
SEARCHENGINE_URL       =
SEARCHDATA_FILE        = searchdata.xml
EXTERNAL_SEARCH_ID     =
EXTRA_SEARCH_MAPPINGS  =

#---------------------------------------------------------------------------
# Configuration options related to the LaTeX output
#---------------------------------------------------------------------------
GENERATE_LATEX         = YES
LATEX_OUTPUT           = latex
LATEX_CMD_NAME         =
MAKEINDEX_CMD_NAME     = makeindex
LATEX_MAKEINDEX_CMD    = \makeindex
COMPACT_LATEX          = NO
PAPER_TYPE             = a4
EXTRA_PACKAGES         =
LATEX_HEADER           =
LATEX_FOOTER           =
LATEX_EXTRA_STYLESHEET =
LATEX_EXTRA_FILES      =
PDF_HYPERLINKS         = YES
USE_PDFLATEX           = YES
LATEX_BATCHMODE        = NO
LATEX_HIDE_INDICES     = NO
LATEX_SOURCE_CODE      = NO
LATEX_BIB_STYLE        = plain
LATEX_TIMESTAMP        = NO
LATEX_EMOJI_DIRECTORY  =

#---------------------------------------------------------------------------
# Configuration options related to the RTF output
#---------------------------------------------------------------------------
GENERATE_RTF           = NO
RTF_OUTPUT             = rtf
COMPACT_RTF            = NO
RTF_HYPERLINKS         = NO
RTF_STYLESHEET_FILE    =
RTF_EXTENSIONS_FILE    =
RTF_SOURCE_CODE        = NO

#---------------------------------------------------------------------------
# Configuration options related to the man page output
#---------------------------------------------------------------------------
GENERATE_MAN           = NO
MAN_OUTPUT             = man
MAN_EXTENSION          = .3
MAN_SUBDIR             =
MAN_LINKS              = NO

#---------------------------------------------------------------------------
# Configuration options related to the XML output
#---------------------------------------------------------------------------
GENERATE_XML           = NO
XML_OUTPUT             = xml
XML_PROGRAMLISTING     = YES
XML_NS_MEMB_FILE_SCOPE = NO

#---------------------------------------------------------------------------
# Configuration options related to the DOCBOOK output
#---------------------------------------------------------------------------
GENERATE_DOCBOOK       = NO
DOCBOOK_OUTPUT         = docbook
DOCBOOK_PROGRAMLISTING = NO

#---------------------------------------------------------------------------
# Configuration options for the AutoGen Definitions output
#---------------------------------------------------------------------------
GENERATE_AUTOGEN_DEF   = NO

#---------------------------------------------------------------------------
# Configuration options related to the Perl module output
#---------------------------------------------------------------------------
GENERATE_PERLMOD       = NO
PERLMOD_LATEX          = NO
PERLMOD_PRETTY         = YES
PERLMOD_MAKEVAR_PREFIX =

#---------------------------------------------------------------------------
# Configuration options related to the preprocessor
#---------------------------------------------------------------------------
ENABLE_PREPROCESSING   = YES
MACRO_EXPANSION        = NO
EXPAND_ONLY_PREDEF     = NO
SEARCH_INCLUDES        = YES
INCLUDE_PATH           =
INCLUDE_FILE_PATTERNS  =
PREDEFINED             =
EXPAND_AS_DEFINED      =
SKIP_FUNCTION_MACROS   = YES

#---------------------------------------------------------------------------
# Configuration options related to external references
#---------------------------------------------------------------------------
TAGFILES               =
GENERATE_TAGFILE       =
ALLEXTERNALS           = NO
EXTERNAL_GROUPS        = YES
EXTERNAL_PAGES         = YES
PERL_PATH              = /usr/bin/perl

#---------------------------------------------------------------------------
# Configuration options related to the dot tool
#---------------------------------------------------------------------------
CLASS_DIAGRAMS         = YES
MSCGEN_PATH            =
DIA_PATH               =
HIDE_UNDOC_RELATIONS   = YES
HAVE_DOT               = NO
DOT_NUM_THREADS        = 0
DOT_FONTNAME           = Helvetica
DOT_FONTSIZE           = 10
DOT_FONTPATH           =
CLASS_GRAPH            = YES
COLLABORATION_GRAPH    = YES
GROUP_GRAPHS           = YES
UML_LOOK               = NO
UML_LIMIT_NUM_FIELDS   = 10
TEMPLATE_RELATIONS     = NO
INCLUDE_GRAPH          = YES
INCLUDED_BY_GRAPH      = YES
CALL_GRAPH             = NO
CALLER_GRAPH           = NO
GRAPHICAL_HIERARCHY    = YES
DIRECTORY_GRAPH        = YES
DOT_IMAGE_FORMAT       = png
INTERACTIVE_SVG        = NO
DOT_PATH               =
DOTFILE_DIRS           =
MSCFILE_DIRS           =
DIAFILE_DIRS           =
PLANTUML_JAR_PATH      =
PLANTUML_CFG_FILE      =
PLANTUML_INCLUDE_PATH  =
DOT_GRAPH_MAX_NODES    = 50
MAX_DOT_GRAPH_DEPTH    = 0
DOT_TRANSPARENT        = NO
DOT_MULTI_TARGETS      = NO
GENERATE_LEGEND        = YES
DOT_CLEANUP            = YES
    - uses: /*******************************************************************************
 *        fpga_overview.doc        Main overview page for code
 *******************************************************************************
 */
/**
\
\version 1.0.0
\date 2020-06-16

\mainpage

\section Introduction
This directory contains the Dune NvmeStorage system that provides data storage on NVMe devices as available on a Xilinx FPGA.
The directory contains the FPGA VHDL source code, VHDL simulation environment, build environment and test software for the NVME test FPGA firmware as well as the nvme_test host software.

See the Readme.pdf file for more information as well as the
<a href="../../DuneNvmeStorageManual.pdf">DuneNvmeStorageManual</a>
and <a href="../../DuneNvmeStorageDesign.pdf">DuneNvmeStorageDesign</a> documents.

\section Directories
<table>
<tr><td>src</td><td>The main VHDL source code</td></tr>
<tr><td>src/ip</td><td>FPGA IP block definitions</td></tr>
<tr><td>sim</td><td>The simulation environment</td></tr>
<tr><td>vivado</td><td>The build environment</td></tr>
<tr><td>test</td><td>Host test programs accessing the FPGA firmware</td></tr>
<tr><td>docsrc</td><td>Source for the documentation</td></tr>
<tr><td>doc</td><td>Output for the documentation</td></tr>
</table>

\section The NvmeSorage FPGA test example
The DuneNvme FPGA bit file for the test example can be built using the Xilinx Vivado tools.

\section Building the FPGA bit file and programming
<ol>
<li>cd vivado </li>
<li>make clean</li>
<li>make all</li>
<li>make program</li>
</ol>

\section To Simulate FPGA

<ol>
<li>cd sim</li>
<li>Edit Makefile and testbench files for simulation required</li>
<li>make</li>
<li>make view</li>
</ol>

*/
    - name: # Doxyfile 1.8.15

PROJECT_NAME           = "DuneNvme"
PROJECT_NUMBER                = 1.0.0
PROJECT_BRIEF          = "This is a simple NVMe test environment that allows experimentation with the low level PCIe NVMe interfaces as available on a Xilinx FPGA environment."
PROJECT_LOGO           =
INPUT                  = host_overview.dox ../test
OUTPUT_DIRECTORY       = ../doc/host
OPTIMIZE_OUTPUT_VHDL   = NO
HTML_EXTRA_STYLESHEET  = beam_doxygen.css

#---------------------------------------------------------------------------
# Project related configuration options
#---------------------------------------------------------------------------
DOXYFILE_ENCODING      = UTF-8
CREATE_SUBDIRS         = NO
ALLOW_UNICODE_NAMES    = NO
OUTPUT_LANGUAGE        = English
OUTPUT_TEXT_DIRECTION  = None
BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
ABBREVIATE_BRIEF       = "The $name class" \
                         "The $name widget" \
                         "The $name file" \
                         is \
                         provides \
                         specifies \
                         contains \
                         represents \
                         a \
                         an \
                         the

ALWAYS_DETAILED_SEC    = NO
INLINE_INHERITED_MEMB  = NO
FULL_PATH_NAMES        = YES
STRIP_FROM_PATH        =
STRIP_FROM_INC_PATH    =
SHORT_NAMES            = NO
JAVADOC_AUTOBRIEF      = NO
QT_AUTOBRIEF           = NO
MULTILINE_CPP_IS_BRIEF = NO
INHERIT_DOCS           = YES
SEPARATE_MEMBER_PAGES  = NO
TAB_SIZE               = 4
ALIASES                =
TCL_SUBST              =
OPTIMIZE_OUTPUT_FOR_C  = NO
OPTIMIZE_OUTPUT_JAVA   = NO
OPTIMIZE_FOR_FORTRAN   = NO
#OPTIMIZE_OUTPUT_VHDL   = NO
OPTIMIZE_OUTPUT_SLICE  = NO
EXTENSION_MAPPING      =
MARKDOWN_SUPPORT       = YES
TOC_INCLUDE_HEADINGS   = 0
AUTOLINK_SUPPORT       = YES
BUILTIN_STL_SUPPORT    = NO
CPP_CLI_SUPPORT        = NO
SIP_SUPPORT            = NO
IDL_PROPERTY_SUPPORT   = YES
DISTRIBUTE_GROUP_DOC   = NO
GROUP_NESTED_COMPOUNDS = NO
SUBGROUPING            = YES
INLINE_GROUPED_CLASSES = NO
INLINE_SIMPLE_STRUCTS  = NO
TYPEDEF_HIDES_STRUCT   = NO
LOOKUP_CACHE_SIZE      = 0

#---------------------------------------------------------------------------
# Build related configuration options
#---------------------------------------------------------------------------
EXTRACT_ALL            = NO
EXTRACT_PRIVATE        = NO
EXTRACT_PACKAGE        = NO
EXTRACT_STATIC         = NO
EXTRACT_LOCAL_CLASSES  = YES
EXTRACT_LOCAL_METHODS  = NO
EXTRACT_ANON_NSPACES   = NO
HIDE_UNDOC_MEMBERS     = NO
HIDE_UNDOC_CLASSES     = NO
HIDE_FRIEND_COMPOUNDS  = NO
HIDE_IN_BODY_DOCS      = NO
INTERNAL_DOCS          = NO
CASE_SENSE_NAMES       = YES
HIDE_SCOPE_NAMES       = NO
HIDE_COMPOUND_REFERENCE= NO
SHOW_INCLUDE_FILES     = YES
SHOW_GROUPED_MEMB_INC  = NO
FORCE_LOCAL_INCLUDES   = NO
INLINE_INFO            = YES
SORT_MEMBER_DOCS       = YES
SORT_BRIEF_DOCS        = NO
SORT_MEMBERS_CTORS_1ST = NO
SORT_GROUP_NAMES       = NO
SORT_BY_SCOPE_NAME     = NO
STRICT_PROTO_MATCHING  = NO
GENERATE_TODOLIST      = YES
GENERATE_TESTLIST      = YES
GENERATE_BUGLIST       = YES
GENERATE_DEPRECATEDLIST= YES
ENABLED_SECTIONS       =
MAX_INITIALIZER_LINES  = 30
SHOW_USED_FILES        = YES
SHOW_FILES             = YES
SHOW_NAMESPACES        = YES
FILE_VERSION_FILTER    =
LAYOUT_FILE            =
CITE_BIB_FILES         =
QUIET                  = NO
WARNINGS               = YES
WARN_IF_UNDOCUMENTED   = YES
WARN_IF_DOC_ERROR      = YES
WARN_NO_PARAMDOC       = NO
WARN_AS_ERROR          = NO
WARN_FORMAT            = "$file:$line: $text"
WARN_LOGFILE           =

#---------------------------------------------------------------------------
# Configuration options related to the input files
#---------------------------------------------------------------------------
#INPUT                  = ../src
INPUT_ENCODING         = UTF-8
FILE_PATTERNS          = *.c \
                         *.cc \
                         *.cxx \
                         *.cpp \
                         *.c++ \
                         *.java \
                         *.ii \
                         *.ixx \
                         *.ipp \
                         *.i++ \
                         *.inl \
                         *.idl \
                         *.ddl \
                         *.odl \
                         *.h \
                         *.hh \
                         *.hxx \
                         *.hpp \
                         *.h++ \
                         *.cs \
                         *.d \
                         *.php \
                         *.php4 \
                         *.php5 \
                         *.phtml \
                         *.inc \
                         *.m \
                         *.markdown \
                         *.md \
                         *.mm \
                         *.dox \
                         *.py \
                         *.pyw \
                         *.f90 \
                         *.f95 \
                         *.f03 \
                         *.f08 \
                         *.f \
                         *.for \
                         *.tcl \
                         *.vhd \
                         *.vhdl \
                         *.ucf \
                         *.qsf \
                         *.ice

RECURSIVE              = YES
EXCLUDE                =
EXCLUDE_SYMLINKS       = NO
EXCLUDE_PATTERNS       =
EXCLUDE_SYMBOLS        =
EXAMPLE_PATH           =
EXAMPLE_PATTERNS       = *
EXAMPLE_RECURSIVE      = NO
IMAGE_PATH             =
INPUT_FILTER           =
FILTER_PATTERNS        =
FILTER_SOURCE_FILES    = NO
FILTER_SOURCE_PATTERNS =
USE_MDFILE_AS_MAINPAGE =

#---------------------------------------------------------------------------
# Configuration options related to source browsing
#---------------------------------------------------------------------------
SOURCE_BROWSER         = NO
INLINE_SOURCES         = NO
STRIP_CODE_COMMENTS    = YES
REFERENCED_BY_RELATION = NO
REFERENCES_RELATION    = NO
REFERENCES_LINK_SOURCE = YES
SOURCE_TOOLTIPS        = YES
USE_HTAGS              = NO
VERBATIM_HEADERS       = YES

#---------------------------------------------------------------------------
# Configuration options related to the alphabetical class index
#---------------------------------------------------------------------------
ALPHABETICAL_INDEX     = YES
COLS_IN_ALPHA_INDEX    = 5
IGNORE_PREFIX          =

#---------------------------------------------------------------------------
# Configuration options related to the HTML output
#---------------------------------------------------------------------------
GENERATE_HTML          = YES
HTML_OUTPUT            = html
HTML_FILE_EXTENSION    = .html
HTML_HEADER            =
HTML_FOOTER            =
HTML_STYLESHEET        =
#HTML_EXTRA_STYLESHEET  =
HTML_EXTRA_FILES       =
HTML_COLORSTYLE_HUE    = 220
HTML_COLORSTYLE_SAT    = 100
HTML_COLORSTYLE_GAMMA  = 80
HTML_TIMESTAMP         = NO
HTML_DYNAMIC_MENUS     = YES
HTML_DYNAMIC_SECTIONS  = NO
HTML_INDEX_NUM_ENTRIES = 100
GENERATE_DOCSET        = NO
DOCSET_FEEDNAME        = "Doxygen generated docs"
DOCSET_BUNDLE_ID       = org.doxygen.Project
DOCSET_PUBLISHER_ID    = org.doxygen.Publisher
DOCSET_PUBLISHER_NAME  = Publisher
GENERATE_HTMLHELP      = NO
CHM_FILE               =
HHC_LOCATION           =
GENERATE_CHI           = NO
CHM_INDEX_ENCODING     =
BINARY_TOC             = NO
TOC_EXPAND             = NO
GENERATE_QHP           = NO
QCH_FILE               =
QHP_NAMESPACE          = org.doxygen.Project
QHP_VIRTUAL_FOLDER     = doc
QHP_CUST_FILTER_NAME   =
QHP_CUST_FILTER_ATTRS  =
QHP_SECT_FILTER_ATTRS  =
QHG_LOCATION           =
GENERATE_ECLIPSEHELP   = NO
ECLIPSE_DOC_ID         = org.doxygen.Project
DISABLE_INDEX          = NO
GENERATE_TREEVIEW      = NO
ENUM_VALUES_PER_LINE   = 4
TREEVIEW_WIDTH         = 250
EXT_LINKS_IN_WINDOW    = NO
FORMULA_FONTSIZE       = 10
FORMULA_TRANSPARENT    = YES
USE_MATHJAX            = NO
MATHJAX_FORMAT         = HTML-CSS
MATHJAX_RELPATH        = https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5/
MATHJAX_EXTENSIONS     =
MATHJAX_CODEFILE       =
SEARCHENGINE           = YES
SERVER_BASED_SEARCH    = NO
EXTERNAL_SEARCH        = NO
SEARCHENGINE_URL       =
SEARCHDATA_FILE        = searchdata.xml
EXTERNAL_SEARCH_ID     =
EXTRA_SEARCH_MAPPINGS  =

#---------------------------------------------------------------------------
# Configuration options related to the LaTeX output
#---------------------------------------------------------------------------
GENERATE_LATEX         = YES
LATEX_OUTPUT           = latex
LATEX_CMD_NAME         =
MAKEINDEX_CMD_NAME     = makeindex
LATEX_MAKEINDEX_CMD    = \makeindex
COMPACT_LATEX          = NO
PAPER_TYPE             = a4
EXTRA_PACKAGES         =
LATEX_HEADER           =
LATEX_FOOTER           =
LATEX_EXTRA_STYLESHEET =
LATEX_EXTRA_FILES      =
PDF_HYPERLINKS         = YES
USE_PDFLATEX           = YES
LATEX_BATCHMODE        = NO
LATEX_HIDE_INDICES     = NO
LATEX_SOURCE_CODE      = NO
LATEX_BIB_STYLE        = plain
LATEX_TIMESTAMP        = NO
LATEX_EMOJI_DIRECTORY  =

#---------------------------------------------------------------------------
# Configuration options related to the RTF output
#---------------------------------------------------------------------------
GENERATE_RTF           = NO
RTF_OUTPUT             = rtf
COMPACT_RTF            = NO
RTF_HYPERLINKS         = NO
RTF_STYLESHEET_FILE    =
RTF_EXTENSIONS_FILE    =
RTF_SOURCE_CODE        = NO

#---------------------------------------------------------------------------
# Configuration options related to the man page output
#---------------------------------------------------------------------------
GENERATE_MAN           = NO
MAN_OUTPUT             = man
MAN_EXTENSION          = .3
MAN_SUBDIR             =
MAN_LINKS              = NO

#---------------------------------------------------------------------------
# Configuration options related to the XML output
#---------------------------------------------------------------------------
GENERATE_XML           = NO
XML_OUTPUT             = xml
XML_PROGRAMLISTING     = YES
XML_NS_MEMB_FILE_SCOPE = NO

#---------------------------------------------------------------------------
# Configuration options related to the DOCBOOK output
#---------------------------------------------------------------------------
GENERATE_DOCBOOK       = NO
DOCBOOK_OUTPUT         = docbook
DOCBOOK_PROGRAMLISTING = NO

#---------------------------------------------------------------------------
# Configuration options for the AutoGen Definitions output
#---------------------------------------------------------------------------
GENERATE_AUTOGEN_DEF   = NO

#---------------------------------------------------------------------------
# Configuration options related to the Perl module output
#---------------------------------------------------------------------------
GENERATE_PERLMOD       = NO
PERLMOD_LATEX          = NO
PERLMOD_PRETTY         = YES
PERLMOD_MAKEVAR_PREFIX =

#---------------------------------------------------------------------------
# Configuration options related to the preprocessor
#---------------------------------------------------------------------------
ENABLE_PREPROCESSING   = YES
MACRO_EXPANSION        = NO
EXPAND_ONLY_PREDEF     = NO
SEARCH_INCLUDES        = YES
INCLUDE_PATH           =
INCLUDE_FILE_PATTERNS  =
PREDEFINED             =
EXPAND_AS_DEFINED      =
SKIP_FUNCTION_MACROS   = YES

#---------------------------------------------------------------------------
# Configuration options related to external references
#---------------------------------------------------------------------------
TAGFILES               =
GENERATE_TAGFILE       =
ALLEXTERNALS           = NO
EXTERNAL_GROUPS        = YES
EXTERNAL_PAGES         = YES
PERL_PATH              = /usr/bin/perl

#---------------------------------------------------------------------------
# Configuration options related to the dot tool
#---------------------------------------------------------------------------
CLASS_DIAGRAMS         = YES
MSCGEN_PATH            =
DIA_PATH               =
HIDE_UNDOC_RELATIONS   = YES
HAVE_DOT               = NO
DOT_NUM_THREADS        = 0
DOT_FONTNAME           = Helvetica
DOT_FONTSIZE           = 10
DOT_FONTPATH           =
CLASS_GRAPH            = YES
COLLABORATION_GRAPH    = YES
GROUP_GRAPHS           = YES
UML_LOOK               = NO
UML_LIMIT_NUM_FIELDS   = 10
TEMPLATE_RELATIONS     = NO
INCLUDE_GRAPH          = YES
INCLUDED_BY_GRAPH      = YES
CALL_GRAPH             = NO
CALLER_GRAPH           = NO
GRAPHICAL_HIERARCHY    = YES
DIRECTORY_GRAPH        = YES
DOT_IMAGE_FORMAT       = png
INTERACTIVE_SVG        = NO
DOT_PATH               =
DOTFILE_DIRS           =
MSCFILE_DIRS           =
DIAFILE_DIRS           =
PLANTUML_JAR_PATH      =
PLANTUML_CFG_FILE      =
PLANTUML_INCLUDE_PATH  =
DOT_GRAPH_MAX_NODES    = 50
MAX_DOT_GRAPH_DEPTH    = 0
DOT_TRANSPARENT        = NO
DOT_MULTI_TARGETS      = NO
GENERATE_LEGEND        = YES
DOT_CLEANUP            = YES
      run: /*******************************************************************************
 *        host_overview.doc        Main overview page for code
 *******************************************************************************
 */
/**
\author Dr Terry Barnaby
\version 1.0.0
\date 2020-06-16

\mainpage

\section Introduction
This directory contains the Dune NvmeStorage system that provides data storage on NVMe devices as available on a Xilinx FPGA.
The directory contains the FPGA VHDL source code, VHDL simulation environment, build environment and test software for the NVME test FPGA firmware as well as the nvme_test host software.

See the Readme.pdf file for more information as well as the
<a href="../../DuneNvmeStorageTestSoftwarel.pdf">DuneNvmeStorageTestSoftware</a>
 the, <a href="../../DuneNvmeStorageManual.pdf">DuneNvmeStorageManual</a>
and <a href="../../DuneNvmeStorageDesign.pdf">DuneNvmeStorageDesign</a> documents.

\section Directories
<table>
<tr><td>src</td><td>The main VHDL source code</td></tr>
<tr><td>src/ip</td><td>FPGA IP block definitions</td></tr>
<tr><td>sim</td><td>The simulation environment</td></tr>
<tr><td>vivado</td><td>The build environment</td></tr>
<tr><td>test</td><td>Host test programs accessing the FPGA firmware</td></tr>
<tr><td>docsrc</td><td>Source for the documentation</td></tr>
<tr><td>doc</td><td>Output for the documentation</td></tr>
</table>

\section The Test Program
The test Linux host program is in the test directory and is called nvme_test. This program communicates with an NVMe devices
through the FPGA connected to the host machines PCIe bus. This program provides the ability to test the NvmeStorage module as
well as allowing experimentation with the NVMe low level PCIe interface.
Included is a simple Linux driver, bfpga, that interfaces to the Xilinx PCIe DMA core.
See the <a href="../../DuneNvmeStorageManual.pdf">DuneNvmeStorageManual</a> for details on the test software and its usage.

\section Building the test software
<ol>
<li>cd test</li>
<li>make clean</li>
<li>make driver</li>
<li>make</li>
</ol>

\section Building the FPGA bit file and programming
<ol>
<li>cd vivado </li>
<li>make clean</li>
<li>make all</li>
<li>make program</li>
</ol>

\section To Simulate FPGA

<ol>
<li>cd sim</li>
<li>Edit Makefile and testbench files for simulation required</li>
<li>make</li>
<li>make view</li>
</ol>

*/
    - name: Run tests
      run: cargo test --verbose
