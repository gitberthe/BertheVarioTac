////////////////////////////////////////////////////////////////////////////////
/// \file CBMP180Pression.h
///
/// \brief Capteur de pression
///
/// \date creation     : 22/09/2024
/// \date modification : 22/01/2025
///

#ifndef _BMP180PRESS_
#define _BMP180PRESS_

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe pour capteur de pression
class CVirtCaptPress
{
public :

    virtual void InitCapteur() = 0 ;
    virtual void MesureAltitudeCapteur() = 0 ;
    virtual float GetTemperatureDegres() = 0 ;

    virtual float GetAltiMetres() ;

    void    SetAltiSolMetres( float AltitudeSolHgt ) ;
    float   GetAltiBaroPureMetres() const
                { return m_AltitudeBaroPure ; } ;
    void    SetAltiSolUndef() ;

protected :
    CMutex  m_Mutex ;
    float   m_AltitudeBaroPure ;            ///< altitude baro pure du capteur
    float   m_DiffAltiBaroHauteurSol = 0 ;  ///< difference alti baro avec la hauteur sol (recalee avant décollage)
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Capteur de pression BMP180
class CBMP180Pression : public CVirtCaptPress
{
public :
    void InitCapteur()
            { InitBMP180() ; } ;
    void InitBMP180() ;

    bool   m_InitOk = false ;
    double m_Temperature;

    void MesureAltitudeCapteur() ;
    float GetTemperatureDegres() { return 0. ; } ;
} ;

#endif
