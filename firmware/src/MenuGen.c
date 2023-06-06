   // Tp4 UsbGen
// C. HUBER  10/02/2015 pour SLO2 2014-2015
// Fichier MenuGen.c
// Gestion du menu  du générateur
// Traitement cyclique à 10 ms

#include <stdint.h>                   
#include <stdbool.h>
#include "MenuGen.h"
#include "DefMenuGen.h"
#include "appgen.h"
#include "Generateur.h"
#include "Mc32DriverLcd.h"
#include "Mc32gest_SerComm.h"
#include "GesPec12.h"


S_ParamGen valParamGen;
//S_ParamGen RemoteParamGen;
S_Pec12_Descriptor S_Pec12_fonction;
S_Curseur Curseur;

E_Switch State = Select_Forme;

//Déclaration
const char MenuFormes[4][21] = {"Sinus","Triangle","Carre","DentDeScie"};
int i = 0;
int valFreq, valAmpli, valOffset;

// Initialisation du menu et des paramètres
void MENU_Initialize(S_ParamGen *pParam)
{
    pParam->Forme = 0;
    valFreq = 1000;
    valAmpli = 10000;

}

// Execution du menu, appel cyclique depuis l'application
void MENU_Execute(S_ParamGen *pParam, bool local)
{       
    //mode remote
    if (local == false)
    {
      Curseur.C_Forme = '#';
      Curseur.C_Freq = '#';
      Curseur.C_Ampli = '#';
      Curseur.C_Offset = '#';
      
      //mise à jour des variables pour l'affichage
      i = pParam->Forme;
      valFreq = pParam->Frequence;
      valAmpli = pParam->Amplitude;
      valOffset = pParam->Offset;
      
      //Mise à jour du signal fonction issue du générateur (Remote)
//      GENSIG_UpdateSignal(&RemoteParamGen);
    }
     
    else if (local == true)
    {
      switch (State)
      { 
       case Select_Forme:

        Curseur.C_Forme = '*';
        Curseur.C_Freq = ' ';
        Curseur.C_Ampli = ' ';
        Curseur.C_Offset = ' ';

        //Incrémentation et décrémentation sélection menu
        if(Pec12IsPlus() == true)
        {
         State = Select_Freq;                 
        }
        if(Pec12IsMinus() == true )
        {
         State = Select_Offset; 
        }
        if(Pec12IsOK() == true)
        {    
         State = Set_Forme;                    
        }       
       break;
        
       //Choix forme
       case Set_Forme:

        Curseur.C_Forme = '?';
        Curseur.C_Freq = ' ';
        Curseur.C_Ampli = ' ';
        Curseur.C_Offset = ' ';
        
        //Incrémentation et décrémentation choix forme
        if(Pec12IsPlus() == true )
        {
         i++;
         if (i > 3)
         {    
           i = 0;
         }               
        }
        if(Pec12IsMinus() == true )
        {
         i--;
         if (i < 0)
         {    
          i = 3;
         }
        }
        
        //Validation forme
        if(Pec12IsOK() == true)
        { 
         pParam->Forme = i;  
         GENSIG_UpdateSignal(&valParamGen);
         State = Select_Forme;
        }
        //sortir sans valider
        if(Pec12IsESC() == true)
        { 
         i = pParam->Forme;
         State = Select_Forme; 
        }  
       break;      

       case Select_Freq:

        Curseur.C_Forme = ' ';
        Curseur.C_Freq = '*';
        Curseur.C_Ampli = ' ';
        Curseur.C_Offset = ' ';

        if(Pec12IsPlus() == true )
        {  
         State = Select_Ampli;
        }
        if(Pec12IsMinus() == true )
        { 
         State = Select_Forme;
        }
        if(Pec12IsOK() == true)
        { 
         State = Set_Freq;
        }
       break;
       
       //Choix fréquence
       case Set_Freq:

        Curseur.C_Forme = ' ';
        Curseur.C_Freq = '?';
        Curseur.C_Ampli = ' ';
        Curseur.C_Offset = ' ';

        if(Pec12IsPlus() == true )
        {
         valFreq = valFreq + 20;
        }
        if(Pec12IsMinus() == true )
        {
         valFreq = valFreq - 20;
        }

        //Rebouclement de la fréquence
        if (valFreq  > 2000)
        {
         valFreq = 20;
        }
        if (valFreq  < 20)
        {
         valFreq  = 2000;
        }
        //Validation de la valeur de fréquence
        if(Pec12IsOK() == true)
        { 
         State = Select_Freq;
         valParamGen.Frequence = valFreq ;
         GENSIG_UpdatePeriode(&valParamGen);
        }
        if(Pec12IsESC() == true)
        {      
         State = Select_Freq;
         valFreq = valParamGen.Frequence ;
        }
       break;

       case Select_Ampli:

        Curseur.C_Forme = ' ';
        Curseur.C_Freq = ' ';
        Curseur.C_Ampli = '*';
        Curseur.C_Offset = ' ';
        
        if(Pec12IsPlus() == true )
        {
         State = Select_Offset;
        }
        if(Pec12IsMinus() == true )
        {
         State = Select_Freq;
        }
        if(Pec12IsOK() == true)
        { 
         State = Set_Ampli;
        }
       break;
        
       //Choix Amplitude
       case Set_Ampli:
            
        Curseur.C_Forme = ' ';
        Curseur.C_Freq = ' ';
        Curseur.C_Ampli = '?';
        Curseur.C_Offset = ' ';   

        if(Pec12IsPlus() == true )
        {
          valAmpli=valAmpli+100 ;
        }
        if(Pec12IsMinus() == true )
        {    
          valAmpli=valAmpli-100 ;
        }
        
        //Rebouclement de l'amplitude
        if (valAmpli > 10000)
        {
          valAmpli = 0;                
        }
        if (valAmpli < 0)
        {
          valAmpli = 10000;                
        }

        if(Pec12IsOK() == true)
        { 
          valParamGen.Amplitude = valAmpli;
          GENSIG_UpdateSignal(&valParamGen);
          State = Select_Ampli;
        }
        if(Pec12IsESC() == true)
        {                    
          State = Select_Ampli;
          valAmpli = valParamGen.Amplitude;
        }                    
       break;

       case Select_Offset:

        Curseur.C_Forme = ' ';
        Curseur.C_Freq = ' ';
        Curseur.C_Ampli = ' ';
        Curseur.C_Offset = '*';

        if(Pec12IsPlus() == true )
        {
         State = Select_Forme;
        }
        if(Pec12IsMinus() == true )
        {
         State = Select_Ampli;
        }
        if(Pec12IsOK() == true)
        {                    
         State = Set_Offset; 
        }
       break;

        //Paramétrage de l'offset
       case Set_Offset:

        Curseur.C_Forme = ' ';
        Curseur.C_Freq = ' ';
        Curseur.C_Ampli = ' ';
        Curseur.C_Offset = '?';     

        //Rotation Pec12
        if(Pec12IsPlus() == true )
        {                  
         valOffset = valOffset +100 ;
        }
        if(Pec12IsMinus() == true )
        { 
         valOffset = valOffset -100 ;
        }
        //Rebouclement de l'offset
        if (valOffset > 5000)
        {
          valOffset = 5000;                
        }
        if (valOffset < -5000)
        {
          valOffset = -5000;                
        }
        //Pression Pec12
        if(Pec12IsOK() == true)
        { 
          valParamGen.Offset = valOffset;
          GENSIG_UpdateSignal(&valParamGen);
          State = Select_Offset;
        }
        if(Pec12IsESC() == true)
        { 
           valOffset = valParamGen.Offset;
           State = Select_Offset;
        }
       break;    
      }
    
      /*if(Pec12NoActivity()== true)
      {   
       lcd_bl_off();
      }   
      else if (Pec12NoActivity()== false)
      { 
       lcd_bl_on();
      } */
      
      Pec12ClearInactivity();
      Pec12ClearPlus();
      Pec12ClearESC();
      Pec12ClearOK();
      Pec12ClearMinus();
      S9ClearOK();
    } 
    
   //Affichage
   lcd_ClearLine(1);              //Effacer la ligne
   lcd_gotoxy(1,1);               //Ecrire sur la 1ère ligne du LCD
   printf_lcd("%cForme = %s \n", Curseur.C_Forme, MenuFormes[i]); 
   lcd_ClearLine(2);              //Effacer la ligne
   lcd_gotoxy(1,2);                  
   printf_lcd("%cFreq[Hz] = %4d \n", Curseur.C_Freq, valFreq); 
   lcd_ClearLine(3);              //Effacer la ligne
   lcd_gotoxy(1,3);
   printf_lcd("%cAmpl[mV] = %5d \n", Curseur.C_Ampli, valAmpli);
   lcd_gotoxy(1,4);
   printf_lcd("%cOffset[mV] = %5d \n", Curseur.C_Offset, valOffset);    
}    





