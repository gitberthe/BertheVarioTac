////////////////////////////////////////////////////////////////////////////////
/// \file CZoneAer.h
///
/// \brief
///
/// \date creation   : 23/03/2024
/// \date 25/11/2024 : ajout de la compression des float en short et lz4.
///                    -DLZ4_MEMORY_USAGE=15 compression maximum avec cette memoire
/// \date 02/12/2024 : modification
///

#ifndef _ZONE_AR_
#define _ZONE_AR_

#define ALTI_BASSE           0
#define ALTI_PERIODE_SEMAINE 1
#define ALTI_PERIODE_WEEKEND 2

// FL 195	5800 mètres
// FL 175	5332 mètres
// FL 155	4722 mètres
// FL 125	3808 mètres
// FL 115	3503 mètres
// FL 085	2589 mètres
// SFC	sol

////////////////////////////////////////////////////////////////////////////////
/// \brief Parametres de date et altitude pour une derogation ffvl de zone
class CZoneAerDerogFfvl
{
public :
    short     m_AltiBassePeriodeSemaine=-1;///< altitude basse dans la periode en semaine
    short     m_AltiBassePeriodeWeekEnd=-1;///< altitude basse dans la periode en week-end
    short     m_PeriodeDebutJour = -1 ;   ///< periode debut jour pour altitude
    short     m_PeriodeFinJour   = -1 ;   ///< periode fin jour pour altitude
    short     m_PeriodeDebutMois = -1 ;   ///< periode debut mois pour altitude
    short     m_PeriodeFinMois   = -1 ;   ///< periode fin mois pour altitude
    short     m_AltiAPrendreEnCompte = ALTI_BASSE ;   ///< altitude a prendre en compte fonction de la date
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Une zone aerienne, avec un nombre de points diminué (distance minimale
/// entre 2 points).
/// Periode semaine/week end, Active ou non. sous forme de tableau de pointeur
/// dans CZonesAerAll.
class CZoneAer
{
public :
    /// \brief un point du polygone
    struct st_coord_poly
        {
        float m_Lat ;   ///< latitude en degres
        float m_Lon ;   ///< longitude en degres
        } ;

    ~CZoneAer() ;

    static  void TriParNom( bool Tri )
                { ms_TriParNom = Tri ; } ;
    int     GetAltiBasse() const ;
    bool    HavePeriod() const
                { return m_pDerogFfvl != NULL ; } ;
    bool    IsProtect() const
                { return m_HauteurSolZoneProtege > 0 ; } ;
    short   GetHauteurSolZoneProtect() const
                { return m_HauteurSolZoneProtege ; } ;

    std::string         m_NomAff ;             ///< nom court de la zone a afficher
    std::string         m_NomOri ;             ///< nom entier de la zone dans fichier origine
    bool                m_Activee = true ;          ///< zone activee
    bool                m_DansFchActivation = false;///< si dans fichier d'activation de zones pour reecriture apres configuration menu
    short               m_AltiBasse=-1 ;            ///< altitude basse de la zone par defaut
    CZoneAerDerogFfvl * m_pDerogFfvl = NULL ; ///< si zone avec derogation ffvl

    void CompressZone() ;
    void UnCompressZone() ;
    void FreeFloat() ;

    friend class CZonesAerAll ;

    bool operator > ( const CZoneAer & Zone ) const ;
    bool operator < ( const CZoneAer & Zone ) const ;

private :
    static int      ms_max_dst_size ;           ///< taille du buffer lz4 de compression
    static int      ms_MaxNombrePtsZone ;       ///< nombre de pts maximum d'une zone
    static char*    ms_compressed_data_lz4 ;    ///< buffer de compression lz4

private :
    CZoneAer::st_coord_poly *   m_PolyStLaLoArr=NULL;   ///< tableau des points de la zone
    short                       m_NbStLaLoPts = 0 ;     ///< nombre de points de la zone

    char                        *m_CharLz4Arr=NULL ;     ///< tableau des short compresse lz4
    short                       m_Lz4BuffSize ;         ///< taille du buffer des short compresses lz4
    bool                        m_CompressLz4 ;         ///< indique si les donnees sont compressee lz4

    CZoneAer::st_coord_poly     m_Barycentre ;          ///< pour une recherche rapide
    float                       m_RayonMetre ;          ///< pour une recherche rapide

    short                       m_ResolutionMetre ;     ///< resolution en metre de la zone lors de compression short
    short                       m_HauteurSolZoneProtege = -1 ; ///< hauteur de la zone protegee

    static bool                 ms_TriParNom ;  ///< pour un tri par nom
} ;

#endif
