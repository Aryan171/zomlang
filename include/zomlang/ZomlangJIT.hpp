#pragma once

#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Support/Error.h"
#include <memory>

namespace zomlang {
    class ZomlangJIT {
    private:
        std::unique_ptr<llvm::orc::LLJIT> JIT;

    public:
        ZomlangJIT(std::unique_ptr<llvm::orc::LLJIT> J) : JIT(std::move(J)) {}
        
        static llvm::Expected<std::unique_ptr<ZomlangJIT>> Create() {
            auto JITOrErr = llvm::orc::LLJITBuilder().create();
            if (!JITOrErr) {
                return JITOrErr.takeError();
            }

            return std::make_unique<ZomlangJIT>(std::move(*JITOrErr));
        }

        const llvm::DataLayout& getDataLayout() const {
            return JIT->getDataLayout();
        }
        
        llvm::Error addModule(std::unique_ptr<llvm::Module> M, std::unique_ptr<llvm::LLVMContext> C) {
            return JIT->addIRModule(
                llvm::orc::ThreadSafeModule(std::move(M), std::move(C))
            );
        }
        
        llvm::Expected<llvm::orc::ExecutorAddr> lookup(llvm::StringRef Name) {
            return JIT->lookup(Name);
        }
    };
}