////////////////////////////////////////////////////////////////////////////////
/// \file CVarioCapBeep.h
///
/// \brief 
///
/// \date creation     : 03/10/2024
/// \date modification : 03/10/2024
///

#ifndef _CVARIOCAPBEEP_
#define _CVARIOCAPBEEP_

////////////////////////////////////////////////////////////////////////////////
/// \brief Lectture capteur pression/cap et generation de la Vz
class CVarioCapBeep
{
public :
    void LanceTacheVarioCapBeep() ;

private :
    static void TacheVarioCapteurBeep(void *param) ;
} ;

#endif
