////////////////////////////////////////////////////////////////////////////////
/// \file CBMP180Pression.h
///
/// \brief Capteur de pression
///
/// \date creation     : 22/09/2024
/// \date modification : 22/09/2024
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
    double m_PressionRef ;

    void SetAltiSolUndef()
        {} ;
    void SetAltiSolMetres( int AltitudeSolHgt )
        {} ;
    float GetAltiMetres() ;
} ;

#endif
