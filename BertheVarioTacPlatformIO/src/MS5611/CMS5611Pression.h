////////////////////////////////////////////////////////////////////////////////
/// \file CMS5611Pression.h
///
/// \brief Fichier du capteur de pression
///
/// \date creation     : 07/03/2024
/// \date modification : 07/09/2024
///

#ifndef _CMS5611_
#define _CMS5611_

////////////////////////////////////////////////////////////////////////////////
/// \brief Capteur de pression et de temperature.
/// Precision du capteur 10cm.
/// Attention à la frequence du bus I2C CGlobalVar::InitI2C().
class CMS5611Pression
{
public :

    void InitMs5611() ;

    void  SetAltiSolMetres( float AltiSol ) ;
    void  SetAltiSolUndef()                 ///< pour demmarrage en vol alti = alti baro pure
            { m_DiffAltiFchAgl = 0. ; } ;

    float GetAltiMetres() ;             ///< alti pression filtree recalee alti gps en debut de vol

private :
    void  Read() ;
    float GetPressureMb() ;
    float GetAltiPressionCapteurMetres() ;
    float GetTemperatureDegres() ;

    float m_AltiPressionFiltree ;       ///< altitude pression filtree
    float m_DiffAltiFchAgl = 0 ;        ///< difference avec alti fichier agl

    float CalcAltitude(float pressure_mb_x100 , float seaLevelPressure = 101325 ) ;
} ;

#endif
