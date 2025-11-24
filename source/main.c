/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the Neural Network Profiler Example
*              for ModusToolbox.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2021-2025, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#include "cybsp.h"
#include "cy_retarget_io.h"
#include "ml_validation.h"

#include "elapsed_timer.h"

#include MTB_ML_INCLUDE_MODEL_FILE(MODEL_NAME)

/*******************************************************************************
* Macros
********************************************************************************/

#ifdef USE_STREAM_DATA
    #define UART_DEFAULT_STREAM_BAUD_RATE   (1000000u)
#else
    #define UART_DEFAULT_STREAM_BAUD_RATE   (115200u)
#endif /* USE_STREAM_DATA */

/* Choose which profiling to enable. Options: 
 *  MTB_ML_PROFILE_DISABLE
 *  MTB_ML_PROFILE_ENABLE_MODEL
 *  MTB_ML_LOG_ENABLE_MODEL_LOG 
 */
#define PROFILE_CONFIGURATION       MTB_ML_PROFILE_ENABLE_MODEL

/* The delay time in milliseconds used to wait for device to print UART messages */
#define DELAY_TIME_MS                 (50u)

/* MTB ML Block priority if using NPU */
#define MTB_ML_PRIORITY               (3)

/*******************************************************************************
* Function Prototypes
********************************************************************************/
static inline void handle_error(void)
{
    /* Disable all interrupts. */
    __disable_irq();

    CY_ASSERT(0);
}
/*******************************************************************************
 * Function Name: cm55_ml_profiler_task
 ********************************************************************************
 * Summary:
 * This is the ML task for CM55. It does...
 *    1. Print welcome message
 *    2. Initialize a timer for cycle counting
 *    3. Initialize the a local or stream regression task
 *    4. Execute the inference engine
 *
 * Parameters:
 *  void * context passed from main function
 *
 * Return:
 *  void
 *
 *******************************************************************************/
static void cm4_ml_profiler_task(void * arg)
{
    cy_rslt_t result;
    CY_UNUSED_PARAMETER(arg);

    mtb_ml_model_bin_t model_bin = {MTB_ML_MODEL_BIN_DATA(MODEL_NAME)};

    /* Initialize retarget-io to use the debug UART port */
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, UART_DEFAULT_STREAM_BAUD_RATE);

#ifdef USE_STREAM_DATA
    /* Data streaming object */
    mtb_data_streaming_interface_t data_stream_obj;
    /* ML stream objects */
    mtb_ml_stream_tag_t stream_tag;
    mtb_ml_stream_interface_t stream_interface = {
        .interface_obj = &data_stream_obj,
        .stream_tag = &stream_tag,
    };
#endif /* USE_STREAM_DATA */

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    printf("****************** "
           "PSOC 6 MCU: Machine Learning Profiler on CM4 "
           "****************** \r\n\n");

    /* Initialize the ModusToolbox ML middleware */
    mtb_ml_init(MTB_ML_PRIORITY);

    result = ml_validation_init(PROFILE_CONFIGURATION, &model_bin);

    if(CY_RSLT_SUCCESS != result)
    {
        printf("ERROR: initialization of the ML validation failed!\r\n");
        handle_error();
    }

    /* Initialize the elapsed timer */
    result = elapsed_timer_init();

    if(CY_RSLT_SUCCESS != result)
    {
        printf("ERROR: initialization of elapsed timer failed!\r\n");
        handle_error();
    }

    for (;;)
    {
#ifdef USE_STREAM_DATA
        result = ml_validation_stream_task(&stream_interface);
#else
        result = ml_validation_local_task();
#endif /* USE_STREAM_DATA */

        if (CY_RSLT_SUCCESS == result)
        {
            printf("\n\rProfiling completed!\n\r");
        }
        else
        {
            printf("\n\rProfiling task failed!\n\r");
        }

    #ifndef USE_STREAM_DATA
        /* Wait for the device to print out the results */
        Cy_SysLib_Delay(DELAY_TIME_MS);

        /* Only run the local regression once */
        while (1)
        {
            Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
        }
    #endif /* USE_STREAM_DATA */

        printf("Restarting...\r\n");
    }
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CM4 application. 
* 
* It sets up a machine learning model to be profiled. It can use local 
* regression data or stream data.
* 
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    cm4_ml_profiler_task(NULL);
    return 0;
}

/* [] END OF FILE */
