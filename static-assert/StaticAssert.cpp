#include <algorithm>
#include <string>
#include <vector>
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

cl::list<std::string> AssertFunctionNames("assert-function-names",
        cl::desc("Function names to detect for static-assert pass"));

struct StaticAssertVisitor {
    SmallVector<const CallInst *, 64> Calls;

    StaticAssertVisitor(const std::vector<std::string> &FunctionNames)
        : FunctionNames(FunctionNames)
    {}

    bool runOnFunction(Function &F) {
        SmallSet<const BasicBlock *, 64> visited;

        SmallVector<const CallInst *, 64> temp;
        bool result = runOnBasicBlock(visited, temp, &F.getEntryBlock());

        if (result && !temp.empty()) {
            Calls.append(temp.begin(), temp.end());
            return true;
        }

        return false;
    }

    private:
    std::vector<std::string> FunctionNames;

    // Run the visitor on a BasicBlock, visiting all instructions, then the successor BasicBlocks
    // if needed.
    //
    // Return true if the assert call if found, false if not.
    // If the assert call if found, the corresponding instructions are appended
    // to the Instructions list.
    bool runOnBasicBlock(SmallSet<const BasicBlock *, 64> &Visited,
                         SmallVector<const CallInst *, 64> &Instructions,
                         const BasicBlock *BB) {
        // This block has already been visited, which means we may be in a loop.
        // As the loop may or may not terminate, we return true in case it terminates
        // and another branch call the assert function.
        if (!Visited.insert(BB).second)
            return true;

        // We pass on each instructions, looking for the one we want.
        for (const Instruction &I: *BB) {
            if (const CallInst *Call = dynamic_cast<const CallInst *>(&I)) {
                Function *Called = Call->getCalledFunction();

                if (std::find(FunctionNames.begin(), FunctionNames.end(), Called->getName()) != FunctionNames.end()) {
                    // We found the assert call, hurray!
                    Instructions.append(1, Call);

                    return true;
                }
            }
        }

        // No matching instruction found, now we search in the successor BasicBlocks
        // of the current one.
        // If, and only if, all the successor BasicBlocks contains the assert calls,
        // we can say the assert is unconditionally called, then we can submit the result.
        SmallVector<const CallInst *, 64> temp;
        bool result = true;
        for (const BasicBlock *Succ: successors(BB)) {
            result = result && runOnBasicBlock(Visited, temp, Succ);

            if (!result)
                break; // No need to continue further, we've already found it's hopeless.
        }

        // If result is true but temp is empty, it means either we're in an infinite loop
        // or this BasicBlock has no successors.
        if (result && !temp.empty()) {
            Instructions.append(temp.begin(), temp.end());
            return true;
        }
        return false;
    }
};

namespace {

    struct StaticAssert: public FunctionPass {
        static char ID;

        std::vector<std::string> FunctionNames;

        StaticAssert(const std::vector<std::string> &FunctionNames)
            : FunctionPass(ID)
            , FunctionNames(FunctionNames)
        {}
        StaticAssert()
            : StaticAssert(AssertFunctionNames)
        {}

        bool runOnFunction(Function &F) override {
            StaticAssertVisitor visitor(FunctionNames);
            if (visitor.runOnFunction(F)) {
                errs() << "Warning: The function `" << F.getName() << "` unconditionally fails with:\n";
                for (const CallInst *Call: visitor.Calls) {
                    Call->dump();
                }
            }
            return false;
        }
    };

}

char StaticAssert::ID = 0;

static RegisterPass<StaticAssert>
X("static-assert", "Log all static false asserts.", false, false);
