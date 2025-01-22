////////////////////////////////////////////////////////////////////////////////
/// \file CRandoVol.h
///
/// \brief
///
/// \date creation     : 13/01/2025
/// \date modification : 22/01/2025
///

#pragma once

#define GPX_DIR "/gpx"

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de suivit de fichier de trace GPX avec le gps.
class CRandoVol
{
public :
    enum EtatRando
        {
        ConfirmeRando ,
        AttenteGps ,
        InitMenu ,
        AfficheMenu ,
        //InitTrace ,
        //Navigation
        } ;

    void LireFichiersGpx() ;

    bool      m_OrientationCapGps = false ;      ///< orientation carte de trace

    const char * GetTrackName( int i ) const ;

    std::vector<CFileGpx*>  m_VecGpx ;          ///< vecteur des traces Gpx
    CFileGpx*               m_pFileGpx = NULL ; ///< fichier selectionne
private :
    void SortGpxFromCurPos() ;
} ;
