////////////////////////////////////////////////////////////////////////////////
/// \file CTrame.h
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 16/10/2024
///

#ifndef _TRAME_
#define _TRAME_

////////////////////////////////////////////////////////////////////////////////
/// \brief Gestion trame Gps
class CTrame
{
public :
    void Push(char c) ;

    static int GetNbSat()   ///< nombre de satellites en vue
        { return m_NombreSatellite ; } ;

protected :
    static unsigned long m_MillisPremierGGA ;   ///< temps du premier GGA pour stabilisation donnees gps
    static int           m_NombreSatellite ;    ///< nombre de satellites pour le GGA

private :
    void TraiteBuffer() ;

    char m_BufRecep[100] = {0} ;   ///< buffer de reception de la trame
    int  m_iBuf = 0 ;              ///< i position buffer reception
} ;

#endif

// ex message le plus long :
// $GPGGA,035054.00,4607.34813,N,00324.32596,E,1,04,2.04,268.1,M,47.3,M,,*52
