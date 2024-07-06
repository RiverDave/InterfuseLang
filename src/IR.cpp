#include "IR.h"
#include "internals.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
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
#include <system_error>
#include <unordered_map>

using namespace llvm;

extern const char *_global_file_path;

CodeGenContext::CodeGenContext(bool verboseMode, std::string fname, std::string exec_name, bool wasm_mode)
    : blocks(), _verbose_mode(verboseMode), dump_file_name(fname), binary_name(exec_name), wasm_compilation(wasm_mode) {
    // Init core llvm
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("Pourer", *TheContext);
    ee = nullptr;


    setTargets();



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

    // Top level block
    std::vector<llvm::Type *> argTypes;
    argTypes.push_back(llvm::IntegerType::getInt32Ty(*TheContext));                                                     // int argc
    argTypes.push_back(llvm::PointerType::get(llvm::PointerType::get(llvm::IntegerType::getInt8Ty(*TheContext), 0), 0));// char** argv
                                                                                                                        //
    llvm::FunctionType *ftype = llvm::FunctionType::get(
            Type::getVoidTy(*TheContext), llvm::ArrayRef(argTypes), false);

    globalFn = Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, "main",//Internal was the default, but it was not finding main in the
                                                                                  //Linking process for any reason
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

        assert(blocks.size() == 1);//Important!! to assure all blocks to write have been popped
        verifyFunction(*globalFn);

        //Output code if requested
        if (_verbose_mode) {
            std::cout << "Printing IR code\n";
            TheModule->print(llvm::errs(), nullptr);
        }

        if (llvm::verifyModule(*TheModule, &errorStream)) {
            std::cerr << "The module is not valid:\n"
                      << errorStream.str() << std::endl;
            throw std::runtime_error("Module is not valid");
        }

        // globalFn->print(llvm::errs());

        // Create the ExecutionEngine
    }
    popBlock();
}

//This should only be run on verbose mode to display
//explicit IR.
llvm::GenericValue CodeGenContext::runCode() {
    GenericValue v;
    try {

        std::string eeError;
        ee = EngineBuilder(std::move(TheModule)).setErrorStr(&eeError).create();
        if (!ee) {
            std::string err = "Failed to create ExecutionEngine: ";
            LogErrorV(err.c_str());
            throw std::runtime_error(err);
        }

        ee->finalizeObject();

        if (ee->hasError()) {

            std::string err = "Failed to create ExecutionEngine " + ee->getErrorMessage();
            LogErrorV(err.c_str());
            throw std::runtime_error(err);
        }

        //supply fn args needs, argc = 0 and argv = NULL
        std::vector<GenericValue> fnargs;

        GenericValue gvArgc;
        gvArgc.IntVal = APInt(32, 0);
        fnargs.push_back(gvArgc);

        GenericValue gvArgv;
        gvArgv.PointerVal = nullptr;
        fnargs.push_back(gvArgv);

        v = ee->runFunction(ee->FindFunctionNamed("main"), fnargs);

    } catch (const std::exception &e) {
        if (e.what()) {
            std::cerr << e.what() << std::endl;
            std::cerr << ee->getErrorMessage() << std::endl;
        }
    }

    return v;
}

void CodeGenContext::setTargets() {
    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();


    if (wasm_compilation) {
        std::string TargetTripleStr = "wasm32-unknown-unknown-wasm";
        std::cout << "COMPILING TO WASM " << TargetTripleStr << std::endl;
        TheModule->setTargetTriple(TargetTripleStr);
    } else {

        auto TargetTriple = LLVMGetDefaultTargetTriple(); //Should run on every llvm capable system (theorically)
        TheModule->setTargetTriple(TargetTriple);
    }
}

int CodeGenContext::dumpIR() {

    std::error_code EC;


    auto trim_extension = [&](std::string &filename) {
        assert(!filename.empty() && !binary_name.empty());
        auto pos = filename.find_last_of('.');
        if (pos != std::string::npos) {
            filename = filename.substr(0, pos);
        }
        filename += ".ll";//Append .ll extension

        std::cout << filename << std::endl;
    };

    trim_extension(dump_file_name);

    llvm::raw_fd_ostream OS(dump_file_name, EC, llvm::sys::fs::OF_None);

    if (EC) {
        errs();
        return EXIT_FAILURE;
    }

    std::string wasm_ext;

    if (wasm_compilation) {
        wasm_ext = "-w";
    }

    //Dump ir to .ll file/ Creates file
    TheModule->print(OS, nullptr);

    //Run sh to compile to binary
    std::string cmd = FUSE_RUNNER_PATH;
    cmd += " " + dump_file_name + " " + binary_name + " " + wasm_ext;

    int result = system(cmd.c_str());//Run sh script
    if (result == -1) {
        std::cerr << "INTERFUSE: sh command error" << std::endl;
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}

// Helpers
//Returns either alloc or global var
llvm::Value *CodeGenContext::checkId(std::string &id) {

    auto localCheck = checkLocal(id, blocks.top());

    if (localCheck && localCheck->allocaSpace) {
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
    if (type == llvm::Type::getInt8Ty(*TheContext)) {
        type = type->getPointerTo();
    }

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

    if (type == llvm::Type::getInt8Ty(*TheContext)) {
        type = type->getPointerTo();
    }

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
        // return id->allocaSpace;
    }

    auto global = context.globals.find(this->name);

    if (global != context.globals.end()) {

        //Global var ptr
        auto gptr = context.TheModule->getGlobalVariable(name);
        //        return gptr;
        return context.Builder->CreateLoad(global->second.val->getType(), gptr, name.c_str());
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
            {"str", llvm::PointerType::getInt8Ty(*context.TheContext)},
            {"float", llvm::Type::getFloatTy(*context.TheContext)},//Not supported at all
            {"void", llvm::Type::getVoidTy(*context.TheContext)}
            //{"bool", llvm::Type::getInt64Ty(*context.TheContext)},
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
    TOKEN_TYPE toktype = op->getType();

    Value *left = lhs->codeGen(context);
    Value *right = rhs->codeGen(context);

    //All operations are done on doubles if one of the operands is a double, to avoid loss of precision

    if (left->getType()->isIntegerTy(64) && right->getType()->isDoubleTy()) {
        left = context.Builder->CreateSIToFP(left, llvm::Type::getDoubleTy(*context.TheContext), "intToFloat");
    }

    if (right->getType()->isIntegerTy(64) && left->getType()->isDoubleTy()) {
        right = context.Builder->CreateSIToFP(right, llvm::Type::getDoubleTy(*context.TheContext), "intToFloat");
        assert(left->getType()->isDoubleTy() && right->getType()->isDoubleTy() && "Invalid types");
    }

    bool double_op = left->getType()->isDoubleTy() && right->getType()->isDoubleTy();


    if (right->getType()->isIntegerTy(1)) {// Check if right is a boolean
        right = context.Builder->CreateZExt(right, llvm::Type::getInt64Ty(*context.TheContext), "booltoint");
    }

    if (left->getType()->isIntegerTy(1)) {// Check if left is a boolean
        left = context.Builder->CreateZExt(left, llvm::Type::getInt64Ty(*context.TheContext), "booltoint");
    }

    switch (toktype) {

        case OPERATOR_PLUS:
            // Additions are called differently for integers and doubles
            if (double_op)
                return context.Builder->CreateFAdd(left,
                                                   right, "tempfadd");
            else
                return context.Builder->CreateAdd(left,
                                                  right, "tempadd");
        case OPERATOR_MINUS:

            if (double_op)
                return context.Builder->CreateFSub(left,
                                                   right, "tempfsub");
            else
                return context.Builder->CreateSub(left,
                                                  right, "tempsub");
        case OPERATOR_DIVIDE:

            if (double_op)
                return context.Builder->CreateFDiv(left,
                                                   right, "tempfdiv");
            else
                return context.Builder->CreateSDiv(left, right, "tempdiv");

        case OPERATOR_MULTIPLY:
            if (double_op)
                return context.Builder->CreateFMul(left,
                                                   right, "tempfmul");
            else
                return context.Builder->CreateMul(left, right, "tempmul");

        case OPERATOR_MODULO:
            if (double_op)
                return context.Builder->CreateFRem(left,
                                                   right, "tempfrem");
            else
                return context.Builder->CreateSRem(left, right, "temprem");

        case OPERATOR_NOT_EQUALS:
            if (double_op)
                return context.Builder->CreateFCmpONE(left,
                                                      right, "tempfne");
            else
                return context.Builder->CreateICmpNE(left,
                                                     right, "tempne");
        case OPERATOR_EQUALS:
            if (double_op)
                return context.Builder->CreateFCmpOEQ(left,
                                                      right, "tempfeq");
            else
                return context.Builder->CreateICmpEQ(left,
                                                     right, "tempeq");
        case OPERATOR_GREATER_THAN:
            if (double_op)
                return context.Builder->CreateFCmpOGT(left,
                                                      right, "tempfgt");
            else
                return context.Builder->CreateICmpSGT(left,
                                                      right, "tempgt");

        case OPERATOR_LESS_THAN:
            if (double_op)
                return context.Builder->CreateFCmpOLT(left,
                                                      right, "tempflt");
            else
                return context.Builder->CreateICmpSLT(left,
                                                      right, "templt");

        case OPERATOR_GREATER_THAN_EQUALS:
            if (double_op)
                return context.Builder->CreateFCmpOGE(left,
                                                      right, "tempfgte");
            else
                return context.Builder->CreateICmpSGE(left,
                                                      right, "tempgte");

        case OPERATOR_LESS_THAN_EQUALS:
            if (double_op)
                return context.Builder->CreateFCmpOLE(left,
                                                      right, "tempfgte");
            else
                return context.Builder->CreateICmpSLE(left,
                                                      right, "tempgte");
        default:
            LogErrorV("Invalid binary op, might not be supported yet");
    }
    return nullptr;
}

llvm::Value *NVariableDeclaration::codeGen(CodeGenContext &context) {

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
    assert(varType);

    if (varType->isVoidTy()) {
        std::string err = "invalid Data type for variables " + this->id->name;
        LogErrorV(err.c_str());
        throw std::runtime_error(err);
    }

    // auto local = context.checkLocal(this->id->name, context.blocks.top());

    if (find == context.blocks.top()->locals.end()) {

        //Current function is entry(main), declare variable as global
        if (parentFunction->getName() == "main") {

            //This returns a global Variable obj
            gVar = reinterpret_cast<GlobalVariable *>(context.insertGlobal(this->id->name, this->type.getType(context)));

            context.globals[id->name] = fuseData::globalInfo{nullptr, varType};

            //var declared in an inner scope, allocate variable on the stack
        } else {

            alloc = context.insertMemOnFnBlock(parentFunction, id->name, varType);
            context.blocks.top()->locals[id->name].allocaSpace = alloc;
        }

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
        if (assignedVal->getType() == llvm::Type::getDoubleTy(*context.TheContext) && varType == llvm::Type::getInt64Ty(*context.TheContext)) {
            //Convert int to double if necessary
            assignedVal = context.Builder->CreateFPToSI(assignedVal, varType);
        }

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
    Value *value = rhs->codeGen(context);

    if (value == nullptr) {
        return nullptr;
    }

    if (value->getType()->isVoidTy()) {
        LogErrorV("Cannot tie void value to expression");
        throw std::runtime_error("Cannot tie void value to expression");
    }

    // check variable existance
    auto *localData = context.checkLocal(lhs->name, context.blocks.top());

    if (localData) {

        localData->allocaVal = value;
        return context.Builder->CreateStore(value, localData->allocaSpace);
    }

    auto global = context.globals.find(this->lhs->name);

    if (global != context.globals.end()) {
        context.globals[lhs->name].val = value;

        auto globalPtr = context.TheModule->getGlobalVariable(lhs->name);
        return context.Builder->CreateStore(value, globalPtr);
    }

    // Prevent void assignment
    LogErrorV("Unknown variable name");
    throw std::runtime_error("Unknown variable name");
}

//TODO: Variables declared on the main block, Should be global variables
llvm::Value *NFnDeclaration::codeGen(CodeGenContext &context) {
    // Prototype generation:
    Function *fn = context.TheModule->getFunction(id.name);

    if (!fn) {
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

        fn = Function::Create(FT, GlobalValue::ExternalLinkage, id.name,
                              context.TheModule.get());
        assert(fn && "Fn instance not created");

        //Define this early to allow recursive calls
        context.globals[this->id.name] =
                fuseData::globalInfo{fn, this->retType.getType(context), fn->getFunctionType()};
    } else if (fn && !fn->empty()) {
        std::string err = "Function '" + id.name + "' already exists and it is defined";
        LogErrorV(err.c_str());
        throw std::runtime_error(err);
    }


    // Function was defined(has block)
    if (fnBlock && fn) {
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

        // generate the function body
        Value *generatedFnCode = fnBlock->codeGen(context);
        context.blocks.top()->return_value = generatedFnCode;//Innecessary for now

        bool hasReturnInst = false;
        //NOTE: If a function contains multiple blocks based on control flow statements
        // the last block does not necessarily need contain a return statement
        // if there's a return statement on both if else blocks. this is very
        // specific but is a thing to consider.(Control flow analysis)
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


        context.TheFPM->run(*fn);
        verifyFunction(*fn);

        context.popBlock();
        context.Builder->SetInsertPoint(context.blocks.top()->blockWrapper);

        // context.globals[this->id.name] =
        //         fuseData::globalInfo{fn, this->retType.getType(context), fn->getFunctionType()};

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

    if (fn && find != context.globals.end()) {// -> Only defined functions shall be called

        //Really hard coded for now, but will improve later(to support chained strings etc...)
        if (this->arguments.size() == 1 && fn->getName() == "printf") {
            //Call to C's stdlib functions
            auto val = this->arguments[0]->codeGen(context);
            llvm::CallInst *call = createOutCall(context, val);
            return call;
        }

        if (fn->empty() && !find->second.isCwrapper) {//C functions are marked as empty functions in llvm
                                                      //So we check for the flag
            std::string err = "Prototype found for Function " + this->id.name + " but not defined";
            LogErrorV(err.c_str());
            throw std::runtime_error(err);
        }

        std::vector<Value *> args;
        if (arguments.size() == fn->arg_size()) {
            unsigned i = 0;
            for (auto &arg: this->arguments) {
                Value *V = arg->codeGen(context);
                // size_t vals = find->second.fnType->getNumParams();
                Type *paramType = fn->args().begin()[i].getType();


                // V->getType()->print(llvm::errs());
                // paramType->print(llvm::errs());
                // std::cout << std::endl;

                if (V->getType() != paramType) {

                    std::string err =
                            "Argument type mismatch in Function: " + this->id.name + "\n";
                    LogErrorV(err.c_str());
                    throw std::runtime_error(err);
                }
                if (fn->getName() == "puts") {
                    args.push_back(V);
                } else {
                    Value *bitcastedArg =
                            context.Builder->CreateBitCast(V, paramType);// thanks bolt!
                    args.push_back(bitcastedArg);
                }

                i++;
            }

        } else {
            std::string err =
                    "Argument size mismatch in Function: " + this->id.name + "\n";
            LogErrorV(err.c_str());
            throw std::runtime_error(err);
        }

        auto isVoid = fn->getReturnType()->isVoidTy();

        CallInst *valCall = context.Builder->CreateCall(
                //Dont pass name if void(thx to LLMDEV)
                fn->getFunctionType(), fn, args, (isVoid ? "" : find->first));

        assert(valCall && "Function call failed");

        return valCall;

    } else {

        std::string err = "Function " + this->id.name + " not found or not defined";
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

    } else {
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

    //Emit ir for true block
    for (auto &expr: trueBlock->statements) {
        expr->codeGen(context);
    }

    ifbb = context.Builder->GetInsertBlock();
    context.popBlock();

    if (!ifbb->getTerminator()) {
        context.Builder->CreateBr(mergebb);
    } else {
        earlyReturn = true;
    }


    if (elsebb) {

        fn->insert(fn->end(), elsebb);

        context.Builder->SetInsertPoint(elsebb);
        context.pushBlock(elsebb);

        //Generate else block

        for (auto &stmt: falseBlock->block->statements) {
            stmt->codeGen(context);
        }

        elsebb = context.Builder->GetInsertBlock();
        context.popBlock();

        if (!elsebb->getTerminator()) {
            context.Builder->CreateBr(mergebb);
        }
    }


    fn->insert(fn->end(), mergebb);
    context.Builder->SetInsertPoint(mergebb);
    context.pushBlock(mergebb);


    if (earlyReturn) {

        context.popBlock();
        context.blocks.top()->blockWrapper = mergebb;
        return llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*context.TheContext));
    }

    context.popBlock();
    return llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*context.TheContext));
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
    context.loop_start_bb = loopBB;
    context.next_jumpable_bb = loopEndBB;

    //context.popBlock(); //FIXME: THIS CAUSES AN EXCEPTION & POTENTIALLY AN INFINITE LOOP(SOMEHOW)

    //Emit IR
    context.Builder->SetInsertPoint(loopBB);
    context.pushBlock(loopBB, "loopbb");
    if (!context.blocks.top()->parent) {

        std::string err = "Parent block not found";
        LogErrorV(err.c_str());
        throw std::runtime_error(err);
        return nullptr;
    }

    //emit loop body
    // bool break_loop = false;

    Value *last = nullptr;
    for (auto stmt: loopBlock->statements) {
        last = stmt->codeGen(context);
        if (last && (llvm::dyn_cast<llvm::ReturnInst>(last) || llvm::dyn_cast<llvm::BranchInst>(last))) {
            break;
        }
    }


    if (!context.exit_loop) {


        //Emit step val
        Value *stepVal = nullptr;
        if (this->iteration) {
            stepVal = iteration->codeGen(context);
            if (!stepVal || stepVal->getType()->isVoidTy()) {
                return nullptr;
            }
        } else {

            std::string err = "Invalid for loop iteration expression";
            LogErrorV(err.c_str());
            throw std::runtime_error(err);
            return nullptr;
        }

        if (isAlloca) {

            //Store step expression into current variabl
            Value *next_val = context.Builder->CreateStore(stepVal, allocInst);
            assert(next_val);

        } else {
            //Store step expression into global var
            Value *next_val = context.Builder->CreateStore(stepVal, gVar);
            assert(next_val);
        }
        context.Builder->CreateBr(stepBB);
    }

    //This pops ensure that anything allocated in these blocks is not accessible outside of the loop scope
    context.popBlock();//pop step block
    context.popBlock();//pop loop block
    context.blocks.top()->blockWrapper = loopEndBB;

    context.next_jumpable_bb.reset();//reset jumpable bb
    context.loop_start_bb.reset();   //reset jumpable bb
    context.exit_loop = false;

    context.Builder->SetInsertPoint(loopEndBB);


    return Constant::getNullValue(Type::getInt64Ty(*context.TheContext));
}

llvm::Value *NString::codeGen(CodeGenContext &context) {
    //0. Defs
    auto str = this->value;
    auto charType = llvm::IntegerType::get(*context.TheContext, 8);


    //1. Initialize chars vector
    std::vector<llvm::Constant *> chars(str.length());
    for (unsigned int i = 0; i < str.size(); i++) {
        chars[i] = llvm::ConstantInt::get(charType, str[i]);
    }

    //1b. add a null terminator too
    chars.push_back(llvm::ConstantInt::get(charType, 0));

    //2. Initialize the string from the characters
    auto stringType = llvm::ArrayType::get(charType, chars.size());

    //3. Create the declaration statement
    auto globalDeclaration = new llvm::GlobalVariable(*context.TheModule, stringType, true, llvm::GlobalValue::PrivateLinkage, llvm::ConstantArray::get(stringType, chars), ".str");
    globalDeclaration->setInitializer(llvm::ConstantArray::get(stringType, chars));
    globalDeclaration->setConstant(true);
    globalDeclaration->setLinkage(llvm::GlobalValue::LinkageTypes::PrivateLinkage);
    globalDeclaration->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);

    std::vector<llvm::Constant *> idx;
    for (auto i = 0u; i < chars.size(); i++) {
        idx.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context.TheContext), 0));
    }

    auto strConstant = llvm::ConstantExpr::getGetElementPtr(
            stringType,
            globalDeclaration,
            idx);

    //4. Return a cast to an i8*
    return llvm::ConstantExpr::getBitCast(globalDeclaration, charType->getPointerTo());
}


llvm::Value *NElseStatement::codeGen(CodeGenContext &context) {
    return nullptr;
}

llvm::Value *NBreakStatement::codeGen(CodeGenContext &context) {

    //This solution particularly bad but Ill be using it for now...
    //Instead of trying to cast each node of the ast to check whether its a break statement(It failed for some reason)
    //in a loop and create the br instruction directly there, I opted for this approach, next jumpabable_bb is the basic block
    //where it is safe to jump to while code-generating a block in a loop. The programmer has to be responsible of assigning this
    //jumpable block in the crossroads of a conditional br(see NForstmt)
    if (context.next_jumpable_bb.has_value()) {

        context.exit_loop = true;
        return context.Builder->CreateBr(context.next_jumpable_bb.value());
    }

    std::string err = "Next jumpable block is not accessible or Break statement is not within a loop";
    LogErrorV(err.c_str());
    throw std::runtime_error(err);
}

llvm::Value *NContinueStatement::codeGen(CodeGenContext &context) {

    if (context.loop_start_bb.has_value()) {
        return context.Builder->CreateBr(context.next_jumpable_bb.value());
    }

    std::string err = "Next jumpable block is not accessible or Continue statement is not within a loop";
    LogErrorV(err.c_str());
    throw std::runtime_error(err);
}
