// This file is generated by WOK (CPPExt).
// Please do not edit this file; modify original file instead.
// The copyright and license terms as defined for the original file apply to 
// this header file considered to be the "object code" form of the original source.

#ifndef _XIntTools_MapIteratorOfMapOfCurveSample_HeaderFile
#define _XIntTools_MapIteratorOfMapOfCurveSample_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _TCollection_BasicMapIterator_HeaderFile
#include <TCollection_BasicMapIterator.hxx>
#endif
#ifndef _Handle_XIntTools_StdMapNodeOfMapOfCurveSample_HeaderFile
#include <Handle_XIntTools_StdMapNodeOfMapOfCurveSample.hxx>
#endif
class Standard_NoSuchObject;
class XIntTools_CurveRangeSample;
class XIntTools_CurveRangeSampleMapHasher;
class XIntTools_MapOfCurveSample;
class XIntTools_StdMapNodeOfMapOfCurveSample;



class XIntTools_MapIteratorOfMapOfCurveSample  : public TCollection_BasicMapIterator {
public:

  void* operator new(size_t,void* anAddress) 
  {
    return anAddress;
  }
  void* operator new(size_t size) 
  {
    return Standard::Allocate(size); 
  }
  void  operator delete(void *anAddress) 
  {
    if (anAddress) Standard::Free((Standard_Address&)anAddress); 
  }

  
  Standard_EXPORT   XIntTools_MapIteratorOfMapOfCurveSample();
  
  Standard_EXPORT   XIntTools_MapIteratorOfMapOfCurveSample(const XIntTools_MapOfCurveSample& aMap);
  
  Standard_EXPORT     void Initialize(const XIntTools_MapOfCurveSample& aMap) ;
  
  Standard_EXPORT    const XIntTools_CurveRangeSample& Key() const;





protected:





private:





};





// other Inline functions and methods (like "C++: function call" methods)


#endif