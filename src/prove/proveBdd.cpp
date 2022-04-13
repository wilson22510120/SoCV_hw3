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
   V3Ntk* ntk = v3Handler.getCurHandler()->getNtk();
   BddNodeV nowNode = BddNodeV::_one;
   for (unsigned i = 0; i < ntk->getLatchSize(); ++i) {
      const V3NetId& nId = ntk->getLatch(i);
      nowNode &= (getBddNodeV(nId.id) ^ BddNodeV::_zero);
   }
   _initState = nowNode;
}

void
BddMgrV::buildPTransRelation()
{
   // TODO : remember to set _tr, _tri
   BddNodeV nowNode = BddNodeV::_one;  
   V3Ntk* ntk = v3Handler.getCurHandler()->getNtk();
   for (unsigned i = 0, n = ntk->getLatchSize(); i < n; ++i) {
      const V3NetId& latchId = ntk->getLatch(i);
      const V3NetId& latchInputId = ntk->getInputNetId(latchId, 0);
      const V3NetId& YId = ntk->getLatch(i + n);
      nowNode &= (getBddNodeV(YId.id) ^ getBddNodeV(latchInputId.id));
   }
   _tri = nowNode;
   for (unsigned i = 0; i < ntk->getInputSize(); ++i) {
      nowNode = nowNode.exist(ntk->getInput(i).id);
   }
   _tr = nowNode;
}

void
BddMgrV::buildPImage( int level )
{
   // TODO : remember to add _reachStates and set _isFixed
   // Note:: _reachStates record the set of reachable states
   V3Ntk* ntk = v3Handler.getCurHandler()->getNtk();
   BddNodeV SnY, SnX = _initState, Sn1Y, Sn1X;
   _reachStates.push_back(_initState);

   for (unsigned i = 0; i < level; ++i) {
      // Sn+1(Y) = TR(Y, X)&Sn(X).exist(X)
      Sn1Y = _tr & SnX;
      for (unsigned j = 0; j < ntk->getLatchSize(); ++i) {
         Sn1Y = Sn1Y.exist(ntk->getLatch(j).id);
      }

      // X <- Y
      Sn1X = Sn1Y;

      // Rn+1(X) = Rn(x) | Sn+1(X)
      _reachStates.push_back(Sn1X);

      // check Rn+1 == Rn

      // next level
      SnX = Sn1X;
      SnY = Sn1Y;
   }

}

void 
BddMgrV::runPCheckProperty( const string &name, BddNodeV monitor )
{
   // TODO : prove the correctness of AG(~monitor)
}
