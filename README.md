# Machine Learning: neural network profiler

This code example demonstrates how to run through the ModusToolbox Machine Learning (MTB-ML) development flow with PSoC 6 MCU, where the end user has a pre-trained Neural Network (NN) model, which can be profiled and validated at the PC and target device.

You can import a pre-trained model using the MTB-ML Configurator Tool, create an embedded, optimized version of this model, and validate the performance on the PC. After this, the files generated are pre-integrated with this code example, so you can run the same validation data as well as profile performance when the ML model is deployed to the PSoC 6 MCU.

**Figure 1. MTB-ML development flow**

![](images/mtb-ml-flow.png)

For more details about the ModusToolbox Machine Learning solution, see this [link](https://www.cypress.com/products/modustoolbox/machine-learning).

[Provide feedback on this code example.](https://cypress.co1.qualtrics.com/jfe/form/SV_1NTns53sK2yiljn?Q_EED=eyJVbmlxdWUgRG9jIElkIjoiQ0UyMzI1NTEiLCJTcGVjIE51bWJlciI6IjAwMi0zMjU1MSIsIkRvYyBUaXRsZSI6Ik1hY2hpbmUgTGVhcm5pbmc6IG5ldXJhbCBuZXR3b3JrIHByb2ZpbGVyIiwicmlkIjoicmxvcyIsIkRvYyB2ZXJzaW9uIjoiMS4xLjAiLCJEb2MgTGFuZ3VhZ2UiOiJFbmdsaXNoIiwiRG9jIERpdmlzaW9uIjoiTUNEIiwiRG9jIEJVIjoiSUNXIiwiRG9jIEZhbWlseSI6IlBTT0MifQ==)

## Requirements

- [ModusToolbox&trade; software](https://www.cypress.com/products/modustoolbox-software-environment) v2.3 with ModusToolbox Tools Patch 2.3.1
- Board support package (BSP) minimum required version: 2.0.0
- Programming language: C
- Associated parts: All [PSoC 6 MCU](http://www.cypress.com/PSoC6) parts

## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm® embedded compiler v9.3.1 (`GCC_ARM`) - Default value of `TOOLCHAIN`
- Arm compiler v6.13 (`ARM`)
- IAR C/C++ compiler v8.42.2 (`IAR`)

## Supported kits (make variable 'TARGET')

- [PSoC 6 Wi-Fi Bluetooth&reg; prototyping kit](https://www.cypress.com/CY8CPROTO-062-4343W) (`CY8CPROTO-062-4343W`) - Default value of `TARGET`
- [PSoC 6 Wi-Fi Bluetooth pioneer kit](https://www.cypress.com/CY8CKIT-062-WiFi-BT) (`CY8CKIT-062-WIFI-BT`)
- [PSoC 62S2 Wi-Fi Bluetooth pioneer kit](https://www.cypress.com/CY8CKIT-062S2-43012) (`CY8CKIT-062S2-43012`)
- [PSoC 62S1 Wi-Fi Bluetooth pioneer kit](https://www.cypress.com/CYW9P62S1-43438EVB-01) (`CYW9P62S1-43438EVB-01`)
- [PSoC 62S1 Wi-Fi Bluetooth pioneer kit](https://www.cypress.com/CYW9P62S1-43012EVB-01) (`CYW9P62S1-43012EVB-01`)
- [PSoC 62S3 Wi-Fi Bluetooth prototyping kit](https://www.cypress.com/CY8CPROTO-062S3-4343W) (`CY8CPROTO-062S3-4343W`)
- [PSoC 64 "Secure Boot" Wi-Fi Bluetooth pioneer kit](http://www.cypress.com/CY8CKIT-064B0S2-4343W) (`CY8CKIT-064B0S2-4343W`)

## Hardware setup

This example uses the board's default configuration. See the kit user guide to ensure that the board is configured correctly.

**Note:** The PSoC 6 Bluetooth LE Pioneer Kit (CY8CKIT-062-BLE) and the PSoC 6 Wi-Fi Bluetooth pioneer kit (CY8CKIT-062-WIFI-BT) ship with KitProg2 installed. The ModusToolbox software requires KitProg3. Before using this code example, make sure that the board is upgraded to KitProg3. The tool and instructions are available in the [Firmware Loader](https://github.com/cypresssemiconductorco/Firmware-loader) GitHub repository. If you do not upgrade, you will see an error like "unable to find CMSIS-DAP device" or "KitProg firmware is out of date".

## Software setup

Install a terminal emulator if you don't have one. Instructions in this document use [Tera Term](https://ttssh2.osdn.jp/index.html.en).

If using TeraTerm, go to menu **Setup** > **Terminal**. Set the **New-line** > **Receive** to *LF*. This fixes the break lines issue when running the command console.

Use the MTB-ML Configurator tool (from *{ModusToolbox install directory}/tools_{version}/ml-configurator/*) to load a pre-trained NN model and generate C files to be used with this code example. Alternatively, you can launch the configurator tool in Eclipse IDE for ModusToolbox from the Quick Launch window. For more information, see the *ModusToolbox Machine Learning User Guide*. Note that you need to install the ModusToolbox 2.3.1 patch.

By default, the Makefile uses a model that comes with the code example. The pre-trained NN model is located in the *pretrained_models* folder. You can use the MTB-ML Configurator to link to this file, or load another model file and generate C files for the target device.

By default, the output files location is set to *mtb_ml_gen*. The project name is set to *test_model*. If you change any of these default settings, edit the following Makefile parameters of this code example:

| Makefile parameter | Description |
| :--------   | :-------- |
| `NN_TYPE=`  | Defines the NN weights and input data format. It can be float, int16x16, int16x8, or int8x8. |
| `NN_MODEL_NAME=` | Defines the name of the model. The name comes from the *project name* defined in the ML Configurator tool. |
| `NN_MODEL_FOLDER=` | Sets the name where the model files will be placed. The name comes from the *output file location* defined in the MTB-ML Configurator tool. |

<br>

To validate the given model with the MTB-ML Configurator tool, click the **Validate in Desktop** button. By default, the project uses testing data stored in the file located at *test_data/test_data.csv*.

Alternatively, you can use a random dataset structure to validate the model. You can click the **Validate** button to initiate the validation. Note that this will generate the regression data files, which is stored in the internal memory of the target device. Because of that, limit the sample count to 100 in the MTB-ML Configurator tool.

## Using the code example

Create the project and open it using one of the following:

<details><summary><b>In Eclipse IDE for ModusToolbox</b></summary>

1. Click the **New Application** link in the **Quick Panel** (or, use **File** > **New** > **ModusToolbox Application**). This launches the [Project Creator](http://www.cypress.com/ModusToolboxProjectCreator) tool.

2. Pick a kit supported by the code example from the list shown in the **Project Creator - Choose Board Support Package (BSP)** dialog.

   When you select a supported kit, the example is reconfigured automatically to work with the kit. To work with a different supported kit later, use the [Library Manager](https://www.cypress.com/ModusToolboxLibraryManager) to choose the BSP for the supported kit. You can use the Library Manager to select or update the BSP and firmware libraries used in this application. To access the Library Manager, click the link from the **Quick Panel**.

   You can also just start the application creation process again and select a different kit.

   If you want to use the application for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work.

3. In the **Project Creator - Select Application** dialog, choose the example by enabling the checkbox.

4. Optionally, change the suggested **New Application Name**.

5. Enter the local path in the **Application(s) Root Path** field to indicate where the application needs to be created.

   Applications that can share libraries can be placed in the same root path.

6. Click **Create** to complete the application creation process.

For more details, see the [Eclipse IDE for ModusToolbox User Guide](https://www.cypress.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox install directory}/ide_{version}/docs/mt_ide_user_guide.pdf*).

</details>

<details><summary><b>In command-line interface (CLI)</b></summary>

ModusToolbox provides the Project Creator as both a GUI tool and a command line tool to easily create one or more ModusToolbox applications. See the "Project Creator Tools" section of the [ModusToolbox User Guide](https://www.cypress.com/ModusToolboxUserGuide) for more details.

Alternatively, you can manually create the application using the following steps:

1. Download and unzip this repository onto your local machine, or clone the repository.

2. Open a CLI terminal and navigate to the application folder.

   On Windows, use the command line "modus-shell" program provided in the ModusToolbox installation instead of a standard Windows command line application. This shell provides access to all ModusToolbox tools. You can access it by typing `modus-shell` in the search box in the Windows menu.

   In Linux and macOS, you can use any terminal application.

   **Note:** The cloned application contains a default BSP file (*TARGET_xxx.mtb*) in the *deps* folder. Use the [Library Manager](https://www.cypress.com/ModusToolboxLibraryManager) (`make modlibs` command) to select and download a different BSP file, if required. If the selected kit does not have the required resources or is not [supported](#supported-kits-make-variable-target), the application may not work.

3. Import the required libraries by executing the `make getlibs` command.

Various CLI tools include a `-h` option that prints help information to the terminal screen about that tool. For more details, see the [ModusToolbox User Guide](https://www.cypress.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>

<details><summary><b>In Third-party IDEs</b></summary>

1. Follow the instructions from the **In command-line interface (CLI)** section to create the application, and import the libraries using the `make getlibs` command.

2. Export the application to a supported IDE using the `make <ide>` command.

   For a list of supported IDEs and more details, see the "Exporting to IDEs" section of the [ModusToolbox User Guide](https://www.cypress.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox install directory}/docs_{version}/mtb_user_guide.pdf*.

3. Follow the instructions displayed in the terminal to create or import the application as an IDE project.

</details>

<br>

## Operation

If using a PSoC 64 "Secure" MCU kit (like CY8CKIT-064B0S2-4343W), the PSoC 64 device must be provisioned with keys and policies before being programmed. Follow the instructions in the ["Secure Boot" SDK user guide](https://www.cypress.com/documentation/software-and-drivers/psoc-64-secure-mcu-secure-boot-sdk-user-guide) to provision the device. If the kit is already provisioned, copy-paste the keys and policy folder to the application folder.

1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector.

2. Open a terminal program and select the KitProg3 COM port. Set the serial port parameters to 8N1 and 115200 baud.

3. Program the board using one of the following:

   <details><summary><b>Using Eclipse IDE for ModusToolbox</b></summary>

      1. Select the application project in the Project Explorer.

      2. In the **Quick Panel**, scroll down, and click **\<Application Name> Program (KitProg3_MiniProg4)**.
   </details>

   <details><summary><b>Using CLI</b></summary>

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target. You can specify a target and toolchain manually:
      ```
      make program TARGET=<BSP> TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make program TARGET=CY8CPROTO-062-4343W TOOLCHAIN=GCC_ARM
      ```
   </details>

4. After programming, the application starts automatically. Confirm that "Neural Network Profiler" and the model name are printed on the UART terminal.

5. Type `help` to list supported commands. Use `profile_all 1` to enable profiling, and then use the `run` command to start profiling.

6. Alternatively, enable the desired profiles by using the `profile_model`, `profile_layer`, `print_output`, or `profile_per_frame` commands, and then use the `run` command.

   **Note:** Depending on the chosen quantization, the `run` command might return a failure if the accuracy is not greater than or equal to 98%.

## Debugging

You can debug the example to step through the code. In the IDE, use the **\<Application Name> Debug (KitProg3_MiniProg4)** configuration in the **Quick Panel**. For more details, see the "Program and Debug" section in the [Eclipse IDE for ModusToolbox User Guide](https://www.cypress.com/MTBEclipseIDEUserGuide).

**Note:** **(Only while debugging)** On the CM4 CPU, some code in `main()` may execute before the debugger halts at the beginning of `main()`. This means that some code executes twice - once before the debugger stops execution, and again after the debugger resets the program counter to the beginning of `main()`. See [KBA231071](https://community.cypress.com/docs/DOC-21143) to learn about this and for the workaround.

## Design and implementation

In this example, you must provide a pre-trained NN model with the weights, regression data, and the model parameters. The MTB-ML Configurator tool can generate such data based on the standard Keras H5 format. By default, these files are located in the *./mtb_ml_gen* folder. The code examples also provides an MTB-ML Configurator tool project file - *design.mtbml*, which points to the pre-trained NN model available in the *pretrained_models* folder.

If you change the output file location in the MTB-ML Configurator tool, you must also reflect the change in the Makefile (the `NN_MODEL_FOLDER` parameter). The model data is stored as a header file or as a binary file (used for filesystem applications). This example uses header files. Depending the type of NN model chosen in the Makefile (the `NN_TYPE` parameter), the application uses the files and variables from the table below (*KEY* is the output file prefix defined by the MTB-ML Configurator tool):

**NN Type: float**

| Folder name | File name | Variable name | Description |
| :--------   | :-------- | :------------- | :------------ |
| *mtb_ml_models* | *KEY_model_all.h* | `KEY_model_flt_bin` | Contains the NN weights in float |
| *mtb_ml_regression_data* | *KEY_x_data_flt.h*<br>*KEY_y_data_flt.h* |  `KEY_x_data_flt_bin`<br>`KEY_y_data_flt_bin` | Contains the input (x) and output (y) regression data in float |


<br>

**NN Type: int16x16**

| Folder name | File name | Variable name | Description |
| :--------   | :-------- | :------------- | :------------ |
| *mtb_ml_models* | *KEY_model_all.h* | `KEY_model_fixed16_bin` | Contains the NN weights in 16-bit format |
| *mtb_ml_regression_data* | *KEY_x_data_fixed.h*<br>*KEY_y_data_fixed.h* | `KEY_x_data_fixed16_bin`<br>`KEY_y_data_fixed_bin` | Contains the input (x) and output (y) regression data in 16-bit format |


<br>

**NN Type: int16x8**

| Folder name | File name | Variable name | Description |
| :--------   | :-------- | :------------- | :------------ |
| *mtb_ml_models* | *KEY_model_all.h* | `KEY_model_fixed8_bin` | Contains the NN weights in 8-bit format |
| *mtb_ml_regression_data* | *KEY_x_data_fixed.h*<br>*KEY_y_data_fixed.h* | `KEY_x_data_fixed16_bin`<br>`KEY_y_data_fixed_bin` | Contains the input (x) and output (x) regression data in 16-bit format |


<br>

**NN Type: int8x8**

| Folder name | File name | Variable name |  Description |
| :--------   | :-------- | :------------- | :------------ |
| *mtb_ml_models* | *KEY_model_all.h* | `KEY_model_fixed8_bin` | Contains the NN weights in 8-bit format |
| *mtb_ml_regression_data* | *KEY_x_data_fixed.h*<br>*KEY_y_data_fixed.h* | `KEY_x_data_fixed8_bin`<br>`KEY_y_data_fixed_bin` | Contains the input (x) regression data in 8-bit format and output (x) regression data in 16-bit format |


<br>

Note that the model parameter file is located at *mtb_ml_models/KEY_model_all.h* and the variable name is `KEY_model_prms_bin` for all NN types. Set the `NN_MODEL_NAME` parameter in the Makefile based on the project name chosen in the ML Configurator tool.

After updating the Makefile, all the model files are built into the application automatically, allowing the NN inference engine to be initialized and fed with the regression data.

This application uses FreeRTOS. There are two tasks in the system - *console task* and the *NN task*. They communicate with each other through queues. Regression data is fed to the input queue, and the result is sent to the output queue.

**Figure 2. RTOS block diagram**

![](images/rtos-block-diagram.png)

This scheme makes it easy to port the NN task to any RTOS-based application. The *console task* can be substituted by a real application task that collects the data and feeds the NN inference engine through the input queue. Another task can then read the result data through the output queue and process it.

The console task supports the following commands:

Command | Description
--------|------------
`run`| Feeds the regression data to the NN inference engine, then gets the results, compares it with the reference, and prints the accuracy.
`profile_model 1\|0` | Enables/disables NN model profiling. It prints general information about the NN model, such as average cycle, peak cycle, and peak frame.
`profile_layer 1\|0` | Enables/disables layer profiling. It prints general information of each layer of the NN model, such as the average cycle, peak cycle, and peak frame.
`print_output 1\|0`| Enables/disables output printing. It prints the output values generated by the inference engine. It is converted to floating point.
`profile_per_frame 1\|0` | Enables/disables per frame profiling. It prints general information for each frame, such as the number of cycles.
`profile_all 1\|0`| Enables/disables all the profiling above.

<br>

When executing the `run` command, it loads the regression data generated by the ML Configurator tool. The regression data consists of a collection of inputs (X) and a collection of outputs (Y). Once the inference engine processes X, it outputs the result. Then the firmware compares the result with the desired value, Y. If these match, the firmware contributes to the accuracy calculation.

All other commands enable/disable some of the profiling features. In this code example, the Makefile defines a macro called `ML_PROFILER`. These commands have an effect only if this macro is defined. To properly calculate the number of cycles it takes to process the inference engine, the application needs to implement the `Cy_ML_Profile_Get_Tsc` function, which returns the current time in cycles.

Before running the NN task, the firmware must parse the NN model, allocate memory, and initialize the inference engine.

**Figure 3. NN flowchart**

![](images/nn-flowchart.png)

When allocating memory, consider the following:

Parameter |Description
----------|-----------
scratch memory| The amount of memory required is extracted from the model in the first step from the flowchart.
persistent memory| The amount of memory required is extracted from the model in the first step from the flowchart.
input buffer| The input buffer for the queue depends on the NN Type and the number of nodes in the input layer.
output buffer| The output buffer for the queue depends on the NN Type and the number of nodes in the output layer.

<br>

When using FreeRTOS in Heap_3 mode, as in this example, the `malloc()` function uses the FreeRTOS heap to allocate the memory. Therefore, special attention needs to be taken when setting the `configTOTAL_HEAP_SIZE` in *FreeRTOSConfig.h*. You can extract information about the scratch and persistent memory of the model when `profile_model` is enabled. The amount of SRAM memory required by the NN Inference Engine can be roughly calculated as:

```
TOTAL SRAM Memory = Scratch Memory + Persistent Memory + Input Layer Size + Output Layer Size + NN Task Stack Size
```

The NN model weights and biases are stored in the flash, so it doesn't affect the FreeRTOS heap size.

### Files and folders

```
|-- mtb_ml_gen/         # Contains the model and regression files
|-- pretrained_models/  # Contains the H5 format model (used by the ML Configurator tool)
|-- test_data/          # Contains a CSV file with test data
|-- command_console/    # Middleware library to implement the command console
|-- source              # Contains the source code files for this example
   |- console.c/h       # Implements the console task
   |- nn_profiler.c/h   # Implements profiling wrapper functions
   |- nn_utils.c/h      # Provides utils functions to manipulate the NN inputs/outputs
   |- nn.c/h            # Implements the NN task, which executes the inference engine
|-- FreeRTOSConfig.h    # FreeRTOS configuration file
|-- design.mtbml        # MTB-ML Configurator tool project file
```

When starting a new project that requires the ML inference library, the only files you might leverage from this project are *nn.c/h* and *nn_utils.h/c*.

### Resources and settings

**Table 1. Application resources**

| Resource  |  Alias/Object     |    Purpose     |
| :------- | :------------    | :------------ |
| UART (HAL)|cy_retarget_io_uart_obj| UART HAL object used by Retarget-IO for Debug UART port  |

<br>

## Related resources

| Application notes                                            |                                                              |
| :----------------------------------------------------------- | :----------------------------------------------------------- |
| [AN228571](https://www.cypress.com/AN228571) – Getting started with PSoC 6 MCU on ModusToolbox | Describes PSoC 6 MCU devices and how to build your first application with ModusToolbox |
| [AN221774](https://www.cypress.com/AN221774) – Getting started with PSoC 6 MCU on PSoC Creator | Describes PSoC 6 MCU devices and how to build your first application with PSoC Creator |
| [AN210781](https://www.cypress.com/AN210781) – Getting started with PSoC 6 MCU with Bluetooth Low Energy connectivity on PSoC Creator | Describes PSoC 6 MCU with Bluetooth LE connectivity devices and how to build your first application with PSoC Creator |
| [AN215656](https://www.cypress.com/AN215656) – PSoC 6 MCU: dual-CPU sstem design | Describes the dual-CPU architecture in PSoC 6 MCU, and shows how to build a simple dual-CPU design |
| **Code examples**                                            |                                                              |
| [Using ModusToolbox](https://github.com/cypresssemiconductorco/Code-Examples-for-ModusToolbox-Software) | [Using PSoC Creator](https://www.cypress.com/documentation/code-examples/psoc-6-mcu-code-examples) |
| **Device documentation**                                     |                                                              |
| [PSoC 6 MCU datasheets](https://www.cypress.com/search/all?f[0]=meta_type%3Atechnical_documents&f[1]=resource_meta_type%3A575&f[2]=field_related_products%3A114026) | [PSoC 6 technical reference manuals](https://www.cypress.com/search/all/PSoC%206%20Technical%20Reference%20Manual?f[0]=meta_type%3Atechnical_documents&f[1]=resource_meta_type%3A583) |
| **Development kits**                                         | Buy at www.cypress.com                                       |
| [CY8CKIT-062-BLE](https://www.cypress.com/CY8CKIT-062-BLE) PSoC 6 Bluetooth LE pioneer kit | [CY8CKIT-062-WiFi-BT](https://www.cypress.com/CY8CKIT-062-WiFi-BT) PSoC 6 Wi-Fi Bluetooth pioneer kit |
| [CY8CPROTO-063-BLE](https://www.cypress.com/CY8CPROTO-063-BLE) PSoC 6 Bluetooth LE prototyping kit | [CY8CPROTO-062-4343W](https://www.cypress.com/CY8CPROTO-062-4343W) PSoC 6 Wi-Fi Bluetooth prototyping kit |
| [CY8CKIT-062S2-43012](https://www.cypress.com/CY8CKIT-062S2-43012) PSoC 62S2 Wi-Fi Bluetooth pioneer kit | [CY8CPROTO-062S3-4343W](https://www.cypress.com/CY8CPROTO-062S3-4343W) PSoC 62S3 Wi-Fi Bluetooth prototyping kit |
| [CYW9P62S1-43438EVB-01](https://www.cypress.com/CYW9P62S1-43438EVB-01) PSoC 62S1 Wi-Fi Bluetooth pioneer kit | [CYW9P62S1-43012EVB-01](https://www.cypress.com/CYW9P62S1-43012EVB-01) PSoC 62S1 Wi-Fi Bluetooth pioneer kit |
| [CY8CKIT-064B0S2-4343W](http://www.cypress.com/CY8CKIT-064B0S2-4343W) PSoC 64 "Secure Boot" Wi-Fi Bluetooth pioneer kit | [CY8CKIT-064S0S2-4343W](http://www.cypress.com/CY8CKIT-064S0S2-4343W) PSoC 64 "Standard Secure" AWS Wi-Fi Bluetooth pioneer kit           |
| **Libraries**                                                |                                                              |
| PSoC 6 peripheral driver library (PDL) and docs  | [mtb-pdl-cat1](https://github.com/cypresssemiconductorco/mtb-pdl-cat1) on GitHub |
| Hardware abstraction layer (HAL) Library and docs    | [mtb-hal-cat1](https://github.com/cypresssemiconductorco/mtb-hal-cat1) on GitHub |
| Retarget IO - A utility library to retarget the standard input/output (STDIO) messages to a UART port | [retarget-io](https://github.com/cypresssemiconductorco/retarget-io) on GitHub |
| **Middleware**                                               |                                                              |
| CapSense&trade; library and docs                                   | [capsense](https://github.com/cypresssemiconductorco/capsense) on GitHub |
| Links to all PSoC 6 MCU middleware                           | [psoc6-middleware](https://github.com/cypresssemiconductorco/psoc6-middleware) on GitHub |
| **Tools**                                                    |                                                              |
| [Eclipse IDE for ModusToolbox](https://www.cypress.com/modustoolbox) | The cross-platform, Eclipse-based IDE for IoT designers that supports application configuration and development targeting converged MCU and wireless systems. |
| [PSoC Creator™](https://www.cypress.com/products/psoc-creator-integrated-design-environment-ide) | The IDE for PSoC and FM0+ MCU development. |

<br>

## Other resources

Cypress provides a wealth of data at www.cypress.com to help you select the right device, and quickly and effectively integrate it into your design.

For PSoC 6 MCU devices, see [How to design with PSoC 6 MCU - KBA223067](https://community.cypress.com/docs/DOC-14644) in the Cypress community.

## Document history

Document title: *CE232551* - *Machine Learning: neural network profiler*

| Version | Description of Change |
| ------- | --------------------- |
| 1.0.0   | New code example      |
| 1.1.0   | Added test_data.csv file for regression |


<br>

All other trademarks or registered trademarks referenced herein are the property of their respective owners.

![banner](images/ifx-cy-banner.png)

-------------------------------------------------------------------------------

© Cypress Semiconductor Corporation, 2021. This document is the property of Cypress Semiconductor Corporation, an Infineon Technologies company, and its affiliates ("Cypress").  This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress’s patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
<br>
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product.  CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach").  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications.  To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document.  Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device.  You shall indemnify and hold Cypress, including its affiliates, and its directors, officers, employees, agents, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device.  Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress’s published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
<br>
Cypress, the Cypress logo, and combinations thereof, WICED, ModusToolBox, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress or a subsidiary of Cypress in the United States or in other countries.  For a more complete list of Cypress trademarks, visit cypress.com.  Other names and brands may be claimed as property of their respective owners.
