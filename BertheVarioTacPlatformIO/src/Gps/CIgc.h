////////////////////////////////////////////////////////////////////////////////
/// \file CIgc.h
///
/// \brief
///
/// \date creation     : 14/03/2024
/// \date modification : 02/10/2024
///

#ifndef _IGC_CLASS_
#define _IGC_CLASS_

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet d'ecrire un fichier IGC. L'altitude prise en compte sur les sites
/// web est l'altibarometrique (recalee gps au debut du vol) tres precise (moins d'1m).
/// l'alti gps n'est precise qu'au debut du vol. Pendant le vol elle derive de 50m.
class CIgc
{
public :

    void InitCurentIgc() ;
    void PushLoc2Igc() ;
    const char * GetIgcFileName() const
        { return m_NomFchCourant ; } ;

private :
    char m_NomFchCourant[20] ;  ///< nom du fichier courant igc
} ;

#endif

