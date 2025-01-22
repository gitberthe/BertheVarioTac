////////////////////////////////////////////////////////////////////////////////
/// \file CMS5611Pression.h
///
/// \brief Fichier du capteur de pression
///
/// \date creation     : 07/03/2024
/// \date modification : 22/01/2025
///

#ifndef _CMS5611_
#define _CMS5611_


////////////////////////////////////////////////////////////////////////////////
/// \brief Capteur de pression et de temperature.
/// Precision du capteur 10cm.
/// Attention à la frequence du bus I2C CGlobalVar::InitI2C().
class CMS5611Pression : public CVirtCaptPress
{
public :

    void InitCapteur()
            { InitMs5611() ; } ;
    void InitMs5611() ;

    void MesureAltitudeCapteur() ;

    float GetAltiMetres() override ;
    float GetTemperatureDegres() override ;

private :
    void  Read() ;
    float GetPressureMb() ;
    float GetAltiPressionCapteurMetres() ;

    float CalcAltitude(float pressure_mb_x100 , float seaLevelPressure = 101325 ) ;
} ;

#endif
