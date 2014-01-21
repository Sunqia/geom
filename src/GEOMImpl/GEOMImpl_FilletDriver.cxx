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

#include <GEOMImpl_FilletDriver.hxx>
#include <GEOMImpl_IFillet.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_ILocalOperations.hxx>
#include <GEOM_Function.hxx>

#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRep_Tool.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>

#include <ShapeFix_ShapeTolerance.hxx>
#include <ShapeFix_Shape.hxx>

#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_FilletDriver::GetID()
{
  static Standard_GUID aFilletDriver("FF1BBB41-5D14-4df2-980B-3A668264EA16");
  return aFilletDriver;
}


//=======================================================================
//function : GEOMImpl_FilletDriver
//purpose  :
//=======================================================================
GEOMImpl_FilletDriver::GEOMImpl_FilletDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_FilletDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IFillet aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  Handle(GEOM_Function) aRefShape = aCI.GetShape();
  TopoDS_Shape aShapeBase = aRefShape->GetValue();

  if (aType == FILLET_SHAPE_VERTEX_2D)
  {
	BRepFilletAPI_MakeFillet2d fill;
	TopoDS_Face aFace;

	Standard_Boolean aWireFlag = Standard_False;
	
	if (aShapeBase.ShapeType() == TopAbs_FACE)
		aFace = TopoDS::Face(aShapeBase);
	else if (aShapeBase.ShapeType() == TopAbs_WIRE)
	{
		TopoDS_Wire aWire = TopoDS::Wire(aShapeBase);
		BRepBuilderAPI_MakeFace aMF(aWire);
		aMF.Build();
		if (!aMF.IsDone()) {
			StdFail_NotDone::Raise("Cannot build initial face from given wire");
		}
		aFace = aMF.Face();
		aWireFlag = Standard_True;
	}
	else
		StdFail_NotDone::Raise("Base shape is neither a face or a wire !");

	fill.Init(aFace);

	double theRadius2d = aCI.GetR();
	int aLen = aCI.GetLength();
	int ind = 1;

	for (; ind <= aLen; ind++)
	{
		TopoDS_Shape aShapeVertex;
		if (GEOMImpl_ILocalOperations::GetSubShape(aShapeBase, aCI.GetVertex(ind), aShapeVertex))
			fill.AddFillet(TopoDS::Vertex(aShapeVertex),theRadius2d);
	}
	fill.Build();
	if (!fill.IsDone()) {
		StdFail_NotDone::Raise("Fillet can't be computed on the given shape with the given radius");
	}
	if (aWireFlag)
	{
		BRepBuilderAPI_MakeWire MW;
		TopExp_Explorer exp (fill.Shape(), TopAbs_EDGE);
		for (; exp.More(); exp.Next())
			MW.Add(TopoDS::Edge(exp.Current()));
		MW.Build();
		if (!MW.IsDone())
			StdFail_NotDone::Raise("Resulting wire cannot be built");
			
		aShape = MW.Shape();
	}
	else
		aShape = fill.Shape();
  }
  else
  {
	  BRepFilletAPI_MakeFillet fill (aShapeBase);

	  if (aType == FILLET_SHAPE_ALL) {
		TopExp_Explorer Exp (aShapeBase, TopAbs_EDGE);
		for (; Exp.More(); Exp.Next()) {
		  TopoDS_Edge E = TopoDS::Edge(Exp.Current());
		  fill.Add(E);
		}
	  } 
      else if (aType == FILLET_SHAPE_EDGES || aType == FILLET_SHAPE_EDGES_2R) {
		int aLen = aCI.GetLength();
		int ind = 1;
		for (; ind <= aLen; ind++) {
		  TopoDS_Shape aShapeEdge;
		  if (GEOMImpl_ILocalOperations::GetSubShape
			  (aShapeBase, aCI.GetEdge(ind), aShapeEdge)) {
			fill.Add(TopoDS::Edge(aShapeEdge));
		  }
		}
	  } 
      else if (aType == FILLET_SHAPE_FACES || aType == FILLET_SHAPE_FACES_2R) {
		int aLen = aCI.GetLength();
		int ind = 1;
		for (; ind <= aLen; ind++) {
		  TopoDS_Shape aShapeFace;
		  if (GEOMImpl_ILocalOperations::GetSubShape
			  (aShapeBase, aCI.GetFace(ind), aShapeFace)) {
			TopExp_Explorer Exp (aShapeFace, TopAbs_EDGE);
			for (; Exp.More(); Exp.Next()) {
			  fill.Add(TopoDS::Edge(Exp.Current()));
			}
		  }
		}
	  } 
      else {
	  }

  if (aType == FILLET_SHAPE_FACES || aType == FILLET_SHAPE_EDGES || aType == FILLET_SHAPE_ALL) {
		for (int i = 1; i <= fill.NbContours(); i++)
		  fill.SetRadius(aCI.GetR(), i, 1);
  }
	  else if (aType == FILLET_SHAPE_FACES_2R || aType == FILLET_SHAPE_EDGES_2R) {
		for (int i = 1; i <= fill.NbContours(); i++) {
		  fill.SetRadius(aCI.GetR1(), aCI.GetR2(), i, 1);
    }
  }

	  fill.Build();
	  if (!fill.IsDone()) {
		StdFail_NotDone::Raise("Fillet can't be computed on the given shape with the given radius");
	  }
	  aShape = fill.Shape();
  }

  if (aShape.IsNull()) return 0;

  // Check shape validity
  BRepCheck_Analyzer ana (aShape, false);
  if (!ana.IsValid()) {
	// 08.07.2008 added by skl during fixing bug 19761 from Mantis
	ShapeFix_ShapeTolerance aSFT;
	aSFT.LimitTolerance(aShape, Precision::Confusion(),
						Precision::Confusion(), TopAbs_SHAPE);
	Handle(ShapeFix_Shape) aSfs = new ShapeFix_Shape(aShape);
	aSfs->Perform();
	aShape = aSfs->Shape();
	ana.Init(aShape);
	if (!ana.IsValid()) {
	  Standard_CString anErrStr("Fillet algorithm has produced an invalid shape result");
	  #ifdef THROW_ON_INVALID_SH
		Standard_ConstructionError::Raise(anErrStr);
	  #else
		MESSAGE(anErrStr);
		//further processing can be performed here
		//...
		//in case of failure of automatic treatment
		//mark the corresponding GEOM_Object as problematic
		TDF_Label aLabel = aFunction->GetOwnerEntry();
		if (!aLabel.IsRoot()) {
		  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel);
		  if (!aMainObj.IsNull())
			aMainObj->SetDirty(Standard_True);
		}
	  #endif
	}
  }

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_FilletDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IFillet aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "FILLET";

  switch ( aType ) {
  case FILLET_SHAPE_ALL:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Selected edges", "all" );
    break;
  case FILLET_SHAPE_EDGES:
  case FILLET_SHAPE_EDGES_2R:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Selected edges" );
    if ( aCI.GetLength() > 1 )
      theParams[1] << aCI.GetLength() << " edges: ";
    for (int i = 1; i <= aCI.GetLength(); ++i )
      theParams[1] << aCI.GetEdge( i ) << " ";
    if ( aType == FILLET_SHAPE_EDGES ) {
      AddParam( theParams, "Radius", aCI.GetR() );
    }
    else {
      AddParam( theParams, "R1", aCI.GetR1() );
      AddParam( theParams, "R2", aCI.GetR2() );
    }
    break;
  case FILLET_SHAPE_FACES:
  case FILLET_SHAPE_FACES_2R:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Selected faces" );
    if ( aCI.GetLength() > 1 )
      theParams[1] << aCI.GetLength() << " faces: ";
    for (int i = 1; i <= aCI.GetLength(); ++i )
      theParams[1] << aCI.GetFace( i ) << " ";
    if ( aType == FILLET_SHAPE_FACES ) {
      AddParam( theParams, "Radius", aCI.GetR() );
    }
    else {
      AddParam( theParams, "R1", aCI.GetR1() );
      AddParam( theParams, "R2", aCI.GetR2() );
    }
    break;
  default:
    return false;
  }
  
  return true;
}

IMPLEMENT_STANDARD_HANDLE (GEOMImpl_FilletDriver,GEOM_BaseDriver);
IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_FilletDriver,GEOM_BaseDriver);
