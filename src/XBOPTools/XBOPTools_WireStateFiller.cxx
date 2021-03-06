// Created on: 2002-02-04
// Created by: Peter KURNEV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <XBOPTools_WireStateFiller.ixx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <BRep_Tool.hxx>

#include <XBooleanOperations_ShapesDataStructure.hxx>
#include <XBooleanOperations_IndexedDataMapOfShapeInteger.hxx>
#include <XBooleanOperations_StateOfShape.hxx>

#include <XBOPTools_SplitShapesPool.hxx>
#include <XBOPTools_CommonBlockPool.hxx>
#include <XBOPTools_CommonBlock.hxx>
#include <XBOPTools_PaveBlock.hxx>
#include <XBOPTools_ListOfPaveBlock.hxx>
#include <XBOPTools_ListOfCommonBlock.hxx>
#include <XBOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <XBOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <XBOPTools_InterferencePool.hxx>


//=======================================================================
// function:  XBOPTools_WireStateFiller::XBOPTools_WireStateFiller
// purpose: 
//=======================================================================
XBOPTools_WireStateFiller::XBOPTools_WireStateFiller(const XBOPTools_PaveFiller& aFiller)
:
  XBOPTools_StateFiller(aFiller)
{
}

//=======================================================================
// function: Do 
// purpose: 
//=======================================================================
  void XBOPTools_WireStateFiller::Do()
{
  TopAbs_ShapeEnum aT1, aT2;

  aT1=(myDS->Object()).ShapeType();
  aT2=(myDS->Tool()).ShapeType();

  myIsDone=Standard_True;
  
  if (aT1==TopAbs_WIRE && aT2==TopAbs_WIRE){
    DoWires(1);
    DoWires(2);
  }
  else if (aT1==TopAbs_WIRE  && aT2==TopAbs_SHELL){
    DoWires(1);
  }
  else if (aT2==TopAbs_WIRE  && aT1==TopAbs_SHELL){
    DoWires(2);
  }
  else if (aT1==TopAbs_WIRE && aT2==TopAbs_SOLID){
    DoWireSolid(1);
  }
  else if (aT2==TopAbs_WIRE && aT1==TopAbs_SOLID){
    DoWireSolid(2);
  }
  else {
    myIsDone=!myIsDone;
  }
  
}

//=======================================================================
// function: DoWires
// purpose: 
//=======================================================================
  void XBOPTools_WireStateFiller::DoWires (const Standard_Integer iRankObj)
{
  const TopoDS_Shape& anObj=(iRankObj==1) ? myDS->Object() : myDS->Tool(); 
  //
  const XBooleanOperations_IndexedDataMapOfShapeInteger& aDSMap=myDS->ShapeIndexMap(iRankObj);
  const XBOPTools_SplitShapesPool& aSplitShapesPool=myFiller->SplitShapesPool();
  const XBOPTools_CommonBlockPool& aCommonBlockPool=myFiller->CommonBlockPool();
  //
  Standard_Integer i, aNbPaveBlocks, nSp, aNbE, nE;
  XBooleanOperations_StateOfShape aSt;
  XBOPTools_ListIteratorOfListOfPaveBlock anItPB;
  XBOPTools_ListIteratorOfListOfCommonBlock anItCB;
  TopTools_IndexedMapOfShape aEM;
  //
  TopExp::MapShapes(anObj, TopAbs_EDGE, aEM);
  aNbE=aEM.Extent();
  //
  // 1
  for (i=1; i<=aNbE; i++) {
    const TopoDS_Edge& aE=TopoDS::Edge(aEM(i));
    nE=aDSMap.FindFromKey(aE);
    //
    if (BRep_Tool::Degenerated(aE)){
      continue;
    }
    //
    const XBOPTools_ListOfCommonBlock& aLCB=aCommonBlockPool(myDS->RefEdge(nE));
    //
    anItCB.Initialize(aLCB);
    for (; anItCB.More(); anItCB.Next()) {
      XBOPTools_CommonBlock& aCB=anItCB.Value();
      XBOPTools_PaveBlock& aPB=aCB.PaveBlock1(nE);
      nSp=aPB.Edge();
      myDS->SetState(nSp, XBooleanOperations_ON);
    }
  }
  //
  // 2
  for (i=1; i<=aNbE; i++) {
    const TopoDS_Edge& aE=TopoDS::Edge(aEM(i));
    nE=aDSMap.FindFromKey(aE);
    //
    if (BRep_Tool::Degenerated(aE)){
      continue;
    }
    //
    const XBOPTools_ListOfPaveBlock& aLPB=aSplitShapesPool(myDS->RefEdge(nE));
    //
    aNbPaveBlocks=aLPB.Extent();
    
    if (!aNbPaveBlocks) {
      myDS->SetState(nE, XBooleanOperations_OUT);
      continue;
    }
    //
    anItPB.Initialize(aLPB);
    for (; anItPB.More(); anItPB.Next()) {
      const XBOPTools_PaveBlock& aPB=anItPB.Value();
      nSp=aPB.Edge();
      aSt=myDS-> GetState(nSp);
      if (aSt!=XBooleanOperations_ON) {
	myDS->SetState(nSp, XBooleanOperations_OUT);
      }
    }
  }
}
//=======================================================================

#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <BRep_Builder.hxx>

#include <XBooleanOperations_IndexedDataMapOfShapeInteger.hxx>

#include <XBOPTools_SplitShapesPool.hxx>
#include <XBOPTools_CommonBlockPool.hxx>
#include <XBOPTools_ListOfPaveBlock.hxx>
#include <XBOPTools_ListOfCommonBlock.hxx>
#include <XBOPTools_PaveBlock.hxx>
#include <XBOPTools_CommonBlock.hxx>
#include <XBOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <XBOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <XBOPTools_IndexedDataMapOfShapeWithState.hxx>
#include <XBOPTools_StateFiller.hxx>

static
  void PropagateState(const TopoDS_Shape& aS,
		      const XBooleanOperations_StateOfShape aState,
		      XBooleanOperations_ShapesDataStructure* pDS,
		      const Standard_Integer iRank,
		      XBOPTools_IndexedDataMapOfShapeWithState& aSWS,
		      TopTools_IndexedMapOfShape& aProcessedShapes);

static
  Standard_Boolean HasConnexity(const TopoDS_Shape& aS,
			       const XBOPTools_IndexedDataMapOfShapeWithState& aSWS,
			       const TopTools_IndexedDataMapOfShapeListOfShape& aMVE,
			       XBooleanOperations_StateOfShape& aState);

//=======================================================================
// function: DoWireSolid
// purpose: 
//=======================================================================
  void XBOPTools_WireStateFiller::DoWireSolid (const Standard_Integer iRankObj)
{
  const TopoDS_Shape& anObj=(iRankObj==1) ? myDS->Object() : myDS->Tool();
  const TopoDS_Shape& aTool=(iRankObj==1) ? myDS->Tool()   : myDS->Object();
  //
  const XBooleanOperations_IndexedDataMapOfShapeInteger& aDSMap=myDS->ShapeIndexMap(iRankObj);
  const XBOPTools_SplitShapesPool& aSplitShapesPool=myFiller->SplitShapesPool();
  const XBOPTools_CommonBlockPool& aCommonBlockPool=myFiller->CommonBlockPool();
  //
  Standard_Integer i, aNb, nE, aNbPB;
  XBooleanOperations_StateOfShape aState;
  TopTools_IndexedMapOfShape aEM, anIntersectedShapes, aNonIntersectedShapes;
  TopTools_IndexedDataMapOfShapeListOfShape aM, aMVE;
  
  //
  // aM Map
  TopExp::MapShapesAndAncestors (anObj, TopAbs_EDGE , TopAbs_WIRE , aM);
  // VE Map
  TopExp::MapShapesAndAncestors (anObj, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
  //
  // 1.2. Edges that have Split parts
  TopExp::MapShapes(anObj, TopAbs_EDGE, aEM);
  aNb=aEM.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aE=aEM(i);
    nE=aDSMap.FindFromKey(aE);
    const XBOPTools_ListOfPaveBlock& aLPB=aSplitShapesPool(myDS->RefEdge(nE));
    aNbPB=aLPB.Extent();
    //
    if (!aNbPB) {
      continue;
    }
    //
    if (aNbPB==1) {
      const XBOPTools_ListOfCommonBlock& aLCB=aCommonBlockPool(myDS->RefEdge(nE));
      if (!aLCB.Extent()) {
	const XBOPTools_PaveBlock& aPB=aLPB.First();
	Standard_Integer nEB=aPB.Edge();
	if (nEB==aPB.OriginalEdge()) {
	  Standard_Boolean bHasInterference=Standard_False;// Wng in Gcc 3.0
	  Standard_Integer j, aNbSuc, nV;
	  aNbSuc=myDS->NumberOfSuccessors(nEB);
	  for (j=1; j<=aNbSuc; j++) {
	    nV=myDS->GetSuccessor(nE, j);
	    bHasInterference=myIntrPool->HasInterference(nV);
	    if (bHasInterference) {
	      break;
	    }
	  }
	  if (!bHasInterference) {
	    continue;
	  }
	}
      }
    }
    anIntersectedShapes.Add(aE);
  }// for (i=1; i<=aNb; i++)
  //
  // 1.3. Write Intersected state for anIntersectedShapes to the DS
  aNb=anIntersectedShapes.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=anIntersectedShapes(i);
    nE=aDSMap.FindFromKey(aS);
    myDS->SetState(nE, XBooleanOperations_INTERSECTED);
  }
  //
  // 2. aNonIntersectedShapes
  //
  aNb=aM.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=aM.FindKey(i);
    if (!anIntersectedShapes.Contains(aS)) {
      aNonIntersectedShapes.Add(aS);
    }
  }
  //
  // 2.1. Processing of Non-intersected shapes 
  BRep_Builder BB;
  TopoDS_Compound aCompound;
  BB.MakeCompound(aCompound);
  aNb=aNonIntersectedShapes.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=aNonIntersectedShapes(i);
    BB.Add(aCompound, aS);
  }
  //
  TopTools_IndexedMapOfShape aProcessedShapes;
  XBOPTools_IndexedDataMapOfShapeWithState aSWS;
  Standard_Boolean bHasConnexity;
  //
  aEM.Clear(); 
  TopExp::MapShapes(aCompound, TopAbs_EDGE, aEM);
  aNb=aEM.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=aEM(i);
    if (!aProcessedShapes.Contains(aS)) {
      bHasConnexity=HasConnexity(aS, aSWS, aMVE, aState);
      if (!bHasConnexity) {
	aState=XBOPTools_StateFiller::ClassifyShapeByRef (aS, aTool);
      }
      aSWS.Add(aS, aState);
      aProcessedShapes.Add(aS);
      PropagateState(aS, aState, myDS, iRankObj, aSWS, aProcessedShapes); 
    }
  }
  //
  // 2.2. Write Stats for Non-intersected Shapes to the DS
  aNb=aSWS.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=aSWS.FindKey(i);
    aState=aSWS.FindFromIndex(i);
    nE=aDSMap.FindFromKey(aS);
    myDS->SetState(nE, aState);
  }
  //---------------------------------------------------
  //
  // 3.  Intersected Edges' Processing
  //
  //---------------------------------------------------
  Standard_Integer nSp, aNBVertices, nV1, nV2;
  XBooleanOperations_StateOfShape aStV1, aStV2;

  aNb=anIntersectedShapes.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=anIntersectedShapes(i);
    if (aS.ShapeType()==TopAbs_EDGE) {
      nE=aDSMap.FindFromKey(aS);
      //
      // 3.1. On Parts Processing
      const XBOPTools_ListOfCommonBlock& aLCB=aCommonBlockPool(myDS->RefEdge(nE));
      XBOPTools_ListIteratorOfListOfCommonBlock anItCB(aLCB);
      for (; anItCB.More(); anItCB.Next()) {
	const XBOPTools_CommonBlock& aCB=anItCB.Value();
	XBOPTools_CommonBlock* pCB=(XBOPTools_CommonBlock*) &aCB;
	XBOPTools_PaveBlock& aPB=pCB->PaveBlock1(nE);
	nSp=aPB.Edge();
	myDS->SetState(nSp, XBooleanOperations_ON);
      }
      //
      // 3.2. IN, OUT Parts Processing
      const XBOPTools_ListOfPaveBlock& aSplits=aSplitShapesPool(myDS->RefEdge(nE));
      XBOPTools_ListIteratorOfListOfPaveBlock anItPB(aSplits);
      for (; anItPB.More(); anItPB.Next()) {
	const XBOPTools_PaveBlock& aPB=anItPB.Value();
	nSp=aPB.Edge();
	const TopoDS_Shape& aSplit=myDS->Shape(nSp);

	aState=myDS->GetState(nSp);
	if (aState==XBooleanOperations_UNKNOWN|| aState==XBooleanOperations_INTERSECTED){
	  aNBVertices=myDS->NumberOfSuccessors(nE);
	  if (aNBVertices==2) {
	    nV1=myDS->GetSuccessor(nSp, 1);
	    aStV1=myDS->GetState(nV1);
	    nV2=myDS->GetSuccessor(nSp, 2);
	    aStV2=myDS->GetState(nV2);
	    if      ((aStV1==XBooleanOperations_IN || aStV1==XBooleanOperations_OUT) 
		     && (aStV2==XBooleanOperations_ON)) {
	      myDS->SetState(nSp, aStV1);
	    }
	    else if ((aStV2==XBooleanOperations_IN || aStV2==XBooleanOperations_OUT)
		     && (aStV1==XBooleanOperations_ON)) {
	      myDS->SetState(nSp, aStV2);
	    }
	    else {
	      aState=XBOPTools_StateFiller::ClassifyShapeByRef(aSplit, aTool);
	      myDS->SetState(nSp, aState);
	      if (aStV1==XBooleanOperations_UNKNOWN) {
		myDS->SetState(nV1, aState);
	      }
	      if (aStV2==XBooleanOperations_UNKNOWN) {
		myDS->SetState(nV2, aState);
	      }
	    }
	  }// if (aNBVertices==2)
	  else {
	    aState=XBOPTools_StateFiller::ClassifyShapeByRef(aSplit, aTool);
	    myDS->SetState(nSp, aState);
	  }
	}// if (aState==XBooleanOperations_UNKNOWN || XBooleanOperations_INTERSECTED)
      }//for (; anItPB.More(); anItPB.Next())
    }// if (aS.ShapeType()==TopAbs_EDGE)
  }// next "Intersected" Edge
  
}

//=======================================================================
// function:  HasConnexity
// purpose: 
//=======================================================================
 Standard_Boolean HasConnexity(const TopoDS_Shape& aS,
			       const XBOPTools_IndexedDataMapOfShapeWithState& aSWS,
			       const TopTools_IndexedDataMapOfShapeListOfShape& aMVE,
			       XBooleanOperations_StateOfShape& aState)
{
  TopAbs_ShapeEnum aType;
  XBooleanOperations_StateOfShape aSt;
  aType=aS.ShapeType();
  if (aType!=TopAbs_EDGE) {
    Standard_Integer i, aNb;
    TopTools_IndexedMapOfShape aME;
    TopExp::MapShapes(aS, TopAbs_EDGE, aME);
    aNb=aME.Extent();
    for (i=1; i<=aNb; i++) {
      const TopoDS_Shape& aE=aME(i);
      if (aSWS.Contains(aE)){
	aSt=aSWS.FindFromKey(aE);
	aState=aSt;
	return Standard_True;
      }
    }
  }
  else {
    TopExp_Explorer anExp (aS, TopAbs_VERTEX);
    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aV=anExp.Current();
      if (aMVE.Contains(aV)) {
	const TopTools_ListOfShape& anEdgesList=aMVE.FindFromKey(aV);
	TopTools_ListIteratorOfListOfShape anIt(anEdgesList);
	for (; anIt.More(); anIt.Next()) {
	  const TopoDS_Shape& aEx=anIt.Value();
	  if (aSWS.Contains(aEx)) {
	    aSt=aSWS.FindFromKey(aEx);
	    aState=aSt;
	    return Standard_True;
	  }
	}
      }
    }
  }
  
  aState=XBooleanOperations_UNKNOWN;
  return Standard_False;
}

//=======================================================================
// function:  PropagateState
// purpose: 
//=======================================================================
void PropagateState(const TopoDS_Shape& aSS,
		    const XBooleanOperations_StateOfShape aState,
		    XBooleanOperations_ShapesDataStructure* pDS,
		    const Standard_Integer iRank,
		    XBOPTools_IndexedDataMapOfShapeWithState& aSWS,
		    TopTools_IndexedMapOfShape& aProcessedShapes)
{
  TopAbs_ShapeEnum aSubType;

  aSubType=XBOPTools_StateFiller::SubType(aSS);
  
  if (aSubType==TopAbs_SHAPE) {
    return;
  }

  const XBooleanOperations_IndexedDataMapOfShapeInteger& aDSMap= pDS->ShapeIndexMap(iRank);

  TopTools_IndexedMapOfShape aSubMap;
  TopExp::MapShapes(aSS, aSubType, aSubMap);

  Standard_Integer i, aNb, nV;
  aNb=aSubMap.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& aS=aSubMap(i);
    if (!aProcessedShapes.Contains(aS)) {
      if (aSubType==TopAbs_VERTEX) {
	nV=aDSMap.FindFromKey(aS);
	XBooleanOperations_StateOfShape aSt=pDS->GetState(nV);
	if (aSt!=XBooleanOperations_UNKNOWN){
	  aProcessedShapes.Add(aS);
	  continue;
	}
      }
      aSWS.Add(aS, aState);
      aProcessedShapes.Add(aS);
      PropagateState (aS, aState, pDS, iRank, aSWS, aProcessedShapes);
    }
  }
}	    
