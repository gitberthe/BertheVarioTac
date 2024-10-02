////////////////////////////////////////////////////////////////////////////////
/// \file CBMP180Pression.cpp
///
/// \brief Capteur de pression
///
/// \date creation     : 22/09/2024
/// \date modification : 22/09/2024
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
//barometer = BMP180();

if (pressure.begin())
    {
    m_InitOk = true ;
     // When we have connected, we reset the device to ensure a clean start.
    //barometer.SoftReset();
    // Now we initialize the sensor and pull the calibration data.
    //barometer.Initialize();

    //barometer.SetResolution( BMP180_Mode_UltraHighResolution , true ) ;
    }
//else
    m_InitOk =false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie l'alti pression filtree recalee alti sol a la stabilisation gps
float CBMP180Pression::GetAltiMetres()
{
char status;
double T,P,p0=seaLevelPressure/100.;

status = pressure.startTemperature();
delay(status);
pressure.getTemperature(T);

const int NbAverage = 4 ;
double PAverage = 0 ;
for ( int i = 0 ; i < NbAverage ; i++ )
    {
    status = pressure.startPressure(3);
    delay(status*3);
    pressure.getPressure(P,T);
    PAverage += P ;
    }
PAverage /= NbAverage ;

return pressure.altitude(PAverage,p0); ;
}
