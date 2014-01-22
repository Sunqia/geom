// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _GEOMImpl_PipeTShapeDriver_HXX
#define _GEOMImpl_PipeTShapeDriver_HXX

#include <TFunction_Driver.hxx>

#include "GEOMAlgo_State.hxx"

#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <gp_Ax2.hxx>

#include <Handle_Geom_Surface.hxx>

class Handle_Standard_Type;
class GEOMImpl_PipeTShapeDriver;


  
#include "GEOM_BaseDriver.hxx"
  
DEFINE_STANDARD_HANDLE( GEOMImpl_PipeTShapeDriver, GEOM_BaseDriver );

class GEOMImpl_PipeTShapeDriver : public GEOM_BaseDriver {
public:
  // Methods PUBLIC
  // 
  Standard_EXPORT GEOMImpl_PipeTShapeDriver();
  Standard_EXPORT virtual  Standard_Integer Execute(TFunction_Logbook& log) const; 
  Standard_EXPORT virtual void Validate(TFunction_Logbook&) const {}
  Standard_EXPORT Standard_Boolean MustExecute(const TFunction_Logbook&) const
  {
    return Standard_True;
  }
  Standard_EXPORT static const Standard_GUID& GetID();
  Standard_EXPORT ~GEOMImpl_PipeTShapeDriver() {};
  
  Standard_EXPORT virtual
  bool GetCreationInformation(std::string&             theOperationName,
                              std::vector<GEOM_Param>& params);
  // Type management
  //
DEFINE_STANDARD_RTTI( GEOMImpl_PipeTShapeDriver )

private:

  /*!
   * \brief Create a T-Shape based on pipes
   * \param r1 - the internal radius of main pipe
   * \param w1 - the thickness of main pipe
   * \param l1 - the half-length of main pipe
   * \param r2 - the internal radius of incident pipe
   * \param w2 - the thickness of incident pipe
   * \param l2 - the half-length of main pipe
   * \retval TopoDS_Shape - Resulting shape
   */
  TopoDS_Shape MakePipeTShape(double r1, double w1, double l1,
                              double r2, double w2, double l2) const;

  /*!
   * \brief Create a quarter of a T-Shape based on pipes
   * \param r1 - the internal radius of main pipe
   * \param w1 - the thickness of main pipe
   * \param l1 - the half-length of main pipe
   * \param r2 - the internal radius of incident pipe
   * \param w2 - the thickness of incident pipe
   * \param l2 - the half-length of main pipe
   * \retval TopoDS_Shape - Resulting shape
   */
  TopoDS_Shape MakeQuarterPipeTShape(double r1, double w1, double l1,
                                     double r2, double w2, double l2) const;

  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
   * \param theSurface - the surface to check state of sub-shapes against
   * \param theShape - the shape to explore
   * \param theShapeType - type of sub-shape of theShape
   * \param theState - required state
   * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Handle(TColStd_HSequenceOfInteger)
    GetShapesOnSurfaceIDs(const Handle(Geom_Surface)& theSurface,
                          const TopoDS_Shape&         theShape,
                          TopAbs_ShapeEnum            theShapeType,
                          GEOMAlgo_State              theState) const;

  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
    * \param theBox - the box to check state of sub-shapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Handle(TColStd_HSequenceOfInteger)
  GetShapesOnBoxIDs(const TopoDS_Shape& aBox,
                 const TopoDS_Shape& aShape,
                 const Standard_Integer theShapeType,
                 GEOMAlgo_State theState) const;

  //=======================================================================
  //function : getCommonShapesOnCylinders
  //purpose  : return the common edge between 2 cylindrical surfaces
  //           along OX and OZ
  //=======================================================================
  void GetCommonShapesOnCylinders(const TopoDS_Shape& theShape,
				  TopAbs_ShapeEnum theShapeType,
                  double r, double r2,
				  Handle(TopTools_HSequenceOfShape)& commonShapes) const;

};

#endif // _GEOMImpl_PipeTShapeDriver_HXX
