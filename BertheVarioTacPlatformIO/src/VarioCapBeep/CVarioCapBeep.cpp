////////////////////////////////////////////////////////////////////////////////
/// \file CVarioCapBeep.h
///
/// \brief
///
/// \date creation     : 03/10/2024
/// \date modification : 03/10/2024
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance la tache de mesure des capteurs et calcul de la VZ.
void CVarioCapBeep::LanceTacheVarioCapBeep()
{
xTaskCreatePinnedToCore(TacheVarioCapteurBeep, "VarioCapteur", VARIOCAPBEEP_STACK_SIZE , this, VARIOCAPBEEP_PRIORITY, NULL, VARIOCAPBEEP_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tache de mesure des capteurs et calcul de la VZ.
void CVarioCapBeep::TacheVarioCapteurBeep(void* param)
{
g_GlobalVar.m_TaskArr[VARIOCAPBEEP_NUM_TASK].m_Stopped = false ;

int count = 0 ;
float LastAlti ;
g_GlobalVar.m_BMP180Pression.MesureAltitudeCapteur() ;
g_GlobalVar.m_TerrainPosCur.m_AltiBaro = g_GlobalVar.m_BMP180Pression.m_AltitudeBaroPure ;
LastAlti = g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;
while (g_GlobalVar.m_TaskArr[VARIOCAPBEEP_NUM_TASK].m_Run)
    {
    // a 2 hz
    count++ ;
    delay( 500 ) ;

    g_GlobalVar.m_BMP180Pression.MesureAltitudeCapteur() ;
    g_GlobalVar.m_TerrainPosCur.m_AltiBaro = g_GlobalVar.m_BMP180Pression.m_AltitudeBaroPure ;

    // a 1 hz
    if ( count%2 )
        continue ;

    // calcul difference alti courante
    float DiffAlti = g_GlobalVar.m_TerrainPosCur.m_AltiBaro - LastAlti ;
    LastAlti = g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;

    // filtrage vz
    float Vz = g_GlobalVar.m_VitVertMS ;
    const float coef = 0.4 ;
    Vz = (1.-coef) * Vz + coef * DiffAlti ;

    // recopie
    g_GlobalVar.m_VitVertMS = Vz ;
    }

g_GlobalVar.m_TaskArr[VARIOCAPBEEP_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}

