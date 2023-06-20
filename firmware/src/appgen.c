/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    appgen.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "appgen.h"
#include "app.h"
#include "Mc32DriverLcd.h"
#include "Inputs.h"
#include "Generator.h"
#include "Mc32NVMUtil.h"
#include "I2C_Seeprom.h"
#include "MenuGen.h"
#include "Mc32gestSpiDac.h"
#include "I2C_Seeprom.h"
#include "Mc32gest_SerComm.h"
#include "Mc32Delays.h"
#include "system_config/pic32mx_eth_sk2/framework/driver/tmr/drv_tmr_static.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APPGEN_DATA appgenData;

IPV4_ADDR  ipAddr;

S_ParamGen LocalParamGen;
S_ParamGen RemoteParamGen;

bool tcpStat, flagIp;

 // *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APPGEN_Initialize ( void )

  Remarks:
    See prototype in appgen.h.
 */

void APPGEN_Initialize ( void )
{
    DRV_TMR0_Initialize();
    DRV_TMR1_Initialize();
    
    /* Place the App state machine in its initial state. */
    /* Place the App state machine in its initial state. */
    appgenData.state = APPGEN_STATE_INIT;
    appgenData.newCharReceived = false;
    appgenData.newChar = 0;
    appgenData.state_service = SERVICE_STATE_START;
    appgenData.cycles = 0;
}


/******************************************************************************
  Function:
    void APPGEN_Tasks ( void )

  Remarks:
    See prototype in appgen.h.
 */

void APPGEN_Tasks ( void )
{
    static uint16_t counterS9 = 0;
    
    /* Check the application's current state. */
    switch ( appgenData.state )
    {
        /* Application's initial state. */
        case APPGEN_STATE_INIT:
        {
            /* Initial display */            
            lcd_init();           
            lcd_bl_on();
            
            lcd_gotoxy(1,1);
            printf_lcd("TP5_IpGen_2022-23");
            lcd_gotoxy(1,2);
            printf_lcd("Subramaniyam");
            lcd_gotoxy(1,3);
            printf_lcd("Decrausaz");
                     
            /* Peripherals initialisation */            
            InputsInit();
                 
            I2C_InitMCP79411();
            SPI_InitLTC2604();
            
            GENSIG_Initialize(&LocalParamGen);
            RemoteParamGen = LocalParamGen;

            /* Timers start */
            DRV_TMR0_Start();
            DRV_TMR1_Start();
            
            /* Going to wait until next interrupt */
            appgenData.state = APPGEN_STATE_WAIT;
           break;
        }
        
        case APPGEN_STATE_WAIT:
        {
            //Ne rien faire
            break;
        }
        

        case APPGEN_STATE_SERVICE_TASKS:
        {
            /* Increment number of cycles */
            appgenData.cycles++;
            
            /* Service tasks state machine */
            switch(appgenData.state_service)
            {
                /* First time in service tasks waits for 3 sec ****************/
                case SERVICE_STATE_START:
                {
                    /* Waiting to reach a certain time */
                    if(appgenData.cycles > START_NB_CYCLES)
                    {
                        /* Reset cycles */
                        appgenData.cycles = 0;
                        appgenData.state_service = SERVICE_STATE_RUN;
                    }
                    break;
                }
                /**************************************************************/
                
                /* Service tasks running every interrupt cycle after 3sec *****/
                case SERVICE_STATE_RUN:
                {
                    ScanAllInputs();
                    
                    /* Go to save menu when button released */
                    if( BtnSavePress() )
                    {
                        BtnSaveClearPress();
                        appgenData.state_service = SERVICE_STATE_SAVE_WAIT;
                    }

                    /* Update display every 10 cycles */
                    if(appgenData.cycles > RUN_NB_CYCLES)
                    {
                        if(flagIp == 1)
                        {
                          flagIp = 0;
                          appgenData.state_service = SERVICE_STATE_ETH_LCDIP; 
                        }
                        /* Menu execution */
                        if(tcpStat)
                        {    
                            MENU_Execute(&RemoteParamGen, false);
                        }
                        else
                        { 
                            MENU_Execute(&LocalParamGen, true);
                        }
                        /* Reset number of cycles */
                        appgenData.cycles = 0;
                    }                    
                    break;
                }
                /**************************************************************/
                /* Execute save when condition reached ************************/
                case SERVICE_STATE_ETH_LCDIP:
                {
                    lcd_ClearLine(1);
                    lcd_ClearLine(2);
                    lcd_ClearLine(3);
                    lcd_ClearLine(4);
                    
                    //ajout SCA : affichage adr. IP   
                    lcd_gotoxy(8,2);
                    printf_lcd("Adr. IP");
                       
                    lcd_gotoxy(2,3);
                    printf_lcd("IP:%03d.%03d.%03d.%03d ", ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);
                        
                    /* Next state */
                    appgenData.state_service = SERVICE_STATE_ETH_WAIT;
                    appgenData.cycles = 0;
                    
                    break;
                }
                /**************************************************************/
                
                /* Temporary state when save has been executed ****************/
                case SERVICE_STATE_ETH_WAIT:
                {                  
                    /* Waiting for a certain time after executing save */
                    if(appgenData.cycles >= 5000)
                    {
                        
                        MENU_Initialize(&RemoteParamGen);
                        
                        /* Going back to RUN state */
                        appgenData.state_service = SERVICE_STATE_RUN;
                    }
                    break;
                }

                /* Temporary state waiting for button to be FULLY released ****/
                case SERVICE_STATE_SAVE_WAIT:
                {
                    /* Scan save button */
                    ScanBtnSave();
                    
                    /* When button is released */
                    if( BtnSaveRelease() )
                    {
                        BtnSaveClearRelease();
                        
                        /* Clear all LCD lines */
                        lcd_ClearLine(1);
                        lcd_ClearLine(2);
                        lcd_ClearLine(3);
                        lcd_ClearLine(4);

                        /* Display the save menu */
                        lcd_gotoxy(1,2);
                        printf_lcd("     SAUVEGARDE     ");
                        lcd_gotoxy(1,3);
                        printf_lcd("   (Appuyez 1sec)   ");
                        lcd_bl_on();
                        
                        appgenData.state_service = SERVICE_STATE_SAVE_EXECUTE;
                    }
                    break;
                }
                /**************************************************************/
                
                /* Execute save when condition reached ************************/
                case SERVICE_STATE_SAVE_EXECUTE:
                {
                    /* Scan save button */
                    ScanBtnSave();
                    
                    /* Increment counter as long as the button is pressed */
                    if(BtnSaveValue() == 0)
                        counterS9++;
                    
                    /* Execute (or dont) SAVE when releasing button */
                    if( BtnSaveRelease() )
                    {
                        /* Going to write on lcd whatever happens */
                        lcd_gotoxy(1, 3);
                        
                        /* Execute SAVE if pressed > 1sec */
                        if(counterS9 > SAVE_PRESS_CYCLES)
                        {
                            I2C_WriteSEEPROM(&LocalParamGen, MCP79411_EEPROM_BEG, sizeof(S_ParamGen));                  
                            printf_lcd("    O VALIDEE O   ");
                        }
                        /* Leave without SAVE if < 1sec */
                        else
                        {
                            printf_lcd("    X ANNULEE X  ");                           
                        }

                        /* Reset counter and cycles */
                        counterS9 = 0;
                        appgenData.cycles = 0;
                        
                        /* Next state */
                        appgenData.state_service = SERVICE_STATE_SAVE_LEAVE;
                    }
                    break;
                }
                /**************************************************************/
                
                /* Temporary state when save has been executed ****************/
                case SERVICE_STATE_SAVE_LEAVE:
                {
                    /* Waiting for a certain time after executing save */
                    if(appgenData.cycles >= SAVE_WAIT_CYCLES)
                    {
                        /* Get back to the original MENU */
                        MENU_Initialize(&LocalParamGen);
                        Pec12ClearInactivity();
                         
                        /* Going back to RUN state */
                        appgenData.state_service = SERVICE_STATE_RUN;
                    }
                    break;
                }
                /**************************************************************/
                
                /* Handling errors in SERVICE_TASKS ***************************/
                default:
                {
                    break;
                }
                /**************************************************************/
            }

            /* Go to wait until next interrupt */
            appgenData.state = APPGEN_STATE_WAIT;
            break;         
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 //mise à jour des états
void APPGEN_UpdateState ( APPGEN_STATES NewState )
{
    appgenData.state  = NewState;
}

void APPGEN_TCP( uint8_t *Buffer )
{    
    /* Flag if save needed */
    bool saveToDo = false;
    
    /* Get message from UART */
    GetMessage(Buffer, &RemoteParamGen, &saveToDo);
    
    GENSIG_UpdateSignal(&RemoteParamGen);

    if(saveToDo)
    {
        I2C_WriteSEEPROM(&RemoteParamGen, MCP79411_EEPROM_BEG, sizeof(S_ParamGen));
        
        lcd_ClearLine(1);
        lcd_ClearLine(2);
        lcd_ClearLine(3);
        lcd_ClearLine(4);
        
        lcd_gotoxy(4, 2);
        printf_lcd("Sauvegarde OK");
        
        delay_ms(800);
                
        MENU_Initialize(&RemoteParamGen);
    }
    
    SendMessage(Buffer, &RemoteParamGen, saveToDo);
}

/*******************************************************************************
 End of File
 */
