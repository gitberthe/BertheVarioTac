////////////////////////////////////////////////////////////////////////////////
/// \file CZoneAer.h
///
/// \brief
///
/// \date creation   : 23/03/2024
/// \date 25/11/2024 : ajout de la compression des float en short et lz4.
///                    -DLZ4_MEMORY_USAGE=15 compression maximum avec cette memoire
/// \date 25/11/2024 : modification
///

#ifndef _ZONE_AR_
#define _ZONE_AR_

#define ALTI_BASSE           0
#define ALTI_PERIODE_SEMAINE 1
#define ALTI_PERIODE_WEEKEND 2

#define ResolCompress        (18.)

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
    int     m_AltiBassePeriodeSemaine=-1;///< altitude basse dans la periode en semaine
    int     m_AltiBassePeriodeWeekEnd=-1;///< altitude basse dans la periode en week-end
    int     m_PeriodeDebutJour = -1 ;   ///< periode debut jour pour altitude
    int     m_PeriodeFinJour   = -1 ;   ///< periode fin jour pour altitude
    int     m_PeriodeDebutMois = -1 ;   ///< periode debut mois pour altitude
    int     m_PeriodeFinMois   = -1 ;   ///< periode fin mois pour altitude
    int     m_AltiAPrendreEnCompte = ALTI_BASSE ;   ///< altitude a prendre en compte fonction de la date
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Une zone aerienne, avec un nombre de points diminué (distance minimale
/// entre 2 points).
/// Periode semaine/week end, Active ou non. sous forme de tableau de pointeur
/// dans CZonesAerAll.
class CZoneAer
{
public :
    ~CZoneAer() ;

    static  void TriParNom( bool Tri )
                { ms_TriParNom = Tri ; } ;
    int     GetAltiBasse() const ;
    bool    HavePeriod() const
                { return m_pDerogFfvl != NULL ; } ;
    std::string  m_NomAff ;             ///< nom court de la zone a afficher
    std::string  m_NomOri ;             ///< nom entier de la zone dans fichier origine
    bool    m_Activee = true ;          ///< zone activee
    bool    m_DansFchActivation = false;///< si dans fichier d'activation de zones pour reecriture apres configuration menu
    int     m_AltiBasse=-1 ;            ///< altitude basse de la zone par defaut
    CZoneAerDerogFfvl * m_pDerogFfvl = NULL ; ///< si zone avec derogation ffvl

    void CompressZone() ;
    void UnCompressZone() ;
    void FreeFloat() ;

    friend class CZonesAerAll ;

    bool operator > ( const CZoneAer & Zone ) const ;
    bool operator < ( const CZoneAer & Zone ) const ;

private :
    static int      ms_max_dst_size ;           ///< taille du buffer lz4
    static int      ms_MaxNombrePtsZone ;       ///< nombre de pts maximum d'une zone
    static char*    ms_compressed_data_lz4 ;    ///< buffer de compression lz4

private :
    CVecZoneReduce::st_coord_poly **m_PolyStLaLoArr=NULL;   ///< tableau des points de la zone
    int                             m_NbStLaLoPts = 0 ;     ///< nombre de points de la zone

    short                          *m_LowResShortArr=NULL;   ///< tableau des coordonnees relatives au barycentre pour compression short
    char                           *m_CharLz4Arr=NULL ;      ///< tableau des short compresse lz4
    short                           m_Lz4BuffSize ;          ///< taille du buffer des lz4

    CVecZoneReduce::st_coord_poly   m_Barycentre ;          ///< pour une recherche rapide
    float                           m_RayonMetre ;          ///< pour une recherche rapide

    static bool ms_TriParNom ;  ///< pour un tri par nom
} ;

#endif
