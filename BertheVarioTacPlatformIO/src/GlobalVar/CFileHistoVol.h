////////////////////////////////////////////////////////////////////////////////
/// \file CFilesHistoVols.h
///
/// \brief
///
/// \date creation     : 22/03/2024
/// \date modification : 02/10/2024
///

#ifndef _FILE_HISTO_
#define _FILE_HISTO_

#define HISTO_DIR "/histo"

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe d'historique d'un vol.
class CHistoVol
{
public :
    int   m_AnneeDuVol = 0 ;    ///< annee du vol
    char  m_NomIgc[15] ;        ///< nom fichier igc
    int   m_ZMax  = 0 ;         ///< altitude max
    int   m_ZDeco  = 0 ;        ///< altitude deco
    float m_VzMax = -10. ;      ///< Vz Max
    float m_VzMin =  10. ;      ///< Vz Min
    float m_VsMax = 0. ;        ///< Vs Max
    float m_DistanceKm = 0. ;   ///< distance
    int   m_TempsDeVol = 0. ;   ///< temps de vol
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe d'historique des vols sur fichiers en plus du vol courant.
/// Contenue dans CGlobalVar.
class CFilesHistoVols : public CHistoVol
{
public :

    std::vector<CHistoVol> m_HistoDir ; ///< pour lecture de directories histo

    void LectureFichiers() ;
    void EcritureFichier( const char * NomFchIgc ) ;
    void DeleteHisto() ;

    float PushHistoVol() ;

    float m_LastLat = 0. ;  ///< pour calcul de distance
    float m_LastLon  = 0. ; ///< pour calcul de distance

private :
} ;

#endif
