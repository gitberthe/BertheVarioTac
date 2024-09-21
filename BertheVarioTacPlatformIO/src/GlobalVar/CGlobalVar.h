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

    static void BeepError() ;
} ;

extern CGlobalVar g_GlobalVar ;

#endif
