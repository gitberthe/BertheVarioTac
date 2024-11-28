////////////////////////////////////////////////////////////////////////////////
/// \file CVecZoneReduce.cpp
///
/// \brief
///
/// \date creation     : 25/11/2024
/// \date modification : 28/11/2024
///

#include "CompZoneAerienne.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Reduit le vecteur en nombre de points. Cumul de 3 methodes. Par distance
/// à une droite,distance entre les points et par angle d'alignement si parametre != -1.
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
             Dist1<(DIST_METRE_PTS*40) && Dist2<(DIST_METRE_PTS*40) )
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

////////////////////////////////////////////////////////////////////////////////
/// \brief Reduit le vecteur en nombre de points. Prends les points 3 a 3 et
/// supprime les points centreaux s'il est a une distance trop importante à
/// la droite qui relie les 2 extremes.
int CVecZoneReduce::ReduceMultiDistanceDroite( float DistanceMetresEcartDroite )
{
std::vector<st_coord_poly*> VecReduce ;
const long size = m_pVecOrigine->size() ;

// pour tous les points
long NbPts = 0 ;
for ( long ipall = 0 ; ipall < size ; ipall+=NbPts-1 )
    {
    bool PtsDeleted = false ;
    NbPts = 0 ;

    // nombre de points croissant/glissant
    std::vector<st_coord_poly*> VecSort ;
    for ( NbPts = 3 ; NbPts + ipall < size ; NbPts++ )
        {
        VecSort.clear() ;
        for ( long ipts = 0 ; ipts < NbPts ; ipts++ )
            if ( ipall+ipts < size )
                VecSort.push_back( (*m_pVecOrigine)[ipall+ipts] ) ;

        CSortPts::Sort( VecSort ) ;

        // vecteur de points en cours
        std::vector< CPoint2D > Vectxpts ;
        for ( long ipb = 0 ; ipb < NbPts ; ipb++ )
            Vectxpts.push_back( CPoint2D( VecSort[ipb]->m_Lon , VecSort[ipb]->m_Lat) ) ;

        /*CVecteur2D VecDir1( Vectxpts[0] , Vectxpts[Vectxpts.size()-1] ) ;
        CVecteur2D VecDir2( Vectxpts[0] , Vectxpts[Vectxpts.size()-2] ) ;
        float Angle = VecDir1.GetAngleDeg(VecDir2) ;*/

        // calcul de la droite des extremites
        CVecteur2D VecDir( Vectxpts[0] , Vectxpts[Vectxpts.size()-1] ) ;
        CDroite2D  Droite( VecDir , Vectxpts[0] ) ;

        // vecteur perpendiculaire à l'avant dernier point rajouté
        CPoint2D PtProj = Droite.GetProjectionDuPoint( Vectxpts[Vectxpts.size()-2] ) ;
        CVecteur2D VecPerendiculaire( PtProj , Vectxpts[Vectxpts.size()-2] ) ;

        // destruction de points si distance a la droite sufisante
        if ( DistanceMetresEcartDroite > 0 && VecPerendiculaire.GetNorm() * MilesParDegres * UnMileEnMetres > DistanceMetresEcartDroite )
        //if ( Angle > 3 )
            {
            //NbPts-- ;
            if ( VecSort.size() > 1 )
                {
                VecSort.resize( VecSort.size()-1 ) ;
                CSortPts::Sort( VecSort ) ;
                }
            break ;
            }

        PtsDeleted = true ;
        }

    // si pas de points supprime
    if ( PtsDeleted )
        {
        // tri en ligne des points par longitude croissante
        AjoutePointSansDoublons( VecReduce , VecSort[0] ) ;
        AjoutePointSansDoublons( VecReduce , VecSort[VecSort.size()-1] ) ;
        NbPts = VecSort.size()-1 ;
        }
    else
        {
        NbPts = 2 ;
        AjoutePointSansDoublons( VecReduce , (*m_pVecOrigine)[ipall] ) ;
        }
    }

// recopie du vecteur reduit
int ret = m_pVecOrigine->size() - VecReduce.size() ;
*m_pVecOrigine = VecReduce ;
return ret ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Reduit les zones par approximation de nuage de points par une droite.
void CVecZoneReduce::ReduceNuageDroite( float DistanceMetresEcartDroite )
{
std::vector<st_coord_poly*> VecReduce ;
const long size = m_pVecOrigine->size() ;

CVecGroupAligne VecGroupAligne ;

// pour tous les points
long NbPts = 0 ;
for ( long ipall = 0 ; ipall < size ; ipall+=NbPts-1 )
    {
//    bool PtsDeleted = false ;
//    NbPts = 0 ;

    // nombre de points croissant/glissant
    const int debpts = 3 ;
    std::vector<st_coord_poly*> VecSort ;
    for ( NbPts = debpts ; NbPts + ipall < size ; NbPts++ )
        {
        // vecteur des points en cours
        VecSort.clear() ;
        for ( long ipts = 0 ; ipts < NbPts ; ipts++ )
            if ( ipall+ipts < size )
                VecSort.push_back( (*m_pVecOrigine)[ipall+ipts] ) ;

        if ( VecSort.size() < debpts )
            break ;

        // tri vecteur par longitude
        CSortPts::Sort( VecSort ) ;

        // droite du nuage de points
        CNuage2Droite Nuage2Droite ;
        CDroite2D Droite = Nuage2Droite.ApproxDroite( VecSort ) ;

        // vecteur de points 2D en cours
        std::vector< CPoint2D > Vectxpts ;
        for ( long ipb = 0 ; ipb < NbPts ; ipb++ )
            Vectxpts.push_back( CPoint2D( VecSort[ipb]->m_Lon , VecSort[ipb]->m_Lat) ) ;

        // calcul de la distance a la droite a chaque point
        int NuageOk = 0 ;
        for ( long ipts = 0 ; ipts < NbPts ; ipts++ )
            {
            // vecteur perpendiculaire à l'avant dernier point rajouté
            CPoint2D PtProj = Droite.GetProjectionDuPoint( Vectxpts[ipts] ) ;
            CVecteur2D VecPerendiculaire( PtProj , Vectxpts[ipts] ) ;

            // destruction de points si distance a la droite sufisante
            if ( DistanceMetresEcartDroite > 0 && VecPerendiculaire.GetNorm() * MilesParDegres * UnMileEnMetres > DistanceMetresEcartDroite )
                NuageOk++ ;
            }

        // si nuage pas aligne
        if ( NuageOk > 0 )
            {
            // on enleve le dernier points
            if ( VecSort.size() > 1 )
                {
                VecSort.resize( VecSort.size()-1 ) ;
                CGroupeAligne GroupAligne ;
                GroupAligne.m_Vect = VecSort ;
                VecGroupAligne.push_back( GroupAligne ) ;
                }
            NbPts-- ;
            break ;
            }
        }
    }

std::sort( VecGroupAligne.begin() , VecGroupAligne.end() ) ;


//for ( size_t ig = 0 ; ig < VecGroupAligne.size() ; ig++ )
for ( size_t ig = VecGroupAligne.size() - 1 ; ig > 0 ; ig-- )
    {
    std::vector<st_coord_poly*> & VecAligne = VecGroupAligne[ig].m_Vect ;

    // tri vecteur par longitude
    //CSortPts::Sort( VecAligne ) ;

    if ( VecAligne.size() > 3 )
        {
        //bool cont = false ;
        // si les bouts sont deja detuit
        //for ( size_t ip = 0 ; ip < VecAligne.size() ; ip++ )
        //if ( VecAligne[0]->m_PtADetuire || VecAligne[VecAligne.size()-1]->m_PtADetuire )
        //    continue ;
        //    if ( VecAligne[ip]->m_PtADetuire )
        //        cont = true ;

        //if ( cont )
        //    continue ;
        for ( size_t ip = 1 ; ip < VecAligne.size()-1 ; ip++ )
            VecAligne[ip]->m_PtADetuire = true ;
        }
    }

for ( size_t ip = 0 ; ip < m_pVecOrigine->size() ; ip++ )
    {
    if ( ! (*m_pVecOrigine)[ip]->m_PtADetuire )
        VecReduce.push_back( (*m_pVecOrigine)[ip] ) ;
    }
// recopie du vecteur reduit
*m_pVecOrigine = VecReduce ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ajoute des points au vecteur sans doublons
void CVecZoneReduce::AjoutePointSansDoublons( std::vector<st_coord_poly*> & VecReduce , st_coord_poly* pStCoord )
{
if ( pStCoord == NULL )
    return ;

for ( size_t ip = 0 ; ip < VecReduce.size() ; ip++ )
    if ( pStCoord == VecReduce[ip] )
        return ;

VecReduce.push_back( pStCoord ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ajoute des points au vecteur sans doublons
void CVecZoneReduce::AjoutePointSansDoublons( std::vector<st_coord_poly*> & VecReduce , int ipa )
{
st_coord_poly* ppt = (*m_pVecOrigine)[ipa] ;

for ( size_t ip = 0 ; ip < VecReduce.size() ; ip++ )
    if ( ppt == VecReduce[ip] )
        return ;

VecReduce.push_back( ppt ) ;
}

/*float CVecZoneReduce::GetBravaisPearson( const std::vector<long> &VecIPts )
{
std::vector<const st_coord_poly*> VecpStruct ;
for ( long ip = 0 ; ip < (long)VecIPts.size() ; ip++ )
    VecpStruct.push_back( (*m_pVecOrigine)[VecIPts[ip]] ) ;

double MoyenneLat = 0 ;
double MoyenneLon = 0 ;
const long size = VecpStruct.size() ;

if ( size <= 2 )
    return 1. ;

for ( long i = 0 ; i  < size ; i++ )
    {
    const st_coord_poly* pStruct = VecpStruct[i] ;
    MoyenneLat += pStruct->m_Lat ;
    MoyenneLon += pStruct->m_Lon ;
    }
MoyenneLat /= size ;
MoyenneLon /= size ;

double Num = 0 ;
double SumDenLat = 0 ;
double SumDenLon = 0 ;
for ( long i = 0 ; i  < size ; i++ )
    {
    const st_coord_poly* pStruct = VecpStruct[i] ;
    Num += (pStruct->m_Lat-MoyenneLat)*(pStruct->m_Lon-MoyenneLon) ;
    SumDenLat += pow(pStruct->m_Lat-MoyenneLat,2) ;
    SumDenLon += pow(pStruct->m_Lon-MoyenneLon,2) ;
    }

return Num / sqrt(SumDenLat*SumDenLon) ;
} */

