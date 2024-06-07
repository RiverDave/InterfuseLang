// This module defines the core functions supported in Interfuse
// David Rivera - 14/05/2024

#include "IR.h"
#include "intercore.hpp"
#include <cassert>
#include <cstdio>
#include <llvm/IR/Type.h>

#ifndef INTERNALS_HPP
#define INTERNALS_HPP

//Taken from toycompiler for now by ISEGAL

using namespace llvm;

extern void printi(long long val);


void generateFuseOut(CodeGenContext &context) {

    llvm::Function *fn = context.TheModule->getFunction("printf");
    assert(fn != nullptr && "printf function not found");

    context.globals["printf"] = fuseData::globalInfo{fn, fn->getReturnType(), fn->getFunctionType()};
}

void generateFusePuts(CodeGenContext &context) {

    llvm::Function *fn = context.TheModule->getFunction("puts");
    assert(fn != nullptr && "puts function not found");

    // std::vector<llvm::Value*>printfArgs;
    // auto val = context.Builder->CreateGlobalStringPtr(llvm::StringRef("lld%"), ".str", 0, context.TheModule.get() );
    // printfArgs.push_back(val);

    context.globals["puts"] = fuseData::globalInfo{fn, fn->getReturnType(), fn->getFunctionType()};
}

llvm::CallInst* createOutCall(CodeGenContext &context, llvm::Value *val) {

    llvm::Function *fn = context.TheModule->getFunction("printf");
    assert(fn != nullptr && "printf function not found aka not generated");

    std::vector<llvm::Value *> printfArgs;

    //Create constant str for printf
    if (val->getType()->isIntegerTy(64)) { 

        printfArgs.push_back(context.Builder->CreateGlobalStringPtr(llvm::StringRef("%lld\n"), ".fmtstr", 0, context.TheModule.get()));
    } else if (val->getType() == llvm::Type::getInt8Ty(*context.TheContext)->getPointerTo()){

        printfArgs.push_back(context.Builder->CreateGlobalStringPtr(llvm::StringRef("%s\n"), ".fmtstr", 0, context.TheModule.get()));
    } else if (val->getType()->isDoubleTy()) {

        printfArgs.push_back(context.Builder->CreateGlobalStringPtr(llvm::StringRef("%f\n"), ".fmtstr", 0, context.TheModule.get()));
    } else {
        printfArgs.push_back(context.Builder->CreateGlobalStringPtr(llvm::StringRef("%lld\n"), ".fmtstr", 0, context.TheModule.get()));
    }

    printfArgs.push_back(val);
    return context.Builder->CreateCall(fn, printfArgs, "printfCall");
}


void generateFunctionsIR(CodeGenContext &context) {

    context.TheModule->getOrInsertFunction("printf",
                                           llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(*context.TheContext),
                                                                   llvm::Type::getInt8Ty(*context.TheContext)->getPointerTo(), true));

    context.TheModule->getOrInsertFunction("puts",
                                           llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(*context.TheContext),
                                                                   llvm::Type::getInt8Ty(*context.TheContext)->getPointerTo(), false));
}

void createCoreFunctions(CodeGenContext &context) {


    //TODO: Simplify the overloads of these functions
    // llvm::Function *printfFn = createCPrint(context, llvm::Type::getInt64Ty(*context.TheContext));
    // createEchoFunction(context, printfFn, llvm::Type::getInt64Ty(*context.TheContext), "out", "%lld\n");

    //Generate C functions
    generateFunctionsIR(context);
    generateFuseOut(context);
    generateFusePuts(context);

    //Generate its alias in Interfuse
}

#endif
