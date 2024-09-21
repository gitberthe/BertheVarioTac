////////////////////////////////////////////////////////////////////////////////
/// \file CAutoPage.h
///
/// \brief Automate de sequencement des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 21/09/2024
///

#ifndef _CAUTOPAGE_
#define _CAUTOPAGE_

class CAutoPages
{
public :
    CAutoPages() ;

    // etats de l'automate
    enum EtatsAuto
        {
        ECRAN_0_Vz ,
        ECRAN_1_Histo ,
        ECRAN_2a_ListeIgc ,
        ECRAN_2b_ConfirmArchIgc ,
        ECRAN_3a_TmaAll ,
        ECRAN_3b_TmaMod ,
        ECRAN_4_CfgFch ,
        ECRAN_5_TmaDessous ,
        ECRAN_6_Sys ,
        ERREUR ,
        FIN
        } ;

    void SequencementPages() ;
    EtatsAuto GetEtatAuto() const   ///< renvoi l'etat de l'automate d'affichage
                { return m_EtatAuto ; } ;

protected :
    virtual EtatsAuto EcranVz()      = 0 ;
    //virtual EtatsAuto EcranHisto()   = 0 ;
    //virtual EtatsAuto EcranTmaAll() = 0 ;
    //virtual EtatsAuto EcranTmaMod() = 0 ;
    virtual EtatsAuto EcranSys() = 0 ;
    //virtual EtatsAuto EcranCfgFch() = 0 ;
    //virtual EtatsAuto EcranListeIgcFch() = 0 ;
    //virtual EtatsAuto EcranConfimeArchIgcFch() = 0 ;
    //virtual EtatsAuto EcranTmaDessous() = 0 ;
    virtual void ScreenRaz() = 0 ;
    //virtual void ScreenOff() = 0 ;

    EtatsAuto Erreur() ;

    const int       m_SecondesRetourEcran0 = 15;///< auto retour ecran 0
    unsigned long   m_MillisEcran0 ;            ///< time out de retour ecran 1 vers 0

    /// \brief etat de l'automate avec son pointeur de fonction
    class CEtatAutoFunc
        {
        public :
            CAutoPages::EtatsAuto (CAutoPages::*m_pFunction)() ;
        } ;

    EtatsAuto        m_EtatAuto = ECRAN_0_Vz ;  ///< etat courant de l'automate
    CEtatAutoFunc    m_Automate[FIN] ;       ///< l'automate
} ;

#endif
