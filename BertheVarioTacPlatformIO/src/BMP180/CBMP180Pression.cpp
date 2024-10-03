////////////////////////////////////////////////////////////////////////////////
/// \file CBMP180Pression.cpp
///
/// \brief Capteur de pression
///
/// \date creation     : 22/09/2024
/// \date modification : 03/10/2024
///

#include "../BertheVarioTac.h"

//BMP180 barometer;

SFE_BMP180 pressure;

// Store the current sea level pressure at your location in Pascals.
float seaLevelPressure = 101325;

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
/// \brief Renvoie l'alti pression filtree recalee alti sol a la stabilisation gps
void CBMP180Pression::MesureAltitudeCapteur()
{
if ( ! m_InitOk )
    {
    m_AltitudeBaroPure = 9999 ;
    return ;
    }

char status;
double T,P,p0=seaLevelPressure/100.;

status = pressure.startTemperature();
delay(status);
pressure.getTemperature(T);

const int NbAverage = 1 ;
double PAverage = 0 ;
for ( int i = 0 ; i < NbAverage ; i++ )
    {
    // mesure haute precision
    status = pressure.startPressure(3);
    // attente 26 * 5 ms d'echantillonnage (3 minimum d'apres la doc)
    delay(status*5);
    // echantillonnage
    pressure.getPressure(P,T);
    PAverage += P ;
    }
PAverage /= NbAverage ;

m_AltitudeBaroPure = pressure.altitude(PAverage,p0); ;
}
