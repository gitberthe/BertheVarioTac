////////////////////////////////////////////////////////////////////////////////
/// \file CVarioCapBeep.h
///
/// \brief
///
/// \date creation     : 03/10/2024
/// \date modification : 04/10/2024
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance la tache de mesure des capteurs et calcul de la VZ.
void CVarioCapBeep::LanceTacheVarioCapBeep()
{
xTaskCreatePinnedToCore(TacheVarioCapteur, "VarioCapteur", VARIOCAP_STACK_SIZE , this, VARIOCAP_PRIORITY, NULL, VARIOCAP_CORE);
delay( 500 ) ;
xTaskCreatePinnedToCore(TacheGenereSonVario, "VarioBeep", VARIOBEEP_STACK_SIZE , this, VARIOBEEP_PRIORITY, NULL, VARIOBEEP_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tache de mesure des capteurs et calcul de la VZ.
void CVarioCapBeep::TacheVarioCapteur(void* param)
{
g_GlobalVar.m_TaskArr[VARIOCAP_NUM_TASK].m_Stopped = false ;

// variables
int count = 0 ;
float LastAlti ;

// premiere altitude
g_GlobalVar.m_BMP180Pression.MesureAltitudeCapteur() ;
g_GlobalVar.m_TerrainPosCur.m_AltiBaro = g_GlobalVar.m_BMP180Pression.GetAltiMetres() ;
LastAlti = g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;

// boucle Vz
while (g_GlobalVar.m_TaskArr[VARIOCAP_NUM_TASK].m_Run)
    {
    // a 2 hz
    count++ ;
    delay( 500 ) ;

    g_GlobalVar.m_QMC5883Mag.LectureCap() ;

    // mesure altitude recalee et mise a jour altitude courante
    g_GlobalVar.m_BMP180Pression.MesureAltitudeCapteur() ;
    g_GlobalVar.m_TerrainPosCur.m_AltiBaro = g_GlobalVar.m_BMP180Pression.GetAltiMetres() ;

    // a 1 hz
    //if ( count%2 )
    //    continue ;

    // calcul difference alti baro pure
    float AltiBaro = g_GlobalVar.m_BMP180Pression.GetAltiBaroPureMetres() ;
    float DiffAlti = AltiBaro - LastAlti ;
    DiffAlti /= 2 ; // car 2hz
    LastAlti = AltiBaro ;

    // filtrage vz
    float Vz = g_GlobalVar.m_VitVertMS ;
    const float coef = g_GlobalVar.m_Config.m_coef_filtre_alti_baro ;
    Vz = (1.-coef) * Vz + coef * DiffAlti ;

    // recopie de Vz
    g_GlobalVar.m_VitVertMS = Vz ;
    }

g_GlobalVar.m_TaskArr[VARIOCAP_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static core 0 qui gere les beep.
void CVarioCapBeep::TacheGenereSonVario(void *param)
{
const float LowFreq = 1100 ;
const float MinFreq = 1200 ;
const float MaxFreq = 8000 ;
const float SeuilVzMin = g_GlobalVar.m_Config.m_vz_seuil_haut ;
while (g_GlobalVar.m_TaskArr[VARIOBEEP_NUM_TASK].m_Run)
    {
    // desactivation du son cause TMA
    bool NotActive = g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone == ZONE_DEDANS ||
                     g_GlobalVar.m_ZonesAerAll.m_LimiteZone == ZONE_LIMITE_ALTI ||
                     g_GlobalVar.m_ZonesAerAll.m_LimiteZone != ZONE_EN_DEHORS ;

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
    //LocalVitVertMS = 4. ;
    //float LocalVitVertMS = g_GlobalVar.m_Config.m_vz_seuil_haut ;
    #ifdef SOUND_DEBUG
     LocalVitVertMS = 6 ;
    #endif

    // si degueulante
    if ( LocalVitVertMS <= g_GlobalVar.m_Config.m_vz_seuil_bas )
        {
        g_GlobalVar.beeper( LowFreq , 400 ) ;
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
                delay( 500 ) ;
            }
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
}
