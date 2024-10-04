////////////////////////////////////////////////////////////////////////////////
/// \file CBMP180Pression.h
///
/// \brief Capteur de pression
///
/// \date creation     : 22/09/2024
/// \date modification : 04/10/2024
///

#ifndef _BMP180PRESS_
#define _BMP180PRESS_

////////////////////////////////////////////////////////////////////////////////
/// \brief Capteur de pression BMP180
class CBMP180Pression
{
public :
    void InitBMP180() ;

    bool   m_InitOk = false ;
    double m_Temperature;

    void SetAltiSolUndef() ;
    void SetAltiSolMetres( int AltitudeSolHgt ) ;
    float GetAltiMetres() ;
    float GetAltiBaroPureMetres()
        { return m_AltitudeBaroPure ; } ;

    void MesureAltitudeCapteur() ;

private :
    CMutex  m_Mutex ;
    float   m_AltitudeBaroPure ;            ///< altitude baro pure du capteur
    float   m_DiffAltiBaroHauteurSol = 0 ;  ///< difference alti baro avec la hauteur sol (recalee avant décollage)
} ;

#endif
