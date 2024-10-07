////////////////////////////////////////////////////////////////////////////////
/// \file CMS5611Pression.cpp
///
/// \brief Fichier du capteur de pression
///
/// \date creation     : 07/03/2024
/// \date modification : 07/09/2024
///

#include "../BertheVarioTac.h"

MS5611 g_MS5611(0x77);

////////////////////////////////////////////////////////////////////////////////
/// \brief Voir aussi CGlobalVar::InitI2C() pour une frequence I2C qui ne plante pas.
void CMS5611Pression::InitMs5611()
{
//g_GlobalVar.m_MutexI2c.PrendreMutex() ;

#ifdef MS5611_DEBUG
    Serial.println("Initialize MS5611 Sensor");
#endif //MS5611_DEBUG

// init MS5611
if (g_MS5611.begin() == true)
    {
    #ifdef MS5611_DEBUG
    Serial.println("MS5611 found.");
    #endif
    }
else
    {
    //CGlobalVar::BeepError() ;
    #ifdef MS5611_DEBUG
     Serial.println("MS5611 not found. halt.");
    #endif
   }

// meilleur resolution de mb
g_MS5611.setOversampling( OSR_ULTRA_HIGH ) ;
g_MS5611.setCompensation( true ) ;

//g_GlobalVar.m_MutexI2c.RelacherMutex() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul l'altitude en fonction de la pression sans compensation de
/// temperature.
float CMS5611Pression::CalcAltitude(float pressure_mb_x100 , float seaLevelPressure)
{
return (44330.0f * (1.0f - powf((float)pressure_mb_x100 / (float)seaLevelPressure, 0.1902949f)));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture des info du capteur
void CMS5611Pression::Read()
{
g_MS5611.read() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la pression en mb.
float CMS5611Pression::GetPressureMb()
{
return g_MS5611.getPressure() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la temperature en degres celsius
float CMS5611Pression::GetTemperatureDegres()
{
return g_MS5611.getTemperature() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie la mesure d'altitude pression directe du capteur
float CMS5611Pression::GetAltiPressionCapteurMetres()
{
return CalcAltitude( GetPressureMb() * 100. ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie l'alti pression filtree recalee alti sol en debut de stabilisation gps
float CMS5611Pression::GetAltiMetres()
{
float Alti = m_AltiPressionFiltree + m_DiffAltiFchAgl ;

// pour parer a un probleme
if ( Alti > 9999. || isnan(Alti) || Alti < -500. )
    {
    Alti = 9999. ;
    m_DiffAltiFchAgl = 0. ; // si probleme de diff alti comme reboot en vol
    //CGlobalVar::BeepError() ;
    //g_GlobalVar.m_MutexI2c.PrendreMutex() ;
     g_MS5611.reset() ;
    //g_GlobalVar.m_MutexI2c.RelacherMutex() ;
    InitMs5611() ;
    //delay( 500 ) ;
    }
return Alti ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne la difference avec l'alti sol.
void CMS5611Pression::SetAltiSolMetres( float AltiSol )
{
//g_GlobalVar.m_MutexI2c.PrendreMutex() ;
 Read() ;
 float AltiMesCapteur = GetAltiPressionCapteurMetres()  ;
 m_DiffAltiFchAgl = AltiSol - AltiMesCapteur ;
//g_GlobalVar.m_MutexI2c.RelacherMutex() ;

g_GlobalVar.m_TerrainPosCur.m_AltiBaro = GetAltiMetres() ;
}


