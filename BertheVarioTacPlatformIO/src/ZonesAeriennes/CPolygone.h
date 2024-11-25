////////////////////////////////////////////////////////////////////////////////
/// \file CPolygone.h
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 25/11/2024
///

#ifndef _POLYGONE_
#define _POLYGONE_

////////////////////////////////////////////////////////////////////////////////
/// \brief Definition d'un polygone par points et test si on est a l'interieur.
/// Donne aussi la surface pour les zones imbriquées.
class CPolygone
{
public :
    static void CalcBarycentre( CVecZoneReduce::st_coord_poly ** PtsArr , int NbPts , CVecZoneReduce::st_coord_poly & PtsBarycentre ) ;
    static float GetAreaSize( CVecZoneReduce::st_coord_poly ** PtsArr , int NbPts , CVecZoneReduce::st_coord_poly & PtsBarycentre ) ;
    static bool IsIn( CVecZoneReduce::st_coord_poly ** PtsArr , int NbPts , const CVecZoneReduce::st_coord_poly & PtsToTest ) ;
    static bool IsInOld( CVecZoneReduce::st_coord_poly ** PtsArr , int NbPts , const CVecZoneReduce::st_coord_poly & PtsToTest ) ;
} ;

#endif
