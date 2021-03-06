// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// Original work from OpenCascade Sketcher 
// (http://sourceforge.net/projects/occsketcher/)
//
// Modifications for GEOM and OCAF
// Authored by : Sioutis Fotios (sfotis@gmail.com)

//------------------------------------------------------------------------------
#include "Sketcher_SnapCenter.hxx"
//------------------------------------------------------------------------------
#include <TColStd_HSequenceOfTransient.hxx>
#include <Geom2d_Circle.hxx>
//------------------------------------------------------------------------------
Sketcher_SnapCenter::Sketcher_SnapCenter()
{
}
//------------------------------------------------------------------------------
Sketcher_SnapCenter::~Sketcher_SnapCenter()
{
}
//------------------------------------------------------------------------------
void Sketcher_SnapCenter::SelectEvent()
{
  findbestPnt2d = Standard_False;
  minDistance = minimumSnapDistance;

  for (Standard_Integer i=1; i<=data->Length(); i++) {
    mySObject = Handle(Sketcher_Object)::DownCast(data->Value(i));
    myGeometryType = mySObject->GetGeometryType();

    switch (myGeometryType) {
      case PointSketcherObject:
        break;
      case LineSketcherObject:
        break;
      case CircleSketcherObject:
      case ArcSketcherObject:
        curGeom2d_Circle = Handle(Geom2d_Circle)::DownCast(mySObject->GetGeometry());
        ProjectOnCurve.Init(curPnt2d,curGeom2d_Circle);

        if (countProject()) {
          bestPnt2d = curGeom2d_Circle->Location();
          curHilightedObj = mySObject->GetAIS_Object();
        }

        objectPnt2d = curGeom2d_Circle->Location();

        if (count()) {
          bestPnt2d = objectPnt2d;
          curHilightedObj = mySObject->GetAIS_Object();
        }

        break;
      default:
        break;
    }
  }

  if (minDistance == minimumSnapDistance)
    bestPnt2d = curPnt2d;
  else   findbestPnt2d = Standard_True;
}
//------------------------------------------------------------------------------
Sketcher_SnapType Sketcher_SnapCenter::GetSnapType()
{
  return SnapCenter;
}
//------------------------------------------------------------------------------
IMPLEMENT_STANDARD_HANDLE(Sketcher_SnapCenter,Sketcher_Snap)
IMPLEMENT_STANDARD_RTTI(Sketcher_SnapCenter)
IMPLEMENT_STANDARD_TYPE(Sketcher_SnapCenter)
IMPLEMENT_STANDARD_SUPERTYPE(Sketcher_Snap)
IMPLEMENT_STANDARD_SUPERTYPE(MMgt_TShared)
IMPLEMENT_STANDARD_SUPERTYPE(Standard_Transient)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(Sketcher_Snap)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(MMgt_TShared)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(Standard_Transient)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(Sketcher_SnapCenter)
//------------------------------------------------------------------------------
