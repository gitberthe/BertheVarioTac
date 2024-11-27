////////////////////////////////////////////////////////////////////////////////
/// \file CVecZoneReduce.cpp
///
/// \brief
///
/// \date creation     : 25/11/2024
/// \date modification : 27/11/2024
///

#include "CompZoneAerienne.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Reduit le vecteur en nombre de points. Prends les points 3 a 3 et
/// supprime le points du centre s'il est a une distance trop importante de la
/// droite qui relie les 2 autres.
void CVecZoneReduce::ReduceToDistanceDroiteAngleDistancePoint( int DistanceMetresEcartDroite , int MemeDirectionEnDegres , int DistanceMetresEntrePoints )
{
// temps que points a deleter
bool DeleteVec = true ;
while ( DeleteVec )
    {
    DeleteVec = false ;

    bool BoucleEnCours = true ;
    // supression des points 3 a 3
    for ( int ipa = 0 ; ipa+2 < (int)m_pVecOrigine->size() && BoucleEnCours ; ipa += 1 )
        {
        // vecteur des 3 poitns
        std::vector< CPoint2D > Vect3pts ;
        for ( long ipb = 0 ; ipb < 3 && ipb < (int)m_pVecOrigine->size() ; ipb++ )
            {
            // un point deja efface
            if ( (*m_pVecOrigine)[ipa+ipb] == NULL )
                {
                BoucleEnCours = false ;
                break ;
                }
            Vect3pts.push_back( CPoint2D( (*m_pVecOrigine)[ipa+ipb]->m_Lon , (*m_pVecOrigine)[ipa+ipb]->m_Lat) ) ;
            }

        // si pas assez de points
        if ( Vect3pts.size() < 3 || !BoucleEnCours )
            break;

        // calcul des vecteur
        CVecteur2D VecDir( Vect3pts[2] , Vect3pts[0] ) ;
        CVecteur2D VecAutre( Vect3pts[1] , Vect3pts[0] ) ;

        // calcul de la droite
        CDroite2D  Droite( VecDir , Vect3pts[0] ) ;

        // vecteur perpendiculaire
        CPoint2D PtProj = Droite.GetProjectionDuPoint( Vect3pts[1] ) ;
        CVecteur2D VecPerendiculaire( PtProj , Vect3pts[1] ) ;

        // destruction de points si distance a la droite sufisante
        if ( DistanceMetresEcartDroite > 0 && VecPerendiculaire.GetNorm() * MilesParDegres * UnMileEnMetres < DistanceMetresEcartDroite )
            {
            // destruction du point du milieu
            if ( VecDir.GetNorm() > VecAutre.GetNorm() )
                {
                DeleteVec = true ;
                delete (*m_pVecOrigine)[ipa+1] ;
                (*m_pVecOrigine)[ipa+1] = NULL ;
                continue ;
                }
            }

        // destruction distance
        float Dist1 = Vect3pts[1].GetDistanceAuPoint( Vect3pts[0] ) * MilesParDegres * UnMileEnMetres ;
        float Dist2 = Vect3pts[1].GetDistanceAuPoint( Vect3pts[2] ) * MilesParDegres * UnMileEnMetres ;

        if ( DistanceMetresEntrePoints > 0 && Dist1 < DistanceMetresEntrePoints )
            {
            DeleteVec = true ;
            delete (*m_pVecOrigine)[ipa] ;
            (*m_pVecOrigine)[ipa] = NULL ;
            continue ;
            }
        if ( DistanceMetresEntrePoints > 0 && Dist2 < DistanceMetresEntrePoints )
            {
            DeleteVec = true ;
            delete (*m_pVecOrigine)[ipa+2] ;
            (*m_pVecOrigine)[ipa+2] = NULL ;
            continue ;
            }

        // destruction angle plat
        float AngleDeg = VecDir.GetAngleDeg(VecAutre) ;
        if ( MemeDirectionEnDegres > 0 &&
             (AngleDeg < MemeDirectionEnDegres || AngleDeg > (180-MemeDirectionEnDegres)) &&
             Dist1<(DIST_METRE_PTS*20) && Dist2<(DIST_METRE_PTS*20) )
            {
            DeleteVec = true ;
            delete (*m_pVecOrigine)[ipa+1] ;
            (*m_pVecOrigine)[ipa+1] = NULL ;
            continue ;
            }
        }

    // recopie des points restant
    std::vector<st_coord_poly*> VecReduce ;
    for ( long ipt = 0 ; ipt < (int)m_pVecOrigine->size() ; ipt++ )
        {
        if ( (*m_pVecOrigine)[ipt] != NULL )
            VecReduce.push_back( (*m_pVecOrigine)[ipt] ) ;
        }

    // recopie du vecteur reduit
    *m_pVecOrigine = VecReduce ;
    //m_pVecOrigine->shrink_to_fit() ;
    }
}
