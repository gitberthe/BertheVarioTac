////////////////////////////////////////////////////////////////////////////////
/// \file CBMP180Pression.cpp
///
/// \brief Capteur de pression
///
/// \date creation     : 22/09/2024
/// \date modification : 30/01/2025
///

#include "../BertheVarioTac.h"

//BMP180 barometer;

SFE_BMP180 pressure;

// Store the current sea level pressure at your location in Pascals.
const float seaLevelPressure = 101325;

////////////////////////////////////////////////////////////////////////////////
/// \brief Determine la difference du jour entre l'altibaro pure et la hauteur
/// sol des fichier hgt
void CVirtCaptPress::SetAltiSolMetres( float AltitudeSolHgt )
{
m_Mutex.PrendreMutex() ;
 m_DiffAltiBaroHauteurSol = AltitudeSolHgt - m_AltitudeBaroPure ;
m_Mutex.RelacherMutex() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief En cas de debut de vol sans stabilisation gps, altibaro == alti recalée
void CVirtCaptPress::SetAltiSolUndef()
{
m_Mutex.PrendreMutex() ;
 m_DiffAltiBaroHauteurSol = 0 ;
m_Mutex.RelacherMutex() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie l'alti pression recalee alti sol a la stabilisation gps
float CVirtCaptPress::GetAltiMetres()
{
m_Mutex.PrendreMutex() ;
 float ret = m_AltitudeBaroPure + m_DiffAltiBaroHauteurSol ;
m_Mutex.RelacherMutex() ;
return ret ;
}

/******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CBMP180Pression::InitBMP180()
{
if (pressure.begin())
    m_InitOk = true ;
else
    m_InitOk =false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Mesure l'altitude baro pure
float CBMP180Pression::GetPressionhPa() const
{
return m_PAverage ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Mesure l'altitude baro pure
void CBMP180Pression::MesureAltitudeCapteur()
{
if ( ! m_InitOk )
    {
    m_Mutex.PrendreMutex() ;
     m_AltitudeBaroPure = 9999 ;
    m_Mutex.RelacherMutex() ;
    return ;
    }

char status;
double P,p0=seaLevelPressure/100.;

status = pressure.startTemperature();
delay(status);
pressure.getTemperature(m_Temperature);

const int NbAverage = 1 ;
double PAverage = 0 ;
for ( int i = 0 ; i < NbAverage ; i++ )
    {
    // mesure haute precision
    status = pressure.startPressure(3);
    // attente 26 * 5 ms d'echantillonnage (3 minimum d'apres la doc)
    delay(status*5);
    // echantillonnage
    pressure.getPressure(P,m_Temperature);
    PAverage += P ;
    }
PAverage /= NbAverage ;
m_PAverage = PAverage ;

m_Mutex.PrendreMutex() ;
 m_AltitudeBaroPure = pressure.altitude(m_PAverage,p0); ;
m_Mutex.RelacherMutex() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Mesure la temperature
float CBMP180Pression::GetTemperatureDegres()
{
double temp ;
m_Mutex.PrendreMutex() ;
 pressure.getTemperature( temp ) ;
m_Mutex.RelacherMutex() ;
return temp ;
}
