#ifndef MenuGen_h
#define MenuGen_h

// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  03.02.2016
// Fichier MenuGen.h
// Gestion du menu  du générateur
// Traitement cyclique à 1 ms du Pec12


#include <stdbool.h>
#include <stdint.h>
#include "DefMenuGen.h"
#include "Inputs.h"
#include "Mc32DriverLcd.h"
#include "bsp.h"
#include "Debounce.h"

#define PARAM_DISPLAY_LOCATION 14
#define SHAPE_DISPLAY_OFFSET "\b\b\b"

typedef enum{
    MENU_STATE_INIT,
    MENU_STATE_NAVIGATION,
    MENU_STATE_SELECTION,
    MENU_STATE_REMOTE,
}E_MenuState;

typedef struct{
    uint8_t old :2;
    uint8_t now :2;
}S_MenuIndex;

typedef struct{
    int16_t minValue;
    int16_t maxValue;
    uint16_t step; 
}S_MenuParameters;


void MENU_Initialize(S_ParamGen *pParam);

void MENU_Execute(S_ParamGen *pParam, bool Local);

void MENU_DisplayLine(S_ParamGen *pParam, uint8_t lcdIndex);
void MENU_DisplayValue(S_ParamGen *pParam, uint8_t lcdIndex);
void MENU_DisplayStar();
//void MENU_WriteLine(S_ParamGen *pParam, uint8_t menuIndex);

#endif




  
   







