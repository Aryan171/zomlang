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
        
        llvm::Error addModule(llvm::orc::ThreadSafeModule TSM, llvm::orc::ResourceTrackerSP RT = nullptr) {
            if (RT) {
                return JIT->addIRModule(RT, std::move(TSM));
            }
            return JIT->addIRModule(std::move(TSM));
        }
        
        llvm::Expected<llvm::orc::ExecutorAddr> lookup(llvm::StringRef Name) {
            return JIT->lookup(Name);
        }
        
        llvm::orc::JITDylib& getMainJITDylib() { 
            return JIT->getMainJITDylib();
        }
    };
}