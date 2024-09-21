////////////////////////////////////////////////////////////////////////////////
/// \file CGlobalVar.h
///
/// \brief Variable globale
///
/// \date creation     : 21/09/2024
/// \date modification : 21/09/2024
///

#ifndef _GLOBALVAR_
#define _GLOBALVAR_

////////////////////////////////////////////////////////////////////////////////
/// \brief Variable globale du projet
class CGlobalVar
{
public :
    CScreen m_Screen ;

    int     m_PercentCore0 ;    ///< pourcentage utilisation core 0
    int     m_PercentCore1 ;    ///< pourcentage utilisation core 1


    static void BeepError() ;
} ;

extern CGlobalVar g_GlobalVar ;

#endif
