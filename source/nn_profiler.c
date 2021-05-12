/******************************************************************************
* File Name:   nn_profiler.c
*
* Description: This file contains the implementation of the neural network
*  profiler.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2021, Cypress Semiconductor Corporation (an Infineon company) or
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
#include "nn_profiler.h"
#include "nn_utils.h"
#include "nn.h"

#include "cyhal.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

/* Include model files */
#include INCLUDE_FILE(MODEL_NAME, MODEL_FILE_EXT)
#include INCLUDE_FILE(MODEL_NAME, X_DATA_FILE_EXT)
#include INCLUDE_FILE(MODEL_NAME, Y_DATA_FILE_EXT)

/*******************************************************************************
* Typedefs
*******************************************************************************/
typedef struct 
{
    const char          *name;
    const unsigned char *weights;
    const unsigned char *parameters;
    const unsigned char *x_regression_data;
    const unsigned char *y_regression_data;
} nn_model_t;

/*******************************************************************************
* Constants
*******************************************************************************/
#define SUCCESS_RATE       98.0f

/*******************************************************************************
* Global Variables
*******************************************************************************/
/* Model information */
nn_model_t nn_profiler_model =
{
    .name              = EXPAND_AND_STRINGIFY(MODEL_NAME),
    .weights           = MODEL_DATA(MODEL_NAME, MODEL_PTR_EXT),
    .parameters        = MODEL_DATA(MODEL_NAME, MODEL_PARAM_EXT),
    .x_regression_data = MODEL_DATA(MODEL_NAME, X_DATA_PTR_EXT),
    .y_regression_data = MODEL_DATA(MODEL_NAME, Y_DATA_PTR_EXT)
};

/* Neural Network Object */
nn_obj_t nn_profiler_obj;

/* Global Timer for Profiling */
cyhal_timer_t nn_profiler_timer;

/* Timer count in seconds */
uint32_t nn_profiler_timer_sec = 0;

/* Timer clock HZ */
uint32_t nn_profiler_timer_hz;

/* CM4 system clock vs. Peripheral clock ratio */
int nn_profiler_cm4_clk_ratio;

/* Output/result buffers for the inference engine */
NN_OUT_Type  *nn_result_buffer;
float        *nn_float_buffer;

/*******************************************************************************
* Local Functions
*******************************************************************************/
void nn_profiler_timer_init(void);
void nn_profiler_timer_callback(void *callback_arg, cyhal_timer_event_t event);

/*******************************************************************************
* Function Name: nn_profiler_init
********************************************************************************
* Summary:
*   Initialize the Neural Network based on the given model.
*
* Parameters:
*   nn: model info ouput
*   model_param: pointer to the model parameters
*   model_data: pointer to the model data
*
* Return:
*   The status of the initialization.
*******************************************************************************/
cy_rslt_t nn_profiler_init(void)
{
    cy_rslt_t result;

    /* Initialize the neural network */
    result = nn_init(&nn_profiler_obj, 
             (void *) nn_profiler_model.parameters, 
             (void *) nn_profiler_model.weights);
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }

    /* Allocate memory for the output buffers */
    nn_result_buffer = (NN_OUT_Type *) malloc(nn_profiler_obj.model_xx_info.n_out_classes * sizeof(NN_OUT_Type));
    if (nn_result_buffer == NULL)
    {
        return NN_RESULT_ALLOC_ERR;
    };
#if CY_ML_FIXED_POINT
    nn_float_buffer =  (float *) malloc(nn_profiler_obj.model_xx_info.n_out_classes * sizeof(NN_OUT_Type));
    if (nn_float_buffer == NULL)
    {
        return NN_RESULT_ALLOC_ERR;
    };
#endif

    printf("Model Name: %s\n\n\r", nn_profiler_model.name);

    /* Initialize the internal timer for profiling */
    nn_profiler_timer_init();

    return CY_RSLT_SUCCESS;
}

/*******************************************************************************
* Function Name: nn_profiler_run
********************************************************************************
* Summary:
*   Run the Neural Network Inference Engine based on the regression data.
*
* Parameters:
*   nn: neural network
*   input: input data
*
* Return:
*   The status of the initialization.
*******************************************************************************/
cy_rslt_t nn_profiler_run(void)
{
    /* Regression pointers */
    NN_IN_Type  *input_reference;
    int         *output_reference;

    uint32_t     num_loop;
    uint32_t     input_size;
    uint32_t     correct_result = 0;
    bool         test_result;
    uint32_t     total_count = 0;

    /* Point to regression data */
    input_reference  = (NN_IN_Type *) nn_profiler_model.x_regression_data;
    output_reference = (int *)        nn_profiler_model.y_regression_data;

    /* Get the number of loops for this regression */
    num_loop = *((int *) input_reference); 
    input_reference += (sizeof(int)/sizeof(NN_IN_Type));

    /* Get the number of inputs of the NN */
    input_size = *((int *) input_reference); 
    input_reference += (sizeof(int)/sizeof(NN_IN_Type));

    /* If using Fixed-point, get the Q value */
#if CY_ML_FIXED_POINT
    nn_profiler_obj.fixed_point_q = *((int *) input_reference);
    input_reference += (sizeof(int)/sizeof(NN_IN_Type));
#endif

    if (input_size != nn_profiler_obj.model_xx_info.input_sz)
    {
        printf("Input buffer size error, input size=%d, model input size=%d, aborting...\r\n", 
                (int) input_size, (int) nn_profiler_obj.model_xx_info.input_sz);
        return NN_RESULT_ERROR;
    }

    /* The following loop runs for number of examples used in regression */
    for (int j = 0; j < num_loop; j++)
    {
        /* Feed the neural network */
        nn_feed(&nn_profiler_obj, input_reference);

        /* Get the result */       
        nn_result(&nn_profiler_obj, nn_result_buffer);

#if CY_ML_FIXED_POINT_16_IN
        /* Convert 16bit fixed-point output to floating-point for visualization/regression */
        nn_utils_convert_int16_to_flt(nn_result_buffer,
                                      nn_float_buffer,
                                      nn_profiler_obj.model_xx_info.n_out_classes,
                                      nn_profiler_obj.in_out_fixed_point_q);
#elif CY_ML_FIXED_POINT_8_IN
        /* Convert 8bit fixed-point output to floating-point for visualization/regression */
        nn_utils_convert_int8_to_flt(nn_result_buffer,
                                      nn_float_buffer,
                                      nn_profiler_obj.model_xx_info.n_out_classes,
                                      nn_profiler_obj.in_out_fixed_point_q);
#else
        nn_float_buffer = nn_result_buffer;
#endif

        /* Check if the results are accurate enough */
        if (nn_utils_find_max_index_flt(nn_float_buffer, nn_profiler_obj.model_xx_info.n_out_classes) == 
            nn_utils_find_max_index_int(output_reference, nn_profiler_obj.model_xx_info.n_out_classes))
        {
            correct_result++;
        }

        /* Increment buffers */
        input_reference  += input_size;
        output_reference += nn_profiler_obj.model_xx_info.n_out_classes;

        total_count++;
    }

    /* Print PASS or FAIL with Accuracy percentage 
     * Only for regression ... */
    {
        float success_rate;
        char pass[] = "PASS";
        char fail[] = "FAIL";

        /* Check if total count is different than ZERO */
        if (total_count == 0)
        {
            success_rate = 0;
        }
        else
        {
            success_rate = correct_result * 100.0f / total_count;
        }
        

        test_result = (success_rate >= SUCCESS_RATE);

        Cy_ML_Profile_Close(nn_profiler_obj.model_xx_obj, test_result);
        
        printf("\r\n***************************************************\r\n");
        if (test_result == true)
        {
            printf("%s with accuracy percentage =%3.2f, total_cnt=%d", pass, success_rate, (int) total_count);
        }
        else
        {
            printf("%s with accuracy percentage =%3.2f, total_cnt=%d", fail, success_rate, (int) total_count);
        }
        printf("\r\n***************************************************\r\n");
    }

    return CY_RSLT_SUCCESS;
}

/*******************************************************************************
* Function Name: nn_profiler_timer_init
********************************************************************************
* Summary:
* This function creates and configures a Timer object. The timer ticks 
* continuously and produces a periodic interrupt on every terminal count 
* event. The period is defined by the 'period' and 'compare_value' of the 
* timer configuration structure 'timer_cfg'. Without any changes, 
* this application is designed to produce an interrupt every 1 second.
*
* Parameters:
*  none
*
*******************************************************************************/
 void nn_profiler_timer_init(void)
 {
    cy_rslt_t result;
    uint32_t peri_bus_freq;

    cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,                 /* Timer compare value, not used */
        .period = 0,                        /* Defines the timer period */
        .direction = CYHAL_TIMER_DIR_UP,    /* Timer counts up */
        .is_compare = false,                /* Don't use compare mode */
        .is_continuous = true,              /* Run timer indefinitely */
        .value = 0                          /* Initial value of counter */
    };

    peri_bus_freq = Cy_SysClk_ClkPeriGetFrequency();
    nn_profiler_cm4_clk_ratio = SystemCoreClock / peri_bus_freq;

    /* Set timer clock equal to peripheral bus frequency which is half of CM4 maximum clock frequency */
    nn_profiler_timer_hz = peri_bus_freq;
    timer_cfg.period = nn_profiler_timer_hz - 1;

    /* Initialize the timer object. Does not use pin output ('pin' is NC) and
     * does not use a pre-configured clock source ('clk' is NULL). */
    result = cyhal_timer_init(&nn_profiler_timer, NC, NULL);

    /* timer init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Configure timer period and operation mode such as count direction, 
       duration */
    cyhal_timer_configure(&nn_profiler_timer, &timer_cfg);

    /* Set the frequency of timer's clock source */
    cyhal_timer_set_frequency(&nn_profiler_timer, nn_profiler_timer_hz);

    /* Assign the ISR to execute on timer interrupt */
    cyhal_timer_register_callback(&nn_profiler_timer, 
                                   nn_profiler_timer_callback, NULL);

    /* Set the event on which timer interrupt occurs and enable it */
    cyhal_timer_enable_event(&nn_profiler_timer, CYHAL_TIMER_IRQ_TERMINAL_COUNT,
                              CYHAL_ISR_PRIORITY_DEFAULT, true);

    /* Start the timer with the configured settings */
    cyhal_timer_start(&nn_profiler_timer);
 }

 /*******************************************************************************
* Function Name: nn_profiler_timer_callback
********************************************************************************
* Summary:
* This is the interrupt handler function for the profiler timer.
*
* Parameters:
*    callback_arg   Arguments passed to the interrupt callback
*    event          Timer/counter interrupt triggers
*
*******************************************************************************/
void nn_profiler_timer_callback(void *callback_arg, cyhal_timer_event_t event)
{
    (void) callback_arg;
    (void) event;

    nn_profiler_timer_sec++;  //count # of seconds
}

/*******************************************************************************
* Function Name: Cy_ML_Profile_Get_Tsc
********************************************************************************
* Summary:
* This the implementation used by the CY_ML for profiling. It returns the 
*  current time in cycles.
*
* Parameters:
*    val            current time in cycles
*
*******************************************************************************/
int Cy_ML_Profile_Get_Tsc(uint32_t *val)
{
    uint32_t cycles;
    
    cycles = cyhal_timer_read(&nn_profiler_timer) + 
                               nn_profiler_timer_hz * nn_profiler_timer_sec;
    /* Convert to CM4 clock cycle number which is twice of peripheral bus
       clock cycles*/
    *val = nn_profiler_cm4_clk_ratio * cycles; 
    
    return 0;
}
