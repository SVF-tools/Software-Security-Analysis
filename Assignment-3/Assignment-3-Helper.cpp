#include "Assignment-3.h"
#include "WPA/Andersen.h"

using namespace SVF;

// according to varieties of cmp insts,
// maybe var X var, var X const, const X var, const X const
// we accept 'var X const' 'var X var' 'const X const'
// if 'const X var', we need to reverse op0 op1 and its predicate 'var X' const'
// X' is reverse predicate of X
// == -> !=, != -> ==, > -> <=, >= -> <, < -> >=, <= -> >
Map<s32_t, s32_t> _reverse_predicate = {
    {CmpStmt::Predicate::FCMP_OEQ, CmpStmt::Predicate::FCMP_ONE}, // == -> !=
    {CmpStmt::Predicate::FCMP_UEQ, CmpStmt::Predicate::FCMP_UNE}, // == -> !=
    {CmpStmt::Predicate::FCMP_OGT, CmpStmt::Predicate::FCMP_OLE}, // > -> <=
    {CmpStmt::Predicate::FCMP_OGE, CmpStmt::Predicate::FCMP_OLT}, // >= -> <
    {CmpStmt::Predicate::FCMP_OLT, CmpStmt::Predicate::FCMP_OGE}, // < -> >=
    {CmpStmt::Predicate::FCMP_OLE, CmpStmt::Predicate::FCMP_OGT}, // <= -> >
    {CmpStmt::Predicate::FCMP_ONE, CmpStmt::Predicate::FCMP_OEQ}, // != -> ==
    {CmpStmt::Predicate::FCMP_UNE, CmpStmt::Predicate::FCMP_UEQ}, // != -> ==
    {CmpStmt::Predicate::ICMP_EQ, CmpStmt::Predicate::ICMP_NE}, // == -> !=
    {CmpStmt::Predicate::ICMP_NE, CmpStmt::Predicate::ICMP_EQ}, // != -> ==
    {CmpStmt::Predicate::ICMP_UGT, CmpStmt::Predicate::ICMP_ULE}, // > -> <=
    {CmpStmt::Predicate::ICMP_ULT, CmpStmt::Predicate::ICMP_UGE}, // < -> >=
    {CmpStmt::Predicate::ICMP_UGE, CmpStmt::Predicate::ICMP_ULT}, // >= -> <
    {CmpStmt::Predicate::ICMP_SGT, CmpStmt::Predicate::ICMP_SLE}, // > -> <=
    {CmpStmt::Predicate::ICMP_SLT, CmpStmt::Predicate::ICMP_SGE}, // < -> >=
    {CmpStmt::Predicate::ICMP_SGE, CmpStmt::Predicate::ICMP_SLT}, // >= -> <
};

Map<s32_t, s32_t> _switch_lhsrhs_predicate = {
    {CmpStmt::Predicate::FCMP_OEQ, CmpStmt::Predicate::FCMP_OEQ}, // == -> ==
    {CmpStmt::Predicate::FCMP_UEQ, CmpStmt::Predicate::FCMP_UEQ}, // == -> ==
    {CmpStmt::Predicate::FCMP_OGT, CmpStmt::Predicate::FCMP_OLT}, // > -> <
    {CmpStmt::Predicate::FCMP_OGE, CmpStmt::Predicate::FCMP_OLE}, // >= -> <=
    {CmpStmt::Predicate::FCMP_OLT, CmpStmt::Predicate::FCMP_OGT}, // < -> >
    {CmpStmt::Predicate::FCMP_OLE, CmpStmt::Predicate::FCMP_OGE}, // <= -> >=
    {CmpStmt::Predicate::FCMP_ONE, CmpStmt::Predicate::FCMP_ONE}, // != -> !=
    {CmpStmt::Predicate::FCMP_UNE, CmpStmt::Predicate::FCMP_UNE}, // != -> !=
    {CmpStmt::Predicate::ICMP_EQ, CmpStmt::Predicate::ICMP_EQ}, // == -> ==
    {CmpStmt::Predicate::ICMP_NE, CmpStmt::Predicate::ICMP_NE}, // != -> !=
    {CmpStmt::Predicate::ICMP_UGT, CmpStmt::Predicate::ICMP_ULT}, // > -> <
    {CmpStmt::Predicate::ICMP_ULT, CmpStmt::Predicate::ICMP_UGT}, // < -> >
    {CmpStmt::Predicate::ICMP_UGE, CmpStmt::Predicate::ICMP_ULE}, // >= -> <=
    {CmpStmt::Predicate::ICMP_SGT, CmpStmt::Predicate::ICMP_SLT}, // > -> <
    {CmpStmt::Predicate::ICMP_SLT, CmpStmt::Predicate::ICMP_SGT}, // < -> >
    {CmpStmt::Predicate::ICMP_SGE, CmpStmt::Predicate::ICMP_SLE}, // >= -> <=
};


IntervalValue AbstractExecution::getAccessOffset(NodeID objId, const GepStmt* gep) {
	auto obj = svfir->getGNode(objId);
	AbstractState& as = getAbsStateFromTrace(gep->getICFGNode());
	// Field-insensitive base object
	if (SVFUtil::isa<BaseObjVar>(obj)) {
		// get base size
		IntervalValue accessOffset = as.getByteOffset(gep);
		return accessOffset;
	}
	// A sub object of an aggregate object
	else if (SVFUtil::isa<GepObjVar>(obj)) {
		IntervalValue accessOffset =
		    bufOverflowHelper.getGepObjOffsetFromBase(SVFUtil::cast<GepObjVar>(obj)) + as.getByteOffset(gep);
		return accessOffset;
	}
	else{
		assert(SVFUtil::isa<DummyObjVar>(obj) && "What other types of object?");
		return IntervalValue::top();
	}
}

/// Report a buffer overflow for a given ICFG node
void AbstractExecution::reportBufOverflow(const ICFGNode* node) {
	// Create an exception with the node's string representation
	AEException bug(node->toString());
	// Add the bug to the reporter using the helper
	bufOverflowHelper.addBugToReporter(bug, node);
}

bool AbstractExecution::isExternalCallForAssignment(const SVF::FunObjVar* func) {
	Set<std::string> extFuncs = {"mem_insert", "str_insert"};
	if (extFuncs.find(func->getName()) != extFuncs.end()) {
		return true;
	} else {
		return false;
	}
}

void AbstractExecution::runOnModule(SVF::ICFG* _icfg) {
	svfir = PAG::getPAG();
	icfg = _icfg;
	analyse();
	bufOverflowHelper.printReport();
}

/**
 * @brief Mark recursive functions in the call graph
 *
 * This function identifies and marks recursive functions in the call graph.
 * It does this by detecting cycles in the call graph's strongly connected components (SCC).
 * Any function found to be part of a cycle is marked as recursive.
 */
void AbstractExecution::initWTO() {
	AndersenWaveDiff* ander = AndersenWaveDiff::createAndersenWaveDiff(svfir);
	// Detect if the call graph has cycles by finding its strongly connected components (SCC)
	Andersen::CallGraphSCC* callGraphScc = ander->getCallGraphSCC();
	callGraphScc->find();
	auto callGraph = ander->getCallGraph();

	// Iterate through the call graph
	for (auto it = callGraph->begin(); it != callGraph->end(); it++) {
		// Check if the current function is part of a cycle
		if (callGraphScc->isInCycle(it->second->getId()))
			recursiveFuns.insert(it->second->getFunction()); // Mark the function as recursive
	}

	// Initialize WTO for each function in the module
	for (const auto& item : *callGraph) {
		const FunObjVar* fun = item.second->getFunction();
		if(fun->isDeclaration())
			continue;
		auto* wto = new ICFGWTO(icfg, icfg->getFunEntryICFGNode(fun));
		wto->init();
		funcToWTO[fun] = wto;
	}
}

/**
 * @brief Update the offset of a GEP (GetElementPtr) object from its base address
 *
 * This function updates the offset of a GEP object from its base address in the abstract state.
 * It handles both field-insensitive base objects and sub-objects of aggregate objects.
 * The function calculates the new offset based on the provided GEP addresses and the offset interval.
 *
 * @param gepAddrs The set of addresses for the GEP object
 * @param objAddrs The set of addresses for the object
 * @param offset The interval value representing the offset
 */
void AbstractExecution::updateGepObjOffsetFromBase(SVF::AddressValue gepAddrs, SVF::AddressValue objAddrs,
                                                   SVF::IntervalValue offset)
{
	for (const auto& objAddr : objAddrs) {
		NodeID objId = AbstractState::getInternalID(objAddr);
		auto obj = svfir->getGNode(objId);
		if (SVFUtil::isa<BaseObjVar>(obj)) {
			for (const auto& gepAddr : gepAddrs) {
				NodeID gepObj = AbstractState::getInternalID(gepAddr);
				const GepObjVar* gepObjVar = SVFUtil::cast<GepObjVar>(svfir->getGNode(gepObj));
				bufOverflowHelper.addToGepObjOffsetFromBase(gepObjVar, offset);
			}
		}
		else if (SVFUtil::isa<GepObjVar>(obj)) {
			const GepObjVar* objVar = SVFUtil::cast<GepObjVar>(obj);
			for (const auto& gepAddr : gepAddrs) {
				NodeID gepObj = AbstractState::getInternalID(gepAddr);
				const GepObjVar* gepObjVar = SVFUtil::cast<GepObjVar>(svfir->getGNode(gepObj));
				if (bufOverflowHelper.hasGepObjOffsetFromBase(objVar)) {
					IntervalValue objOffsetFromBase = bufOverflowHelper.getGepObjOffsetFromBase(objVar);
					/// make sure gepObjVar has not been written before
					if (!bufOverflowHelper.hasGepObjOffsetFromBase(gepObjVar))
						bufOverflowHelper.addToGepObjOffsetFromBase(gepObjVar, objOffsetFromBase + offset);
				}
				else {
					assert(false && "gepRhsObjVar has no gepObjOffsetFromBase");
				}
			}
		}
	}
}

/**
 * @brief  Propagate the states from predecessors to the current node and return true if the control-flow is feasible
 *
 * This function attempts to propagate the execution state to a given block by merging the states
 * of its predecessor blocks. It handles two scenarios: intra-block edges and call edges.
 *  Scenario 1: preblock -----(intraEdge)----> block, join the preES of inEdges
 *  Scenario 2: preblock -----(callEdge)----> block
 * If the propagation is feasible, it updates the execution state and returns true. Otherwise, it returns false.
 *
 * @param block The ICFG node (block) for which the state propagation is attempted
 * @return bool True if the state propagation is feasible and successful, false otherwise
 */
bool AbstractExecution::mergeStatesFromPredecessors(const ICFGNode* block, AbstractState& as) {
	u32_t inEdgeNum = 0; // Initialize the number of incoming edges with feasible states
	as = AbstractState();
	// Iterate over all incoming edges of the given block
	for (auto& edge : block->getInEdges()) {
		// Check if the source node of the edge has a post-execution state recorded
		if (postAbsTrace.find(edge->getSrcNode()) != postAbsTrace.end()) {
			const IntraCFGEdge* intraCfgEdge = SVFUtil::dyn_cast<IntraCFGEdge>(edge);

			// If the edge is an intra-block edge and has a condition
			if (intraCfgEdge && intraCfgEdge->getCondition()) {
				AbstractState tmpEs = postAbsTrace[edge->getSrcNode()];
				// Check if the branch condition is feasible
				if (isBranchFeasible(intraCfgEdge, tmpEs)) {
					as.joinWith(tmpEs); // Merge the state with the current state
					inEdgeNum++;
				}
				// If branch is not feasible, do nothing
			}
			else {
				// For non-conditional edges, directly merge the state
				as.joinWith(postAbsTrace[edge->getSrcNode()]);
				inEdgeNum++;
			}
		}
		// If no post-execution state is recorded for the source node, do nothing
	}

	// If no incoming edges have feasible states, return false
	if (inEdgeNum == 0) {
		return false;
	}
	else {
		return true;
	}
	assert(false && "implement this part"); // This part should not be reached
}

bool AbstractExecution::isCmpBranchFeasible(const CmpStmt* cmpStmt, s64_t succ, AbstractState& as) {
	AbstractState new_es = as;
	// get cmp stmt's op0, op1, and predicate
	NodeID op0 = cmpStmt->getOpVarID(0);
	NodeID op1 = cmpStmt->getOpVarID(1);
	NodeID res_id = cmpStmt->getResID();
	s32_t predicate = cmpStmt->getPredicate();

	// if op0 or op1 is undefined, return;
	// skip address compare
	if (new_es.inVarToAddrsTable(op0) || new_es.inVarToAddrsTable(op1)) {
		as = new_es;
		return true;
	}
	// get '%1 = load i32 s', and load inst may not exist
	auto getLoadOp = [](SVFVar* opVar) -> const LoadStmt* {
		if (!opVar->getInEdges().empty()) {
			SVFStmt* loadVar0InStmt = *opVar->getInEdges().begin();
			if (const LoadStmt* loadStmt = SVFUtil::dyn_cast<LoadStmt>(loadVar0InStmt)) {
				return loadStmt;
			}
			else if (const CopyStmt* copyStmt = SVFUtil::dyn_cast<CopyStmt>(loadVar0InStmt)) {
				if (!copyStmt->getRHSVar()->getInEdges().empty()) {
					SVFStmt* loadVar0InStmt2 = *opVar->getInEdges().begin();
					if (const LoadStmt* loadStmt = SVFUtil::dyn_cast<LoadStmt>(loadVar0InStmt2)) {
						return loadStmt;
					}
				}
			}
		}
		return nullptr;
	};
	const LoadStmt* load_op0 = getLoadOp(svfir->getGNode(op0));
	const LoadStmt* load_op1 = getLoadOp(svfir->getGNode(op1));

	// for const X const, we may get concrete resVal instantly
	// for var X const, we may get [0,1] if the intersection of var and const is not empty set
	IntervalValue resVal = new_es[res_id].getInterval();
	resVal.meet_with(IntervalValue((s64_t)succ, succ));
	// If Var X const generates bottom value, it means this branch path is not feasible.
	if (resVal.isBottom()) {
		return false;
	}

	bool b0 = new_es[op0].getInterval().is_numeral();
	bool b1 = new_es[op1].getInterval().is_numeral();

	// if const X var, we should reverse op0 and op1.
	if (b0 && !b1) {
		std::swap(op0, op1);
		std::swap(load_op0, load_op1);
		predicate = _switch_lhsrhs_predicate[predicate];
	}
	else {
		// if var X var, we cannot preset the branch condition to infer the intervals of var0,var1
		if (!b0 && !b1) {
			as = new_es;
			return true;
		}
		// if const X const, we can instantly get the resVal
		else if (b0 && b1) {
			as = new_es;
			return true;
		}
	}
	// if cmp is 'var X const == false', we should reverse predicate 'var X' const == true'
	// X' is reverse predicate of X
	if (succ == 0) {
		predicate = _reverse_predicate[predicate];
	}
	else {
	}
	// change interval range according to the compare predicate
	AddressValue addrs;
	if (load_op0 && new_es.inVarToAddrsTable(load_op0->getRHSVarID()))
		addrs = new_es[load_op0->getRHSVarID()].getAddrs();

	IntervalValue &lhs = new_es[op0].getInterval(), &rhs = new_es[op1].getInterval();
	switch (predicate) {
	case CmpStmt::Predicate::ICMP_EQ:
	case CmpStmt::Predicate::FCMP_OEQ:
	case CmpStmt::Predicate::FCMP_UEQ: {
		// Var == Const, so [var.lb, var.ub].meet_with(const)
		lhs.meet_with(rhs);
		break;
	}
	case CmpStmt::Predicate::ICMP_NE:
	case CmpStmt::Predicate::FCMP_ONE:
	case CmpStmt::Predicate::FCMP_UNE:
		// Compliment set
		break;
	case CmpStmt::Predicate::ICMP_UGT:
	case CmpStmt::Predicate::ICMP_SGT:
	case CmpStmt::Predicate::FCMP_OGT:
	case CmpStmt::Predicate::FCMP_UGT:
		// Var > Const, so [var.lb, var.ub].meet_with([Const+1, +INF])
		lhs.meet_with(IntervalValue(rhs.lb() + 1, IntervalValue::plus_infinity()));
		break;
	case CmpStmt::Predicate::ICMP_UGE:
	case CmpStmt::Predicate::ICMP_SGE:
	case CmpStmt::Predicate::FCMP_OGE:
	case CmpStmt::Predicate::FCMP_UGE: {
		// Var >= Const, so [var.lb, var.ub].meet_with([Const, +INF])
		lhs.meet_with(IntervalValue(rhs.lb(), IntervalValue::plus_infinity()));
		break;
	}
	case CmpStmt::Predicate::ICMP_ULT:
	case CmpStmt::Predicate::ICMP_SLT:
	case CmpStmt::Predicate::FCMP_OLT:
	case CmpStmt::Predicate::FCMP_ULT: {
		// Var < Const, so [var.lb, var.ub].meet_with([-INF, const.ub-1])
		lhs.meet_with(IntervalValue(IntervalValue::minus_infinity(), rhs.ub() - 1));
		break;
	}
	case CmpStmt::Predicate::ICMP_ULE:
	case CmpStmt::Predicate::ICMP_SLE:
	case CmpStmt::Predicate::FCMP_OLE:
	case CmpStmt::Predicate::FCMP_ULE: {
		// Var <= Const, so [var.lb, var.ub].meet_with([-INF, const.ub])
		lhs.meet_with(IntervalValue(IntervalValue::minus_infinity(), rhs.ub()));
		break;
	}
	case CmpStmt::Predicate::FCMP_FALSE: break;
	case CmpStmt::Predicate::FCMP_TRUE: break;
	default: assert(false && "implement this part"); abort();
	}

	for (const auto& addr : addrs) {
		NodeID objId = SVF::AbstractState::getInternalID(addr);
		if (new_es.inAddrToValTable(objId)) {
			switch (predicate) {
			case CmpStmt::Predicate::ICMP_EQ:
			case CmpStmt::Predicate::FCMP_OEQ:
			case CmpStmt::Predicate::FCMP_UEQ: {
				new_es.load(addr).meet_with(rhs);
				break;
			}
			case CmpStmt::Predicate::ICMP_NE:
			case CmpStmt::Predicate::FCMP_ONE:
			case CmpStmt::Predicate::FCMP_UNE:
				// Compliment set
				break;
			case CmpStmt::Predicate::ICMP_UGT:
			case CmpStmt::Predicate::ICMP_SGT:
			case CmpStmt::Predicate::FCMP_OGT:
			case CmpStmt::Predicate::FCMP_UGT:
				// Var > Const, so [var.lb, var.ub].meet_with([Const+1, +INF])
				new_es.load(addr).meet_with(IntervalValue(rhs.lb() + 1, IntervalValue::plus_infinity()));
				break;
			case CmpStmt::Predicate::ICMP_UGE:
			case CmpStmt::Predicate::ICMP_SGE:
			case CmpStmt::Predicate::FCMP_OGE:
			case CmpStmt::Predicate::FCMP_UGE: {
				// Var >= Const, so [var.lb, var.ub].meet_with([Const, +INF])
				new_es.load(addr).meet_with(IntervalValue(rhs.lb(), IntervalValue::plus_infinity()));
				break;
			}
			case CmpStmt::Predicate::ICMP_ULT:
			case CmpStmt::Predicate::ICMP_SLT:
			case CmpStmt::Predicate::FCMP_OLT:
			case CmpStmt::Predicate::FCMP_ULT: {
				// Var < Const, so [var.lb, var.ub].meet_with([-INF, const.ub-1])
				new_es.load(addr).meet_with(IntervalValue(IntervalValue::minus_infinity(), rhs.ub() - 1));
				break;
			}
			case CmpStmt::Predicate::ICMP_ULE:
			case CmpStmt::Predicate::ICMP_SLE:
			case CmpStmt::Predicate::FCMP_OLE:
			case CmpStmt::Predicate::FCMP_ULE: {
				// Var <= Const, so [var.lb, var.ub].meet_with([-INF, const.ub])
				new_es.load(addr).meet_with(IntervalValue(IntervalValue::minus_infinity(), rhs.ub()));
				break;
			}
			case CmpStmt::Predicate::FCMP_FALSE: break;
			case CmpStmt::Predicate::FCMP_TRUE: break;
			default: assert(false && "implement this part"); abort();
			}
		}
	}

	as = new_es;
	return true;
}

bool AbstractExecution::isSwitchBranchFeasible(const SVFVar* var, s64_t succ, AbstractState& as) {
	AbstractState new_es = as;
	IntervalValue& switch_cond = new_es[var->getId()].getInterval();
	s64_t value = succ;
	FIFOWorkList<const SVFStmt*> workList;
	for (SVFStmt* cmpVarInStmt : var->getInEdges()) {
		workList.push(cmpVarInStmt);
	}
	switch_cond.meet_with(IntervalValue(value, value));
	if (switch_cond.isBottom()) {
		return false;
	}
	while (!workList.empty()) {
		const SVFStmt* stmt = workList.pop();
		if (SVFUtil::isa<CopyStmt>(stmt)) {
			IntervalValue& copy_cond = new_es[var->getId()].getInterval();
			copy_cond.meet_with(IntervalValue(value, value));
		}
		else if (const LoadStmt* load = SVFUtil::dyn_cast<LoadStmt>(stmt)) {
			if (new_es.inVarToAddrsTable(load->getRHSVarID())) {
				AddressValue& addrs = new_es[load->getRHSVarID()].getAddrs();
				for (const auto& addr : addrs) {
					NodeID objId = SVF::AbstractState::getInternalID(addr);
					if (new_es.inAddrToValTable(objId)) {
						new_es.load(addr).meet_with(switch_cond);
					}
				}
			}
		}
	}
	as = new_es;
	return true;
}

bool AbstractExecution::isBranchFeasible(const IntraCFGEdge* intraEdge, AbstractState& as) {
	const SVFVar* cmpVar = intraEdge->getCondition();
	if (cmpVar->getInEdges().empty()) {
		return isSwitchBranchFeasible(cmpVar, intraEdge->getSuccessorCondValue(), as);
	}
	else {
		assert(!cmpVar->getInEdges().empty() && "no in edges?");
		SVFStmt* cmpVarInStmt = *cmpVar->getInEdges().begin();
		if (const CmpStmt* cmpStmt = SVFUtil::dyn_cast<CmpStmt>(cmpVarInStmt)) {
			return isCmpBranchFeasible(cmpStmt, intraEdge->getSuccessorCondValue(), as);
		}
		else {
			return isSwitchBranchFeasible(cmpVar, intraEdge->getSuccessorCondValue(), as);
		}
	}
}

/// handle global node
void AbstractExecution::handleGlobalNode() {
	AbstractState as;
	const ICFGNode* node = icfg->getGlobalICFGNode();
	postAbsTrace[node] = preAbsTrace[node];
	postAbsTrace[node][0] = AddressValue();
	// Global Node, we just need to handle addr, load, store, copy and gep
	for (const SVFStmt* stmt : node->getSVFStmts()) {
		updateAbsState(stmt);
	}
}

void AbstractExecution::ensureAllAssertsValidated() {
	u32_t svf_assert_count = 0;
	u32_t overflow_count = 0;
	for (auto it = svfir->getICFG()->begin(); it != svfir->getICFG()->end(); ++it) {
		const ICFGNode* node = it->second;
		if (const CallICFGNode* call = SVFUtil::dyn_cast<CallICFGNode>(node)) {
			if (const FunObjVar* fun = call->getCalledFunction()) {
				if (fun->getName() == "svf_assert" || fun->getName() == "OVERFLOW") {
					if (fun->getName() == "svf_assert") {
						svf_assert_count++;
					}
					else {
						overflow_count++;
					}
					if (assert_points.find(call) == assert_points.end()) {
						std::stringstream ss;
						ss << "The stub function calliste (svf_assert or OVERFLOW) has not been checked: "
						   << call->toString();
						std::cerr << ss.str() << std::endl;
						assert(false);
					}
				}
			}
		}
	}
	if (overflow_count == 0) {
		SVFBugReport& report = bufOverflowHelper.getBugReporter();
		assert (report.getBugSet().size() == 0 && "There should be no overflow bugs");
	} else {
		assert(overflow_count <= bufOverflowHelper.getBugReporter().getBugSet().size() &&
		       "The number of overflow bugs should be equal to the number of stub functions called");
	}
}


/**
 * @brief The driver program
 *
 * This function conducts the overall analysis of the program by initializing and processing
 * various components of the control flow graph (ICFG) and handling global nodes and WTO cycles.
 * It marks recursive functions, initializes WTOs for each function, and processes the main function.
 */
void AbstractExecution::analyse() {
	// Init WTOs for all functions, and handle Global ICFGNode of SVFModule
	initWTO();
	utils = new AbsExtAPI(postAbsTrace);

	// Handle the global node
	handleGlobalNode();

	// Process the main function if it exists
	if (const FunObjVar* fun = svfir->getFunObjVar("main")) {
		// arguments of main are initialised as \top to represent all possible inputs
		for (u32_t i = 0; i < fun->arg_size(); ++i) {
			AbstractState& as = getAbsStateFromTrace(icfg->getGlobalICFGNode());
			as[fun->getArg(i)->getId()] = IntervalValue::top();
		}
		ICFGWTO* wto = funcToWTO[fun];
		handleWTOComponents(wto->getWTOComponents());
	}
	return;
}

/**
 * @brief Hanlde two types of WTO components (singleton and cycle)
 */
void AbstractExecution::handleWTOComponents(const std::list<const ICFGWTOComp*>& wtoComps) {
	for (const ICFGWTOComp* wtoNode : wtoComps) {
		if (const ICFGSingletonWTO* node = SVFUtil::dyn_cast<ICFGSingletonWTO>(wtoNode)) {
			handleSingletonWTO(node);
		}
		// Handle WTO cycles
		else if (const ICFGCycleWTO* cycle = SVFUtil::dyn_cast<ICFGCycleWTO>(wtoNode)) {
			handleCycleWTO(cycle);
		}
		// Assert false for unknown WTO types
		else {
			assert(false && "unknown WTO type!");
		}
	}
}

/**
 * @brief Handle a Weak Topological Order (WTO) node in the control flow graph
 *
 * This function processes a WTO node, which typically represents a loop in the control flow graph.
 * It first propagates the state to the node if feasible. Then, it updates the abstract state
 * and performs buffer overflow detection for each statement in the node. If the node is a call
 * node, it handles the call site or specific stub functions.
 *
 * @param node The WTO node to be processed
 */
void AbstractExecution::handleSingletonWTO(const ICFGSingletonWTO* singletonWTO) {
	const ICFGNode* node = singletonWTO->getICFGNode();
	// Propagate the states from predecessors to the current node and return true if the control-flow is feasible
	bool is_feasible = mergeStatesFromPredecessors(node, preAbsTrace[node]);
	if (is_feasible) {
		postAbsTrace[node] = preAbsTrace[node];
	}
	else {
		return;
	}

	std::deque<const ICFGNode*> worklist;

	// Handle each SVF statement in the node
	for (const SVFStmt* stmt : node->getSVFStmts()) {
		updateAbsState(stmt); // Update the abstract state with the current statement
		bufOverflowDetection(stmt); // Perform buffer overflow detection for the statement
	}

	// Handle call nodes by inlining the callee function
	if (const CallICFGNode* callnode = SVFUtil::dyn_cast<CallICFGNode>(node)) {
		// Get the name of the callee function
		std::string funName = callnode->getCalledFunction()->getName();
		if (funName == "OVERFLOW" || funName == "svf_assert" || funName == "svf_assert_eq") {
			handleStubFunctions(callnode); // Handle specific stub functions
		}
		else {
			handleCallSite(callnode); // Handle general call sites
		}
	}
}

/**
 * @brief Handle state updates for each type of SVF statement
 *
 * This function updates the abstract state based on the type of SVF statement provided.
 * It dispatches the handling of each specific statement type to the corresponding update function.
 *
 * @param stmt The SVF statement for which the state needs to be updated
 */
void AbstractExecution::updateAbsState(const SVFStmt* stmt) {
	// Handle address statements
	if (const AddrStmt* addr = SVFUtil::dyn_cast<AddrStmt>(stmt)) {
		updateStateOnAddr(addr);
	}
	// Handle binary operation statements
	else if (const BinaryOPStmt* binary = SVFUtil::dyn_cast<BinaryOPStmt>(stmt)) {
		updateStateOnBinary(binary);
	}
	// Handle comparison statements
	else if (const CmpStmt* cmp = SVFUtil::dyn_cast<CmpStmt>(stmt)) {
		updateStateOnCmp(cmp);
	}
	// Handle load statements
	else if (const LoadStmt* load = SVFUtil::dyn_cast<LoadStmt>(stmt)) {
		updateStateOnLoad(load);
	}
	// Handle store statements
	else if (const StoreStmt* store = SVFUtil::dyn_cast<StoreStmt>(stmt)) {
		updateStateOnStore(store);
	}
	// Handle copy statements
	else if (const CopyStmt* copy = SVFUtil::dyn_cast<CopyStmt>(stmt)) {
		updateStateOnCopy(copy);
	}
	// Handle GEP (GetElementPtr) statements
	else if (const GepStmt* gep = SVFUtil::dyn_cast<GepStmt>(stmt)) {
		updateStateOnGep(gep);
	}
	// Handle phi statements
	else if (const PhiStmt* phi = SVFUtil::dyn_cast<PhiStmt>(stmt)) {
		updateStateOnPhi(phi);
	}
	// Handle call procedure entries
	else if (const CallPE* callPE = SVFUtil::dyn_cast<CallPE>(stmt)) {
		updateStateOnCall(callPE);
	}
	// Handle return procedure entries
	else if (const RetPE* retPE = SVFUtil::dyn_cast<RetPE>(stmt)) {
		updateStateOnRet(retPE);
	}
	// Handle select statements
	else if (const SelectStmt* select = SVFUtil::dyn_cast<SelectStmt>(stmt)) {
		updateStateOnSelect(select);
	}
	// Handle unary operations and branch statements (no action needed)
	else if (SVFUtil::isa<UnaryOPStmt>(stmt) || SVFUtil::isa<BranchStmt>(stmt)) {
		// Nothing needs to be done here as BranchStmt is handled in hasBranchES
	}
	// Assert false for unsupported statement types
	else {
		assert(false && "implement this part");
	}
}


/**
 * @brief Handle a call site in the control flow graph
 *
 * This function processes a call site by updating the abstract state, handling the called function,
 * and managing the call stack. It resumes the execution state after the function call.
 *
 * @param node The call site node to be handled
 */
void AbstractExecution::handleCallSite(const CallICFGNode* callNode) {
	// Get the callee function associated with the call site
	const FunObjVar* callee = callNode->getCalledFunction();

	if (isExternalCallForAssignment(callee)) {
		// implement external calls for the assignment
		updateStateOnExtCall(callNode);
	}
	else if (SVFUtil::isExtCall(callee)) {
		// handle external API calls
		utils->handleExtAPI(callNode);
	}
	else if (recursiveFuns.find(callee) != recursiveFuns.end()) {
		// skip recursive functions
		return;
	}
	else {
		// Push the call node onto the call site stack
		callSiteStack.push_back(callNode);
		// Handle the callee function
		ICFGWTO* wto = funcToWTO[callee];
		handleWTOComponents(wto->getWTOComponents());

		// Pop the call node from the call site stack
		callSiteStack.pop_back();
	}
}

/**
 * @brief Handle stub functions for verifying abstract interpretation results
 *
 * This function handles specific stub functions (`svf_assert` and `OVERFLOW`) to check whether
 * the abstract interpretation results are as expected. For `svf_assert(expr)`, the expression must hold true.
 * For `OVERFLOW(object, offset_access)`, the size of the object must be less than or equal to the offset access.
 *
 * @param callnode The call node representing the stub function to be handled
 */

void AbstractExecution::handleStubFunctions(const SVF::CallICFGNode* callNode) {
	// Handle the 'svf_assert' stub function
	if (callNode->getCalledFunction()->getName() == "svf_assert") {
		assert_points.insert(callNode);
		// If the condition is false, the program is infeasible
		u32_t arg0 = callNode->getArgument(0)->getId();
		AbstractState& as = getAbsStateFromTrace(callNode);

		// Check if the interval for the argument is infinite
		if (as[arg0].getInterval().is_infinite()) {
			SVFUtil::errs() << "svf_assert Fail. " << callNode->toString() << "\n";
			assert(false);
		}
		else {
			if (as[arg0].getInterval().equals(IntervalValue(1, 1))) {
				std::stringstream ss;
				ss << "The assertion (" << callNode->toString() << ")"
				   << " is successfully verified!!\n";
				SVFUtil::outs() << ss.str() << std::endl;
			}
			else {
				std::stringstream ss;
				ss << "The assertion (" << callNode->toString() << ")"
				   << " is unsatisfiable!!\n";
				SVFUtil::outs() << ss.str() << std::endl;
				assert(false);
			}
		}
		return;
	}
	else if (callNode->getCalledFunction()->getName() == "svf_assert_eq")  {
		u32_t arg0 = callNode->getArgument(0)->getId();
		u32_t arg1 = callNode->getArgument(1)->getId();
		AbstractState& as = getAbsStateFromTrace(callNode);
		if (as[arg0].getInterval().equals(as[arg1].getInterval()))
		{
			SVFUtil::errs() << SVFUtil::sucMsg("The assertion is successfully verified!!\n");
		}
		else
		{
			SVFUtil::errs() <<"svf_assert_eq Fail. " << callNode->toString() << "\n";
			assert(false);
		}
		return;
	}
	// Handle the 'OVERFLOW' stub function
	else if (callNode->getCalledFunction()->getName() == "OVERFLOW") {
		// If the condition is false, the program is infeasible
		assert_points.insert(callNode);
		u32_t arg0 = callNode->getArgument(0)->getId();
		u32_t arg1 = callNode->getArgument(1)->getId();

		AbstractState& as = getAbsStateFromTrace(callNode);
		AbstractValue gepRhsVal = as[arg0];

		// Check if the RHS value is an address
		if (gepRhsVal.isAddr()) {
			bool overflow = false;
			for (const auto& addr : gepRhsVal.getAddrs()) {
				u64_t access_offset = as[arg1].getInterval().getIntNumeral();
				NodeID objId = AbstractState::getInternalID(addr);
				const GepObjVar* gepLhsObjVar = SVFUtil::cast<GepObjVar>(svfir->getGNode(objId));
				auto size = svfir->getBaseObject(objId)->getByteSizeOfObj();
				if (bufOverflowHelper.hasGepObjOffsetFromBase(gepLhsObjVar)) {
					overflow = (bufOverflowHelper.getGepObjOffsetFromBase(gepLhsObjVar).ub().getIntNumeral() + access_offset
					            >= size);
				}
				else {
					assert(false && "pointer not found in gepObjOffsetFromBase");
				}
			}
			if (overflow) {
				std::cerr << "Your implementation successfully detected the buffer overflow\n";
			}
			else {
				SVFUtil::errs() << "Your implementation failed to detect the buffer overflow!"
				                << callNode->toString() << "\n";
				assert(false);
			}
		}
		else {
			SVFUtil::errs() << "Your implementation failed to detect the buffer overflow!"
			                << callNode->toString() << "\n";
			assert(false);
		}
	}
}


