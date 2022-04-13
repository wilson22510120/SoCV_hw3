/****************************************************************************
  FileName     [ proveBdd.cpp ]
  PackageName  [ prove ]
  Synopsis     [ For BDD-based verification ]
  Author       [ ]
  Copyright    [ Copyleft(c) 2010-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include "v3NtkUtil.h"
#include "v3Msg.h"
#include "bddMgrV.h"

void
BddMgrV::buildPInitialState()
{
   // TODO : remember to set _initState
   // Set Initial State to All Zero

   BddNodeV nowNode = BddNodeV::_one;
   for (unsigned i = 0; i < v3Handler.getCurHandler()->getNtk()->getLatchSize(); ++i) {
      const V3NetId& nId = v3Handler.getCurHandler()->getNtk()->getLatch(i);
      nowNode &= (getBddNodeV(nId.id) ^ BddNodeV::_zero);
   }
   _initState = nowNode;
}

void
BddMgrV::buildPTransRelation()
{
   // TODO : remember to set _tr, _tri
   BddNodeV nowNode = BddNodeV::_one;  
   for (unsigned i = 0, n = v3Handler.getCurHandler()->getNtk()->getLatchSize(); i < n; ++i) {
      const V3NetId& latchId = v3Handler.getCurHandler()->getNtk()->getLatch(i);
      const V3NetId& latchInputId = v3Handler.getCurHandler()->getNtk()->getInputNetId(latchId, 0);
      const V3NetId& YId = v3Handler.getCurHandler()->getNtk()->getLatch(i + n);
      nowNode &= (getBddNodeV(YId.id) ^ getBddNodeV(latchInputId.id));
   }
   _tri = nowNode;
   for (unsigned i = 0; i < v3Handler.getCurHandler()->getNtk()->getInputSize(); ++i) {
      nowNode = nowNode.exist(v3Handler.getCurHandler()->getNtk()->getInput(i).id);
   }
   _tr = nowNode;
}

void
BddMgrV::buildPImage( int level )
{
   // TODO : remember to add _reachStates and set _isFixed
   // Note:: _reachStates record the set of reachable states
   _reachStates.push_back(_initState);
   BddNodeV SnY, SnX = _initState, Sn1Y, Sn1X;
   for (unsigned i = 0; i < level; ++i) {
       
   }
}

void 
BddMgrV::runPCheckProperty( const string &name, BddNodeV monitor )
{
   // TODO : prove the correctness of AG(~monitor)
}
