// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Standard_Stream.hxx>

#include <GEOMImpl_TranslateDriver.hxx>
#include <GEOMImpl_ITranslate.hxx>
#include <GEOMImpl_ITransformOperations.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>
#include <GEOMUtils.hxx>

#include <ShapeFix_Shape.hxx>
#include <ShapeFix_ShapeTolerance.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>

#include <gp_Trsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_TranslateDriver::GetID()
{
  static Standard_GUID aTranslateDriver("FF1BBB03-5D14-4df2-980B-3A668264EA16");
  return aTranslateDriver;
}


//=======================================================================
//function : GEOMImpl_TranslateDriver
//purpose  :
//=======================================================================

GEOMImpl_TranslateDriver::GEOMImpl_TranslateDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_TranslateDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  if (aFunction.IsNull()) return 0;

  GEOMImpl_ITranslate TI(aFunction);
  gp_Trsf aTrsf;
  gp_Pnt aP1, aP2;
  Standard_Integer aType = aFunction->GetType();

  Handle(GEOM_Function) anOriginalFunction = TI.GetOriginal();
  if (anOriginalFunction.IsNull()) return 0;
  TopoDS_Shape aShape, anOriginal = anOriginalFunction->GetValue();
  if (anOriginal.IsNull()) return 0;

  if (aType == TRANSLATE_TWO_POINTS || aType == TRANSLATE_TWO_POINTS_COPY) {
    Handle(GEOM_Function) aPoint1 = TI.GetPoint1();
    Handle(GEOM_Function) aPoint2 = TI.GetPoint2();
    if(aPoint1.IsNull() || aPoint2.IsNull()) return 0;
    TopoDS_Shape aV1 = aPoint1->GetValue();
    TopoDS_Shape aV2 = aPoint2->GetValue();
    if(aV1.IsNull() || aV1.ShapeType() != TopAbs_VERTEX) return 0;
    if(aV2.IsNull() || aV2.ShapeType() != TopAbs_VERTEX) return 0;

    aP1 = BRep_Tool::Pnt(TopoDS::Vertex(aV1));
    aP2 = BRep_Tool::Pnt(TopoDS::Vertex(aV2));

    aTrsf.SetTranslation(aP1, aP2);
    //NPAL18620: performance problem: multiple locations are accumulated
    //           in shape and need a great time to process
    //BRepBuilderAPI_Transform aTransformation(anOriginal, aTrsf, Standard_False);
    //aShape = aTransformation.Shape();
    TopLoc_Location aLocOrig = anOriginal.Location();
    gp_Trsf aTrsfOrig = aLocOrig.Transformation();
    TopLoc_Location aLocRes (aTrsf * aTrsfOrig);
    aShape = anOriginal.Located(aLocRes);
  }
  else if (aType == TRANSLATE_VECTOR || aType == TRANSLATE_VECTOR_COPY) {
    Handle(GEOM_Function) aVector = TI.GetVector();
    if(aVector.IsNull()) return 0;
    TopoDS_Shape aV = aVector->GetValue();
    if(aV.IsNull() || aV.ShapeType() != TopAbs_EDGE) return 0;
    TopoDS_Edge anEdge = TopoDS::Edge(aV);

    aP1 = BRep_Tool::Pnt(TopExp::FirstVertex(anEdge));
    aP2 = BRep_Tool::Pnt(TopExp::LastVertex(anEdge));

    aTrsf.SetTranslation(aP1, aP2);
    //NPAL18620: performance problem: multiple locations are accumulated
    //           in shape and need a great time to process
    //BRepBuilderAPI_Transform aTransformation(anOriginal, aTrsf, Standard_False);
    //aShape = aTransformation.Shape();
    TopLoc_Location aLocOrig = anOriginal.Location();
    gp_Trsf aTrsfOrig = aLocOrig.Transformation();
    TopLoc_Location aLocRes (aTrsf * aTrsfOrig);
    aShape = anOriginal.Located(aLocRes);
  }
  else if (aType == TRANSLATE_VECTOR_DISTANCE) {
    Handle(GEOM_Function) aVector = TI.GetVector();
    double aDistance = TI.GetDistance();
    if(aVector.IsNull()) return 0;
    TopoDS_Shape aV = aVector->GetValue();
    if(aV.IsNull() || aV.ShapeType() != TopAbs_EDGE) return 0;
    TopoDS_Edge anEdge = TopoDS::Edge(aV);

    aP1 = BRep_Tool::Pnt(TopExp::FirstVertex(anEdge));
    aP2 = BRep_Tool::Pnt(TopExp::LastVertex(anEdge));

    gp_Vec aVec (aP1, aP2);
    aVec.Normalize();
    aTrsf.SetTranslation(aVec * aDistance);

    TopLoc_Location aLocOrig = anOriginal.Location();
    gp_Trsf aTrsfOrig = aLocOrig.Transformation();
    TopLoc_Location aLocRes (aTrsf * aTrsfOrig);
    aShape = anOriginal.Located(aLocRes);
  }
  else if (aType == TRANSLATE_XYZ || aType == TRANSLATE_XYZ_COPY) {
    gp_Vec aVec (TI.GetDX(), TI.GetDY(), TI.GetDZ());
    aTrsf.SetTranslation(aVec);
    //NPAL18620: performance problem: multiple locations are accumulated
    //           in shape and need a great time to process
    //BRepBuilderAPI_Transform aTransformation (anOriginal, aTrsf, Standard_False);
    //aShape = aTransformation.Shape();
    TopLoc_Location aLocOrig = anOriginal.Location();
    gp_Trsf aTrsfOrig = aLocOrig.Transformation();
    TopLoc_Location aLocRes (aTrsf * aTrsfOrig);
    aShape = anOriginal.Located(aLocRes);
  }
  else if (aType == TRANSLATE_1D) {
    Standard_Real DX, DY, DZ, step = TI.GetStep1();
    Standard_Integer nbtimes = TI.GetNbIter1();
    gp_Vec aVec;
    TopoDS_Compound aCompound;
    BRep_Builder B;
    B.MakeCompound( aCompound );

    Handle(GEOM_Function) aVector = TI.GetVector();
    gp_Vec Vec = gp::DX();
    if (!aVector.IsNull()) {
      Vec = GEOMUtils::GetVector( aVector->GetValue(), Standard_False );
    Vec.Normalize();
    }

    TopLoc_Location aLocOrig = anOriginal.Location();
    gp_Trsf aTrsfOrig = aLocOrig.Transformation();

    for (int i = 0; i < nbtimes; i++) {
      DX = i * step * Vec.X();
      DY = i * step * Vec.Y();
      DZ = i * step * Vec.Z();
      aVec.SetCoord( DX, DY, DZ );
      aTrsf.SetTranslation(aVec);
      //NPAL18620: performance problem: multiple locations are accumulated
      //           in shape and need a great time to process
      //BRepBuilderAPI_Transform aTransformation(anOriginal, aTrsf, Standard_False);
      //B.Add(aCompound, aTransformation.Shape());
      TopLoc_Location aLocRes (aTrsf * aTrsfOrig);
      B.Add(aCompound, anOriginal.Located(aLocRes));
    }
    aShape = aCompound;
    //aShape = GEOMImpl_ITransformOperations::TranslateShape1D(anOriginal, &TI);
  }
  else if (aType == TRANSLATE_2D) {
    Standard_Integer nbtimes1 = TI.GetNbIter1(), nbtimes2 = TI.GetNbIter2();
    Standard_Real DX, DY, DZ,  step1 = TI.GetStep1(),  step2 = TI.GetStep2();
    Handle(GEOM_Function) aVector = TI.GetVector();
    Handle(GEOM_Function) aVector2 = TI.GetVector2();

    gp_Vec Vec1 = gp::DX();
    gp_Vec Vec2 = gp::DY();

    if (!aVector.IsNull()) {
      Vec1 = GEOMUtils::GetVector( aVector->GetValue(), Standard_False );
      Vec1.Normalize();
    }

    if (!aVector2.IsNull()) {
      Vec2 = GEOMUtils::GetVector( aVector2->GetValue(), Standard_False );
    Vec2.Normalize();
    }

    TopoDS_Compound aCompound;
    BRep_Builder B;
    B.MakeCompound( aCompound );

    TopLoc_Location aLocOrig = anOriginal.Location();
    gp_Trsf aTrsfOrig = aLocOrig.Transformation();
    gp_Vec aVec;

    for (int i = 0; i < nbtimes1; i++) {
      for (int j = 0; j < nbtimes2; j++) {
        DX = i * step1 * Vec1.X() + j * step2 * Vec2.X();
        DY = i * step1 * Vec1.Y() + j * step2 * Vec2.Y();
        DZ = i * step1 * Vec1.Z() + j * step2 * Vec2.Z();
        aVec.SetCoord( DX, DY, DZ );
        aTrsf.SetTranslation(aVec);
        //NPAL18620: performance problem: multiple locations are accumulated
        //           in shape and need a great time to process
        //BRepBuilderAPI_Transform aBRepTransformation(anOriginal, aTrsf, Standard_False);
        //B.Add(aCompound, aBRepTransformation.Shape());
        TopLoc_Location aLocRes (aTrsf * aTrsfOrig);
        B.Add(aCompound, anOriginal.Located(aLocRes));
      }
    }
   aShape = aCompound;
   //aShape = GEOMImpl_ITransformOperations::TranslateShape2D(anOriginal, &TI);
  }
  else if (aType == TRANSLATE_ALONG_RAIL) {
    TopoDS_Wire aWireRail;

    Handle(GEOM_Function) aRailFunction = TI.GetRailShape();
    if (aRailFunction.IsNull()) return 0;

    TopoDS_Shape aShapeRail = aRailFunction->GetValue();

    if (aShapeRail.IsNull()) return 0;

    if( aShapeRail.ShapeType() == TopAbs_EDGE) {
      TopoDS_Edge anEdge = TopoDS::Edge(aShapeRail);
      aWireRail = BRepBuilderAPI_MakeWire(anEdge);
    }
    else if( aShapeRail.ShapeType() == TopAbs_WIRE) {
      aWireRail = TopoDS::Wire(aShapeRail);
    }
    else {
      Standard_ConstructionError::Raise("Rail is not of the requested type.");
    }

    //get the compcurve adaptor
    BRepAdaptor_CompCurve aCompoundCurve(aWireRail, Standard_True);
    Standard_Real theTotalLength = std::fabs(aCompoundCurve.LastParameter() - aCompoundCurve.FirstParameter());

    //get the initial position
    gp_Trsf aTrsfOrig = anOriginal.Location().Transformation();
    gp_Ax3 aStartAx3 = GEOMImpl_IMeasureOperations::GetPosition(anOriginal);

    //calculate step and count based on type
    double step = TI.GetStep1();
    int myCount = TI.GetNbIter1();
    if (step != 0. && myCount == 0)
      myCount = std::floor(theTotalLength / step);
    else if (myCount != 0 && step == 0.) {
      myCount--;
      if (myCount < 1)
        Standard_ConstructionError::Raise("Count of items cannot be 1 or less.Use TRANSLATE routine instead.");
      step = theTotalLength / myCount;
    }
    else if (myCount == 0 && step == 0.)
      Standard_ConstructionError::Raise("Both count and step cannot be 0.");
    else
      myCount--;

    Standard_Integer myRotationType = TI.GetNbIter2();  //0 = No rotation
                                                        //1 = Plane based rotation
                                                        //2 = Freeform rotation
    //do the transformation-s
    gp_Pnt aPnt;
    gp_Vec aVec;
    TopoDS_Compound aCompound;
    BRep_Builder B;
    B.MakeCompound( aCompound );
    for (int cn = 0; cn <= myCount;cn++) {
      if (myRotationType == 0) {
        aCompoundCurve.D0( (cn * step), aPnt );
        aTrsf.SetTranslation(aStartAx3.Location(), aPnt);
      }
      else if (myRotationType == 1) {
        Standard_ConstructionError::Raise("Not yet implemented.");
      }
      else if (myRotationType == 2) {
        aCompoundCurve.D1( (cn * step), aPnt, aVec );
        aTrsf.SetDisplacement(aStartAx3, gp_Ax3(aPnt, aVec));
      }
      else {
        Standard_ConstructionError::Raise("Not yet implemented.");
      }
      TopLoc_Location aLocRes (aTrsf);
      B.Add(aCompound, anOriginal.Located(aLocRes * aTrsfOrig));
    }

    //assign the result
    aShape = aCompound;
  /*
    //TAKEN FROM 5.1.1 POSTIRON EXAMPLE FROM UPSTREAM
    
    Handle(GeomFill_TrihedronLaw) TLaw = new GeomFill_CorrectedFrenet();
    Handle(GeomFill_CurveAndTrihedron) aLocationLaw = new GeomFill_CurveAndTrihedron( TLaw );
    Handle(BRepFill_LocationLaw) aLocation = new BRepFill_Edge3DLaw(aWire, aLocationLaw);

    aLocation->TransformInCompatibleLaw( 0.01 );

    //Calculate a Parameter
    Standard_Real aFirstParam1 = 0, aLastParam1 = 0; // Parameters of the First edge
    Standard_Real aFirstParam2 = 0, aLastParam2 = 0; // Parameters of the Last edge
    aLocation->CurvilinearBounds(aLocation->NbLaw(), aFirstParam2, aLastParam2);

    if ( aLocation->NbLaw() > 1)
      aLocation->CurvilinearBounds(1, aFirstParam1, aLastParam1);
    else if ( aLocation->NbLaw() == 1 )
      aFirstParam1 = aFirstParam2;
    else
      return 0;

    Standard_Real aParam = (aFirstParam1 + (aLastParam2 - aFirstParam1)*aParameter );

    TopoDS_Shape CopyShape = aShapeBase;
    BRepFill_SectionPlacement Place( aLocation, aShapeBase );
    TopLoc_Location Loc2(Place.Transformation()), Loc1;
    Loc1 = CopyShape.Location();
    CopyShape.Location(Loc2.Multiplied(Loc1));

    aLocation->D0( aParam, CopyShape );
    aShape = CopyShape;
  */
  }
  else return 0;

  if (aShape.IsNull()) return 0;

  BRepCheck_Analyzer ana (aShape, Standard_True);
  if (!ana.IsValid()) {
    ShapeFix_ShapeTolerance aSFT;
    aSFT.LimitTolerance(aShape,Precision::Confusion(),Precision::Confusion());
    Handle(ShapeFix_Shape) aSfs = new ShapeFix_Shape(aShape);
    aSfs->SetPrecision(Precision::Confusion());
    aSfs->Perform();
    aShape = aSfs->Shape();

    ana.Init(aShape, Standard_False);
	if (!ana.IsValid()) {
	  Standard_CString anErrStr("Translation aborted : algorithm has produced an invalid shape result");
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

bool GEOMImpl_TranslateDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_ITranslate aCI( function );
  Standard_Integer aType = function->GetType();

  switch ( aType ) {
  case TRANSLATE_TWO_POINTS:
  case TRANSLATE_TWO_POINTS_COPY:
    theOperationName = "TRANSLATION";
    AddParam( theParams, "Object", aCI.GetOriginal() );
    AddParam( theParams, "Point 1", aCI.GetPoint1() );
    AddParam( theParams, "Point 2", aCI.GetPoint2() );
    break;
  case TRANSLATE_VECTOR:
  case TRANSLATE_VECTOR_COPY:
    theOperationName = "TRANSLATION";
    AddParam( theParams, "Object", aCI.GetOriginal() );
    AddParam( theParams, "Vector", aCI.GetVector() );
    break;
  case TRANSLATE_VECTOR_DISTANCE:
    theOperationName = "TRANSLATION";
    AddParam( theParams, "Object", aCI.GetOriginal() );
    AddParam( theParams, "Vector", aCI.GetVector() );
    AddParam( theParams, "Distance", aCI.GetDistance() );
    break;
  case TRANSLATE_XYZ:
  case TRANSLATE_XYZ_COPY:
    theOperationName = "TRANSLATION";
    AddParam( theParams, "Object", aCI.GetOriginal() );
    AddParam( theParams, "Dx", aCI.GetDX() );
    AddParam( theParams, "Dy", aCI.GetDY() );
    AddParam( theParams, "Dz", aCI.GetDZ() );
    break;
  case TRANSLATE_1D:
    theOperationName = "MUL_TRANSLATION";
    AddParam( theParams, "Main Object", aCI.GetOriginal() );
    AddParam( theParams, "Vector", aCI.GetVector(), "DX" );
    AddParam( theParams, "Step", aCI.GetStep1() );
    AddParam( theParams, "Nb. Times", aCI.GetNbIter1() );
    break;
  case TRANSLATE_2D:
    theOperationName = "MUL_TRANSLATION";
    AddParam( theParams, "Main Object", aCI.GetOriginal() );
    AddParam( theParams, "Vector U", aCI.GetVector(), "DX" );
    AddParam( theParams, "Vector V", aCI.GetVector2(), "DY" );
    AddParam( theParams, "Step U", aCI.GetStep1() );
    AddParam( theParams, "Nb. Times U", aCI.GetNbIter1() );
    AddParam( theParams, "Step V", aCI.GetStep2() );
    AddParam( theParams, "Nb. Times V", aCI.GetNbIter2() );
    break;
  default:
    return false;
  }

  return true;
}

IMPLEMENT_STANDARD_HANDLE (GEOMImpl_TranslateDriver,GEOM_BaseDriver);
IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_TranslateDriver,GEOM_BaseDriver);
