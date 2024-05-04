#include "Util/SVFBugReport.h"
#include "AE/Core/ICFGWTO.h"
#include "WPA/Andersen.h"
#include "AE/Svfexe/AbstractInterpretation.h"

namespace SVF
{
class AbsExe;

class Assign3Exception : public std::exception {
public:
    Assign3Exception(const std::string& message): msg_(message) {}

    virtual const char* what() const throw() {
        return msg_.c_str();
    }

private:
    std::string msg_;
};



/// AbstractInterpretation is same as Abstract Execution
class AbsExe: virtual public AbstractInterpretation
{
public:
    /// Constructor
    AbsExe() : AbstractInterpretation() {
    }

    virtual void runOnModule(ICFG* icfg) {
        AbstractInterpretation::runOnModule(icfg);
    }

    void handleWTONode(const ICFGNode* node);
    virtual void handleCycle(const ICFGWTOCycle* cycle);

    virtual void handleFunc(const SVFFunction* func) {
        AbstractInterpretation::handleFunc(func);
    }

    virtual void updateAbsState(const SVFStmt* stmt);
    virtual void bufOverflowDetection(const SVFStmt* stmt);


    // handle SVF Statements
    void updateStateOnAddr(const AddrStmt* addr);
    void updateStateOnGep(const GepStmt* gep);
    void updateStateOnStore(const StoreStmt* store);
    void updateStateOnLoad(const LoadStmt* load);
    void updateStateOnCmp(const CmpStmt* cmp);
    void updateStateOnCall(const CallPE* call);
    void updateStateOnRet(const RetPE* retPE);
    void updateStateOnCopy(const CopyStmt* copy);
    void updateStateOnPhi(const PhiStmt* phi);
    void updateStateOnBinary(const BinaryOPStmt* binary);
    void updateStateOnSelect(const SelectStmt *select);

    void addBugToReporter(const Assign3Exception& e, const ICFGNode* node);

    // helper functions related to gep
    void initSVFVar(AbstractState& es, u32_t varId);
    void initObjVar(AbstractState& es, const ObjVar *objVar, u32_t varId);
    AbstractValue getGepObjAddress(AbstractState& es, u32_t pointer, APOffset offset);
    AbstractValue getByteOffset(const AbstractState& es, const GepStmt *gep);
    AbstractValue getElemIndex(const AbstractState& es, const GepStmt *gep);

    /// Destructor
    virtual ~AbsExe() {
    }
protected:
    Map<NodeID, AbstractValue> obj2size;
    // preAbsTrace and postAbsTrace

};
}