////////////////////////////////////////////////////////////////////////////////
/// \file CAutoPage.h
///
/// \brief Automate de sequencement des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 23/09/2024
///

#ifndef _CAUTOPAGE_
#define _CAUTOPAGE_

////////////////////////////////////////////////////////////////////////////////
/// \brief Automate de sequencement des pages.
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
        ECRAN_7_Wifi ,
        ERREUR ,
        FIN
        } ;

    void      SequencementPages() ;
    EtatsAuto GetEtatAuto() const   ///< renvoie l'etat de l'automate d'affichage
                { return m_EtatAuto ; } ;
    void      SetLastEtatAuto() ;

    bool IsPageChanged() const      ///< renvoie si l'on vien de chabger de page
        { return m_PageChanged ; } ;
    void ResetTimeOut()             ///< remet le time out page Vz à zero lors de configuration Cfg file ou Tma
        { m_MillisEcran0 = millis() ; } ;

protected :

    virtual EtatsAuto EcranVz() = 0 ;
    virtual EtatsAuto EcranHisto() = 0 ;
    virtual EtatsAuto EcranListeIgcFch() = 0 ;
    virtual EtatsAuto EcranTmaAll() = 0 ;
    virtual EtatsAuto EcranCfgFch() = 0 ;
    virtual EtatsAuto EcranTmaDessous() = 0 ;
    virtual EtatsAuto EcranSys() = 0 ;
    virtual EtatsAuto EcranWifi() = 0 ;

    virtual EtatsAuto EcranTmaMod() = 0 ;
    virtual EtatsAuto EcranConfimeArchIgcFch() = 0 ;

    virtual void ScreenRaz() = 0 ;
    virtual void ScreenRazButtons() = 0 ;
    //virtual void ScreenOff() = 0 ;

    EtatsAuto Erreur() ;

    /// \brief etat de l'automate avec son pointeur de fonction
    class CEtatAutoFunc
        {
        public :
            CAutoPages::EtatsAuto (CAutoPages::*m_pFunction)() ;
        } ;

    int     m_CfgFileiChamps = -1 ; ///< lors du deplacement dans les variables cfg file

private :
    EtatsAuto       m_EtatAuto = ECRAN_0_Vz ;   ///< etat courant de l'automate
    //EtatsAuto       m_LastEtatAuto = ECRAN_0_Vz ;   ///< pour le retour en arriere
    CEtatAutoFunc   m_Automate[FIN] ;       ///< l'automate
    bool            m_PageChanged = false ; ///< si l'on vient de changer de page
    const int       m_SecondesRetourEcran0 = 15;///< auto retour ecran 0
    unsigned long   m_MillisEcran0 ;            ///< time out de retour ecran 1 vers 0

} ;

#endif
