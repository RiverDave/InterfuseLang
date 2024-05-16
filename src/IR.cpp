#include "../include/IR.h"
#include "../include/internals.hpp"
#include <algorithm>
#include <iterator>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Triple.h>
#include <memory>
#include <unordered_map>

using namespace llvm;

CodeGenContext::CodeGenContext() : blocks() {
    // Init core llvm
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("Pourer", *TheContext);
    ee = nullptr;


    setTargets();

    auto JITOrErr = llvm::orc::KaleidoscopeJIT::Create();

    if (!JITOrErr) {
        llvm::errs() << "Failed to create JIT: " << JITOrErr.takeError() << "\n";
        return;
    }

    TheJIT = std::move(*JITOrErr);
    assert(TheJIT && "Failed to create TheJIT");

    TheModule->setDataLayout(TheJIT->getDataLayout());
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
    globalFn = nullptr;
    createCoreFunctions(*this);//Generate core functions(printf for now)

    TheFPM = std::make_unique<legacy::FunctionPassManager>(TheModule.get());

    TheLAM = std::make_unique<llvm::LoopAnalysisManager>();
    TheFAM = std::make_unique<llvm::FunctionAnalysisManager>();
    TheCGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
    TheMAM = std::make_unique<llvm::ModuleAnalysisManager>();
    ThePIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
    TheSI = std::make_unique<llvm::StandardInstrumentations>(*TheContext,
                                                             /*DebugLogging*/ true);

    //Add standard passes to the manager
    TheSI->registerCallbacks(*ThePIC, TheMAM.get());
    // Add transform passes.
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    TheFPM->add(llvm::createInstructionCombiningPass());
    // TheFPM->addPass(InstCombinePass());
    // // Reassociate expressions.
    TheFPM->add(llvm::createReassociatePass());
    // // Eliminate Common SubExpressions.
    TheFPM->add(llvm::createGVNPass());
    // // Simplify the control flow graph (deleting unreachable blocks, etc).
    //CFG PASS
    TheFPM->add(llvm::createCFGSimplificationPass());
    TheFPM->doInitialization();//Initialize passes mentioned above

    // Register analysis passes used in these transform passes.
    llvm::PassBuilder PB;
    PB.registerModuleAnalyses(*TheMAM);
    PB.registerFunctionAnalyses(*TheFAM);
    PB.crossRegisterProxies(*TheLAM, *TheFAM, *TheCGAM, *TheMAM);
}

// Entry point for code generation
void CodeGenContext::emitIR(NBlock &srcRoot) {
    std::cout << "Mixing Pour..." << std::endl;

    // Top level block
    std::vector<llvm::Type *> argTypes;
    llvm::FunctionType *ftype = llvm::FunctionType::get(
            Type::getVoidTy(*TheContext), llvm::ArrayRef(argTypes), false);

    globalFn = Function::Create(ftype, llvm::GlobalValue::InternalLinkage, "main",
                                TheModule.get());

    BasicBlock *bblock = BasicBlock::Create(*TheContext, "entry", globalFn, 0);
    pushBlock(bblock);
    Builder->SetInsertPoint(bblock);

    // Recursively generate code for each node in the AST
    if (srcRoot.codeGen(*this)) {

        //Emit return instance to main function
        ReturnInst::Create(*TheContext, bblock);

        //Verifyt module before transfering ownership to JIT
        std::string moduleStr;
        llvm::raw_string_ostream errorStream(moduleStr);
        if (llvm::verifyModule(*TheModule, &errorStream)) {
            std::cerr << "The module is not valid:\n"
                      << errorStream.str() << std::endl;
            throw std::runtime_error("Module is not valid");
        }

        globalFn->print(llvm::errs());

        // Create the ExecutionEngine
        std::string eeError;
        ee = EngineBuilder(std::move(TheModule)).setErrorStr(&eeError).create();
        if (!ee) {
            std::string err = "Failed to create ExecutionEngine: ";
            LogErrorV(err.c_str());
            throw std::runtime_error(err);
        }

        //ORC stuff, (Basically evaluate top level expression of the bytecode generated)
        //               auto RT = TheJIT->getMainJITDylib().createResourceTracker();
        //
        //               auto TSM = llvm::orc::ThreadSafeModule(std::move(TheModule), std::move(TheContext));
        //               ExitOnErr(TheJIT->addModule(std::move(TSM), RT));
        //        //        //NOTE: Could encapsulate constructors init in a function
        //        //
        //               auto ExprSymbol = TheJIT->lookup("my_fn");
        //        //
        //               if (!ExprSymbol) {
        //                   ExitOnErr(ExprSymbol.takeError());
        //               }
        //
        //        //        // Get the symbol's address and cast it to the right type (takes no
        //        //        // arguments, returns a double) so we can call it as a native function.
        //               double (*FP)() = ExprSymbol->getAddress().toPtr<double (*)()>();
        //               fprintf(stderr, "Evaluated to %f\n", FP());
        //        //
        //        //        // Delete the anonymous expression module from the JIT.
        //               ExitOnErr(RT->remove());
    }
    popBlock();
}

llvm::GenericValue CodeGenContext::runCode() {
    std::cout << "Running code...\n";
    GenericValue v;
    try {

        // FIXME: Returning exception for some reason!! after calling function
        ee->finalizeObject();

        if (ee->hasError()) {

            std::string err = "Failed to create ExecutionEngine " + ee->getErrorMessage();
            LogErrorV(err.c_str());
            throw std::runtime_error(err);
        }

        std::vector<GenericValue> noargs;
        v = ee->runFunction(ee->FindFunctionNamed("main"), noargs);
        std::cout << "Code was run.\n";

    } catch (const std::exception &e) {
        if (e.what()) {
            std::cout << e.what() << std::endl;
            std::cout << ee->getErrorMessage() << std::endl;
        }
    }

    return v;
}

void CodeGenContext::setTargets() {
    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();//FIXME: This wouldn't link for some
    // reason

    auto TargetTriple = LLVMGetDefaultTargetTriple();
    TheModule->setTargetTriple(TargetTriple);
}

// Helper
llvm::Value *LogErrorV(const char *Str) {
    LogError(Str);
    return nullptr;
}

// TODO: Test this
llvm::AllocaInst *
CodeGenContext::checkLocal(std::string &id,
                           std::shared_ptr<CodeGenBlock> parent_block) {

    auto it = parent_block->locals.find(id);
    if (it != parent_block->locals.end()) {
        return it->second.allocaSpace;// Variable found, return stack allocation
    }

    // Recursively check in parent block
    // If not found and there's a parent blocks, check in the parent block
    if (parent_block->parent) {
        return checkLocal(id, parent_block->parent);
    }

    return nullptr;
}

llvm::Value *CodeGenContext::insertGlobal(std::string &id, llvm::Type *type) {
    TheModule->getOrInsertGlobal(id, type);
    GlobalVariable *gval = TheModule->getNamedGlobal(id);
    assert(gval && "Global variable not emitted");
    gval->setLinkage(GlobalValue::ExternalLinkage);
    gval->setInitializer(Constant::getNullValue(type));//Important, else doesn't emit var for some reason
    const DataLayout &DL = TheModule->getDataLayout();

    auto alignment = DL.getABITypeAlign(type);
    gval->setAlignment(llvm::MaybeAlign(alignment));

    return gval;
}

std::vector<std::pair<std::string, varData>>
CodeGenContext::getOuterVars() {

    std::vector<std::pair<std::string, varData>> allocations;

    // Start with the current block
    auto currentBlock = blocks.top();

    // Iterate while there is a parent block
    while (currentBlock) {
        const auto &currentLocals = currentBlock->locals;

        std::copy(currentLocals.begin(), currentLocals.end(),
                  std::inserter(allocations, allocations.end()));

        // Move to the parent block
        if (currentBlock->parent) {
            currentBlock = currentBlock->parent;

        } else {
            break;
        }
    }

    return allocations;
}

llvm::AllocaInst *CodeGenContext::insertMemOnFnBlock(llvm::Function *fn,
                                                     std::string &id,
                                                     llvm::Type *type) {
    if (Builder->GetInsertBlock() == nullptr && fn != nullptr) {
        // std::cout << "FN has no insertBLock" << std::endl;
        // Insert at the beginning of the function
        Builder->SetInsertPoint(&fn->getEntryBlock());
    } else if (Builder->GetInsertBlock()->getParent() == nullptr) {
        std::cout << "FN has no parent" << std::endl;
        return nullptr;
    }

    fn = Builder->GetInsertBlock()->getParent();

    IRBuilder<> TmpB(&fn->getEntryBlock(), fn->getEntryBlock().begin());
    return TmpB.CreateAlloca(type, nullptr, id.c_str());
}

// CODE GEN STUFF:
llvm::Value *NBlock::codeGen(CodeGenContext &context) {
    Value *last = nullptr;

    std::for_each(statements.begin(), statements.end(),
                  [&](NStatement *stmt) {
                      last = stmt->codeGen(context);
                  });

    return last;
}

llvm::Value *NDouble::codeGen(CodeGenContext &context) {
    std::cout << "Generating code for double: " << value << std::endl;
    // Communicates context with generated bytecode
    return llvm::ConstantFP::get(*context.TheContext, llvm::APFloat(this->value));
}

llvm::Value *NInteger::codeGen(CodeGenContext &context) {
    // std::cout << "Generating code for Integer: " << value << std::endl;
    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context.TheContext),
                                  value, true);
}

llvm::Value *NExpressionStatement::codeGen(CodeGenContext &context) {
    return expression.codeGen(context);
}

// Variable expression
llvm::Value *NIdentifier::codeGen(CodeGenContext &context) {
    AllocaInst *V = context.checkLocal(this->name, context.blocks.top());
    auto global = context.globals.find(this->name);

    if (V) {
        // Load variable from the stack
        return context.Builder->CreateLoad(V->getAllocatedType(), V, name.c_str());
    }

    if (global != context.globals.end()) {

        return context.Builder->CreateLoad(global->second.type, global->second.val, name.c_str());
    }

    std::string err = "Variable not found! " + this->name;
    LogErrorV(err.c_str());
    throw std::runtime_error(err);
}

llvm::Type *NIdentifier::getType(CodeGenContext &context) const {
    // TODO: Complement types?

    std::unordered_map<std::string, llvm::Type *> typeMap{
            {"int", llvm::Type::getInt64Ty(*context.TheContext)},
            {"double", llvm::Type::getDoubleTy(*context.TheContext)},
            {"float", llvm::Type::getFloatTy(*context.TheContext)},
            {"void", llvm::Type::getVoidTy(*context.TheContext)}
            //{"bool", llvm::Type::getInt64Ty(*context.TheContext)},
            //{"int", llvm::Type::getInt64Ty(*context.TheContext)},
    };

    auto it = typeMap.find(name);
    if (it == typeMap.end()) {
        // Throw Error: Unknown type

        std::string err = "Unknown type: " + name;
        LogErrorV(err.c_str());
        throw std::runtime_error(err);
    }

    return it->second;
}

llvm::Value *NBinaryOperator::codeGen(CodeGenContext &context) {
    auto toktype = op.getType();

    switch (toktype) {
        case OPERATOR_PLUS:
            // std::cout << "Creating addition :" << std::endl;
            return context.Builder->CreateAdd(lhs.codeGen(context),
                                              rhs.codeGen(context), "tempadd");
        case OPERATOR_MINUS:
            // std::cout << "Creating subtraction :" << std::endl;
            return context.Builder->CreateSub(lhs.codeGen(context),
                                              rhs.codeGen(context), "tempsub");

        case OPERATOR_DIVIDE:
            // std::cout << "Creating division :" << std::endl;
            return context.Builder->CreateSDiv(lhs.codeGen(context),
                                               rhs.codeGen(context), "tempdiv");

        case OPERATOR_MULTIPLY:
            // std::cout << "Creating multiplication :" << std::endl;
            return context.Builder->CreateMul(lhs.codeGen(context),
                                              rhs.codeGen(context), "tempmul");

        default:
            LogErrorV("Invalid binary op");
    }

    return nullptr;
}

llvm::Value *NVariableDeclaration::codeGen(CodeGenContext &context) {
    // Function *parentFunction = context.Builder->GetInsertBlock()->getParent();

    Function *parentFunction = context.blocks.top()->blockWrapper->getParent();

    if (context.Builder->GetInsertBlock() == nullptr) {

        // std::cout << "FN has no insertBLock" << std::endl;
        //  Insert at the beginning of the function
        context.Builder->SetInsertPoint(&parentFunction->getEntryBlock());

    } else if (context.Builder->GetInsertBlock()->getParent() == nullptr) {
        // NOTE: Unsure about this logic path might not be necessary...
        std::cout << "FN has no parent" << std::endl;
        return nullptr;
    }

    AllocaInst *alloc = nullptr;
    Value *gVar = nullptr;

    //    auto locals = context.blocks.top()->locals;

    auto find = context.blocks.top()->locals.find(id->name);

    Type *varType = this->type.getType(context);

    if (varType->isVoidTy()) {
        std::string err = "invalid Data type for variables " + this->id->name;
        LogErrorV(err.c_str());
        throw std::runtime_error(err);
    }

    assert(varType);
    // auto local = context.checkLocal(this->id->name, context.blocks.top());

    if (find == context.blocks.top()->locals.end()) {

        //Current function is parent, declare variable as global
        if (parentFunction->getName() == "main") {
            gVar = context.insertGlobal(this->id->name, this->type.getType(context));

            if (!gVar) {
                std::string err{"Error inserting global variable"};
                LogErrorV(err.c_str());
                throw std::runtime_error(err);
            }

        } else {

            alloc = context.insertMemOnFnBlock(parentFunction, id->name, varType);
            context.blocks.top()->locals[id->name].allocaSpace = alloc;
        }

        // std::cout << "Creating variable declaration: " << id->name << std::endl;
    } else {
        // Variable redefinition in the same scope detected, prevent allocation

        LogErrorV("Variable redefinition detected\n");
        throw std::runtime_error("Redefinition detected");
    }

    Value *assignedVal = nullptr;
    if (!assignmentExpr) {
        // Initialize empty val to null
        assignedVal = Constant::getNullValue(varType);
    } else {
        assignedVal = assignmentExpr->codeGen(context);

        if (alloc) {

            context.blocks.top()->locals[id->name].allocaVal = assignedVal;
        } else {

            //Map global var
            context.globals[this->id->name] = fnInfo{gVar, varType};
        }
    }


    // assignedVal->print(llvm::outs(), false);
    // std::cout << std::endl;
    auto storedData = alloc ? alloc : gVar;

    return context.Builder->CreateStore(assignedVal, storedData);
}

llvm::Value *NAssignment::codeGen(CodeGenContext &context) {
    Value *value = rhs.codeGen(context);


    if (value == nullptr) {
        return nullptr;
    }

    if (value->getType()->isVoidTy()) {
        LogErrorV("Cannot tie void value to expression");
        throw std::runtime_error("Cannot tie void value to expression");
    }

    // check variable existance
    auto *alloc = context.checkLocal(lhs.name, context.blocks.top());
    auto global = context.globals.find(this->lhs.name);

    if (alloc) {

        //        context.blocks.top()->locals[lhs.name].allocaVal = value;
        return context.Builder->CreateStore(value, alloc);
    }

    if (global != context.globals.end()) {
        return context.Builder->CreateStore(value, global->second.val);
    }

    // Prevent void assignment

    LogErrorV("Unknown variable name");
    throw std::runtime_error("Unknown variable name");
}

//TODO: Variables declared on the main block, Should be global variables
llvm::Value *NFnDeclaration::codeGen(CodeGenContext &context) {
    // Prototype generation:
    if (Function *existingFn = context.TheModule->getFunction(id.name)) {
        std::string err = "Function '" + id.name + "' already exists";
        LogErrorV(err.c_str());
        throw std::runtime_error(err);
    }

    std::vector<Type *> argTypes;
    std::for_each(
            params.begin(), params.end(), [&](NVariableDeclaration *var_decl) {
                argTypes.push_back(var_decl->type.getType(context));

                if (var_decl->type.getType(context)->isVoidTy()) {
                    std::string err =
                            "Invalid data type for function parameter " + var_decl->id->name;
                    LogErrorV(err.c_str());
                    throw std::runtime_error(err);
                }
            });

    Type *fnRetType = this->retType.getType(context);

    if (!fnRetType) {
        std::string err = "Unknown fn return type";
        LogErrorV(err.c_str());
        throw std::runtime_error(err);
    }

    llvm::FunctionType *FT = FunctionType::get(fnRetType, argTypes, false);

    Function *fn = Function::Create(FT, GlobalValue::ExternalLinkage, id.name,
                                    context.TheModule.get());
    assert(fn);

    // Function was defined(has block)
    if (fnBlock) {

        size_t i = 0;
        for (auto &arg: fn->args()) {
            arg.setName(params[i++]->id->name);
        }

        BasicBlock *BB = BasicBlock::Create(*context.TheContext.get(), "fnBlock", fn);
        context.pushBlock(BB);
        context.Builder->SetInsertPoint(BB);

        i = 0;

        for (const auto &arg: fn->args()) {
            auto argValue = fn->args().begin() + i;

            auto nameStr = arg.getName().str();
            AllocaInst *alloc = context.insertMemOnFnBlock(fn, nameStr, arg.getType());
            context.blocks.top()->locals[arg.getName().str()].allocaSpace = alloc;

            Value *nullVal = Constant::getNullValue(arg.getType());
            assert(nullVal && alloc && argValue);
            //Store arg
            context.Builder->CreateStore(*&argValue, alloc, false);
        }

        // Now generate the function body
        Value *generatedFnCode = fnBlock->codeGen(context);
        context.blocks.top()->return_value = generatedFnCode;

        bool hasReturnInst = false;
        for (auto &I: *BB) {
            if (llvm::isa<llvm::ReturnInst>(&I)) {
                hasReturnInst = true;
                break;
            }
        }

        if (!hasReturnInst && !fn->getReturnType()->isVoidTy()) {
            std::string err =
                    "Function " + this->id.name + " should have a return value";
            LogErrorV(err.c_str());
            throw std::runtime_error(err);
        } else if (!hasReturnInst && fn->getReturnType()->isVoidTy()) {
            //Insert return instruction in case
            //user does not implicitly inserts a return statement
            ReturnInst::Create(*context.TheContext, BB);
        }


        context.TheFPM->run(*fn);
        verifyFunction(*fn);

        context.popBlock();
        context.Builder->SetInsertPoint(context.blocks.top()->blockWrapper);

        context.globals[this->id.name] =
                fnInfo{fn, this->retType.getType(context), fn->getFunctionType()};

        fn->print(llvm::errs());

        return fn;
    }


    context.globals[this->id.name] =
            fnInfo{fn, this->retType.getType(context), fn->getFunctionType()};
    return fn;
}


llvm::Value *NUnaryOperator::codeGen(CodeGenContext &context) {
    return nullptr;
}

// Doubtful bout this
llvm::Value *NReturnStatement::codeGen(CodeGenContext &context) {
    if (!this->expression) {// Void return

        // Check if the function is NOT void considering you can't return nothing
        // from a non void function
        if (!context.blocks.top()
                     ->blockWrapper->getParent()
                     ->getReturnType()
                     ->isVoidTy()) {
            std::string err = "Function " +
                              context.blocks.top()->blockWrapper->getName().str() +
                              " should return a value";
            LogErrorV(err.c_str());
            throw std::runtime_error(err);
        }
        ReturnInst::Create(*context.TheContext, context.blocks.top()->blockWrapper);
        return nullptr;
    }
    Value *retVal = expression->codeGen(context);

    // Insert instruction in the current block
    ReturnInst::Create(*context.TheContext, retVal,
                       context.blocks.top()->blockWrapper);

    if (!retVal) {
        std::string_view err = "Return statement codegen failed";
        LogErrorV(err.data());
        throw std::runtime_error(err.data());
    }

    //    retVal->print(llvm::outs(), false);
    //    std::cout << std::endl;
    return retVal;
}

llvm::Value *NFnCall::codeGen(CodeGenContext &context) {
    // 1. Make sure fn is defined within globals
    // 2. make sure fn is callable(is not a variable) TODO:
    // 4. Check if it has a block within it. TODO:
    // 4. Get expression(s) from ast

    // Get relevant info to call the function
    auto find = context.globals.find(this->id.name);

    // Check if Fn was defined
    const Function *fn = context.TheModule->getFunction(id.name);

    if (fn && find != context.globals.end()) {
        std::vector<Value *> args;
        if (arguments.size() == fn->arg_size()) {
            unsigned i = 0;
            for (auto &arg: this->arguments) {
                Value *V = arg->codeGen(context);
                // size_t vals = find->second.fnType->getNumParams();
                Type *paramType = fn->args().begin()[i].getType();
                assert(V->getType() == paramType && "Argument type mismatch ");
                Value *bitcastedArg =
                        context.Builder->CreateBitCast(V, paramType);// thanks bolt!
                args.push_back(bitcastedArg);
                i++;
            }

            std::vector<std::pair<std::string, varData>> outerVars =
                    context.getOuterVars();

            for (const auto &var: outerVars) {
                args.push_back(var.second.allocaVal);
            }

        } else {
            std::string err =
                    "Argument size mismatch in Function: " + this->id.name + "\n";
            LogErrorV(err.c_str());
            throw std::runtime_error(err);
        }

        // auto valCall = context.Builder->CreateCall(
        //     ffind, args);

        // assert(find->first == ffind->getName());

        auto isVoid = fn->getReturnType()->isVoidTy();

        CallInst *valCall = context.Builder->CreateCall(
                //Dont pass name if void(thx to LLMDEV)
                fn->getFunctionType(), find->second.val, args, (isVoid ? "" : find->first));

        assert(valCall && "Function call failed");

        return valCall;

    } else {

        std::string err = "Function " + this->id.name + " not found";
        LogErrorV(err.c_str());
        throw std::runtime_error(err);
    }
}

llvm::Value *NWhileStatement::codeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *NForStatement::codeGen(CodeGenContext &context) { return nullptr; }

llvm::Value *NString::codeGen(CodeGenContext &context) { return nullptr; }

llvm::Value *NIfStatement::codeGen(CodeGenContext &context) { return nullptr; }

llvm::Value *NElseStatement::codeGen(CodeGenContext &context) {
    return nullptr;
}
