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
      nowNode &= ~getSupport(nId.id);
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
      BddNodeV latchInputNode = getBddNodeV(latchInputId.id);
      if(latchInputId.cp) latchInputNode = ~latchInputNode;       // BddNodeV latchInputNode = latchId.cp ? ~getBddNodeV(latchInputId.id) : getBddNodeV(latchInputId.id);
      // const BddNodeV& Y = getSupport(n + latchId.id);
      nowNode &= ~(getSupport(n + latchId.id) ^ latchInputNode);
   }
   _tri = nowNode;
   for (unsigned i = 0; i < ntk->getInputSize(); ++i) {
      nowNode = nowNode.exist(ntk->getInput(i).id);
   }
   _tr = nowNode;

   
}

void
BddMgrV::buildPImage(int level)
{
   // TODO : remember to add _reachStates and set _isFixed
   // Note:: _reachStates record the set of reachable states
   V3Ntk* ntk = v3Handler.getCurHandler()->getNtk();

   if (_reachStates.empty()) _reachStates.push_back(_initState);

   if (_isFixed) {
      cout << "Fixed point is reached ( time : "<< _reachStates.size() - 2 <<" )" <<endl;
      return;
   }

   BddNodeV SnY, SnX = getPReachState(), Sn1Y, Sn1X;

   for (unsigned i = 0; i < level; ++i) {
      // Sn+1(Y) = TR(Y, X)&Sn(X).exist(X)
      Sn1Y = _tr & SnX;
      for (unsigned j = 0; j < ntk->getLatchSize(); ++j) {
         Sn1Y = Sn1Y.exist(ntk->getLatch(j).id);
      }

      // X <- Y
      bool isMoved;
      const V3NetId& id = ntk->getLatch(0);
      Sn1X = Sn1Y.nodeMove(id.id + ntk->getLatchSize(), id.id, isMoved);

      // Rn+1(X) = Rn(x) | Sn+1(X)
      const BddNodeV& back =  _reachStates.back();
      _reachStates.push_back(Sn1X | back);

      // check Rn+1 == Rn
      if (_reachStates.back() == _reachStates[_reachStates.size() - 2]) {
         cout << "Fixed point is reached (time : "<< _reachStates.size() - 2 <<")" <<endl;
         _isFixed = true;
         return;
      }

      // next level
      SnX = Sn1X;
      SnY = Sn1Y;
   }

}

void 
BddMgrV::runPCheckProperty( const string &name, BddNodeV monitor )
{
   // TODO : prove the correctness of AG(~monitor)
   V3Ntk* ntk = v3Handler.getCurHandler()->getNtk();
   const BddNodeV& target = monitor & getPReachState();
   if (!target.countCube()) {
      // target is safe
      if (_isFixed) {
         cout << "Monitor \"" << name << "\" is safe." << endl;
      }
      else {
         cout << "Monitor \"" << name << "\" is safe up to time " << _reachStates.size() - 1 << "." << endl;
      }
   }
   else {
      // counter example
      cout << "Monitor \"" << name << "\" is violated." << endl; 
      cout << "Counter Example:" << endl;

      BddNodeV Sn1X = target.getCube();
      bool isMoved;
      int t = 0;
      while (!(monitor & _reachStates[t]).countCube()) ++t;
      for (int k = t; t >= 0; --t) {
         Sn1X = Sn1X.nodeMove(ntk->getLatch(0).id, ntk->getLatch(0).id + ntk->getLatchSize(), isMoved);
         BddNodeV VnI = _tri & Sn1X;
         for (unsigned i = 0; i < ntk->getLatchSize(); ++i) {
            VnI = VnI.exist(ntk->getLatch(i).id);
         }

         Sn1X = _tri & Sn1X & VnI;
         for (unsigned i = 0; i < ntk->getInputSize(); ++i) {
            Sn1X = Sn1X.exist(ntk->getInput(i).id);
         }
         for (unsigned i = 0; i < ntk->getLatchSize(); ++i) {
            Sn1X = Sn1X.exist(ntk->getLatch(i).id + ntk->getLatchSize());
         }


         cout << k-t << ": " << VnI.getCube().toString()[1] << endl;
      }
   }
}
