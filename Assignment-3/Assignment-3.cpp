#include "Util/WorkList.h"
#include "SVFIR/SVFIR.h"
#include "Assignment-3.h"
#include "Util/Options.h"

using namespace SVF;
using namespace SVFUtil;

/// handle wto cycle (loop)
void AbsExe::handleWTONode(const ICFGNode* node) {

    if (!propagateStateIfFeasible(node))
    {
        // No AS on the in edges - Infeasible block
        return;
    }
    else
    {
        // Has AS on the in edges - Feasible block
        // Get execution state from in edges
        _postAbsTrace[node] = _preAbsTrace[node];
    }

    std::deque<const ICFGNode*> worklist;

    // handle SVF Stmt
    for (const SVFStmt *stmt: node->getSVFStmts())
    {
        updateAbsState(stmt);
        bufOverflowDetection(stmt);
    }
    // inlining the callee by calling handleFunc for the callee function
    if (const CallICFGNode* callnode = SVFUtil::dyn_cast<CallICFGNode>(node))
    {
        handleCallSite(callnode);
    }
    else
    {
    }
}

/// Handle each type of SVFStmt for state updates
void AbsExe::updateAbsState(const SVFStmt* stmt) {
    if (const AddrStmt *addr = SVFUtil::dyn_cast<AddrStmt>(stmt))
    {
        updateStateOnAddr(addr);
    }
    else if (const BinaryOPStmt *binary = SVFUtil::dyn_cast<BinaryOPStmt>(stmt))
    {
        updateStateOnBinary(binary);
    }
    else if (const CmpStmt *cmp = SVFUtil::dyn_cast<CmpStmt>(stmt))
    {
        updateStateOnCmp(cmp);
    }
    else if (const LoadStmt *load = SVFUtil::dyn_cast<LoadStmt>(stmt))
    {
        updateStateOnLoad(load);
    }
    else if (const StoreStmt *store = SVFUtil::dyn_cast<StoreStmt>(stmt))
    {
        updateStateOnStore(store);
    }
    else if (const CopyStmt *copy = SVFUtil::dyn_cast<CopyStmt>(stmt))
    {
        updateStateOnCopy(copy);
    }
    else if (const GepStmt *gep = SVFUtil::dyn_cast<GepStmt>(stmt))
    {
        updateStateOnGep(gep);
    }
    else if (const PhiStmt *phi = SVFUtil::dyn_cast<PhiStmt>(stmt))
    {
        updateStateOnPhi(phi);
    }
    else if (const CallPE *callPE = SVFUtil::dyn_cast<CallPE>(stmt))
    {
        updateStateOnCall(callPE);
    }
    else if (const RetPE *retPE = SVFUtil::dyn_cast<RetPE>(stmt))
    {
        updateStateOnRet(retPE);
    }
    else if (const SelectStmt *select = SVFUtil::dyn_cast<SelectStmt>(stmt))
    {
        updateStateOnSelect(select);
    }
    else if (SVFUtil::isa<UnaryOPStmt>(stmt) || SVFUtil::isa<BranchStmt>(stmt))
    {
        // nothing needs to be done here as BranchStmt is handled in hasBranchES
    }
    else
        assert(false && "implement this part");

}

/// TODO: handle object allocation and record its size
/// TODO: handle GepStmt and detect buffer overflow
void AbsExe::bufOverflowDetection(const SVF::SVFStmt *stmt) {
    if (!SVFUtil::isa<CallICFGNode>(stmt->getICFGNode())) {
        if (const AddrStmt *addr = SVFUtil::dyn_cast<AddrStmt>(stmt)) {
            /// TODO: your code starts from here

        } else if (const GepStmt *gep = SVFUtil::dyn_cast<GepStmt>(stmt)) {
            /// TODO: your code starts from here

        } else {
            // nothing to do here
        }
    }
}


/// TODO : Implement the handleCycle function
void AbsExe::handleCycle(const ICFGWTOCycle *cycle)
{
    // Get execution states from in edges
    if (!propagateStateIfFeasible(cycle->head()))
    {
        // No ES on the in edges - Infeasible block
        return;
    }
    AbstractState pre_as = _preAbsTrace[cycle->head()];
    // set -widen-delay
    s32_t widen_delay = Options::WidenDelay();
    bool incresing = true;

    /// TODO: your code starts from here
}


/// TODO : Implement the state updates for Copy, Store, Load and Gep
void AbsExe::updateStateOnCopy(const CopyStmt* copy) {
    /// TODO: your code starts from here
}

void AbsExe::updateStateOnStore(const StoreStmt* store)
{
    /// TODO: your code starts from here
}

void AbsExe::updateStateOnLoad(const LoadStmt* load)
{
    /// TODO: your code starts from here
}

void AbsExe::updateStateOnGep(const GepStmt* gep) {
    /// TODO: your code starts from here
}

/// Abstract state updates on an AddrStmt
void AbsExe::updateStateOnAddr(const AddrStmt* addr) {
    const ICFGNode* node = addr->getICFGNode();
    AbstractState& as = getAbsState(node);
    initSVFVar(as, addr->getRHSVarID());
    if (as.inVarToValTable(addr->getRHSVarID()))
    {
        as[addr->getLHSVarID()] = as[addr->getRHSVarID()];
    }
    else if (as.inVarToAddrsTable(addr->getRHSVarID()))
    {
        as[addr->getLHSVarID()] = as[addr->getRHSVarID()];
    }
    else
    {
        assert(false && "not number or virtual addrs?");
    }
}


/// Abstract state updates on an CmpStmt
void AbsExe::updateStateOnCmp(const CmpStmt* cmp)
{
    const ICFGNode* node = cmp->getICFGNode();
    AbstractState& as = getAbsState(node);
    u32_t op0 = cmp->getOpVarID(0);
    u32_t op1 = cmp->getOpVarID(1);
    u32_t res = cmp->getResID();
    if (as.inVarToValTable(op0) && as.inVarToValTable(op1))
    {
        AbstractValue resVal;
        AbstractValue &lhs = as[op0], &rhs = as[op1];
        //AbstractValue
        auto predicate = cmp->getPredicate();
        switch (predicate)
        {
            case CmpStmt::ICMP_EQ:
            case CmpStmt::FCMP_OEQ:
            case CmpStmt::FCMP_UEQ:
                resVal = (lhs == rhs);
                // resVal = (lhs.getInterval() == rhs.getInterval());
                break;
            case CmpStmt::ICMP_NE:
            case CmpStmt::FCMP_ONE:
            case CmpStmt::FCMP_UNE:
                resVal = (lhs != rhs);
                break;
            case CmpStmt::ICMP_UGT:
            case CmpStmt::ICMP_SGT:
            case CmpStmt::FCMP_OGT:
            case CmpStmt::FCMP_UGT:
                resVal = (lhs > rhs);
                break;
            case CmpStmt::ICMP_UGE:
            case CmpStmt::ICMP_SGE:
            case CmpStmt::FCMP_OGE:
            case CmpStmt::FCMP_UGE:
                resVal = (lhs >= rhs);
                break;
            case CmpStmt::ICMP_ULT:
            case CmpStmt::ICMP_SLT:
            case CmpStmt::FCMP_OLT:
            case CmpStmt::FCMP_ULT:
                resVal = (lhs < rhs);
                break;
            case CmpStmt::ICMP_ULE:
            case CmpStmt::ICMP_SLE:
            case CmpStmt::FCMP_OLE:
            case CmpStmt::FCMP_ULE:
                resVal = (lhs <= rhs);
                break;
            case CmpStmt::FCMP_FALSE:
                resVal = IntervalValue(0, 0);
                break;
            case CmpStmt::FCMP_TRUE:
                resVal = IntervalValue(1, 1);
                break;
            default:
            {
                assert(false && "undefined compare: ");
            }
        }
        as[res] = resVal;
    }
    else if (as.inVarToAddrsTable(op0) && as.inVarToAddrsTable(op1))
    {
        IntervalValue resVal;
        AbstractValue &lhs = as.getAddrs(op0), &rhs = as.getAddrs(op1);
        assert(!lhs.getAddrs().empty() && !rhs.getAddrs().empty() && "empty address?");
        auto predicate = cmp->getPredicate();
        switch (predicate)
        {
            case CmpStmt::ICMP_EQ:
            case CmpStmt::FCMP_OEQ:
            case CmpStmt::FCMP_UEQ:
            {
                if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1)
                {
                    resVal = IntervalValue(lhs.equals(rhs));
                }
                else
                {
                    if (lhs.getAddrs().hasIntersect(rhs.getAddrs()))
                    {
                        resVal = IntervalValue::top();
                    }
                    else
                    {
                        resVal = IntervalValue(0);
                    }
                }
                break;
            }
            case CmpStmt::ICMP_NE:
            case CmpStmt::FCMP_ONE:
            case CmpStmt::FCMP_UNE:
            {
                if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1)
                {
                    resVal = IntervalValue(!lhs.equals(rhs));
                }
                else
                {
                    if (lhs.getAddrs().hasIntersect(rhs.getAddrs()))
                    {
                        resVal = IntervalValue::top();
                    }
                    else
                    {
                        resVal = IntervalValue(1);
                    }
                }
                break;
            }
            case CmpStmt::ICMP_UGT:
            case CmpStmt::ICMP_SGT:
            case CmpStmt::FCMP_OGT:
            case CmpStmt::FCMP_UGT:
            {
                if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1)
                {
                    resVal = IntervalValue(*lhs.getAddrs().begin() > *rhs.getAddrs().begin());
                }
                else
                {
                    resVal = IntervalValue::top();
                }
                break;
            }
            case CmpStmt::ICMP_UGE:
            case CmpStmt::ICMP_SGE:
            case CmpStmt::FCMP_OGE:
            case CmpStmt::FCMP_UGE:
            {
                if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1)
                {
                    resVal = IntervalValue(*lhs.getAddrs().begin() >= *rhs.getAddrs().begin());
                }
                else
                {
                    resVal = IntervalValue::top();
                }
                break;
            }
            case CmpStmt::ICMP_ULT:
            case CmpStmt::ICMP_SLT:
            case CmpStmt::FCMP_OLT:
            case CmpStmt::FCMP_ULT:
            {
                if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1)
                {
                    resVal = IntervalValue(*lhs.getAddrs().begin() < *rhs.getAddrs().begin());
                }
                else
                {
                    resVal = IntervalValue::top();
                }
                break;
            }
            case CmpStmt::ICMP_ULE:
            case CmpStmt::ICMP_SLE:
            case CmpStmt::FCMP_OLE:
            case CmpStmt::FCMP_ULE:
            {
                if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1)
                {
                    resVal = IntervalValue(*lhs.getAddrs().begin() <= *rhs.getAddrs().begin());
                }
                else
                {
                    resVal = IntervalValue::top();
                }
                break;
            }
            case CmpStmt::FCMP_FALSE:
                resVal = IntervalValue(0, 0);
                break;
            case CmpStmt::FCMP_TRUE:
                resVal = IntervalValue(1, 1);
                break;
            default:
            {
                assert(false && "undefined compare: ");
            }
        }
        as[res] = resVal;
    }
}

/// Abstract state updates on an CallPE
void AbsExe::updateStateOnCall(const CallPE* call) {
    const ICFGNode* node = call->getICFGNode();
    AbstractState& as = getAbsState(node);
    NodeID lhs = call->getLHSVarID();
    NodeID rhs = call->getRHSVarID();
    if (as.inVarToValTable(rhs) || as.inVarToAddrsTable(rhs))
    {
        as[lhs] = as[rhs];
    }
}

/// Abstract state updates on an RetPE
void AbsExe::updateStateOnRet(const RetPE* retPE) {
    const ICFGNode* node = retPE->getICFGNode();
    AbstractState& as = getAbsState(node);
    NodeID lhs = retPE->getLHSVarID();
    NodeID rhs = retPE->getRHSVarID();
    if (as.inVarToValTable(rhs) || as.inVarToAddrsTable(rhs))
    {
        as[lhs] = as[rhs];
    }
}

/// Abstract state updates on an PhiStmt
void AbsExe::updateStateOnPhi(const PhiStmt* phi) {
    const ICFGNode* node = phi->getICFGNode();
    AbstractState& as = getAbsState(node);
    u32_t res = phi->getResID();
    AbstractValue rhs(AbstractValue::UnknownType);
    for (u32_t i = 0; i < phi->getOpVarNum(); i++)
    {
        NodeID curId = phi->getOpVarID(i);

        if (as.inVarToValTable(curId) || as.inVarToAddrsTable(curId))
        {
            rhs.join_with(as[curId]);
        }
    }
    if (!rhs.isUnknown())
        as[res] = rhs;
}

/// Abstract state updates on an BinaryOPStmt
void AbsExe::updateStateOnBinary(const BinaryOPStmt* binary) {
    const ICFGNode *node = binary->getICFGNode();
    AbstractState &as = getAbsState(node);
    u32_t op0 = binary->getOpVarID(0);
    u32_t op1 = binary->getOpVarID(1);
    u32_t res = binary->getResID();
    if (!as.inVarToValTable(op0)) as[op0] = IntervalValue::top();
    if (!as.inVarToValTable(op1)) as[op1] = IntervalValue::top();
    if (as.inVarToValTable(op0) && as.inVarToValTable(op1)) {
        AbstractValue &lhs = as[op0], &rhs = as[op1];
        AbstractValue resVal;
        switch (binary->getOpcode()) {
            case BinaryOPStmt::Add:
            case BinaryOPStmt::FAdd:
                resVal = (lhs + rhs);
                break;
            case BinaryOPStmt::Sub:
            case BinaryOPStmt::FSub:
                resVal = (lhs - rhs);
                break;
            case BinaryOPStmt::Mul:
            case BinaryOPStmt::FMul:
                resVal = (lhs * rhs);
                break;
            case BinaryOPStmt::SDiv:
            case BinaryOPStmt::FDiv:
            case BinaryOPStmt::UDiv:
                resVal = (lhs / rhs);
                break;
            case BinaryOPStmt::SRem:
            case BinaryOPStmt::FRem:
            case BinaryOPStmt::URem:
                resVal = (lhs % rhs);
                break;
            case BinaryOPStmt::Xor:
                resVal = (lhs ^ rhs);
                break;
            case BinaryOPStmt::And:
                resVal = (lhs & rhs);
                break;
            case BinaryOPStmt::Or:
                resVal = (lhs | rhs);
                break;
            case BinaryOPStmt::AShr:
                resVal = (lhs >> rhs);
                break;
            case BinaryOPStmt::Shl:
                resVal = (lhs << rhs);
                break;
            case BinaryOPStmt::LShr:
                resVal = (lhs >> rhs);
                break;
            default: {
                assert(false && "undefined binary: ");
            }
        }
        as[res] = resVal;
    }
}

/// Abstract state updates on an SelectStmt
void AbsExe::updateStateOnSelect(const SelectStmt *select) {
    const ICFGNode *node = select->getICFGNode();
    AbstractState &as = getAbsState(node);
    u32_t res = select->getResID();
    u32_t tval = select->getTrueValue()->getId();
    u32_t fval = select->getFalseValue()->getId();
    u32_t cond = select->getCondition()->getId();
    if (as.inVarToValTable(tval) && as.inVarToValTable(fval) && as.inVarToValTable(cond))
    {
        if (as[cond].getInterval().is_numeral())
        {
            as[res] = as[cond].getInterval().is_zero() ? as[fval] : as[tval];
        }
        else
        {
            as[res] = as[cond];
        }
    }
    else if (as.inVarToAddrsTable(tval) && as.inVarToAddrsTable(fval) && as.inVarToValTable(cond))
    {
        if (as[cond].getInterval().is_numeral())
        {
            as.getAddrs(res) = as[cond].getInterval().is_zero() ? as.getAddrs(fval) : as.getAddrs(tval);
        }
    }
}

void AbsExe::analyse()
{
    // handle Global ICFGNode of SVFModule
    handleGlobalNode();
    getAbsState(_icfg->getGlobalICFGNode())[PAG::getPAG()->getBlkPtr()] = IntervalValue::top();
    if (const SVFFunction* fun = _svfir->getModule()->getSVFFunction("main"))
    {
        ICFGWTO* wto = _funcToWTO[fun];
        // set function entry ES
        for (auto it = wto->begin(); it!= wto->end(); ++it)
        {
            const ICFGWTOComp* cur = *it;
            if (const ICFGWTONode* vertex = SVFUtil::dyn_cast<ICFGWTONode>(cur))
            {
                handleWTONode(vertex->node());
            }
            else if (const ICFGWTOCycle* cycle = SVFUtil::dyn_cast<ICFGWTOCycle>(cur))
            {
                handleCycle(cycle);
            }
            else
            {
                assert(false && "unknown WTO type!");
            }
        }
    }
}
