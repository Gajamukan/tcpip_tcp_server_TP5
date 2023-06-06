/*******************************************************************************
 *  _______  ___________  ___  ___   __               _______      _______ 
 * |   ____||           ||   \/   | |  |             |   ____|    /       |
 * |  |__   `---|  |----`|  \  /  | |  |      ______ |  |__      |   (----`
 * |   __|      |  |     |  |\/|  | |  |     |______||   __|      \   \    
 * |  |____     |  |     |  |  |  | |  `----.        |  |____ .----)   |   
 * |_______|    |__|     |__|  |__| |_______|        |_______||_______/                                                      
 * 
 * @file MenuGen.c
 * @brief 
 * 
 * Manage generator menu display
 * Cyclic call every 10ms
 * 
 * @author Miguel Santos
 * @author Ali Zoubir
 * 
 * @date 14.03.2023
 * 
 ******************************************************************************/

#include <string.h>
#include "MenuGen.h"
#include "Generator.h"

/* Array storing informations about each line of menu */
S_MenuParameters A_MenuParameters[4] = {
    {.minValue = 0,          .maxValue = 3,           .step = 1},
    {.minValue = FREQ_MIN,   .maxValue = FREQ_MAX,    .step = 20},
    {.minValue = AMP_MIN,    .maxValue = AMP_MAX,     .step = 100},
    {.minValue = OFFSET_MIN, .maxValue = OFFSET_MAX,  .step = 100}
};

/* Display strings for the shape parameter */
const char menuShapes[4][NAME_LENGTH] = {"Sinus", "Triangle", "DentDeScie", "Carre"};
const char menuHeads[4][HEAD_LENGTH] = {"Forme", "Freq", "Ampl", "Offset"};
const char menuUnits[4][UNIT_LENGTH] = {"", "Hz", "mV", "mV"};

/* Menu's index going from 0 to 3 */
S_MenuIndex menuIndex = {.old = 1 , .now = 0};

/* Menu's state */
E_MenuState menuState = MENU_STATE_INIT;

/**
 * Menu initial state
 * @param pParam
 */
void MENU_Initialize(S_ParamGen *pParam)
{
    uint8_t line;
    
    /* Clear and display the menu */
    for(line = 1; line <= 4; line++)
    {
        lcd_ClearLine(line);
        MENU_DisplayLine(pParam, line);
    }
 
    MENU_DisplayStar();
}


// Execution du menu, appel cyclique depuis l'application
void MENU_Execute(S_ParamGen *pParam, bool Local)
{
    /* Generator parameters save */
    static int16_t saveParam;

    /* Selected paramaters value */
    static int16_t *pCurrentParam;
    
    /* Iterance variable for remote state */
    uint8_t i_rem;

    switch(menuState)
    {
        case MENU_STATE_INIT:
        {
            MENU_Initialize(pParam);            
            GENSIG_UpdateSignal(pParam);            
            menuState = MENU_STATE_NAVIGATION;            
            break;
        }
         
        /* ------------------- Navigation inside the menu ------------------- */
        case MENU_STATE_NAVIGATION:
        {            
            menuIndex.old = menuIndex.now;
            
            /* Increment or decrement detection -------- */
            if(Pec12IsInc())
            {
                Pec12ClearInc();
                menuIndex.now++;
            }
            else if(Pec12IsDec())
            {
                Pec12ClearDec();
                menuIndex.now--;
            }
            /* ----------------------------------------- */
            
            /* Star position update -------------------- */
            if(menuIndex.old != menuIndex.now)    
                MENU_DisplayStar();
            /* ----------------------------------------- */
            
            /* OK action leads to selection ------------ */
            if(Pec12IsOK())
            {
                /* Debounce Reset */
                Pec12ClearOK();
                
                /* Add selection symbol to the current index */
                lcd_gotoxy(1,(menuIndex.now + 1));
                printf_lcd("?");
                
                /* Save current configuration */
                saveParam = pParam->parameters.key[menuIndex.now];
                
                /* Go into selection mode */
                menuState = MENU_STATE_SELECTION;
            }
            /* ----------------------------------------- */
             
          break;
        }
        /* -------------------------------------------------------------------*/
        
        /* -------------------------- Value select -------------------------- */
        case MENU_STATE_SELECTION:
        {            
            /* Pointing to current parameter value */
            pCurrentParam = &pParam->parameters.key[menuIndex.now];
            
            /* Increment or decrement detection */
            if(Pec12IsInc())
            {
                /* Debounce Reset */
                Pec12ClearInc();

                /* Increment with loop back */
                if(*pCurrentParam < A_MenuParameters[menuIndex.now].maxValue)
                    *pCurrentParam += A_MenuParameters[menuIndex.now].step;
                else if(menuIndex.now == 3)
                    *pCurrentParam = A_MenuParameters[menuIndex.now].maxValue;
                else
                    *pCurrentParam = A_MenuParameters[menuIndex.now].minValue;

                /* Update the value on display */
                MENU_DisplayValue(pParam, menuIndex.now + 1);
            }
            else if(Pec12IsDec())
            {
                /* Debounce Reset */
                Pec12ClearDec();

                /* Decrement with loop back */
                if(*pCurrentParam > A_MenuParameters[menuIndex.now].minValue)
                    *pCurrentParam -= A_MenuParameters[menuIndex.now].step;
                else if(menuIndex.now == 3)
                    *pCurrentParam = A_MenuParameters[menuIndex.now].minValue;
                else
                    *pCurrentParam = A_MenuParameters[menuIndex.now].maxValue;

                /* Update the value on display */
                MENU_DisplayValue(pParam, menuIndex.now + 1);
            }
            /* If pressure on OK */
            else if(Pec12IsOK())
            {   
                Pec12ClearOK();

                /* Add navigation star to the current index */
                MENU_DisplayStar();
                
                GENSIG_UpdateSignal(pParam);
                
                /* Go into navigation mode */
                menuState = MENU_STATE_NAVIGATION;
            }
            else if(Pec12IsESC())
            {
                /* Clear flag */
                Pec12ClearESC();
                
                /* Restore saved parameters */
                pParam->parameters.key[menuIndex.now] = saveParam;

                /* Add navigation star to the current index */
                MENU_DisplayStar();
                
                /* Update the value on display */
                MENU_DisplayValue(pParam, menuIndex.now + 1);
                
                /* Go into navigation mode */
                menuState = MENU_STATE_NAVIGATION;
            }
            
            break;
        }
        /* -------------------------------------------------------------------*/
        case MENU_STATE_REMOTE:
            
            /* Waiting for local state */
            if(Local)
            {
                for(i_rem = 1; i_rem < 5; i_rem++)
                {
                    MENU_DisplayValue(pParam, i_rem);
                    lcd_gotoxy(1, i_rem);
                    printf_lcd(" ");
                }
                menuState = MENU_STATE_NAVIGATION;
            }
            else
            {
                for(i_rem = 1; i_rem < 5; i_rem++)
                {
                    MENU_DisplayValue(pParam, i_rem);
                    lcd_gotoxy(1, i_rem);
                    printf_lcd("#");
                }
            }
            
            break;
        
        default:
            break;
    }

    /* Backlight based on inactivity and local state */
    if(Pec12NoActivity() && Local)
        lcd_bl_off();
    else
        lcd_bl_on();

    /* Automatic state change when not on local */
    if(!Local)
        menuState = MENU_STATE_REMOTE;
}

void MENU_DisplayLine(S_ParamGen *pParam, uint8_t lcdLine)
{    
    /* Header of the line */
    lcd_gotoxy(HEAD_POS, lcdLine);
    printf_lcd(menuHeads[lcdLine - 1]);
    
    /* Equal symbol */
    lcd_gotoxy(EQUAL_POS, lcdLine);
    printf_lcd("=");
    
    /* Current value of generator */
    MENU_DisplayValue(pParam, lcdLine);
    
    /* Unit */
    lcd_gotoxy(UNIT_POS, lcdLine);
    printf_lcd(menuUnits[lcdLine - 1]);
}

void MENU_DisplayValue(S_ParamGen *pParam, uint8_t lcdLine)
{
    lcd_gotoxy(VALUE_POS, lcdLine);
    switch(lcdLine)
    {
        case 1:
            printf_lcd("%-10s", menuShapes[pParam->parameters.key[lcdLine - 1]]);
            break;
            
        case 2:
        case 3:
        case 4:
            printf_lcd("%-+6d", pParam->parameters.key[lcdLine - 1]);
            break;
            
        default:
            break;
    }
}

void MENU_DisplayStar()
{
    /* Remove symbol from the previous location */
    lcd_gotoxy(1,(menuIndex.old + 1));
    printf_lcd(" ");

    /* Add symbol to the current index */
    lcd_gotoxy(1,(menuIndex.now + 1));
    printf_lcd("*");
}

