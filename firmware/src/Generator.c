/*******************************************************************************
 *  _______  ___________  ___  ___   __               _______      _______ 
 * |   ____||           ||   \/   | |  |             |   ____|    /       |
 * |  |__   `---|  |----`|  \  /  | |  |      ______ |  |__      |   (----`
 * |   __|      |  |     |  |\/|  | |  |     |______||   __|      \   \    
 * |  |____     |  |     |  |  |  | |  `----.        |  |____ .----)   |   
 * |_______|    |__|     |__|  |__| |_______|        |_______||_______/                                                      
 * 
 * @file Generator.c
 * @brief 
 * 
 * Signal generator manipulation
 * 
 * @author Miguel Santos
 * @author Ali Zoubir
 * 
 * @date 14.03.2023
 * 
 ******************************************************************************/

#include <proc/p32mx795f512l.h>
#include "Generator.h"
#include "DefMenuGen.h"
#include "Mc32gestSpiDac.h"
#include "Mc32NVMUtil.h"
#include "Mc32DriverLcd.h"
#include "driver/tmr/drv_tmr_compatibility.h"
#include "driver/tmr/drv_tmr_mapping.h"
#include "I2C_Seeprom.h"

#define GEN_DAC_CHANNEL 0
#define MAX_ECH 100
#define MAX_ADC 65535.0
#define MID_ADC 32767.0

#define F_SYS 80000000
#define TMR_MAX 65535


uint16_t A_signalADC[100];

/* Patern of all the different shapes */
const float A_shapesRef[4][100] = {{ 0.0, 0.0628, 0.1253, 0.1874, 0.2487, 0.309, 0.3681, 0.4258, 0.4818, 0.5358, 0.5878, 0.6374, 0.6845, 0.729, 0.7705, 0.809, 0.8443, 0.8763, 0.9048, 0.9298, 0.9511, 0.9686, 0.9823, 0.9921, 0.998, 1.0, 0.998, 0.9921, 0.9823, 0.9686, 0.9511, 0.9298, 0.9048, 0.8763, 0.8443, 0.809, 0.7705, 0.729, 0.6845, 0.6374, 0.5878, 0.5358, 0.4818, 0.4258, 0.3681, 0.309, 0.2487, 0.1874, 0.1253, 0.0628, -0.0, -0.0628, -0.1253, -0.1874, -0.2487, -0.309, -0.3681, -0.4258, -0.4818, -0.5358, -0.5878, -0.6374, -0.6845, -0.729, -0.7705, -0.809, -0.8443, -0.8763, -0.9048, -0.9298, -0.9511, -0.9686, -0.9823, -0.9921, -0.998, -1.0, -0.998, -0.9921, -0.9823, -0.9686, -0.9511, -0.9298, -0.9048, -0.8763, -0.8443, -0.809, -0.7705, -0.729, -0.6845, -0.6374, -0.5878, -0.5358, -0.4818, -0.4258, -0.3681, -0.309, -0.2487, -0.1874, -0.1253, -0.0628},
                                                        {-1.0, -0.96, -0.92, -0.88, -0.84, -0.8, -0.76, -0.72, -0.68, -0.64, -0.6, -0.56, -0.52, -0.48, -0.44, -0.4, -0.36, -0.32, -0.28, -0.24, -0.2, -0.16, -0.12, -0.08, -0.04, 0.0, 0.04, 0.08, 0.12, 0.16, 0.2, 0.24, 0.28, 0.32, 0.36, 0.4, 0.44, 0.48, 0.52, 0.56, 0.6, 0.64, 0.68, 0.72, 0.76, 0.8, 0.84, 0.88, 0.92, 0.96, 1.0, 0.96, 0.92, 0.88, 0.84, 0.8, 0.76, 0.72, 0.68, 0.64, 0.6, 0.56, 0.52, 0.48, 0.44, 0.4, 0.36, 0.32, 0.28, 0.24, 0.2, 0.16, 0.12, 0.08, 0.04, 0.0, -0.04, -0.08, -0.12, -0.16, -0.2, -0.24, -0.28, -0.32, -0.36, -0.4, -0.44, -0.48, -0.52, -0.56, -0.6, -0.64, -0.68, -0.72, -0.76, -0.8, -0.84, -0.88, -0.92, -0.96},
                                                        {-1.0, -0.98, -0.96, -0.94, -0.92, -0.9, -0.88, -0.86, -0.84, -0.82, -0.8, -0.78, -0.76, -0.74, -0.72, -0.7, -0.68, -0.66, -0.64, -0.62, -0.6, -0.58, -0.56, -0.54, -0.52, -0.5, -0.48, -0.46, -0.44, -0.42, -0.4, -0.38, -0.36, -0.34, -0.32, -0.3, -0.28, -0.26, -0.24, -0.22, -0.2, -0.18, -0.16, -0.14, -0.12, -0.1, -0.08, -0.06, -0.04, -0.02, 0.0, 0.02, 0.04, 0.06, 0.08, 0.1, 0.12, 0.14, 0.16, 0.18, 0.2, 0.22, 0.24, 0.26, 0.28, 0.3, 0.32, 0.34, 0.36, 0.38, 0.4, 0.42, 0.44, 0.46, 0.48, 0.5, 0.52, 0.54, 0.56, 0.58, 0.6, 0.62, 0.64, 0.66, 0.68, 0.7, 0.72, 0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98}, 
                                                        {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}};


/* Signal generator initialisation */
void GENSIG_Initialize ( S_ParamGen *pParam )
{

    I2C_ReadSEEPROM(pParam, MCP79411_EEPROM_BEG, sizeof(S_ParamGen));
    
    lcd_gotoxy(1, 4);
    
    if(pParam->Magic == MAGIC)
    {
        printf_lcd("Memory LOADED !");
    }
    else
    {
        printf_lcd("Memory FAILED !");
        pParam->Magic                       = MAGIC;
        pParam->shape                       = SignalSinus;
        pParam->parameters.data.frequency   = FREQ_TYP; //1000Hz
        pParam->parameters.data.amplitude   = AMP_TYP;
        pParam->parameters.data.offset      = 0;   //0 
    }
}
  
/**
 * GENSIG_UpdatePeriode
 * Update of sampling period
 * @param pParam 
 */
void GENSIG_UpdatePeriode ( S_ParamGen *pParam )
{
    uint16_t period = 0;    
    uint32_t f_timer = 0;
    TMR_PRESCALE prescaler;
    uint32_t prescaler_val = 1;
    float prescaler_min = 0;

    f_timer = pParam->parameters.data.frequency * 100;
    prescaler_min = F_SYS / (TMR_MAX * f_timer);
    
    if(prescaler_min < 1)
        prescaler = TMR_PRESCALE_VALUE_1;
    else if (prescaler_min < 2)
        prescaler = TMR_PRESCALE_VALUE_2;
    else if(prescaler_min < 4)
        prescaler = TMR_PRESCALE_VALUE_4;
    else if(prescaler_min < 8)
        prescaler = TMR_PRESCALE_VALUE_8;
    else if(prescaler_min < 16)
        prescaler = TMR_PRESCALE_VALUE_16;
    else if(prescaler_min < 32)
        prescaler = TMR_PRESCALE_VALUE_32;
    else if(prescaler_min < 64)
        prescaler = TMR_PRESCALE_VALUE_64;
    else
        prescaler = TMR_PRESCALE_VALUE_256;
    
    prescaler_val = prescaler_val << prescaler;
    
    period = (F_SYS / (prescaler_val * f_timer))-1;
  
    DRV_TMR1_Stop();
    DRV_TMR1_CounterClear();
    
    /* Select prescaler value */
    PLIB_TMR_PrescaleSelect(TMR_ID_3, prescaler);

    DRV_TMR1_PeriodValueSet(period);
    DRV_TMR1_Start(); 
}

/* Update signal with parameters */
uint16_t maxcap ( int32_t value )
{   
    uint16_t valToReturn = value;
    
    if(value > MAX_ADC)
        valToReturn = MAX_ADC;
    else if(value < 0)
        valToReturn = 0;
    
    return valToReturn;
}

/* Update signal with parameters */
void GENSIG_UpdateSignal ( S_ParamGen *pParam )
{
    uint16_t time = 0;
    int16_t Offset = 0;
    
    /* Cross rule to convert volts to ADC value */
    Offset = pParam->parameters.data.offset * MID_ADC / OFFSET_DELTA;
    
    /* Apply the amplitude and the offset to all the elements in the reference (patern) array, for the right shape */
    for(time=0;time<MAX_ECH;time++)
        A_signalADC[time] = maxcap((int32_t)((MID_ADC + A_shapesRef[pParam->parameters.data.name][time]*(float)pParam->parameters.data.amplitude*(float)(MID_ADC/AMP_MAX))-Offset));    
    
    GENSIG_UpdatePeriode(pParam);
}

/**
 * GENSIG_Execute
 * Execute signal on DAC based on parameters stored in array
 * Using SPI to transmit datas
 */
void GENSIG_Execute ( void )
{    
   /* Sample iteration variable */
   static uint16_t sample = 0;
   
   /* Execute signal on DAC with SPI */
   SPI_WriteToDac(GEN_DAC_CHANNEL, A_signalADC[sample]);
   
   /* Update sample number */
   sample++;   
   sample = sample % MAX_ECH;
}
