// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Standard_Stream.hxx>

#include <GEOMImpl_CylinderDriver.hxx>
#include <GEOMImpl_ICylinder.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRep_Tool.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>

#include <Standard_TypeMismatch.hxx>
#include <Standard_NullObject.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt.hxx>
#include <gp.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_CylinderDriver::GetID()
{
  static Standard_GUID aCylinderDriver("FF1BBB14-5D14-4df2-980B-3A668264EA16");
  return aCylinderDriver;
}


//=======================================================================
//function : GEOMImpl_CylinderDriver
//purpose  :
//=======================================================================
GEOMImpl_CylinderDriver::GEOMImpl_CylinderDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_CylinderDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_ICylinder aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  gp_Pnt aP;
  gp_Vec aV;

  if (aType == CYLINDER_R_H) {
    aP = gp::Origin();
    aV = gp::DZ();
  }
  else if (aType == CYLINDER_PNT_VEC_R_H) {
    Handle(GEOM_Function) aRefPoint  = aCI.GetPoint();
    Handle(GEOM_Function) aRefVector = aCI.GetVector();
    TopoDS_Shape aShapePnt = aRefPoint->GetValue();
    TopoDS_Shape aShapeVec = aRefVector->GetValue();
    if (aShapePnt.IsNull() || aShapeVec.IsNull()) {
      Standard_NullObject::Raise("Cylinder creation aborted: point or vector is not defined");
    }
    if (aShapePnt.ShapeType() != TopAbs_VERTEX ||
        aShapeVec.ShapeType() != TopAbs_EDGE) {
      Standard_TypeMismatch::Raise("Cylinder creation aborted: point or vector shapes has wrong type");
    }

    aP = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt));

    TopoDS_Edge anE = TopoDS::Edge(aShapeVec);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anE, V1, V2, Standard_True);
    if (V1.IsNull() || V2.IsNull()) {
      Standard_NullObject::Raise("Cylinder creation aborted: vector is not defined");
    }
    aV = gp_Vec(BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
  }
  else {
    return 0;
  }

  if (aCI.GetH() < 0.0) aV.Reverse();
  gp_Ax2 anAxes (aP, aV);

  double theAngle = aCI.GetAngle();
  if (theAngle == 0.)
	theAngle = M_PI*2.;

  BRepPrimAPI_MakeCylinder MC (anAxes, aCI.GetR(), Abs(aCI.GetH()), theAngle);
  MC.Build();
  if (!MC.IsDone()) {
    StdFail_NotDone::Raise("Cylinder can't be computed from the given parameters");
  }

  TopoDS_Shape aShape = MC.Shape();
  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_CylinderDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_ICylinder aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "CYLINDER";

  switch ( aType ) {
  case CYLINDER_R_H:
    AddParam( theParams, "Radius", aCI.GetR() );
    AddParam( theParams, "Height", aCI.GetH() );
    break;
  case CONE_PNT_VEC_R1_R2_H:
    AddParam( theParams, "Base Point", aCI.GetPoint() );
    AddParam( theParams, "Vector", aCI.GetVector() );
    AddParam( theParams, "Radius", aCI.GetR() );
    AddParam( theParams, "Height", aCI.GetH() );
    break;
  default:
    return false;
}

  return true;
  }

IMPLEMENT_STANDARD_HANDLE (GEOMImpl_CylinderDriver,GEOM_BaseDriver);

IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_CylinderDriver,GEOM_BaseDriver);
