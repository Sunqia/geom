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

#include <GEOMImpl_BooleanDriver.hxx>
#include <GEOMImpl_IBoolean.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_GlueDriver.hxx>
#include <GEOM_Function.hxx>
#include <GEOMUtils.hxx>

#include <TNaming_CopyShape.hxx>

#include <ShapeFix_ShapeTolerance.hxx>
#include <ShapeFix_Shape.hxx>

#include <BRep_Builder.hxx>
#include <BRepAlgo.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BOPAlgo_CheckerSI.hxx>
#include <BOPDS_DS.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TColStd_IndexedDataMapOfTransientTransient.hxx>

#include <Precision.hxx>

#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>

// Depth of self-intersection check (see BOPAlgo_CheckerSI::SetLevelOfCheck() for more details)
// Default value for BOPAlgo_CheckerSI gives very long computation when checking face-to-face intersections;
// here check level is decreased to more appropriate value to avoid problems with performance).
#define BOP_SELF_INTERSECTIONS_LEVEL 4

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_BooleanDriver::GetID()
{
  static Standard_GUID aBooleanDriver("FF1BBB21-5D14-4df2-980B-3A668264EA16");
  return aBooleanDriver;
}

//=======================================================================
//function : GEOMImpl_BooleanDriver
//purpose  :
//=======================================================================
GEOMImpl_BooleanDriver::GEOMImpl_BooleanDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_BooleanDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IBoolean aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();
  const Standard_Boolean isCheckSelfInte = aCI.GetCheckSelfIntersection();

  TopoDS_Shape aShape;

  switch (aType) {
  case BOOLEAN_COMMON:
  case BOOLEAN_CUT:
  case BOOLEAN_FUSE:
  case BOOLEAN_SECTION:
    {
  Handle(GEOM_Function) aRefShape1 = aCI.GetShape1();
  Handle(GEOM_Function) aRefShape2 = aCI.GetShape2();
  TopoDS_Shape aShape1 = aRefShape1->GetValue();
  TopoDS_Shape aShape2 = aRefShape2->GetValue();

      if (!aShape1.IsNull() && !aShape2.IsNull()) {
    // check arguments for Mantis issue 0021019
    BRepCheck_Analyzer ana (aShape1, Standard_True);
    if (!ana.IsValid())
          StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is not valid");
    ana.Init(aShape2);
    if (!ana.IsValid())
      StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is not valid");

        if (isCheckSelfInte) {
          BOPAlgo_CheckerSI aCSI;  // checker of self-interferences
          aCSI.SetLevelOfCheck(BOP_SELF_INTERSECTIONS_LEVEL);
          BOPCol_ListOfShape aList1, aList2;
          aList1.Append(aShape1);
          aList2.Append(aShape2);
          aCSI.SetArguments(aList1);
          aCSI.Perform();
          if (aCSI.ErrorStatus() || aCSI.DS().Interferences().Extent() > 0)
            StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is self-intersected");
          aCSI.SetArguments(aList2);
          aCSI.Perform();
          if (aCSI.ErrorStatus() || aCSI.DS().Interferences().Extent() > 0)
            StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is self-intersected");
        }

        // Make a copy to prevent the original shape changes.
        TopoDS_Shape aShapeCopy1;
        TopoDS_Shape aShapeCopy2;
        TColStd_IndexedDataMapOfTransientTransient aMapTShapes;
        TNaming_CopyShape::CopyTool(aShape1, aMapTShapes, aShapeCopy1);
        TNaming_CopyShape::CopyTool(aShape2, aMapTShapes, aShapeCopy2);

        aShape = performOperation (aShapeCopy1, aShapeCopy2, aType);

        if (aShape.IsNull())
          return 0;
      }
    }
    break;
  case BOOLEAN_COMMON_LIST:
  case BOOLEAN_FUSE_LIST:
    {
      Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
      const Standard_Integer nbShapes = aShapes->Length();
      Standard_Integer i;
      Handle(GEOM_Function) aRefShape;
      TopoDS_Shape aShape2;
      Standard_Integer aSimpleType =
        (aType == BOOLEAN_FUSE_LIST ? BOOLEAN_FUSE : BOOLEAN_COMMON);

      if (nbShapes > 0) {
        aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(1));
        aShape = aRefShape->GetValue();
	
        if (!aShape.IsNull()) {
          BRepCheck_Analyzer anAna (aShape, Standard_True);
          if (!anAna.IsValid()) {
            StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is not valid");
          }

          BOPAlgo_CheckerSI aCSI;  // checker of self-interferences

          if (isCheckSelfInte) {
            aCSI.SetLevelOfCheck(BOP_SELF_INTERSECTIONS_LEVEL);
            BOPCol_ListOfShape aList1;
            aList1.Append(aShape);
            aCSI.SetArguments(aList1);
            aCSI.Perform();
            if (aCSI.ErrorStatus() || aCSI.DS().Interferences().Extent() > 0) {
              StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is self-intersected");
            }
          }

          // Copy shape
          TopoDS_Shape aShapeCopy;
          TColStd_IndexedDataMapOfTransientTransient aMapTShapes;

          TNaming_CopyShape::CopyTool(aShape, aMapTShapes, aShapeCopy);
          aShape = aShapeCopy;

          for (i = 2; i <= nbShapes; i++) {
	    aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(i));
	    aShape2 = aRefShape->GetValue();
	    anAna.Init(aShape2);
	    
	    if (!anAna.IsValid()) {
	      StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is not valid");
	    }
	    
            if (isCheckSelfInte) {
              BOPCol_ListOfShape aList2;
              aList2.Append(aShape2);
              aCSI.SetArguments(aList2);
              aCSI.Perform();
              if (aCSI.ErrorStatus() || aCSI.DS().Interferences().Extent() > 0) {
                StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is self-intersected");
              }
            }

            // Copy shape
            aShapeCopy.Nullify();
            TNaming_CopyShape::CopyTool(aShape2, aMapTShapes, aShapeCopy);
	    aShape = performOperation (aShape, aShapeCopy, aSimpleType);
	    
	    if (aShape.IsNull()) {
	      return 0;
	    }
	  }
	}
      }
    }
    break;
  case BOOLEAN_CUT_LIST:
    {
      Handle(GEOM_Function) aRefObject = aCI.GetShape1();

      aShape = aRefObject->GetValue();

      if (!aShape.IsNull()) {
        // check arguments for Mantis issue 0021019
        BRepCheck_Analyzer anAna (aShape, Standard_True);

        if (!anAna.IsValid()) {
          StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is not valid");
        }

	BOPAlgo_CheckerSI aCSI;  // checker of self-interferences

        if (isCheckSelfInte) {
          aCSI.SetLevelOfCheck(BOP_SELF_INTERSECTIONS_LEVEL);
          BOPCol_ListOfShape aList1;
          aList1.Append(aShape);
          aCSI.SetArguments(aList1);
          aCSI.Perform();
          if (aCSI.ErrorStatus() || aCSI.DS().Interferences().Extent() > 0) {
            StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is self-intersected");
          }
        }

        // Copy shape
        TopoDS_Shape aShapeCopy;
        TColStd_IndexedDataMapOfTransientTransient aMapTShapes;

        TNaming_CopyShape::CopyTool(aShape, aMapTShapes, aShapeCopy);
        aShape = aShapeCopy;
	
        Handle(TColStd_HSequenceOfTransient) aTools = aCI.GetShapes();
        const Standard_Integer nbShapes = aTools->Length();
        Standard_Integer i;
        Handle(GEOM_Function) aRefTool;
        TopoDS_Shape aTool;

        for (i = 1; i <= nbShapes; i++) {
          aRefTool = Handle(GEOM_Function)::DownCast(aTools->Value(i));
          aTool = aRefTool->GetValue();
          anAna.Init(aTool);

          if (!anAna.IsValid()) {
            StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is not valid");
          }

          if (isCheckSelfInte) {
            BOPCol_ListOfShape aList2;
            aList2.Append(aTool);
            aCSI.SetArguments(aList2);
            aCSI.Perform();
            if (aCSI.ErrorStatus() || aCSI.DS().Interferences().Extent() > 0) {
              StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is self-intersected");
            }
          }

          // Copy shape
          aShapeCopy.Nullify();
          TNaming_CopyShape::CopyTool(aTool, aMapTShapes, aShapeCopy);
          aShape = performOperation (aShape, aShapeCopy, BOOLEAN_CUT);

          if (aShape.IsNull()) {
            return 0;
          }
        }
      }
    }
    break;
  default:
    break;
  }

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}

//=======================================================================
//function : performOperation
//purpose  :
//=======================================================================
TopoDS_Shape GEOMImpl_BooleanDriver::performOperation
                               (const TopoDS_Shape theShape1,
                                const TopoDS_Shape theShape2,
                                const Standard_Integer theType)const
{
  TopoDS_Shape aShape;

    // perform COMMON operation
  if (theType == BOOLEAN_COMMON) {
      BRep_Builder B;
      TopoDS_Compound C;
      B.MakeCompound(C);

      TopTools_ListOfShape listShape1, listShape2;
    GEOMUtils::AddSimpleShapes(theShape1, listShape1);
    GEOMUtils::AddSimpleShapes(theShape2, listShape2);

      Standard_Boolean isCompound =
        (listShape1.Extent() > 1 || listShape2.Extent() > 1);

      TopTools_ListIteratorOfListOfShape itSub1 (listShape1);
      for (; itSub1.More(); itSub1.Next()) {
        TopoDS_Shape aValue1 = itSub1.Value();
        TopTools_ListIteratorOfListOfShape itSub2 (listShape2);
        for (; itSub2.More(); itSub2.Next()) {
          TopoDS_Shape aValue2 = itSub2.Value();
          BRepAlgoAPI_Common BO (aValue1, aValue2);
          if (!BO.IsDone()) {
            StdFail_NotDone::Raise("Common operation can not be performed on the given shapes");
          }
          if (isCompound) {
            TopoDS_Shape aStepResult = BO.Shape();

            // check result of this step: if it is a compound (boolean operations
            // allways return a compound), we add all sub-shapes of it.
            // This allows to avoid adding empty compounds,
            // resulting from COMMON on two non-intersecting shapes.
            if (aStepResult.ShapeType() == TopAbs_COMPOUND) {
              TopoDS_Iterator aCompIter (aStepResult);
              for (; aCompIter.More(); aCompIter.Next()) {
                // add shape in a result
                B.Add(C, aCompIter.Value());
              }
            }
            else {
              // add shape in a result
              B.Add(C, aStepResult);
            }
          }
          else
            aShape = BO.Shape();
        }
      }

      if (isCompound) {
        // As GlueFaces has been improved to keep all kind of shapes
        TopExp_Explorer anExp (C, TopAbs_VERTEX);
        if (anExp.More())
          aShape = GEOMImpl_GlueDriver::GlueFaces(C, Precision::Confusion(), Standard_True);
        else
		  aShape = C;
      }
    }

    // perform CUT operation
  else if (theType == BOOLEAN_CUT) {
      BRep_Builder B;
      TopoDS_Compound C;
      B.MakeCompound(C);

      TopTools_ListOfShape listShapes, listTools;
    GEOMUtils::AddSimpleShapes(theShape1, listShapes);
    GEOMUtils::AddSimpleShapes(theShape2, listTools);

      Standard_Boolean isCompound = (listShapes.Extent() > 1);

      TopTools_ListIteratorOfListOfShape itSub1 (listShapes);
      for (; itSub1.More(); itSub1.Next()) {
        TopoDS_Shape aCut = itSub1.Value();
        // tools
        TopTools_ListIteratorOfListOfShape itSub2 (listTools);
        for (; itSub2.More(); itSub2.Next()) {
          TopoDS_Shape aTool = itSub2.Value();
          BRepAlgoAPI_Cut BO (aCut, aTool);
          if (!BO.IsDone()) {
            StdFail_NotDone::Raise("Cut operation can not be performed on the given shapes");
          }
          aCut = BO.Shape();
        }
        if (isCompound) {
		  // check result of this step: if it is a compound (boolean operations
          // allways return a compound), we add all sub-shapes of it.
          // This allows to avoid adding empty compounds,
          // resulting from CUT of parts
          if (aCut.ShapeType() == TopAbs_COMPOUND) {
            TopoDS_Iterator aCompIter (aCut);
            for (; aCompIter.More(); aCompIter.Next()) {
              // add shape in a result
              B.Add(C, aCompIter.Value());
            }
          }
          else {
            // add shape in a result
            B.Add(C, aCut);
          }
        }
        else
          aShape = aCut;
      }

      if (isCompound) {
        // As GlueFaces has been improved to keep all kind of shapes
        TopExp_Explorer anExp (C, TopAbs_VERTEX);
        if (anExp.More())
          aShape = GEOMImpl_GlueDriver::GlueFaces(C, Precision::Confusion(), Standard_True);
        else
          aShape = C;
      }
    }

    // perform FUSE operation
  else if (theType == BOOLEAN_FUSE) {
      // Perform
    BRepAlgoAPI_Fuse BO (theShape1, theShape2);
      if (!BO.IsDone()) {
        StdFail_NotDone::Raise("Fuse operation can not be performed on the given shapes");
      }
      aShape = BO.Shape();
    }

    // perform SECTION operation
  else if (theType == BOOLEAN_SECTION) {
      BRep_Builder B;
      TopoDS_Compound C;
      B.MakeCompound(C);

      TopTools_ListOfShape listShape1, listShape2;
    GEOMUtils::AddSimpleShapes(theShape1, listShape1);
    GEOMUtils::AddSimpleShapes(theShape2, listShape2);

      Standard_Boolean isCompound =
        (listShape1.Extent() > 1 || listShape2.Extent() > 1);

      TopTools_ListIteratorOfListOfShape itSub1 (listShape1);
      for (; itSub1.More(); itSub1.Next()) {
        TopoDS_Shape aValue1 = itSub1.Value();
        TopTools_ListIteratorOfListOfShape itSub2 (listShape2);
        for (; itSub2.More(); itSub2.Next()) {
          TopoDS_Shape aValue2 = itSub2.Value();
          BRepAlgoAPI_Section BO (aValue1, aValue2, Standard_False);
          // Set approximation to have an attached 3D BSpline geometry to each edge,
          // where analytic curve is not possible. Without this flag in some cases
          // we obtain BSpline curve of degree 1 (C0), which is slowly
          // processed by some algorithms (Partition for example).
          BO.Approximation(Standard_True);
        //modified by NIZNHY-PKV Tue Oct 18 14:34:16 2011f
        BO.ComputePCurveOn1(Standard_True);
        BO.ComputePCurveOn2(Standard_True);
        //modified by NIZNHY-PKV Tue Oct 18 14:34:18 2011t
  
          BO.Build();
          if (!BO.IsDone()) {
            StdFail_NotDone::Raise("Section operation can not be performed on the given shapes");
          }
          if (isCompound) {
            TopoDS_Shape aStepResult = BO.Shape();

            // check result of this step: if it is a compound (boolean operations
            // allways return a compound), we add all sub-shapes of it.
            // This allows to avoid adding empty compounds,
            // resulting from SECTION on two non-intersecting shapes.
            if (aStepResult.ShapeType() == TopAbs_COMPOUND) {
              TopoDS_Iterator aCompIter (aStepResult);
              for (; aCompIter.More(); aCompIter.Next()) {
                // add shape in a result
                B.Add(C, aCompIter.Value());
              }
            }
            else {
              // add shape in a result
              B.Add(C, aStepResult);
            }
          }
          else
            aShape = BO.Shape();
        }
      }

      if (isCompound){
        // As GlueFaces has been improved to keep all kind of shapes
        TopExp_Explorer anExp (C, TopAbs_VERTEX);
        if (anExp.More())
          aShape = GEOMImpl_GlueDriver::GlueFaces(C, Precision::Confusion(), Standard_True);
        else
          aShape = C;
      }
    }

	else if (theType == BOOLEAN_FUSE_OLD)
	{
      BRepAlgo_Fuse BO (theShape1, theShape2);
      if (!BO.IsDone()) {
        StdFail_NotDone::Raise("Fuse operation can not be performed on the given shapes");
      }
      aShape = BO.Shape();
	}
	else if (theType == BOOLEAN_SECTION_OLD)
	{
      BRepAlgo_Section BO (theShape1, theShape2);
      if (!BO.IsDone()) {
        StdFail_NotDone::Raise("Section operation can not be performed on the given shapes");
      }
      aShape = BO.Shape();
	}
	else if (theType == BOOLEAN_COMMON_OLD)
	{
      BRepAlgo_Common BO (theShape1, theShape2);
      if (!BO.IsDone()) {
        StdFail_NotDone::Raise("Common operation can not be performed on the given shapes");
      }
      aShape = BO.Shape();
	}
	else if (theType == BOOLEAN_CUT_OLD)
	{
      BRepAlgo_Cut BO (theShape1, theShape2);
      if (!BO.IsDone()) {
        StdFail_NotDone::Raise("Cut operation can not be performed on the given shapes");
      }
      aShape = BO.Shape();
	}
    // UNKNOWN operation
    else {
    }

  if (aShape.IsNull()) return aShape;

  // as boolean operations always produce compound, lets simplify it
  // for the case, if it contains only one sub-shape
  TopTools_ListOfShape listShapeRes;
  GEOMUtils::AddSimpleShapes(aShape, listShapeRes);
  if (listShapeRes.Extent() == 1) {
    aShape = listShapeRes.First();
    if (aShape.IsNull()) return aShape;
  }

  // 08.07.2008 skl for bug 19761 from Mantis
  BRepCheck_Analyzer ana (aShape, Standard_True);
  ana.Init(aShape);
  if (!ana.IsValid()) {
    ShapeFix_ShapeTolerance aSFT;
    aSFT.LimitTolerance(aShape, Precision::Confusion(),
                        Precision::Confusion(), TopAbs_SHAPE);
    Handle(ShapeFix_Shape) aSfs = new ShapeFix_Shape(aShape);
    aSfs->Perform();
    aShape = aSfs->Shape();
	ana.Init(aShape);
	if (!ana.IsValid()) {
	  Standard_CString anErrStr("Boolean operation aborted : non valid shape result");
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

  // BEGIN: Mantis issue 0021060: always limit tolerance of BOP result
  // 1. Get shape parameters for comparison
  int nbTypes [TopAbs_SHAPE];
  {
    for (int iType = 0; iType < TopAbs_SHAPE; ++iType)
      nbTypes[iType] = 0;
    nbTypes[aShape.ShapeType()]++;

    TopTools_MapOfShape aMapOfShape;
    aMapOfShape.Add(aShape);
    TopTools_ListOfShape aListOfShape;
    aListOfShape.Append(aShape);

    TopTools_ListIteratorOfListOfShape itL (aListOfShape);
    for (; itL.More(); itL.Next()) {
      TopoDS_Iterator it (itL.Value());
      for (; it.More(); it.Next()) {
        TopoDS_Shape s = it.Value();
        if (aMapOfShape.Add(s)) {
          aListOfShape.Append(s);
          nbTypes[s.ShapeType()]++;
        }
      }
    }
  }

  // 2. Limit tolerance
  TopoDS_Shape aShapeCopy;
  TColStd_IndexedDataMapOfTransientTransient aMapTShapes;
  TNaming_CopyShape::CopyTool(aShape, aMapTShapes, aShapeCopy);
  ShapeFix_ShapeTolerance aSFT;
  aSFT.LimitTolerance(aShapeCopy, Precision::Confusion(), Precision::Confusion(), TopAbs_SHAPE);
  Handle(ShapeFix_Shape) aSfs = new ShapeFix_Shape (aShapeCopy);
  aSfs->Perform();
  aShapeCopy = aSfs->Shape();

  // 3. Check parameters
  ana.Init(aShapeCopy);
  if (ana.IsValid()) {
    int iType, nbTypesCopy [TopAbs_SHAPE];

    for (iType = 0; iType < TopAbs_SHAPE; ++iType)
      nbTypesCopy[iType] = 0;
    nbTypesCopy[aShapeCopy.ShapeType()]++;

    TopTools_MapOfShape aMapOfShape;
    aMapOfShape.Add(aShapeCopy);
    TopTools_ListOfShape aListOfShape;
    aListOfShape.Append(aShapeCopy);

    TopTools_ListIteratorOfListOfShape itL (aListOfShape);
    for (; itL.More(); itL.Next()) {
      TopoDS_Iterator it (itL.Value());
      for (; it.More(); it.Next()) {
        TopoDS_Shape s = it.Value();
        if (aMapOfShape.Add(s)) {
          aListOfShape.Append(s);
          nbTypesCopy[s.ShapeType()]++;
        }
      }
    }

    bool isEqual = true;
    for (iType = 0; iType < TopAbs_SHAPE && isEqual; ++iType) {
      if (nbTypes[iType] != nbTypesCopy[iType])
        isEqual = false;
    }
    if (isEqual)
      aShape = aShapeCopy;
  }
  // END: Mantis issue 0021060

  return aShape;
  }

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_BooleanDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IBoolean aCI (function);
  Standard_Integer aType = function->GetType();

  switch ( aType ) {
  case BOOLEAN_COMMON:
    theOperationName = "COMMON";
    AddParam( theParams, "Object 1", aCI.GetShape1() );
    AddParam( theParams, "Object 2", aCI.GetShape2() );
    break;
  case BOOLEAN_CUT:
    theOperationName = "CUT";
    AddParam( theParams, "Main Object", aCI.GetShape1() );
    AddParam( theParams, "Tool Object", aCI.GetShape2() );
    break;
  case BOOLEAN_FUSE:
    theOperationName = "FUSE";
    AddParam( theParams, "Object 1", aCI.GetShape1() );
    AddParam( theParams, "Object 2", aCI.GetShape2() );
    break;
  case BOOLEAN_SECTION:
    theOperationName = "SECTION";
    AddParam( theParams, "Object 1", aCI.GetShape1() );
    AddParam( theParams, "Object 2", aCI.GetShape2() );
    break;
  case BOOLEAN_COMMON_LIST:
    theOperationName = "COMMON";
    AddParam( theParams, "Selected objects", aCI.GetShapes() );
    break;
  case BOOLEAN_FUSE_LIST:
    theOperationName = "FUSE";
    AddParam( theParams, "Selected objects", aCI.GetShapes() );
    break;
  case BOOLEAN_CUT_LIST:
    theOperationName = "CUT";
    AddParam( theParams, "Main Object", aCI.GetShape1() );
    AddParam( theParams, "Tool Objects", aCI.GetShapes() );
    break;
  default:
    return false;
}

  return true;
  }

IMPLEMENT_STANDARD_HANDLE (GEOMImpl_BooleanDriver,GEOM_BaseDriver);

IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_BooleanDriver,GEOM_BaseDriver);
