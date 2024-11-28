////////////////////////////////////////////////////////////////////////////////
/// \file CVecZoneReduce.h
///
/// \brief
///
/// \date creation     : 25/11/2024
/// \date modification : 28/11/2024
///

#ifndef _VECZONEREDUCE_
#define _VECZONEREDUCE_

#define DIST_METRE_PTS         40  ///< distance en metre pour le calcul/compression entre 2 points
#define ANGLE_DEGRES            4   ///< angle pour supression/compression si points alignés
#define DIST_METRE_DROITE       7   ///< distance à la droite pour supression
#define DIST_METRE_MULTI_DROITE 5   ///< distance à la droite pour supression
#define DIST_METRE_NUAGE_DROITE 40   ///< distance à la droite pour supression

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de reduction du nombre de points par suppression de ceux qui
/// sont espacés de 50 metres ou qui sont en ligne droite de 6°.
/// Uilise par BVTZoneAerienne lors de la creation du fichier de zones.
class CVecZoneReduce
{
public :

    /// \brief un point du polygone
    struct st_coord_poly
        {
        float m_Lat ;   ///< latitude en degres
        float m_Lon ;   ///< longitude en degres
        bool  m_PtADetuire = false ;
        } ;

    void Set( std::vector<st_coord_poly*> & VecToReduce )
        { m_pVecOrigine = & VecToReduce ; } ;
    void ReduceToDistanceDroiteAngleDistancePoint( int DistanceMetresEcartDroite , int MemeDirectionEnDegres , int DistanceMetresEntrePoints ) ;
    int ReduceMultiDistanceDroite( float DistanceMetresEcartDroite ) ;
    void ReduceNuageDroite( float DistanceMetresEcartDroite ) ;

private :
    std::vector<st_coord_poly*> * m_pVecOrigine ;
    void AjoutePointSansDoublons( std::vector<st_coord_poly*> & VecReduce , int ipa ) ;
    void AjoutePointSansDoublons( std::vector<st_coord_poly*> & VecReduce , st_coord_poly* pStCoord ) ;

    //float GetBravaisPearson( const std::vector<long> &VecIPts ) ;

    /*struct st_pts_i_b
        {
        int   m_ipts ;          ///< position vecteur origine
        float m_DistanceBari ;  ///< distance baricentre

        bool operator > ( const st_pts_i_b & pts ) const
            { return m_DistanceBari > pts.m_DistanceBari ; } ;
        bool operator < ( const st_pts_i_b & pts ) const
            { return m_DistanceBari < pts.m_DistanceBari  ; } ;
        } ; */
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de tri de points par longitude
class CSortPts
{
public :
    float m_Lon  ;
    int   m_ipts ;

   static void Sort( std::vector<CVecZoneReduce::st_coord_poly*> & VecToSort ) ;

    bool operator > ( const CSortPts & pts ) const
            { return m_Lon > pts.m_Lon ; } ;
    bool operator < ( const CSortPts & pts ) const
            { return m_Lon < pts.m_Lon ; } ;

} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe d'approximation de droite d'un nuage de points
class CNuage2Droite
{
private :
    double m_ax ;
    double m_b ;
    void GetDroite( CDroite2D & Droite ) const ;

public :
    CDroite2D ApproxDroite( const std::vector<CVecZoneReduce::st_coord_poly*> & VecNuage ) ;
} ;

class CGroupeAligne
{
public :
    std::vector<CVecZoneReduce::st_coord_poly*> m_Vect ;

    bool operator > ( const CGroupeAligne & group ) const
            { return m_Vect.size() > group.m_Vect.size() ; } ;
    bool operator < ( const CGroupeAligne & group ) const
            { return m_Vect.size() < group.m_Vect.size() ; } ;

} ;

class CVecGroupAligne : public std::vector<CGroupeAligne>
{
} ;

#endif
