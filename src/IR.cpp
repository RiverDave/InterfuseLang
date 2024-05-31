#include "../include/IR.h"
#include "../include/internals.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
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
    pushBlock(bblock, "Entry block");
    Builder->SetInsertPoint(bblock);

    // Recursively generate code for each node in the AST
    if (srcRoot.codeGen(*this)) {

        //Emit return instance to main function
        ReturnInst::Create(*TheContext, blocks.top()->blockWrapper);

        //Verify module before transfering ownership to JIT
        std::string moduleStr;
        llvm::raw_string_ostream errorStream(moduleStr);

        assert(blocks.size() == 1);//Important!!
        verifyFunction(*globalFn);

        TheModule->print(llvm::errs(), nullptr);
        if (llvm::verifyModule(*TheModule, &errorStream)) {
            std::cerr << "The module is not valid:\n"
                      << errorStream.str() << std::endl;
            throw std::runtime_error("Module is not valid");
        }

        // globalFn->print(llvm::errs());

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

// Helpers
llvm::Value *CodeGenContext::checkId(std::string &id) {

    auto localCheck = checkLocal(id, blocks.top());

    if (localCheck->allocaSpace) {
        return localCheck->allocaSpace;
    }

    GlobalVariable *globalCheck = TheModule->getNamedGlobal(id);

    if (globalCheck) {
        return globalCheck;
    }

    return nullptr;
}

llvm::Value *LogErrorV(const char *Str) {
    LogError(Str);
    return nullptr;
}

fuseData::varData *CodeGenContext::checkLocal(std::string &id,
                                              std::shared_ptr<CodeGenBlock> parent_block) {

    auto it = parent_block->locals.find(id);
    if (it != parent_block->locals.end()) {
        return &it->second;// Variable found, return stack allocation
    }

    // Recursively check in parent block
    // If not found and there's a parent blocks, check in the parent block
    if (parent_block->parent) {
        return checkLocal(id, parent_block->parent);
    }

    return nullptr;
}

llvm::GlobalVariable *CodeGenContext::insertGlobal(std::string &id, llvm::Type *type) {
    TheModule->getOrInsertGlobal(id, type);
    GlobalVariable *gval = TheModule->getNamedGlobal(id);
    assert(gval && "Global variable not emitted");
    gval->setLinkage(GlobalValue::ExternalLinkage);
    gval->setInitializer(Constant::getNullValue(type));//Important, else doesn't emit for some reason
    const DataLayout &DL = TheModule->getDataLayout();

    auto alignment = DL.getABITypeAlign(type);
    gval->setAlignment(llvm::MaybeAlign(alignment));

    //This is a global variable basically
    return gval;
}

//Unsure about the actual use case of this function
//
std::vector<std::pair<std::string, fuseData::varData>>
CodeGenContext::getOuterVars() {

    std::vector<std::pair<std::string, fuseData::varData>> allocations;

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
                      //                          if (isa<llvm::ReturnInst>(last.value())) {
                      //                              return;
                      //                          };
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
    auto id = context.checkLocal(this->name, context.blocks.top());

    //Check the current scope, else check for the global variables
    if (id && id->allocaSpace) {
        // Load variable from the stack
        return context.Builder->CreateLoad(id->allocaSpace->getAllocatedType(), id->allocaSpace, name.c_str());
    }

    auto global = context.globals.find(this->name);

    if (global != context.globals.end()) {

        //Global var ptr
        auto gptr = context.TheModule->getGlobalVariable(name);
        return context.Builder->CreateLoad(global->second.type, gptr, name.c_str());
    }

    std::string err = "Variable not found! " + this->name;
    LogErrorV(err.c_str());
    throw std::runtime_error(err);
    return nullptr;
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
    TOKEN_TYPE toktype = op.getType();

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
            //TODO: IMPLEMENT THIS FOR FLOATS?
        case OPERATOR_NOT_EQUALS:
            // std::cout << "Creating not equals :" << std::endl;
            return context.Builder->CreateICmpNE(lhs.codeGen(context),
                                                 rhs.codeGen(context), "tempne");
        case OPERATOR_EQUALS:
            // std::cout << "Creating equals :" << std::endl;
            return context.Builder->CreateICmpEQ(lhs.codeGen(context),
                                                 rhs.codeGen(context), "tempeq");
        case OPERATOR_GREATER_THAN:
            // std::cout << "Creating greater than :" << std::endl;
            return context.Builder->CreateICmpSGT(lhs.codeGen(context),
                                                  rhs.codeGen(context), "tempgt");
        case OPERATOR_LESS_THAN: {

            Value *left = this->lhs.codeGen(context);
            Value *right = this->rhs.codeGen(context);
            return context.Builder->CreateICmpSLT(left, right, "templ");
        }
            // std::cout << "Creating less than :" << std::endl;

        case OPERATOR_GREATER_THAN_EQUALS:
            // std::cout << "Creating greater than equals :" << std::endl;
            return context.Builder->CreateICmpSGE(lhs.codeGen(context),
                                                  rhs.codeGen(context), "tempgte");
        case OPERATOR_LESS_THAN_EQUALS:
            // std::cout << "Creating less than equals :" << std::endl;
            return context.Builder->CreateICmpSLE(lhs.codeGen(context),
                                                  rhs.codeGen(context), "temple");


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
        //  Insert alloc at the beginning of the function
        context.Builder->SetInsertPoint(&parentFunction->getEntryBlock());

    } else if (!context.Builder->GetInsertBlock()->getParent()) {
        // NOTE: Unsure about this logic path might not be necessary...
        std::cout << "FN has no parent" << std::endl;
        return nullptr;
    }

    AllocaInst *alloc = nullptr;
    GlobalVariable *gVar = nullptr;

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

        //Current function is entry(main), declare variable as global
        if (parentFunction->getName() == "main") {

            //This returns a global Variable obj
            gVar = reinterpret_cast<GlobalVariable *>(context.insertGlobal(this->id->name, this->type.getType(context)));

            if (!gVar) {
                std::string err{"Error inserting global variable"};
                LogErrorV(err.c_str());
                throw std::runtime_error(err);
            }

            context.globals[id->name] = fuseData::globalInfo{nullptr, varType};

            //var declared in an inner scope, allocate variable on the stack
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
        context.globals[this->id->name].val = assignedVal;
    } else {
        //Emit assignment IR
        assignedVal = assignmentExpr->codeGen(context);

        if (alloc) {

            context.blocks.top()->locals[id->name].allocaVal = assignedVal;
        } else {

            //Map global var: rather than storing directly the global var obj directly, it is much more
            //convinient to store the value we'd stored in it. We can get the global obj any time with
            //the module.
            context.globals[this->id->name] = fuseData::globalInfo{assignedVal, varType};
        }
    }


    // assignedVal->print(llvm::outs(), false);
    // std::cout << std::endl;
    //    auto storedData = alloc ? alloc : gVar;
    if (alloc) {

        return context.Builder->CreateStore(assignedVal, alloc);
    } else {
        return context.Builder->CreateStore(assignedVal, gVar);
    }
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
    auto *localData = context.checkLocal(lhs.name, context.blocks.top());

    if (localData) {

        localData->allocaVal = value;
        return context.Builder->CreateStore(value, localData->allocaSpace);
    }

    auto global = context.globals.find(this->lhs.name);

    if (global != context.globals.end()) {
        context.globals[lhs.name].val = value;

        auto globalPtr = context.TheModule->getGlobalVariable(lhs.name);
        return context.Builder->CreateStore(value, globalPtr);
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
        context.pushBlock(BB, "fnentryblock");
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
        context.blocks.top()->return_value = generatedFnCode;//Innecessary for now


        //NOTE: Experimental for now...
        //        if (context.blocks.top()->blockWrapper != BB) {//Basic block has probably changed
        //            //Top should now be last block emmited from code gen
        //
        //            auto nBlock = context.blocks.top();
        //            //NOTE: THis might lead to errors, considering a function
        //            //can have multiple blocks inserted at its end because of
        //            //if stmts or loops...
        //            while (context.blocks.top()->blockWrapper != &fn->getEntryBlock()) {
        //                context.popBlock();
        //            }
        //            context.blocks.top() = nBlock;// Copy last emitted block into the top
        //        }


        bool hasReturnInst = false;
        //TODO: If a function contains multiple blocks based on control flow statements
        // the last block does not necessarily need contain a return statement
        // if there's a return statement on both if else blocks. this is very
        // specific but is a thing to consider.
        for (auto &I: *context.blocks.top()->blockWrapper) {// -> check last emmited block from function
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
            context.Builder->CreateRetVoid();
        }


        //        context.TheFPM->run(*fn);
        verifyFunction(*fn);

        context.popBlock();
        context.Builder->SetInsertPoint(context.blocks.top()->blockWrapper);

        context.globals[this->id.name] =
                fuseData::globalInfo{fn, this->retType.getType(context), fn->getFunctionType()};

        // fn->print(llvm::errs());

        return fn;
    }


    context.globals[this->id.name] =
            fuseData::globalInfo{fn, this->retType.getType(context), fn->getFunctionType()};
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
        // context.Builder->SetInsertPoint(context.blocks.top()->blockWrapper);
        context.Builder->CreateRetVoid();

        return nullptr;
    }
    Value *retVal = expression->codeGen(context);

    if (!retVal) {
        std::string_view err = "Return statement codegen failed";
        LogErrorV(err.data());
        throw std::runtime_error(err.data());
    }

    context.Builder->CreateRet(retVal);
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
    Function *fn = context.TheModule->getFunction(id.name);

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

            //            std::vector<std::pair<std::string, varData>> outerVars =
            //                    context.getOuterVars();
            //
            //            for (const auto &var: outerVars) {
            //                args.push_back(var.second.allocaVal);
            //            }

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
                fn->getFunctionType(), fn, args, (isVoid ? "" : find->first));

        assert(valCall && "Function call failed");

        return valCall;

    } else {

        std::string err = "Function " + this->id.name + " not found";
        LogErrorV(err.c_str());
        throw std::runtime_error(err);
    }
}

llvm::Value *NIfStatement::codeGen(CodeGenContext &context) {

    Value *vcond = this->condition->codeGen(context);
    Type *vtype = vcond->getType();

    if (!vcond) {
        return nullptr;
    }

    //Compare val with its null equivalent to create bool comparison
    if (vtype->isIntegerTy()) {

        vcond = context.Builder->CreateICmpNE(vcond, ConstantInt::get(*context.TheContext, APInt(vcond->getType()->getIntegerBitWidth(), 0)), "ifcond");
    } else if (vtype->isFloatingPointTy()) {

        vcond = context.Builder->CreateFCmpONE(vcond, ConstantFP::get(*context.TheContext, APFloat(0.0)), "ifcond");

    } else {//TODO: STR stuff
        std::string err = "Unknown condition type detected";
        LogErrorV(err.c_str());
        throw std::runtime_error(err);
    }

    Function *fn = context.blocks.top()->blockWrapper->getParent();
    //Determine if there's an exit to the function
    bool earlyReturn = false;

    BasicBlock *ifbb = BasicBlock::Create(*context.TheContext, "trueblock", fn);
    BasicBlock *elsebb;
    if (this->falseBlock) {
        elsebb = BasicBlock::Create(*context.TheContext, "falseblock");
    } else {

        elsebb = nullptr;
    }

    //Basically basic block where any of the control flows unite
    BasicBlock *mergebb = BasicBlock::Create(*context.TheContext, "truecont");

    if (elsebb) {
        context.Builder->CreateCondBr(vcond, ifbb, elsebb);
    } else {
        context.Builder->CreateCondBr(vcond, ifbb, mergebb);
    }

    context.Builder->SetInsertPoint(ifbb);
    context.pushBlock(ifbb);
    Value *ifval = nullptr;

    //Emit ir for true block
    for (auto &expr: trueBlock->statements) {
        ifval = expr->codeGen(context);
    }

    ifbb = context.Builder->GetInsertBlock();
    context.popBlock();

    if (!ifbb->getTerminator()) {
        context.Builder->CreateBr(mergebb);
    } else {
        earlyReturn = true;
    }


    Value *elseval = nullptr;
    if (elsebb) {

        fn->insert(fn->end(), elsebb);

        context.Builder->SetInsertPoint(elsebb);
        context.pushBlock(elsebb);

        //Generate else block

        for (auto &stmt: falseBlock->block->statements) {
            elseval = stmt->codeGen(context);
        }

        elsebb = context.Builder->GetInsertBlock();
        context.popBlock();

        if (!elsebb->getTerminator()) {
            context.Builder->CreateBr(mergebb);
        }
    }


    //Update for phi node
    fn->insert(fn->end(), mergebb);
    context.Builder->SetInsertPoint(mergebb);
    context.pushBlock(mergebb);

    //Basically expect an expression of the boolean expression datatype
    //to terminate the block.

    if (ifval->getType()->isVoidTy() || (elseval && elseval->getType()->isVoidTy()) /* || ifval->getType() != elseval->getType() */) {
        //Return void
        context.popBlock();
        context.blocks.top()->blockWrapper = mergebb;
        return llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*context.TheContext));
    }

    if (earlyReturn) {

        context.popBlock();
        context.blocks.top()->blockWrapper = mergebb;
        return ifval;
    }

    //NOTE: Don't access this block if there's an early return
    //It took me fucking days to figure that out...
    PHINode *PN = context.Builder->CreatePHI(vtype, 2, "iftmp");

    PN->addIncoming(ifval, ifbb);
    if (elseval) {
        PN->addIncoming(elseval, elsebb);
    }

    context.popBlock();
    // return context.Builder->CreateRet(PN);
    return PN;
}

llvm::Value *NWhileStatement::codeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *NForStatement::codeGen(CodeGenContext &context) {

    //Loop init expression

    Function *parentFn = context.blocks.top()->blockWrapper->getParent();
    Value *initVar = context.checkId(this->initialization->name);

    bool isAlloca = false;

    if (initVar) {
        if (llvm::AllocaInst *allocInst = llvm::dyn_cast<llvm::AllocaInst>(initVar)) {
            isAlloca = true;
        }
    }

    //Allocation in case a new variable is inserted(var was not defined)
    BasicBlock *stepBB = BasicBlock::Create(*context.TheContext, "stepBlock", parentFn);
    BasicBlock *loopBB = BasicBlock::Create(*context.TheContext, "loop", parentFn);


    AllocaInst *allocInst = nullptr;
    GlobalVariable *gVar;
    //Variable reference not found, allocate memory.
    if (!initVar) {

        //I assume loop variables are whole integers
        //TODO: This alloc should be inserted on a new block(loop block?)
        allocInst = context.insertMemOnFnBlock(parentFn, this->initialization->name, llvm::Type::getInt64Ty(*context.TheContext));

        if (!allocInst) {
            std::string err =
                    "Error initiallizing loop alloca: " + this->initialization->name;
            LogErrorV(err.c_str());
            throw std::runtime_error(err);
        }


        Value *nStoredVal = Constant::getNullValue(Type::getInt64Ty(*context.TheContext));
        context.Builder->CreateStore(nStoredVal, allocInst);

        context.Builder->CreateBr(stepBB);
        context.Builder->SetInsertPoint(stepBB);
        context.pushBlock(stepBB, "stepbb");


        context.blocks.top()->locals[initialization->name] = fuseData::varData{initialization->name, Type::getInt64Ty(*context.TheContext), allocInst, nStoredVal};
        isAlloca = true;// Var didn't exist so allocate it for the sake of the current loop

    } else {

        //Assert variable as an integer:

        allocInst = llvm::dyn_cast<llvm::AllocaInst>(initVar);// TODO: REFORMAT THIS WHOLE THING
        if (isAlloca) {
            auto allocType = allocInst->getAllocatedType();
            if (!allocType->isIntegerTy()) {
                std::string err =
                        "Attempt to initialize non integer variable in for loop: " + this->initialization->name;
                LogErrorV(err.c_str());
                throw std::runtime_error(err);
            }

            context.Builder->CreateLoad(Type::getInt64Ty(*context.TheContext), allocInst, this->initialization->name.c_str());
        } else {
            //I love spaghetti code
            //Search amongst globals to verify that its type is an integer
            auto global = context.globals.find(this->initialization->name);
            if (!global->second.type->isIntegerTy()) {
                std::string err =
                        "Attempt to initialize non integer variable in for loop: " + this->initialization->name;
                LogErrorV(err.c_str());
                throw std::runtime_error(err);
            }

            gVar = context.TheModule->getGlobalVariable(initialization->name);
            assert(gVar);
            context.Builder->CreateLoad(global->second.type, gVar, global->first.c_str());
        }

        context.Builder->CreateBr(stepBB);
        context.Builder->SetInsertPoint(stepBB);
        context.pushBlock(stepBB, "stepbb");
    }


    //Emit end condition
    Value *endCond = this->condition->codeGen(context);

    BasicBlock *loopEndBB = BasicBlock::Create(*context.TheContext, "loopend", parentFn);
    //This is basically what produces the loop, as long as this condition evaluates to false, else we'll create a goto
    //back to the loop label
    context.Builder->CreateCondBr(endCond, loopBB, loopEndBB);
    context.next_jumpable_bb = loopEndBB;

    //context.popBlock(); //FIXME: THIS CAUSES AN EXCEPTION & POTENTIALLY AN INFINITE LOOP(SOMEHOW)

    //Emit IR
    context.Builder->SetInsertPoint(loopBB);
    context.pushBlock(loopBB, "loopbb");
    assert(context.blocks.top()->parent);

    //emit loop body
    bool break_loop = false;

    Value *last = nullptr;
    for (auto stmt: loopBlock->statements) {
        last = stmt->codeGen(context);
    }

    if (last && llvm::dyn_cast<llvm::BranchInst>(last)) {
        break_loop = true;
    };

    if (!break_loop) {


        //Emit step val
        Value *stepVal = nullptr;
        if (this->iteration) {
            stepVal = iteration->codeGen(context);
            if (!stepVal || stepVal->getType()->isVoidTy()) {
                return nullptr;
            }
            stepVal->print(llvm::errs());
        }

        if (isAlloca) {
            //Store step expression into current variabl
            Value *next_val = context.Builder->CreateStore(stepVal, allocInst);
            next_val->print(llvm::errs());
            assert(next_val);

        } else {
            Value *next_val = context.Builder->CreateStore(stepVal, gVar);
            next_val->print(llvm::errs());
            assert(next_val);
        }
        context.Builder->CreateBr(stepBB);
    }

    //This pops ensure that anything allocated in these blocks is not accessible outside of the loop scope
    context.popBlock();//pop step block
    context.popBlock();//pop loop block
    context.blocks.top()->blockWrapper = loopEndBB;

    context.next_jumpable_bb = {};//reset jumpable bb
    context.Builder->SetInsertPoint(loopEndBB);


    return Constant::getNullValue(Type::getInt64Ty(*context.TheContext));
}

llvm::Value *NString::codeGen(CodeGenContext &context) { return nullptr; }


llvm::Value *NElseStatement::codeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *NBreakStatement::codeGen(CodeGenContext &context) {
    std::cout << "Generating code for break statement" << std::endl;

    //This solution particularly bad but Ill be using it for now...
    //Instead of trying to cast each node of the ast to check whether its a break statement(It failed for some reason)
    //in a loop and create the br instruction directly there, I opted for this approach, next jumpabable_bb is the basic block
    //where it is safe to jump to while code-generating a block in a loop. The programmer has to be responsible of assigning this
    //jumpable block in the crossroads of a conditional br(see NForstmt)
    if (context.next_jumpable_bb.has_value()) {
        return context.Builder->CreateBr(context.next_jumpable_bb.value());
    }

    std::string err = "Next jumpable block is not accessible or Break statement is not within a loop";
    LogErrorV(err.c_str());
    throw std::runtime_error(err);
}
