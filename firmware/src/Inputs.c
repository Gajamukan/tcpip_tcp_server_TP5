/*******************************************************************************
 *  _______  ___________  ___  ___   __               _______      _______ 
 * |   ____||           ||   \/   | |  |             |   ____|    /       |
 * |  |__   `---|  |----`|  \  /  | |  |      ______ |  |__      |   (----`
 * |   __|      |  |     |  |\/|  | |  |     |______||   __|      \   \    
 * |  |____     |  |     |  |  |  | |  `----.        |  |____ .----)   |   
 * |_______|    |__|     |__|  |__| |_______|        |_______||_______/                                                      
 * 
 * @file Inputs.c
 * @brief 
 * 
 * File used to access all inputs peripherals
 * on ETML-ES PIC32 kit
 * 
 * @author Miguel Santos
 * @author Ali Zoubir
 * 
 * @date 14.03.2023
 * 
 ******************************************************************************/

#include "app.h"
#include "Inputs.h"
#include "Debounce.h"

/* PEC12 descriptors */
S_Switch DescrA;
S_Switch DescrB;
S_Switch DescrPB;
S_Pec12 Pec12;

/* Buttons descriptors */
S_Switch BtnSave;

/**
 * InputsInit
 * Initialize all inputs, including PEC12
 */
void InputsInit ( void )
{
    Pec12Init();
    DebounceInit(&BtnSave, BTN_SAVE_INPUT, BTN_MAX_DEBOUNCE);
}

void ScanAllInputs ( void )
{
    /* Scan PEC12 inputs */
    ScanPec12( PEC12_A_INPUT, PEC12_B_INPUT, PEC12_PB_INPUT);
    ScanBtnSave();
}

void ScanBtnSave ( void )
{
    /* Scan button S9 input */
    Debounce(&BtnSave, BTN_SAVE_INPUT);
}

bool BtnSavePress ( void )
{
    return BtnSave.Bits.Fall;
}

void BtnSaveClearPress ( void )
{
    BtnSave.Bits.Fall = false;
}

bool BtnSaveRelease ( void )
{
    return BtnSave.Bits.Rise;
}

void BtnSaveClearRelease ( void )
{
    BtnSave.Bits.Rise = false;
}

bool BtnSaveValue ( void )
{
    return BtnSave.Bits.ValueNow;
}

/**
 * Pec12Init 
 * Initialize PEC12 descriptors
 */
void Pec12Init ( void )
{
    /* PEC12 signals initialisation */
    DebounceInit(&DescrA, PEC12_A_INPUT, PEC12_MAX_DEBOUNCE);
    DebounceInit(&DescrB, PEC12_B_INPUT, PEC12_MAX_DEBOUNCE);
    DebounceInit(&DescrPB, PEC12_PB_INPUT, PEC12_MAX_DEBOUNCE);
   
   /* PEC12 structure values initilisation */
    Pec12.Inc           = false; /* Increment event */ 
    Pec12.Dec           = false; /* Decrement event */
    Pec12.OK            = false; /* OK Action event */
    Pec12.ESC           = false; /* ESC Action event */
    Pec12.NoActivity    = false; /* Inactivity flag */
    Pec12.PressDuration = 0;     /* PB press duration */
    Pec12.InactivityDuration = 0;/* Inactivity duration*/  
 } /* Pec12Init */

void ScanPec12 (bool ValA, bool ValB, bool ValPB)
{
    /* Debounce action on signals */
    Debounce(&DescrA,  ValA);
    Debounce(&DescrB,  ValB);
    Debounce(&DescrPB, ValPB);
   
    /* Detecting an increment or decrement */   
    if( DebounceFalled(&DescrB) )
    {
        DebounceClearFalled(&DescrB);
       
        if( DebounceGetInput(&DescrA) )
            Pec12.Inc = false;
        else
            Pec12.Inc = true;

        /* Decrement flag is always opposite of increment flag */
        Pec12.Dec = !Pec12.Inc;

        Pec12ClearInactivity();        
    } 
   
    /* Change actions flags when releasing button */
    if( DebounceRised(&DescrPB) )
    {
        DebounceClearRised(&DescrPB);
        
        if( (Pec12.PressDuration > 0) && (Pec12.PressDuration < 500))
        {
            Pec12.OK = true;
            Pec12.ESC = false;
        }
        else if( Pec12.PressDuration >= 500)
        {
            Pec12.OK = false;
            Pec12.ESC = true;
        }
    }

    /* Push button and inactivity detection */
    if( DebounceGetInput(&DescrPB) )
    {
        Pec12.PressDuration = 0;
        Pec12.InactivityDuration += 1;       
    }
    /* Button is pressed */
    else
    {
        Pec12.PressDuration += 1;
        Pec12ClearInactivity();
    }

    if( Pec12.InactivityDuration >= INACTIVITY_TIME)
        Pec12.NoActivity = true;
   
 } // ScanPec12



/******************************************************************************/

/* 
 * Pec12IsInc
 * @return : Increment detection flag
*/
bool Pec12IsInc ( void ) {
   return Pec12.Inc;
}

/******************************************************************************/

/*
 * Pec12IsDec
 * @return : Decrement detection flag
*/
bool Pec12IsDec (void) {
   return Pec12.Dec;
}

/******************************************************************************/

/*
 * Pec12IsOK 
 * @return : Action OK flag
*/
bool Pec12IsOK ( void ) {
   return Pec12.OK;
}

/******************************************************************************/

/**
 * Pec12IsESC 
 * @return : Action ESC flag
 */
bool Pec12IsESC ( void ) {
   return (Pec12.ESC);
}

/**
 * Pec12NoActivity
 * @return : True when inactivity detected on PEC12
 */
bool Pec12NoActivity ( void ) {
   return (Pec12.NoActivity);
}

/**
 * Pec12ClearInc
 * Remove incrementation flag
 */
void Pec12ClearInc ( void ) {
   Pec12.Inc = 0;
}

/**
 * Pec12ClearDec
 * Remove decrementation flag
 */
void Pec12ClearDec ( void ) {
   Pec12.Dec = 0;
}

/**
 * Pec12ClearOK
 * Remove action OK flag
 */
void Pec12ClearOK ( void ) {
   Pec12.OK = 0;
}

/**
 * Pec12ClearESC
 * Remove action ESC flag
 */
void Pec12ClearESC ( void ) {
   Pec12.ESC = 0;
}

/**
 * Pec12ClearInactivity
 * Remove inactivity flag
 * and reset inactivity counter
 */
void Pec12ClearInactivity   (void) {
  Pec12.NoActivity = 0;
  Pec12.InactivityDuration = 0;
}


