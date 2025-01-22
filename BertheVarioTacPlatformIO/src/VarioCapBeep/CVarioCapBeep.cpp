////////////////////////////////////////////////////////////////////////////////
/// \file CVarioCapBeep.h
///
/// \brief
///
/// \date creation     : 03/10/2024
/// \date modification : 22/01/2025
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance la tache de mesure des capteurs et calcul de la VZ.
void CVarioCapBeep::LanceTacheVarioCapBeep()
{
g_GlobalVar.m_TaskArr[VARIOCAP_NUM_TASK].m_Run = true ;
g_GlobalVar.m_TaskArr[VARIOCAP_NUM_TASK].m_Stopped = false ;
xTaskCreatePinnedToCore(TacheVarioCapteur, "VarioCapteur", VARIOCAP_STACK_SIZE , this, VARIOCAP_PRIORITY, NULL, VARIOCAP_CORE);

delay( 500 ) ;

g_GlobalVar.m_TaskArr[VARIOBEEP_NUM_TASK].m_Run = true ;
g_GlobalVar.m_TaskArr[VARIOBEEP_NUM_TASK].m_Stopped = false ;
xTaskCreatePinnedToCore(TacheGenereSonVario, "VarioBeep", VARIOBEEP_STACK_SIZE , this, VARIOBEEP_PRIORITY, NULL, VARIOBEEP_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tache de mesure des capteurs et calcul de la VZ.
void CVarioCapBeep::TacheVarioCapteur(void* param)
{
// variables
const int DIV_SECONDES = 3 ;
float AltiPressForVzArr[DIV_SECONDES+1] ;

// premiere altitude
g_GlobalVar.m_pCapteurPression->MesureAltitudeCapteur() ;
float AltiBaroFiltree = g_GlobalVar.m_TerrainPosCur.m_AltiBaro = g_GlobalVar.m_pCapteurPression->GetAltiBaroPureMetres() ;

// pile des alti baro
for ( int i = 0 ; i <= DIV_SECONDES ; i++ )
    AltiPressForVzArr[i] = AltiBaroFiltree ;

// boucle Vz et cap magnetique
while (g_GlobalVar.m_TaskArr[VARIOCAP_NUM_TASK].m_Run)
    {
    // a 3 hz
    delay( 1000/DIV_SECONDES ) ;

    // lecture cap magnetique
    g_GlobalVar.m_QMC5883Mag.LectureCap() ;

    // mesure altitude recalee et mise a jour altitude courante
    const float coef = g_GlobalVar.m_Config.m_coef_filtre_alti_baro ;
    g_GlobalVar.m_pCapteurPression->MesureAltitudeCapteur() ;
    AltiBaroFiltree = (1.-coef) * g_GlobalVar.m_pCapteurPression->GetAltiBaroPureMetres() + coef * AltiBaroFiltree ;
    g_GlobalVar.m_TerrainPosCur.m_AltiBaro = g_GlobalVar.m_pCapteurPression->GetAltiMetres() ;

    // decalage du tableau alti fifo par 0 sur x secondes
    for ( int i = DIV_SECONDES ; i>0 ; i-- )
        AltiPressForVzArr[ i ] = AltiPressForVzArr[ i - 1 ] ;
    AltiPressForVzArr[ 0 ] = AltiBaroFiltree ;

    // calcul difference alti baro pure
    float DiffAlti = AltiPressForVzArr[ 0 ] - AltiPressForVzArr[ DIV_SECONDES ] ;

    // recopie de Vz
    g_GlobalVar.m_VitVertMS = DiffAlti ;
    }

g_GlobalVar.m_TaskArr[VARIOCAP_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static core 0 qui gere les beep.
void CVarioCapBeep::TacheGenereSonVario(void *param)
{
const float LowFreq = 1000 ;
const float MinFreq = 1200 ;
const float MaxFreq = 8000 ;
const float SeuilVzMin = g_GlobalVar.m_Config.m_vz_seuil_haut ;
while (g_GlobalVar.m_TaskArr[VARIOBEEP_NUM_TASK].m_Run)
    {
    // desactivation du son cause TMA
    bool NotActive = g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone == ZONE_DEDANS ||
                     g_GlobalVar.m_ZonesAerAll.m_LimiteZone == ZONE_LIMITE_ALTI ||
                     g_GlobalVar.m_ZonesAerAll.m_LimiteZone == ZONE_LIMITE_FRONTIERE ;

    // si alarme zone desactivé => son vario active
    if ( !g_GlobalVar.m_BeepAttenteGVZone )
        NotActive = false ;

    // si son vario desactive
    if ( NotActive )
        {
        delay( 500 ) ;
        continue ;
        }

    float LocalVitVertMS = g_GlobalVar.m_VitVertMS ;
    //LocalVitVertMS = 0.5 ;
    //float LocalVitVertMS = g_GlobalVar.m_Config.m_vz_seuil_haut ;
    #ifdef SOUND_DEBUG
     LocalVitVertMS = 6 ;
    #endif

    // si degueulante
    if ( LocalVitVertMS <= g_GlobalVar.m_Config.m_vz_seuil_bas )
        {
        g_GlobalVar.beeper( LowFreq , 200 ) ;
        delay( 400 ) ;
        continue ;
        }
    // descente normale
    else if ( LocalVitVertMS > g_GlobalVar.m_Config.m_vz_seuil_bas && LocalVitVertMS < 0. )
        {
        delay( 500 ) ;
        continue ;
        }
    // zerotage en vol (pas au sol)
    else if ( LocalVitVertMS >= 0. && LocalVitVertMS < SeuilVzMin )
        {
        static int BeepZerotage = 0 ;
        BeepZerotage++ ;
        BeepZerotage = BeepZerotage%9 ;
        // si le vol a debuté
        if ( g_GlobalVar.m_DureeVolMin >= 0 )
            {
            int MidFreq = (LowFreq + MinFreq) / 2 ;
            if ( BeepZerotage == 0 || BeepZerotage == 3 )
                g_GlobalVar.beeper( MinFreq , 100 ) ;
            else if ( BeepZerotage == 1 || BeepZerotage == 4 )
                g_GlobalVar.beeper( MidFreq , 100 ) ;
            else if ( BeepZerotage == 2 || BeepZerotage == 5 )
                g_GlobalVar.beeper( LowFreq , 100 ) ;
            else
                delay( 2000 ) ;
            }
        // si le vol n'a pas debute
        else
            delay( 100 ) ;
        continue ;
        }

    // coefficient de Vz
    float Coef01 = (LocalVitVertMS-SeuilVzMin) / g_GlobalVar.m_Config.m_vz_seuil_max ;
    if ( Coef01 > 1 )
        Coef01 = 1. ;
    if ( Coef01 < 0 )
        Coef01 = 0. ;

    // calcul frequence son
    float Freq = MinFreq + Coef01 * ( MaxFreq - MinFreq ) ;

    // calcul de la recurrence
    float RecurrenceMs = 700 - Coef01 * 550 ;

    // calcul de la largeur du beep
    float LargeurBeepMs = 100 ;

    // attente
    delay(RecurrenceMs) ;


    // emmission son
    g_GlobalVar.beeper( Freq, LargeurBeepMs ) ;
    }

g_GlobalVar.m_TaskArr[VARIOBEEP_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}
