// Created on: 2001-05-30
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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



#include <XBOPTools_PCurveMaker.ixx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <Precision.hxx>

#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_HSurface.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>

#include <XBooleanOperations_ShapesDataStructure.hxx>

#include <XIntTools_Curve.hxx>

#include <XBOPTools_InterferencePool.hxx>
#include <XBOPTools_CArray1OfSSInterference.hxx>
#include <XBOPTools_SSInterference.hxx>
#include <XBOPTools_SequenceOfCurves.hxx>
#include <XBOPTools_Curve.hxx>
#include <XBOPTools_ListOfPaveBlock.hxx>
#include <XBOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <XBOPTools_PaveBlock.hxx>
#include <XBOPTools_Tools2D.hxx>

//modified by NIZNHY-PKV Tue Jun 28 13:24:42 2011f
static 
  void UpdateVertices(const TopoDS_Edge& aE, const TopoDS_Face& aF);
//modified by NIZNHY-PKV Tue Jun 28 13:24:46 2011t

//=======================================================================
// function: XBOPTools_PCurveMaker
// purpose: 
//=======================================================================
  XBOPTools_PCurveMaker::XBOPTools_PCurveMaker(const XBOPTools_PaveFiller& aFiller)
:
  myIsDone(Standard_False)
{
  myFiller=(XBOPTools_PaveFiller*) &aFiller;
  myDS=myFiller->DS();
}
//=======================================================================
// function: IsDone
// purpose: 
//=======================================================================
  Standard_Boolean XBOPTools_PCurveMaker::IsDone() const
{
  return myIsDone;
}
//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void XBOPTools_PCurveMaker::Do()
{
  XBOPTools_CArray1OfSSInterference& aFFs=(myFiller->InterfPool())->SSInterferences();
  
  Standard_Integer i, aNb,  nF1, nF2, nE;
  XBOPTools_ListIteratorOfListOfPaveBlock anIt;

  aNb=aFFs.Extent();
  for (i=1; i<=aNb; i++) {
    XBOPTools_SSInterference& aFF=aFFs(i);
    // Faces
    nF1=aFF.Index1();
    const TopoDS_Face& aF1=TopoDS::Face(myDS->Shape(nF1));
    nF2=aFF.Index2();
    const TopoDS_Face& aF2=TopoDS::Face(myDS->Shape(nF2));
    //
    // 1.Forwarding the Faces
    TopoDS_Face aF1FWD=aF1;
    aF1FWD.Orientation(TopAbs_FORWARD);
    TopoDS_Face aF2FWD=aF2;
    aF2FWD.Orientation(TopAbs_FORWARD);
    //
    // 2. In, On parts processing
    const XBOPTools_ListOfPaveBlock& aLPBInOn=aFF.PaveBlocks();
    anIt.Initialize(aLPBInOn);
    for (; anIt.More(); anIt.Next()) {
      const XBOPTools_PaveBlock& aPBInOn=anIt.Value();
      nE=aPBInOn.Edge();
      const TopoDS_Edge& aE=TopoDS::Edge(myDS->Shape(nE));
      
      XBOPTools_Tools2D::BuildPCurveForEdgeOnFace(aE, aF1FWD);
      XBOPTools_Tools2D::BuildPCurveForEdgeOnFace(aE, aF2FWD);
    }
    //
    // 3. Section Edges processing
    Standard_Integer j, aNbCurves, k, aNbV;   
    XBOPTools_SequenceOfCurves& aSC=aFF.Curves();
    aNbCurves=aSC.Length();
    for (j=1; j<=aNbCurves; j++) {
      const XBOPTools_Curve& aBC=aSC(j);
      const XBOPTools_ListOfPaveBlock& aLPB=aBC.NewPaveBlocks();
      anIt.Initialize(aLPB);
      for (; anIt.More(); anIt.Next()) {
	const XBOPTools_PaveBlock& aPB=anIt.Value();
	nE=aPB.Edge();
	const TopoDS_Edge& aE=TopoDS::Edge(myDS->Shape(nE));
	//
	Standard_Real aTolEdge, aTolR2D, aTolFact, aTolV, aTolVmax;
	//
	aTolEdge=BRep_Tool::Tolerance(aE);
	aTolR2D=aFF.TolR2D();
	aTolFact=Max(aTolEdge, aTolR2D);
	//
	// Check vertices tolerances and correct them if necessary 
	// to prevent situation when TolE > TolV
	//
	TopTools_IndexedMapOfShape aVMap;
	TopExp::MapShapes(aE, TopAbs_VERTEX, aVMap);

	aTolVmax=-1.;
	aNbV=aVMap.Extent();
	for (k=1; k<=aNbV; ++k) {
	  const TopoDS_Vertex& aV=TopoDS::Vertex(aVMap(k));
	  aTolV=BRep_Tool::Tolerance(aV);
	  if (aTolV>aTolVmax) {
	    aTolVmax=aTolV;
	  }
	}
	//
	if (aTolFact>aTolVmax) {
	  aTolFact=aTolVmax;
	}
	//
	BRep_Builder aBB;
	const XIntTools_Curve& aIC=aBC.Curve();
	
	//ZZ const Handle(Geom_Curve)& aC3D=aIC.Curve();
	// 
	Standard_Real aTFirst, aTLast, aOutFirst, aOutLast, aOutTol;
	const Handle(Geom_Curve)& aC3DE=BRep_Tool::Curve(aE, aTFirst, aTLast);
	Handle(Geom_TrimmedCurve)aC3DETrim=new Geom_TrimmedCurve(aC3DE, aTFirst, aTLast);
	//
	Handle(Geom2d_Curve) aC2D1, aC2D1A, aC2D2, aC2D2A;
	//
	aC2D1=aIC.FirstCurve2d();
	if (aC2D1.IsNull()) {
	  XBOPTools_Tools2D::BuildPCurveForEdgeOnFace(aE, aF1FWD);
	  XBOPTools_Tools2D::CurveOnSurface(aE, aF1FWD, aC2D1, aOutFirst, aOutLast, aOutTol, Standard_True);
	}
	//
	if (aC3DE->IsPeriodic()) {
	  XBOPTools_Tools2D::AdjustPCurveOnFace(aF1FWD, aTFirst, aTLast,  aC2D1, aC2D1A); 
	}
	else {
	  XBOPTools_Tools2D::AdjustPCurveOnFace(aF1FWD, aC3DETrim, aC2D1, aC2D1A); 
	}
	//
	aBB.UpdateEdge(aE, aC2D1A, aF1FWD, aTolFact);
	//modified by NIZNHY-PKV Tue Jun 28 07:52:55 2011f
	UpdateVertices(aE, aF1FWD);
	//modified by NIZNHY-PKV Tue Jun 28 07:52:57 2011t

	aC2D2=aIC.SecondCurve2d();
	if (aC2D2.IsNull()) {
	  XBOPTools_Tools2D::BuildPCurveForEdgeOnFace(aE, aF2FWD);
	  XBOPTools_Tools2D::CurveOnSurface(aE, aF2FWD, aC2D2, aOutFirst, aOutLast, aOutTol, Standard_True);
	}
	//
	if (aC3DE->IsPeriodic()) {
	  XBOPTools_Tools2D::AdjustPCurveOnFace(aF2FWD, aTFirst, aTLast, aC2D2, aC2D2A); 
	}
	else {
	  XBOPTools_Tools2D::AdjustPCurveOnFace(aF2FWD, aC3DETrim, aC2D2, aC2D2A); 
	}
	//
	aBB.UpdateEdge(aE, aC2D2A, aF2FWD, aTolFact);
	//modified by NIZNHY-PKV Tue Jun 28 07:52:55 2011f
	UpdateVertices(aE, aF2FWD);
	//modified by NIZNHY-PKV Tue Jun 28 07:52:57 2011t
      } 
    }// for (j=1; j<=aNbCurves; j++)
    
  } //  for (i=1; i<=aNb; i++)
  myIsDone=Standard_True;
}

//modified by NIZNHY-PKV Tue Jun 28 07:37:47 2011f
//=======================================================================
//function : UpdateVertices
//purpose  : update tolerances of vertices comparing extremities of
//           3d and 2d curves
//=======================================================================
void UpdateVertices(const TopoDS_Edge& aE, const TopoDS_Face& aF)
{
  Standard_Integer j;
  Standard_Real aT[2], aUx, aVx, aTolV2, aD2, aD;
  gp_Pnt aP3D, aP3Dx;
  gp_Pnt2d aP2Dx;
  Handle(Geom_Surface) aS;
  Handle(Geom_Curve) aC3D;
  Handle(Geom2d_Curve) aC2D;
  TopoDS_Edge aEf;
  TopoDS_Vertex aV[2];
  BRep_Builder aBB;
  //
  aEf=aE;
  aEf.Orientation(TopAbs_FORWARD);
  //
  TopExp::Vertices(aEf, aV[0], aV[1]);
  //
  aS=BRep_Tool::Surface(aF);
  aC3D=BRep_Tool::Curve(aEf, aT[0], aT[1]);
  aC2D=BRep_Tool::CurveOnSurface(aEf, aF, aT[0], aT[1]);
  //
  for (j=0; j<2; ++j) {
    aTolV2=BRep_Tool::Tolerance(aV[j]);
    aTolV2=aTolV2*aTolV2;
    //
    aC3D->D0(aT[j], aP3D);
    aC2D->D0(aT[j], aP2Dx);
    aP2Dx.Coord(aUx, aVx);
    aS->D0(aUx, aVx, aP3Dx);
    aD2=aP3D.SquareDistance(aP3Dx);
    if (aD2>aTolV2) {
      aD=sqrt(aD2);
      aBB.UpdateVertex(aV[j], aD);
    }
  }
}
//modified by NIZNHY-PKV Tue Jun 28 07:37:50 2011t
