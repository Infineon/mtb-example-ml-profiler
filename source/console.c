/******************************************************************************
* File Name:   console.c
*
* Description: This file contains the implementation of the command console.
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
#include "console.h"

#include "command_console.h"
#include "cy_retarget_io.h"

#include "cy_sysint.h"
#include "cy_utils.h"

#include "nn.h"
#include "nn_profiler.h"

/*******************************************************************************
* Constants
*******************************************************************************/
/* Command console buffers */
#define CONSOLE_COMMAND_MAX_PARAMS     (32)
#define CONSOLE_COMMAND_MAX_LENGTH     (256)
#define CONSOLE_COMMAND_HISTORY_LENGTH (10)

/* List of console commands */
#define CONSOLE_COMMANDS \
    { "run", console_run, 0, NULL, NULL, (char *)"", (char *)"Run regression on the inference engine"}, \
    { "profile_model", console_profile_model, 1, NULL, NULL, (char *)"", (char *)"Enable (1) / Disable (0) the NN model profiling."}, \
    { "profile_layer", console_profile_layer, 1, NULL, NULL, (char *)"", (char *)"Enable (1) / Disable (0) the layer profiling."}, \
    { "print_output", console_print_output, 1, NULL, NULL, (char *)"", (char *)"Enable (1) / Disable (0) output printing."}, \
    { "profile_per_frame", console_profile_per_frame, 1, NULL, NULL, (char *)"", (char *)"Enable (1) / Disable (0) per frame profiling."}, \
    { "profile_all", console_profile_all, 1, NULL, NULL, (char *)"", (char *)"Enable (1) / Disable (0) all profiling."}, \

/*******************************************************************************
* Local Functions
*******************************************************************************/
int console_run(int argc, char *argv[], tlv_buffer_t **data);
int console_profile_model(int argc, char *argv[], tlv_buffer_t **data);
int console_profile_layer(int argc, char *argv[], tlv_buffer_t **data);
int console_print_output(int argc, char *argv[], tlv_buffer_t **data);
int console_profile_per_frame(int argc, char *argv[], tlv_buffer_t **data);
int console_profile_all(int argc, char *argv[], tlv_buffer_t **data);

/*******************************************************************************
* Global Variables
*******************************************************************************/
/* Command table */
const cy_command_console_cmd_t console_command_table[] =
 {
    CONSOLE_COMMANDS
    CMD_TABLE_END
 };

/* Console buffer variables */
static char command_buffer[CONSOLE_COMMAND_MAX_LENGTH];
static char command_history_buffer[CONSOLE_COMMAND_MAX_LENGTH * CONSOLE_COMMAND_HISTORY_LENGTH];
const char* console_delimiter_string = " ";

/*******************************************************************************
* Function Name: console_init
********************************************************************************
* Summary:
*   Initialize the console to process commands.
*
*******************************************************************************/
void console_init(void)
{
    cy_command_console_cfg_t console_cfg;
    cy_rslt_t result;

    result = nn_profiler_init();
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Error initializing the NN Profiler: %x\n\r", (unsigned int) result);
        CY_ASSERT(0);
    }

    console_cfg.serial             = (void *)&cy_retarget_io_uart_obj;
    console_cfg.line_len           = sizeof(command_buffer);
    console_cfg.buffer             = command_buffer;
    console_cfg.history_len        = CONSOLE_COMMAND_HISTORY_LENGTH;
    console_cfg.history_buffer_ptr = command_history_buffer;
    console_cfg.delimiter_string   = console_delimiter_string;
    console_cfg.params_num         = CONSOLE_COMMAND_MAX_PARAMS;

    /* Initialize command console library */
    result = cy_command_console_init(&console_cfg);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Error Console Init: %x\n\r", (unsigned int) result);
        CY_ASSERT(0);
    }

    result = cy_command_console_add_table( console_command_table );
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Error Add table: %x\n\r", (unsigned int) result);
        CY_ASSERT(0);
    }
}

/*******************************************************************************
* Function Name: console_run
********************************************************************************
* Summary:
*   Run the inference engine.
*
* Parameters:
*   argc: number of arguments
*   argv: list of arguments
*   data: not used
*
* Return
*   zero
*******************************************************************************/
int console_run(int argc, char *argv[], tlv_buffer_t **data)
{
    nn_profiler_run();

    return 0;
}

/*******************************************************************************
* Function Name: console_profile_model
********************************************************************************
* Summary:
*   Enable/Disable the NN model profiling.
*
* Parameters:
*   argc: number of arguments
*   argv: list of arguments
*   data: not used
*
* Return
*   zero
*******************************************************************************/
int console_profile_model(int argc, char *argv[], tlv_buffer_t **data)
{
    if (*argv[1] == '0')
    {
        Cy_ML_Enable_Model_Profile(false);
        printf("Disabling model profiling\n\r");
    }
    else if (*argv[1] == '1')
    {
        Cy_ML_Enable_Model_Profile(true);
        printf("Enabling model profiling\n\r");
    }
    else
    {
        return ERR_BAD_ARG;
    }

    return ERR_CMD_OK;
}

/*******************************************************************************
* Function Name: console_profile_layer
********************************************************************************
* Summary:
*   Enable/Disable the layer profiling.
*
* Parameters:
*   argc: number of arguments
*   argv: list of arguments
*   data: not used
*
* Return
*   zero
*******************************************************************************/
int console_profile_layer(int argc, char *argv[], tlv_buffer_t **data)
{
    if (*argv[1] == '0')
    {
        Cy_ML_Enable_Layer_Profile(false);
        printf("Disabling layer profiling\n\r");
    }
    else if (*argv[1] == '1')
    {
        Cy_ML_Enable_Layer_Profile(true);
        printf("Enabling layer profiling\n\r");
    }
    else
    {
        return ERR_BAD_ARG;
    }

    return ERR_CMD_OK;
}

/*******************************************************************************
* Function Name: console_print_output
********************************************************************************
* Summary:
*   Enable/Disable the output printing.
*
* Parameters:
*   argc: number of arguments
*   argv: list of arguments
*   data: not used
*
* Return
*   zero
*******************************************************************************/
int console_print_output(int argc, char *argv[], tlv_buffer_t **data)
{
    if (*argv[1] == '0')
    {
        Cy_ML_Enable_Model_Output(false);
        printf("Disabling out layer profiling\n\r");
    }
    else if (*argv[1] == '1')
    {
        Cy_ML_Enable_Model_Output(true);
        printf("Enabling out layer profiling\n\r");
    }
    else
    {
        return ERR_BAD_ARG;
    }

    return ERR_CMD_OK;
}

/*******************************************************************************
* Function Name: console_profile_per_frame
********************************************************************************
* Summary:
*   Enable/Disable the per frame profiling.
*
* Parameters:
*   argc: number of arguments
*   argv: list of arguments
*   data: not used
*
* Return
*   zero
*******************************************************************************/
int console_profile_per_frame(int argc, char *argv[], tlv_buffer_t **data)
{
    if (*argv[1] == '0')
    {
        Cy_ML_Enable_Per_Frame_Profile(false);
        printf("Disabling per frame profiling\n\r");
    }
    else if (*argv[1] == '1')
    {
        Cy_ML_Enable_Per_Frame_Profile(true);
        printf("Enabling per frame profiling\n\r");
    }
    else
    {
        return ERR_BAD_ARG;
    }

    return ERR_CMD_OK;
}

/*******************************************************************************
* Function Name: console_profile_all
********************************************************************************
* Summary:
*   Enable/Disable all profiling.
*
* Parameters:
*   argc: number of arguments
*   argv: list of arguments
*   data: not used
*
* Return
*   zero
*******************************************************************************/
int console_profile_all(int argc, char *argv[], tlv_buffer_t **data)
{
    if (*argv[1] == '0')
    {
        Cy_ML_Enable_Model_Profile(false);
        Cy_ML_Enable_Per_Frame_Profile(false);
        Cy_ML_Enable_Layer_Profile(false);
        Cy_ML_Enable_Model_Output(false);
        printf("Disabling all profiling\n\r");
    }
    else if (*argv[1] == '1')
    {
        Cy_ML_Enable_Model_Profile(true);
        Cy_ML_Enable_Per_Frame_Profile(true);
        Cy_ML_Enable_Layer_Profile(true);
        Cy_ML_Enable_Model_Output(true);
        printf("Enabling all profiling\n\r");
    }
    else
    {
        return ERR_BAD_ARG;
    }

    return ERR_CMD_OK;
}

/* [] END OF FILE */