
#include "Dependency.h"

using namespace klee;

namespace klee {

std::map<const Array *, const Array *> ShadowArray::shadowArray;

UpdateNode *
ShadowArray::getShadowUpdate(const UpdateNode *source,
                             std::vector<const Array *> &replacements) {
  if (!source) return 0;

  return new UpdateNode(
      ShadowArray::getShadowUpdate(source->next, replacements),
      ShadowArray::getShadowExpression(source->index, replacements),
      ShadowArray::getShadowExpression(source->value, replacements));
}

void ShadowArray::addShadowArrayMap(const Array *source, const Array *target) {
  shadowArray[source] = target;
}

ref<Expr>
ShadowArray::getShadowExpression(ref<Expr> expr,
                                 std::vector<const Array *> &replacements) {
  ref<Expr> ret;

  switch (expr->getKind()) {
  case Expr::Read: {
    ReadExpr *readExpr = static_cast<ReadExpr *>(expr.get());
    const Array *replacementArray =
        ShadowArray::shadowArray[readExpr->updates.root];

    if (std::find(replacements.begin(), replacements.end(), replacementArray) ==
        replacements.end())
      replacements.push_back(replacementArray);

    UpdateList newUpdates(
        replacementArray,
        ShadowArray::getShadowUpdate(readExpr->updates.head, replacements));
    ret = ReadExpr::alloc(newUpdates, ShadowArray::getShadowExpression(
                                          readExpr->index, replacements));
    break;
  }
  case Expr::Constant: {
    ret = expr;
    break;
  }
  case Expr::Concat: {
    ret = ConcatExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Select: {
    ret = SelectExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements),
        ShadowArray::getShadowExpression(expr->getKid(2), replacements));
    break;
  }
  case Expr::Extract: {
    ExtractExpr *extractExpr = static_cast<ExtractExpr *>(expr.get());
    ret = ExtractExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        extractExpr->offset, extractExpr->width);
    break;
  }
  case Expr::ZExt: {
    CastExpr *castExpr = static_cast<CastExpr *>(expr.get());
    ret = ZExtExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        castExpr->getWidth());
    break;
  }
  case Expr::SExt: {
    CastExpr *castExpr = static_cast<CastExpr *>(expr.get());
    ret = SExtExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        castExpr->getWidth());
    break;
  }
  case Expr::Add: {
    ret = AddExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Sub: {
    ret = SubExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Mul: {
    ret = MulExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::UDiv: {
    ret = UDivExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::SDiv: {
    ret = SDivExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::URem: {
    ret = URemExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::SRem: {
    ret = SRemExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Not: {
    ret = NotExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements));
    break;
  }
  case Expr::And: {
    ret = AndExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Or: {
    ret = OrExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Xor: {
    ret = XorExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Shl: {
    ret = ShlExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::LShr: {
    ret = LShrExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::AShr: {
    ret = AShrExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Eq: {
    ret = EqExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Ne: {
    ret = NeExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Ult: {
    ret = UltExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Ule: {
    ret = UleExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Ugt: {
    ret = UgtExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Uge: {
    ret = UgeExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Slt: {
    ret = SltExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Sle: {
    ret = SleExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Sgt: {
    ret = SgtExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  case Expr::Sge: {
    ret = SgeExpr::alloc(
        ShadowArray::getShadowExpression(expr->getKid(0), replacements),
        ShadowArray::getShadowExpression(expr->getKid(1), replacements));
    break;
  }
  default: {
    assert(0 && "unhandled Expr type");
    ret = expr;
    break;
  }
  }

  return ret;
}

/**/

bool Allocation::isComposite() const {
  // We return true by default as composites are
  // more generally handled.
  return true;
}

void Allocation::print(llvm::raw_ostream &stream) const {
  // Do nothing
}

/**/

void CompositeAllocation::print(llvm::raw_ostream &stream) const {
  stream << "A(composite)";
  if (core)
    stream << "(I)";
  stream << "[";
  site->print(stream);
  stream << "] ";
}

/**/

  bool VersionedAllocation::isComposite() const {
    // Only non-composite allocations can be versioned
    // and destructively updated
    return false;
  }

  void VersionedAllocation::print(llvm::raw_ostream& stream) const {
    stream << "A(singleton)";
    if (core)
      stream << "(I)";
    stream << "[";
    site->print(stream);
    stream << "]#" << reinterpret_cast<uintptr_t>(this);
  }

  /**/

  llvm::Value *EnvironmentAllocation::canonicalAllocation = 0;

  bool EnvironmentAllocation::hasAllocationSite(llvm::Value *site) const {
    return Dependency::Util::isEnvironmentAllocation(site);
  }

  void EnvironmentAllocation::print(llvm::raw_ostream& stream) const {
    stream << "A";
    if (this->Allocation::core)
      stream << "(I)";
    stream << "[@__environ]" << reinterpret_cast<uintptr_t>(this);
  }

  /**/

  void VersionedValue::print(llvm::raw_ostream& stream) const {
    stream << "V";
    if (inInterpolant)
      stream << "(I)";
    stream << "[";
    value->print(stream);
    stream << ":";
    valueExpr->print(stream);
    stream << "]#" << reinterpret_cast<uintptr_t>(this);
    ;
  }

  /**/

  void PointerEquality::print(llvm::raw_ostream& stream) const {
    stream << "(";
    value->print(stream);
    stream << "==";
    allocation->print(stream);
    stream << ")";
  }

  /**/

  void StorageCell::print(llvm::raw_ostream& stream) const {
    stream << "[";
    allocation->print(stream);
    stream << ",";
    value->print(stream);
    stream << "]";
  }

  /**/

  void FlowsTo::print(llvm::raw_ostream &stream) const {
    source->print(stream);
    stream << "->";
    target->print(stream);
    if (via) {
      stream << " via ";
      via->print(stream);
    }
  }

  /**/

  bool AllocationGraph::isVisited(Allocation *alloc) {
    for (std::vector<AllocationNode *>::iterator it = allNodes.begin(),
                                                 itEnd = allNodes.end();
         it != itEnd; ++it) {
      if ((*it)->getAllocation() == alloc) {
        return true;
      }
    }
    return false;
  }

  void AllocationGraph::addNewSink(Allocation *candidateSink) {
    if (isVisited(candidateSink))
      return;

    AllocationNode *newNode = new AllocationNode(candidateSink);
    allNodes.push_back(newNode);
    sinks.push_back(newNode);
  }

  void AllocationGraph::addNewEdge(Allocation *source, Allocation *target) {
    AllocationNode *sourceNode = 0;
    AllocationNode *targetNode = 0;

    for (std::vector<AllocationNode *>::iterator it = allNodes.begin(),
                                                 itEnd = allNodes.end();
         it != itEnd; ++it) {
      if (!targetNode && (*it)->getAllocation() == target) {
        targetNode = (*it);
        if (sourceNode)
          break;
      }

      if (!sourceNode && (*it)->getAllocation() == source) {
        sourceNode = (*it);
        if (targetNode)
          break;
      }
    }

    bool newNode = false; // indicates whether a new node is created

    if (!sourceNode) {
      sourceNode = new AllocationNode(source);
      allNodes.push_back(sourceNode);
      newNode = true; // An edge actually added, return true
    } else {
      // Delete the source from the set of sinks
      std::vector<AllocationNode *>::iterator pos =
          std::find(sinks.begin(), sinks.end(), sourceNode);
      if (pos != sinks.end())
        sinks.erase(pos);
    }

    if (!targetNode) {
      targetNode = new AllocationNode(target);
      allNodes.push_back(targetNode);
      sinks.push_back(targetNode);

      newNode = true; // An edge actually added, return true
    }

    if (newNode || !targetNode->isCurrentParent(sourceNode)) {
      targetNode->addParent(sourceNode);
    }
  }

  void AllocationGraph::consumeSinkNode(Allocation *allocation) {
    std::vector<AllocationNode *>::iterator pos = sinks.end();
    for (std::vector<AllocationNode *>::iterator it = sinks.begin(),
                                                 itEnd = sinks.end();
         it != itEnd; ++it) {
      if ((*it)->getAllocation() == allocation) {
        pos = it;
        break;
      }
    }

    if (pos == sinks.end())
      return;

    std::vector<AllocationNode *> parents = (*pos)->getParents();

    sinks.erase(pos);

    for (std::vector<AllocationNode *>::iterator it = parents.begin(),
                                                 itEnd = parents.end();
         it != itEnd; ++it) {
      if (std::find(sinks.begin(), sinks.end(), (*it)) == sinks.end()) {
        sinks.push_back(*it);
      }
    }
  }

  std::vector<Allocation *> AllocationGraph::getSinkAllocations() const {
    std::vector<Allocation *> sinkAllocations;

    for (std::vector<AllocationNode *>::const_iterator it = sinks.begin(),
                                                       itEnd = sinks.end();
         it != itEnd; ++it) {
      sinkAllocations.push_back((*it)->getAllocation());
    }

    return sinkAllocations;
  }

  std::vector<Allocation *> AllocationGraph::getSinksWithAllocations(
      std::vector<Allocation *> valuesList) const {
    std::vector<Allocation *> sinkAllocations;

    for (std::vector<AllocationNode *>::const_iterator it = sinks.begin(),
                                                       itEnd = sinks.end();
         it != itEnd; ++it) {
      if (std::find(valuesList.begin(), valuesList.end(),
                    (*it)->getAllocation()) != valuesList.end())
        sinkAllocations.push_back((*it)->getAllocation());
    }

    return sinkAllocations;
  }

  void AllocationGraph::consumeNodesWithAllocations(
      std::vector<Allocation *> versionedAllocations,
      std::vector<Allocation *> compositeAllocations) {
    std::vector<Allocation *> sinkAllocs(
        getSinksWithAllocations(versionedAllocations));
    std::vector<Allocation *> tmp(
        getSinksWithAllocations(compositeAllocations));
    sinkAllocs.insert(sinkAllocs.begin(), tmp.begin(), tmp.end());

    if (sinkAllocs.empty())
      return;

    for (std::vector<Allocation *>::iterator it = sinkAllocs.begin(),
                                             itEnd = sinkAllocs.end();
         it != itEnd; ++it) {
      consumeSinkNode((*it));
    }

    // Recurse until fixpoint
    consumeNodesWithAllocations(versionedAllocations, compositeAllocations);
  }

  void AllocationGraph::print(llvm::raw_ostream &stream) const {
    std::vector<AllocationNode *> printed;
    print(stream, sinks, printed, 0);
  }

  void AllocationGraph::print(llvm::raw_ostream &stream,
                              std::vector<AllocationNode *> nodes,
                              std::vector<AllocationNode *> &printed,
                              const unsigned tabNum) const {
    if (nodes.size() == 0)
      return;

    std::string tabs = makeTabs(tabNum);

    for (std::vector<AllocationNode *>::iterator it = nodes.begin(),
                                                 itEnd = nodes.end();
         it != itEnd; ++it) {
      Allocation *alloc = (*it)->getAllocation();
      stream << tabs;
      alloc->print(stream);
      if (std::find(printed.begin(), printed.end(), (*it)) != printed.end()) {
        stream << " (printed)\n";
      } else if ((*it)->getParents().size()) {
        stream << " depends on\n";
        printed.push_back((*it));
        print(stream, (*it)->getParents(), printed, tabNum + 1);
      } else {
        stream << "\n";
      }
    }
  }

  /**/

  VersionedValue *Dependency::getNewVersionedValue(llvm::Value *value,
                                                   ref<Expr> valueExpr) {
    VersionedValue *ret = new VersionedValue(value, valueExpr);
    valuesList.push_back(ret);
    return ret;
  }

  Allocation *Dependency::getInitialAllocation(llvm::Value *allocation) {
    Allocation *ret;
    if (Util::isEnvironmentAllocation(allocation)) {
      ret = new EnvironmentAllocation(allocation);

      // An environment allocation is a special kind of composite allocation
      // ret->getSite() will give us the right canonical allocation
      compositeAllocationsList.push_back(ret);
        return ret;
    } else if (Util::isCompositeAllocation(allocation)) {
      ret = new CompositeAllocation(allocation);

      // We register composites in a special list
      compositeAllocationsList.push_back(ret);
      return ret;
    }

    ret = new VersionedAllocation(allocation);
    versionedAllocationsList.push_back(ret);
    return ret;
  }

  Allocation *Dependency::getNewAllocationVersion(llvm::Value *allocation) {
    Allocation *ret = getLatestAllocation(allocation);
    if (ret && ret->isComposite())
      return ret;

    return getInitialAllocation(allocation);
  }

  std::vector<Allocation *> Dependency::getAllVersionedAllocations() const {
    std::vector<Allocation *> allAlloc = versionedAllocationsList;
    if (parentDependency) {
      std::vector<Allocation *> parentVersionedAllocations =
          parentDependency->getAllVersionedAllocations();
      allAlloc.insert(allAlloc.begin(), parentVersionedAllocations.begin(),
                      parentVersionedAllocations.end());
    }
    return allAlloc;
  }

  std::map<llvm::Value *, ref<Expr> >
  Dependency::getLatestCoreExpressions(std::vector<const Array *> &replacements,
                                       bool interpolantValueOnly) const {
    std::vector<Allocation *> allAlloc = getAllVersionedAllocations();
    std::map<llvm::Value *, ref<Expr> > ret;

    for (std::vector<Allocation *>::iterator allocIter = allAlloc.begin(),
                                             allocIterEnd = allAlloc.end();
         allocIter != allocIterEnd; ++allocIter) {

      if (interpolantValueOnly &&
          std::find(interpolantAllocations.begin(),
                    interpolantAllocations.end(),
                    *allocIter) == interpolantAllocations.end())
        continue;

      std::vector<VersionedValue *> stored = stores(*allocIter);

      // We should only get the latest value and no other
      assert(stored.size() <= 1);

      if (stored.size()) {
        VersionedValue *v = stored.at(0);
        llvm::Value *site = (*allocIter)->getSite();

        if (!interpolantValueOnly) {
          ref<Expr> expr = v->getExpression();
          ret[site] = expr;
        } else if (v->valueInInterpolant()) {
          ref<Expr> expr = v->getExpression();
          ret[site] = ShadowArray::getShadowExpression(expr, replacements);
        }
      }
    }
    return ret;
  }

  std::vector<Allocation *> Dependency::getAllCompositeAllocations() const {
    std::vector<Allocation *> allAlloc = compositeAllocationsList;
    if (parentDependency) {
      std::vector<Allocation *> parentCompositeAllocations =
          parentDependency->getAllCompositeAllocations();
      allAlloc.insert(allAlloc.begin(), parentCompositeAllocations.begin(),
                      parentCompositeAllocations.end());
    }
    return allAlloc;
  }

  std::map<llvm::Value *, std::vector<ref<Expr> > >
  Dependency::getCompositeCoreExpressions(
      std::vector<const Array *> &replacements,
      bool interpolantValueOnly) const {
    std::vector<Allocation *> allAlloc = getAllCompositeAllocations();
    std::map<llvm::Value *, std::vector<ref<Expr> > > ret;

    for (std::vector<Allocation *>::iterator allocIter = allAlloc.begin(),
                                             allocIterEnd = allAlloc.end();
         allocIter != allocIterEnd; ++allocIter) {

      if (interpolantValueOnly &&
          std::find(interpolantAllocations.begin(),
                    interpolantAllocations.end(),
                    *allocIter) == interpolantAllocations.end())
        continue;

      std::vector<VersionedValue *> stored = stores(*allocIter);
      llvm::Value *site = (*allocIter)->getSite();

      for (std::vector<VersionedValue *>::iterator valueIter = stored.begin(),
                                                   valueIterEnd = stored.end();
           valueIter != valueIterEnd; ++valueIter) {
        if (!interpolantValueOnly) {
          std::vector<ref<Expr> > &elemList = ret[site];
          elemList.push_back((*valueIter)->getExpression());
        } else if ((*valueIter)->valueInInterpolant()) {
          std::vector<ref<Expr> > &elemList = ret[site];
          elemList.push_back(ShadowArray::getShadowExpression(
              (*valueIter)->getExpression(), replacements));
        }
      }
    }
    return ret;
  }

  VersionedValue *Dependency::getLatestValue(llvm::Value *value,
                                             ref<Expr> valueExpr) {
    assert(value && "value cannot be null");

    if (llvm::isa<llvm::Constant>(value) &&
        !llvm::isa<llvm::PointerType>(value->getType()))
      return getNewVersionedValue(value, valueExpr);

    for (std::vector<VersionedValue *>::const_reverse_iterator
             it = valuesList.rbegin(),
             itEnd = valuesList.rend();
         it != itEnd; ++it) {
      if ((*it)->hasValue(value))
        return *it;
    }

    if (parentDependency)
      return parentDependency->getLatestValue(value, valueExpr);

    return 0;
  }

  VersionedValue *
  Dependency::getLatestValueNoConstantCheck(llvm::Value *value) const {
    assert(value && "value cannot be null");

    for (std::vector<VersionedValue *>::const_reverse_iterator
             it = valuesList.rbegin(),
             itEnd = valuesList.rend();
         it != itEnd; ++it) {
      if ((*it)->hasValue(value))
        return *it;
    }

    if (parentDependency)
      return parentDependency->getLatestValueNoConstantCheck(value);

    return 0;
  }

  Allocation *Dependency::getLatestAllocation(llvm::Value *allocation) const {

    if (Util::isEnvironmentAllocation(allocation)) {
      // Search for existing environment allocation
      for (std::vector<Allocation *>::const_reverse_iterator
               it = compositeAllocationsList.rbegin(),
               itEnd = compositeAllocationsList.rend();
           it != itEnd; ++it) {
        if (llvm::isa<EnvironmentAllocation>(*it))
          return *it;
      }

      if (parentDependency)
        return parentDependency->getLatestAllocation(allocation);

      return 0;
    } else if (Util::isCompositeAllocation(allocation)) {

      // Search for existing composite non-environment allocation
      for (std::vector<Allocation *>::const_reverse_iterator
               it = compositeAllocationsList.rbegin(),
               itEnd = compositeAllocationsList.rend();
           it != itEnd; ++it) {
        if (!llvm::isa<EnvironmentAllocation>(*it) &&
            (*it)->hasAllocationSite(allocation))
          return *it;
      }

      if (parentDependency)
        return parentDependency->getLatestAllocation(allocation);

      return 0;
    }

    // The case for versioned allocation
    for (std::vector<Allocation *>::const_reverse_iterator
             it = versionedAllocationsList.rbegin(),
             itEnd = versionedAllocationsList.rend();
         it != itEnd; ++it) {
      if ((*it)->hasAllocationSite(allocation))
        return *it;
    }

    if (parentDependency)
      return parentDependency->getLatestAllocation(allocation);

    return 0;
  }

  Allocation *Dependency::resolveAllocation(VersionedValue *val) const {
    if (!val) return 0;
    for (std::vector< PointerEquality *>::const_reverse_iterator
	it = equalityList.rbegin(),
	itEnd = equalityList.rend(); it != itEnd; ++it) {
      Allocation *alloc = (*it)->equals(val);
      if (alloc)
        return alloc;
    }

    if (parentDependency)
      return parentDependency->resolveAllocation(val);

    return 0;
  }

  std::vector<Allocation *>
  Dependency::resolveAllocationTransitively(VersionedValue *value) const {
    std::vector<Allocation *> ret;
    Allocation *singleRet = resolveAllocation(value);
    if (singleRet) {
	ret.push_back(singleRet);
	return ret;
    }

    std::vector<VersionedValue *> valueSources = allFlowSourcesEnds(value);
    for (std::vector<VersionedValue *>::const_iterator it = valueSources.begin(),
	itEnd = valueSources.end(); it != itEnd; ++it) {
	singleRet = resolveAllocation(*it);
	if (singleRet) {
	    ret.push_back(singleRet);
	}
    }
    return ret;
  }

  void Dependency::addPointerEquality(const VersionedValue *value,
                                      Allocation *allocation) {
    equalityList.push_back(new PointerEquality(value, allocation));
  }

  void Dependency::updateStore(Allocation *allocation, VersionedValue *value) {
    storesList.push_back(new StorageCell(allocation, value));
  }

  void Dependency::addDependency(VersionedValue *source,
                                 VersionedValue *target) {
    flowsToList.push_back(new FlowsTo(source, target));
  }

  void Dependency::addDependencyViaAllocation(VersionedValue *source,
                                              VersionedValue *target,
                                              Allocation *via) {
    flowsToList.push_back(new FlowsTo(source, target, via));
  }

  std::vector<VersionedValue *>
  Dependency::stores(Allocation *allocation) const {
    std::vector<VersionedValue *> ret;

    if (allocation->isComposite()) {
      // In case of composite allocation, we return all possible stores
      // due to field-insensitivity of the dependency relation
      for (std::vector<StorageCell *>::const_iterator it = storesList.begin(),
                                                      itEnd = storesList.end();
           it != itEnd; ++it) {
        VersionedValue *value = (*it)->stores(allocation);
        if (value) {
          ret.push_back(value);
        }
      }

      if (parentDependency) {
        std::vector<VersionedValue *> parentStoredValues =
            parentDependency->stores(allocation);
        ret.insert(ret.begin(), parentStoredValues.begin(),
                   parentStoredValues.end());
      }
      return ret;
    }

    for (std::vector<StorageCell *>::const_iterator it = storesList.begin(),
                                                    itEnd = storesList.end();
         it != itEnd; ++it) {
        VersionedValue *value = (*it)->stores(allocation);
        if (value) {
          ret.push_back(value);
          return ret;
        }
      }
      if (parentDependency)
        return parentDependency->stores(allocation);
    return ret;
  }

  std::vector<VersionedValue *>
  Dependency::directLocalFlowSources(VersionedValue *target) const {
    std::vector<VersionedValue *> ret;
    for (std::vector<FlowsTo *>::const_iterator it = flowsToList.begin(),
                                          itEnd = flowsToList.end();
         it != itEnd; ++it) {
      if ((*it)->getTarget() == target) {
	ret.push_back((*it)->getSource());
      }
    }
    return ret;
  }

  std::vector<VersionedValue *>
  Dependency::directFlowSources(VersionedValue *target) const {
    std::vector<VersionedValue *> ret = directLocalFlowSources(target);
    if (parentDependency) {
	std::vector<VersionedValue *> ancestralSources =
	    parentDependency->directFlowSources(target);
	ret.insert(ret.begin(), ancestralSources.begin(),
	           ancestralSources.end());
    }
    return ret;
  }

  std::vector<VersionedValue *>
  Dependency::allFlowSources(VersionedValue *target) const {
    std::vector<VersionedValue *> stepSources = directFlowSources(target);
    std::vector<VersionedValue *> ret = stepSources;

    for (std::vector<VersionedValue *>::iterator it = stepSources.begin(),
                                                 itEnd = stepSources.end();
         it != itEnd; ++it) {
      std::vector<VersionedValue *> src = allFlowSources(*it);
      ret.insert(ret.begin(), src.begin(), src.end());
    }

    // We include the target as well
    ret.push_back(target);

    // Ensure there are no duplicates in the return
    std::sort(ret.begin(), ret.end());
    std::unique(ret.begin(), ret.end());
    return ret;
  }

  std::vector<VersionedValue *>
  Dependency::allFlowSourcesEnds(VersionedValue *target) const {
    std::vector<VersionedValue *> stepSources = directFlowSources(target);
    std::vector<VersionedValue *> ret;
    if (stepSources.size() == 0) {
      ret.push_back(target);
      return ret;
    }
    for (std::vector<VersionedValue *>::iterator it = stepSources.begin(),
                                                 itEnd = stepSources.end();
         it != itEnd; ++it) {
      std::vector<VersionedValue *> src = allFlowSourcesEnds(*it);
      if (src.size() == 0) {
        ret.push_back(*it);
      } else {
        ret.insert(ret.begin(), src.begin(), src.end());
      }
    }

    // Ensure there are no duplicates in the return
    std::sort(ret.begin(), ret.end());
    std::unique(ret.begin(), ret.end());
    return ret;
  }

  std::vector<VersionedValue *>
  Dependency::populateArgumentValuesList(llvm::CallInst *site,
                                         std::vector<ref<Expr> > &arguments) {
    unsigned numArgs = site->getCalledFunction()->arg_size();
    std::vector<VersionedValue *> argumentValuesList;
    for (unsigned i = numArgs; i > 0;) {
      llvm::Value *argOperand = site->getArgOperand(--i);
      VersionedValue *latestValue = getLatestValue(argOperand, arguments.at(i));

      if (latestValue)
        argumentValuesList.push_back(latestValue);
      else {
        // This is for the case when latestValue was NULL, which means there is
        // no source dependency information for this node, e.g., a constant.
        argumentValuesList.push_back(
            new VersionedValue(argOperand, arguments[i]));
      }
    }
    return argumentValuesList;
  }

  bool Dependency::buildLoadDependency(llvm::Value *fromValue,
                                       ref<Expr> fromValueExpr,
                                       llvm::Value *toValue,
                                       ref<Expr> toValueExpr) {
    VersionedValue *arg = getLatestValue(fromValue, fromValueExpr);
    if (!arg)
      return false;

    std::vector<Allocation *> allocList = resolveAllocationTransitively(arg);
    if (allocList.size() > 0) {
      for (std::vector<Allocation *>::iterator it0 = allocList.begin(),
                                               it0End = allocList.end();
           it0 != it0End; ++it0) {
        std::vector<VersionedValue *> valList = stores(*it0);
        if (valList.size() > 0) {
          for (std::vector<VersionedValue *>::iterator it1 = valList.begin(),
                                                       it1End = valList.end();
               it1 != it1End; ++it1) {
            std::vector<Allocation *> alloc2 =
                resolveAllocationTransitively(*it1);
            if (alloc2.size() > 0) {
              for (std::vector<Allocation *>::iterator it2 = alloc2.begin(),
                                                       it2End = alloc2.end();
                   it2 != it2End; ++it2) {
                addPointerEquality(getNewVersionedValue(toValue, toValueExpr),
                                   *it2);
              }
            } else {
              addDependencyViaAllocation(
                  *it1, getNewVersionedValue(toValue, toValueExpr), *it0);
            }
          }
        } else {
          // We could not find the stored value, create
          // a new one.
          updateStore(*it0, getNewVersionedValue(toValue, toValueExpr));
        }
      }
    } else {
	assert (!"operand is not an allocation");
    }

    return true;
  }

  Dependency::Dependency(Dependency *prev) : parentDependency(prev) {}

  Dependency::~Dependency() {
    // Delete the locally-constructed relations
    Util::deletePointerVector(equalityList);
    Util::deletePointerVector(storesList);
    Util::deletePointerVector(flowsToList);

    // Delete the locally-constructed objects
    Util::deletePointerVector(valuesList);
    Util::deletePointerVector(compositeAllocationsList);
    Util::deletePointerVector(versionedAllocationsList);
  }

  Dependency *Dependency::cdr() const { return parentDependency; }

  void Dependency::executeMemoryOperation(llvm::Instruction *i,
                                          ref<Expr> valueExpr,
                                          ref<Expr> address) {
    // The basic design principle that we need to be careful here
    // is that we should not store quadratic-sized structures in
    // the database of computed relations, e.g., not storing the
    // result of traversals of the graph. We keep the
    // quadratic blow up for only when querying the database.

    switch (i->getOpcode()) {
      case llvm::Instruction::Load: {
        if (Util::isEnvironmentAllocation(i)) {
          // The load corresponding to a load of the environment address
          // that was never allocated within this program.
          addPointerEquality(getNewVersionedValue(i, valueExpr),
                             getNewAllocationVersion(i));
          break;
        }

        if (!buildLoadDependency(i->getOperand(0), address, i, valueExpr)) {
          Allocation *alloc = getInitialAllocation(i->getOperand(0));
          updateStore(alloc, getNewVersionedValue(i, valueExpr));
        }
        break;
      }
      case llvm::Instruction::Store: {
        VersionedValue *dataArg = getLatestValue(i->getOperand(0), valueExpr);
        std::vector<Allocation *> addressList = resolveAllocationTransitively(
            getLatestValue(i->getOperand(1), address));

        // If there was no dependency found, we should create
        // a new value
        if (!dataArg)
          dataArg = getNewVersionedValue(i->getOperand(0), valueExpr);

        for (std::vector<Allocation *>::iterator it = addressList.begin(),
                                                 itEnd = addressList.end();
             it != itEnd; ++it) {
            Allocation *allocation = getLatestAllocation((*it)->getSite());
            if (!allocation || !allocation->isComposite()) {
              allocation = getInitialAllocation((*it)->getSite());
              VersionedValue *allocationValue =
                  getNewVersionedValue((*it)->getSite(), valueExpr);
              addPointerEquality(allocationValue, allocation);
            }
            updateStore(allocation, dataArg);
        }

        break;
      }
      default: {
        assert(0 && "should not execute instruction here");
        break;
      }
      }
  }

  void Dependency::executeBinary(llvm::Instruction *i, ref<Expr> result,
                                 ref<Expr> op1Expr, ref<Expr> op2Expr) {

    switch (i->getOpcode()) {
    case llvm::Instruction::Select: {

      VersionedValue *op1 = getLatestValue(i->getOperand(1), op1Expr);
      VersionedValue *op2 = getLatestValue(i->getOperand(2), op2Expr);
      VersionedValue *newValue = 0;
      if (op1) {
        newValue = getNewVersionedValue(i, result);
        addDependency(op1, newValue);
      }
      if (op2) {
        if (newValue)
          addDependency(op2, newValue);
        else
          addDependency(op2, getNewVersionedValue(i, result));
      }
      break;
    }

    case llvm::Instruction::Add:
    case llvm::Instruction::Sub:
    case llvm::Instruction::Mul:
    case llvm::Instruction::UDiv:
    case llvm::Instruction::SDiv:
    case llvm::Instruction::URem:
    case llvm::Instruction::SRem:
    case llvm::Instruction::And:
    case llvm::Instruction::Or:
    case llvm::Instruction::Xor:
    case llvm::Instruction::Shl:
    case llvm::Instruction::LShr:
    case llvm::Instruction::AShr:
    case llvm::Instruction::ICmp:
    case llvm::Instruction::FAdd:
    case llvm::Instruction::FSub:
    case llvm::Instruction::FMul:
    case llvm::Instruction::FDiv:
    case llvm::Instruction::FRem:
    case llvm::Instruction::FCmp:
    case llvm::Instruction::InsertValue: {
      VersionedValue *op1 = getLatestValue(i->getOperand(0), op1Expr);
      VersionedValue *op2 = getLatestValue(i->getOperand(1), op2Expr);

      VersionedValue *newValue = 0;
      if (op1) {
        newValue = getNewVersionedValue(i, result);
        addDependency(op1, newValue);
      }
      if (op2) {
        if (newValue)
          addDependency(op2, newValue);
        else
          addDependency(op2, getNewVersionedValue(i, result));
      }
      break;
    }
    default: {
      assert(0 && "should not execute instruction here");
      break;
    }
    }
  }

  void Dependency::execute(llvm::Instruction *i, ref<Expr> valueExpr) {
    // The basic design principle that we need to be careful here
    // is that we should not store quadratic-sized structures in
    // the database of computed relations, e.g., not storing the
    // result of traversals of the graph. We keep the
    // quadratic blow up for only when querying the database.

    switch (i->getOpcode()) {
    case llvm::Instruction::Alloca: {
      addPointerEquality(getNewVersionedValue(i, valueExpr),
                         getInitialAllocation(i));
      break;
    }
      case llvm::Instruction::GetElementPtr: {
	if (llvm::isa<llvm::Constant>(i->getOperand(0))) {
          Allocation *a = getLatestAllocation(i->getOperand(0));
          if (!a)
            a = getInitialAllocation(i->getOperand(0));

          // We simply propagate the pointer to the current
          // value field-insensitively.
          addPointerEquality(getNewVersionedValue(i, valueExpr), a);
          break;
        }

        VersionedValue *arg = getLatestValue(i->getOperand(0), valueExpr);
        assert(arg != 0 && "operand not found");

        std::vector<Allocation *> a = resolveAllocationTransitively(arg);

        if (a.size() > 0) {
          VersionedValue *newValue = getNewVersionedValue(i, valueExpr);
          for (std::vector<Allocation *>::iterator it = a.begin(),
                                                   itEnd = a.end();
               it != itEnd; ++it) {
            addPointerEquality(newValue, *it);
          }
        } else {
          // Could not resolve to argument to an address,
          // simply add flow dependency
          std::vector<VersionedValue *> vec = directFlowSources(arg);
          if (vec.size() > 0) {
            VersionedValue *newValue = getNewVersionedValue(i, valueExpr);
            for (std::vector<VersionedValue *>::iterator it = vec.begin(),
                                                         itEnd = vec.end();
                 it != itEnd; ++it) {
              addDependency((*it), newValue);
            }
          }
        }
        break;
      }
      case llvm::Instruction::Trunc:
      case llvm::Instruction::ZExt:
      case llvm::Instruction::SExt:
      case llvm::Instruction::IntToPtr:
      case llvm::Instruction::PtrToInt:
      case llvm::Instruction::BitCast:
      case llvm::Instruction::FPTrunc:
      case llvm::Instruction::FPExt:
      case llvm::Instruction::FPToUI:
      case llvm::Instruction::FPToSI:
      case llvm::Instruction::UIToFP:
      case llvm::Instruction::SIToFP:
      case llvm::Instruction::ExtractValue:
	{
        VersionedValue *val = getLatestValue(i->getOperand(0), valueExpr);

          if (val) {
            addDependency(val, getNewVersionedValue(i, valueExpr));
          } else if (!llvm::isa<llvm::Constant>(i->getOperand(0)))
              // Constants would kill dependencies, the remaining is for
              // cases that may actually require dependencies.
          {
            assert(!"operand not found");
          }
          break;
      }
      default: {
        assert(0 && "should not execute instruction here");
        break;
      }
    }

  }

  void Dependency::bindCallArguments(llvm::Instruction *instr,
                                     std::vector<ref<Expr> > &arguments) {
    llvm::CallInst *site = llvm::dyn_cast<llvm::CallInst>(instr);

    if (!site)
      return;

    llvm::Function *callee = site->getCalledFunction();

    // Sometimes the callee information is missing, in which case
    // the calle is not to be symbolically tracked.
    if (!callee)
      return;

    argumentValuesList = populateArgumentValuesList(site, arguments);
    unsigned index = 0;
    for (llvm::Function::ArgumentListType::iterator
             it = callee->getArgumentList().begin(),
             itEnd = callee->getArgumentList().end();
         it != itEnd; ++it) {
      if (argumentValuesList.back()) {
        addDependency(argumentValuesList.back(),
                      getNewVersionedValue(
                          it, argumentValuesList.back()->getExpression()));
      }
      argumentValuesList.pop_back();
      ++index;
    }
  }

  void Dependency::bindReturnValue(llvm::CallInst *site,
                                   llvm::Instruction *inst,
                                   ref<Expr> returnValue) {
    llvm::ReturnInst *retInst = llvm::dyn_cast<llvm::ReturnInst>(inst);
    if (site && retInst &&
        retInst->getReturnValue() // For functions returning void
        ) {
      VersionedValue *value =
          getLatestValue(retInst->getReturnValue(), returnValue);
      if (value)
        addDependency(value, getNewVersionedValue(site, returnValue));
    }
  }

  void Dependency::markAllValues(AllocationGraph *g, VersionedValue *value) {
    buildAllocationGraph(g, value);
    std::vector<VersionedValue *> allSources = allFlowSources(value);
    for (std::vector<VersionedValue *>::iterator it = allSources.begin(),
                                                 itEnd = allSources.end();
         it != itEnd; ++it) {
      (*it)->includeInInterpolant();
    }
  }

  void Dependency::markAllValues(AllocationGraph *g, llvm::Value *val) {
    VersionedValue *value = getLatestValueNoConstantCheck(val);

    buildAllocationGraph(g, value);
    std::vector<VersionedValue *> allSources = allFlowSources(value);
    for (std::vector<VersionedValue *>::iterator it = allSources.begin(),
                                                 itEnd = allSources.end();
         it != itEnd; ++it) {
      (*it)->includeInInterpolant();
    }
  }

  void Dependency::computeInterpolantAllocations(AllocationGraph *g) {
    interpolantAllocations = g->getSinkAllocations();

    if (parentDependency) {
      g->consumeNodesWithAllocations(versionedAllocationsList,
                                     compositeAllocationsList);
      parentDependency->computeInterpolantAllocations(g);
    }
  }

  std::map<VersionedValue *, Allocation *>
  Dependency::directLocalAllocationSources(VersionedValue *target) const {
    std::map<VersionedValue *, Allocation *> ret;

    for (std::vector<FlowsTo *>::const_iterator it = flowsToList.begin(),
                                                itEnd = flowsToList.end();
         it != itEnd; ++it) {
      if ((*it)->getTarget() == target) {
        std::map<VersionedValue *, Allocation *> extra;

        if (!(*it)->getAllocation()) {
          // Transitively get the source
          extra = directLocalAllocationSources((*it)->getSource());
          if (extra.size()) {
            ret.insert(extra.begin(), extra.end());
          } else {
            ret[(*it)->getSource()] = 0;
          }
        } else {
          ret[(*it)->getSource()] = (*it)->getAllocation();
        }
      }
    }

    if (ret.empty()) {
      // We try to find allocation in the local store instead
      for (std::vector<StorageCell *>::const_iterator it = storesList.begin(),
                                                      itEnd = storesList.end();
           it != itEnd; ++it) {
        if (Allocation *alloc = (*it)->storageOf(target)) {
          // It is possible that the first component was nil, as
          // in this case there was no source value
          ret[0] = alloc;
          break;
        }
      }
    }

    return ret;
  }

  std::map<VersionedValue *, Allocation *>
  Dependency::directAllocationSources(VersionedValue *target) const {
    std::map<VersionedValue *, Allocation *> ret =
        directLocalAllocationSources(target);

    if (ret.empty() && parentDependency)
      return parentDependency->directAllocationSources(target);

    std::map<VersionedValue *, Allocation *> tmp;
    std::map<VersionedValue *, Allocation *>::iterator nextPos = ret.begin(),
                                                       itEnd = ret.end();

    bool elementErased = true;
    while (elementErased) {
      elementErased = false;
      for (std::map<VersionedValue *, Allocation *>::iterator it = nextPos;
           it != itEnd; ++it) {
        if (!it->second) {
          std::map<VersionedValue *, Allocation *>::iterator deletionPos = it;

          // Here we check that it->first was non-nil, as it is possibly so.
          if (parentDependency && it->first) {
            std::map<VersionedValue *, Allocation *> ancestralSources =
                parentDependency->directAllocationSources(it->first);
            tmp.insert(ancestralSources.begin(), ancestralSources.end());
          }

          nextPos = ++it;
          ret.erase(deletionPos);
          elementErased = true;
          break;
        }
      }
    }

    ret.insert(tmp.begin(), tmp.end());
    return ret;
  }

  void Dependency::recursivelyBuildAllocationGraph(AllocationGraph *g,
                                                   VersionedValue *target,
                                                   Allocation *alloc) const {
    if (!target)
      return;

    std::vector<Allocation *> ret;
    std::map<VersionedValue *, Allocation *> sourceEdges =
        directAllocationSources(target);

    for (std::map<VersionedValue *, Allocation *>::iterator
             it0 = sourceEdges.begin(),
             it0End = sourceEdges.end();
         it0 != it0End; ++it0) {
      if (it0->second != alloc) {
        g->addNewEdge(it0->second, alloc);
        recursivelyBuildAllocationGraph(g, it0->first, it0->second);
      }
    }
  }

  void Dependency::buildAllocationGraph(AllocationGraph *g,
                                        VersionedValue *target) const {
    std::vector<Allocation *> ret;
    std::map<VersionedValue *, Allocation *> sourceEdges =
        directAllocationSources(target);

    for (std::map<VersionedValue *, Allocation *>::iterator
             it0 = sourceEdges.begin(),
             it0End = sourceEdges.end();
         it0 != it0End; ++it0) {
      g->addNewSink(it0->second);
      recursivelyBuildAllocationGraph(g, it0->first, it0->second);
    }
  }

  void Dependency::print(llvm::raw_ostream &stream) const {
    this->print(stream, 0);
  }

  void Dependency::print(llvm::raw_ostream &stream,
                         const unsigned tabNum) const {
    std::string tabs = makeTabs(tabNum);
    stream << tabs << "EQUALITIES:";
    std::vector<PointerEquality *>::const_iterator equalityListBegin =
        equalityList.begin();
    std::vector<StorageCell *>::const_iterator storesListBegin =
        storesList.begin();
    std::vector<FlowsTo *>::const_iterator flowsToListBegin =
        flowsToList.begin();
    for (std::vector<PointerEquality *>::const_iterator
             it = equalityListBegin,
             itEnd = equalityList.end();
         it != itEnd; ++it) {
      if (it != equalityListBegin)
        stream << ",";
      (*it)->print(stream);
    }
    stream << "\n";
    stream << tabs << "STORAGE:";
    for (std::vector<StorageCell *>::const_iterator it = storesList.begin(),
                                                    itEnd = storesList.end();
         it != itEnd; ++it) {
      if (it != storesListBegin)
        stream << ",";
      (*it)->print(stream);
    }
    stream << "\n";
    stream << tabs << "FLOWDEPENDENCY:";
    for (std::vector<FlowsTo *>::const_iterator it = flowsToList.begin(),
                                                itEnd = flowsToList.end();
         it != itEnd; ++it) {
      if (it != flowsToListBegin)
        stream << ",";
      (*it)->print(stream);
    }

    if (parentDependency) {
      stream << "\n" << tabs << "--------- Parent Dependencies ----------\n";
      parentDependency->print(stream, tabNum);
    }
  }

  /**/

  template <typename T>
  void Dependency::Util::deletePointerVector(std::vector<T *> &list) {
    typedef typename std::vector<T *>::iterator IteratorType;

    for (IteratorType it = list.begin(), itEnd = list.end(); it != itEnd;
         ++it) {
      delete *it;
    }
    list.clear();
  }

  bool Dependency::Util::isEnvironmentAllocation(llvm::Value *site) {
    llvm::LoadInst *inst = llvm::dyn_cast<llvm::LoadInst>(site);

    if (!inst)
      return false;

    llvm::Value *address = inst->getOperand(0);
    if (llvm::isa<llvm::Constant>(address) &&
	address->getName() == "__environ") {
	return true;
    }
    return false;
  }

  bool Dependency::Util::isCompositeAllocation(llvm::Value *site) {
    // We define composite allocation to be non-environment
    if (isEnvironmentAllocation(site))
      return false;

    llvm::AllocaInst *inst = llvm::dyn_cast<llvm::AllocaInst>(site);

    if (inst != 0)
      return llvm::isa<llvm::CompositeType>(inst->getAllocatedType());

    switch (site->getType()->getTypeID()) {
    case llvm::Type::ArrayTyID:
    case llvm::Type::PointerTyID:
    case llvm::Type::StructTyID:
    case llvm::Type::VectorTyID: { return true; }
    default:
      break;
    }

    return false;
  }

  /**/

  std::string makeTabs(const unsigned tabNum) {
    std::string tabsString;
    for (unsigned i = 0; i < tabNum; i++) {
      tabsString += appendTab(tabsString);
    }
    return tabsString;
  }

  std::string appendTab(const std::string &prefix) {
    return prefix + "        ";
  }
}