// Canevas manipulation GenSig avec menu
// C. HUBER  09/02/2015
// Fichier Generateur.C
// Gestion  du générateur

// Prévu pour signal de 40 echantillons

// Migration sur PIC32 30.04.2014 C. Huber


#include "Generateur.h"
#include "DefMenuGen.h"
#include "Mc32gestSpiDac.h"
#include "Mc32DriverLcd.h"

#include "peripheral/tmr/plib_tmr.h"


// T.P. 2016 100 echantillons
#define MAX_ECH 100
#define NBRMAX 65535
#define VAL_ECH 32767

//Déclaration et initialisation variables 
uint16_t Nbr_Tic = 0;
uint16_t Ampl_min;
uint16_t Ampl_max;
          
uint16_t EchNb = 0;
uint16_t NumAff = 0;

int32_t Result[100] ;
S_ParamGen valParamGen;
int16_t Offset = 0;

// Initialisation du  générateur
void  GENSIG_Initialize(S_ParamGen *pParam)
{

    //Test si match de la valeur Magic
    if (valParamGen.Magic == MAGIC)
    {
        //Garde automatiquement les valeurs precedentes sauvegardées
        //lcd_gotoxy(1,4);
        //printf_lcd("Datas Restored");       
    }
    else
    {
        //lcd_gotoxy(1,4);
        //printf_lcd("Datas Default");
        //Set les valeurs aux valeurs par defaut
        valParamGen.Magic = MAGIC;
        valParamGen.Amplitude = 10000;
        valParamGen.Forme = 0;
        valParamGen.Frequence = 1000;
        valParamGen.Offset = 0;  
    }
    GENSIG_UpdatePeriode(pParam);
}
  
// Mise à jour de la periode d'échantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam)
{
    uint16_t Periode = 0;
    
    uint32_t Nombreechantillons = (MAX_ECH * valParamGen.Frequence);
    uint32_t Frequence2 = (80000000/64);
    
    Periode = ((Frequence2/Nombreechantillons) -1 );

    PLIB_TMR_Period16BitSet(TMR_ID_3,Periode);   
}

// Mise à jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen *pParam)
{
    int test = 0; 
    //amplitude 
    Nbr_Tic = valParamGen.Amplitude * VAL_ECH /10000;
    Ampl_min = VAL_ECH - Nbr_Tic;
    Ampl_max = VAL_ECH + Nbr_Tic;
    //Calul du tableau
    for (EchNb = 0;EchNb < 100; EchNb++)
    {
        switch (valParamGen.Forme)
        {
            case SignalSinus : //ok sa fonctionne 
            //Test.Offset = Test.Offset+ (Test.Amplitude/2);
            Result[EchNb] = (NBRMAX/2 -(((valParamGen.Offset)*(NBRMAX/20))/1000)+(((NBRMAX/2* (sin((EchNb*3.6)*(3.14/180) )))/10000)* (valParamGen.Amplitude)))  ;
                break ;

            case SignalCarre : //ok sa fonctionne
            if(EchNb > (MAX_ECH/2))
            {
                Result[EchNb] = NBRMAX/2 - (((valParamGen.Offset)*(NBRMAX/20))/1000) + (((NBRMAX/2)/10000)*(valParamGen.Amplitude)) ;
            }
            else 
            {
              Result[EchNb] = NBRMAX/2 - (((valParamGen.Offset)*(NBRMAX/20))/1000) - (((NBRMAX/2)/10000)*(valParamGen.Amplitude)) ;
            }     
                break;
            case SignalTriangle :
            if (EchNb< 1)
            {
                Offset = -((valParamGen.Offset*VAL_ECH)/10000); 
            }
            float const a = ((float)VAL_ECH/(float)10000)/(float)25;//0.131
            float const b = valParamGen.Amplitude * 65536/10000 + VAL_ECH;

            if (EchNb < 25 )
                {
                    //calcul pour la pente montante du triangle (du centre Ã  la val max)
                    Result[EchNb]  = ((valParamGen.Amplitude * (a * EchNb)) + VAL_ECH ) + Offset;
                }

                else if ((EchNb < 75) && (EchNb >= 25))
                {
                    //calcul pour la pente descendante du triangle (de la val max- la val min)
                    Result[EchNb]  = ((valParamGen.Amplitude *( (-a) * EchNb) )+ b )+ Offset;
                }

                else if (EchNb >= 75 )
                {
                    //calcul pour la pente montante du triangle (de la val min au centre)
                    Result[EchNb]  = ((valParamGen.Amplitude *( a * (EchNb - 100)))+VAL_ECH )+ Offset ;
                }
               break;

            case SignalDentDeScie :
            if (EchNb< 1)
            {
                Offset = -((valParamGen.Offset*VAL_ECH)/10000); 
            } 
             uint16_t static Step;
            //dÃ©terminer la valeur de step 
            Step = ((Nbr_Tic*2) / MAX_ECH);
            
            Result[EchNb] = ((Step * EchNb)+ Ampl_min + Offset);
                break ;
            default : 
                break;
        }
        if (Result[EchNb] > 65535  )
        {
            Result[EchNb] = 65535;  
        }
        else if (Result[EchNb]< 0 )
        {
            Result[EchNb] = 0; 
        } 
    }
    test++; 
}

// Execution du générateur
// Fonction appelée dans Int timer3 (cycle variable variable)

// Version provisoire pour test du DAC
void  GENSIG_Execute(void)
{ 
    SPI_WriteToDac(0,Result[NumAff]);
    NumAff ++;
    NumAff = NumAff %  MAX_ECH;       
}
