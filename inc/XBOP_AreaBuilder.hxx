// This file is generated by WOK (CPPExt).
// Please do not edit this file; modify original file instead.
// The copyright and license terms as defined for the original file apply to 
// this header file considered to be the "object code" form of the original source.

#ifndef _XBOP_AreaBuilder_HeaderFile
#define _XBOP_AreaBuilder_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _XBOP_ListOfListOfLoop_HeaderFile
#include <XBOP_ListOfListOfLoop.hxx>
#endif
#ifndef _XBOP_ListIteratorOfListOfListOfLoop_HeaderFile
#include <XBOP_ListIteratorOfListOfListOfLoop.hxx>
#endif
#ifndef _XBOP_ListIteratorOfListOfLoop_HeaderFile
#include <XBOP_ListIteratorOfListOfLoop.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _TopAbs_State_HeaderFile
#include <TopAbs_State.hxx>
#endif
#ifndef _Handle_XBOP_Loop_HeaderFile
#include <Handle_XBOP_Loop.hxx>
#endif
#ifndef _XBOP_LoopEnum_HeaderFile
#include <XBOP_LoopEnum.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
class XBOP_LoopSet;
class XBOP_LoopClassifier;
class XBOP_Loop;
class XBOP_ListOfLoop;
class XBOP_ListIteratorOfListOfLoop;



class XBOP_AreaBuilder  {
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

  
//! Empty  Constructor <br>
  Standard_EXPORT   XBOP_AreaBuilder();
  
//! Creates the objectr to build the areas on <br>
//! the shapes described by <LS> using the classifier <LC>. <br>
  Standard_EXPORT   XBOP_AreaBuilder(XBOP_LoopSet& LS,XBOP_LoopClassifier& LC,const Standard_Boolean ForceClass = Standard_False);
  
//! Destructor <br>
  Standard_EXPORT   virtual  void Delete() ;
Standard_EXPORT virtual ~XBOP_AreaBuilder(){Delete() ; }
  
//! Sets a AreaBuilder to find the areas on <br>
//! the shapes described by <LS> using the classifier <LC>. <br>
  Standard_EXPORT   virtual  void InitAreaBuilder(XBOP_LoopSet& LS,XBOP_LoopClassifier& LC,const Standard_Boolean ForceClass = Standard_False) ;
  
//! Initialize iteration on areas. <br>
  Standard_EXPORT     Standard_Integer InitArea() ;
  
  Standard_EXPORT     Standard_Boolean MoreArea() const;
  
  Standard_EXPORT     void NextArea() ;
  
//! Initialize iteration on loops of current Area. <br>
  Standard_EXPORT     Standard_Integer InitLoop() ;
  
  Standard_EXPORT     Standard_Boolean MoreLoop() const;
  
  Standard_EXPORT     void NextLoop() ;
  
//! Returns the current Loop in the current area. <br>
  Standard_EXPORT    const Handle_XBOP_Loop& Loop() const;
  
//! Internal <br>
  Standard_EXPORT   virtual  void ADD_Loop_TO_LISTOFLoop(const Handle(XBOP_Loop)& L,XBOP_ListOfLoop& LOL) const;
  
//! Internal <br>
  Standard_EXPORT   virtual  void REM_Loop_FROM_LISTOFLoop(XBOP_ListIteratorOfListOfLoop& ITLOL,XBOP_ListOfLoop& LOL) const;
  
//! Internal <br>
  Standard_EXPORT   virtual  void ADD_LISTOFLoop_TO_LISTOFLoop(XBOP_ListOfLoop& LOL1,XBOP_ListOfLoop& LOL2) const;





protected:

  
//! Compare position of the Loop <L> with the Area <LOL> <br>
//! using the Loop Classifier <LC>. <br>
//!       According to <aWhat>, Loops of <LOL> are selected or not <br>
//!       during <LOL> exploration. <br>
  Standard_EXPORT     TopAbs_State CompareLoopWithListOfLoop(XBOP_LoopClassifier& LC,const Handle(XBOP_Loop)& L,const XBOP_ListOfLoop& LOL,const XBOP_LoopEnum aWhat) const;
  
//! Internal <br>
  Standard_EXPORT     void Atomize(TopAbs_State& state,const TopAbs_State newstate) const;


XBOP_ListOfListOfLoop myArea;
XBOP_ListIteratorOfListOfListOfLoop myAreaIterator;
XBOP_ListIteratorOfListOfLoop myLoopIterator;
Standard_Boolean myUNKNOWNRaise;


private:





};





// other Inline functions and methods (like "C++: function call" methods)


#endif
