////////////////////////////////////////////////////////////////////////////////
/// \file CVecZoneReduce.h
///
/// \brief
///
/// \date creation     : 25/11/2024
/// \date modification : 27/11/2024
///

#ifndef _VECZONEREDUCE_
#define _VECZONEREDUCE_

#define DIST_METRE_PTS         40  ///< distance en metre pour le calcul/compression entre 2 points
#define ANGLE_DEGRES            5   ///< angle pour supression/compression si points alignés
#define DIST_METRE_DROITE       7   ///< distance à la droite pour supression

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
        } ;

    void Set( std::vector<st_coord_poly*> & VecToReduce )
        { m_pVecOrigine = & VecToReduce ; } ;
    void ReduceToDistanceDroiteAngleDistancePoint( int DistanceMetresEcartDroite , int MemeDirectionEnDegres , int DistanceMetresEntrePoints ) ;

private :
    std::vector<st_coord_poly*> * m_pVecOrigine ;

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

#endif
