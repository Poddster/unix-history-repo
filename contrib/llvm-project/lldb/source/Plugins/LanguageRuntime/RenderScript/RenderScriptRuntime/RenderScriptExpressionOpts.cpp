//===-- RenderScriptExpressionOpts.cpp --------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <string>

#include "llvm/ADT/None.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#include "clang/Basic/TargetOptions.h"

#include "lldb/Target/Process.h"
#include "lldb/Target/Target.h"
#include "lldb/Utility/Log.h"

#include "RenderScriptExpressionOpts.h"
#include "RenderScriptRuntime.h"
#include "RenderScriptx86ABIFixups.h"

using namespace lldb_private;
using namespace lldb_renderscript;

// [``slang``](https://android.googlesource.com/platform/frameworks/compile/slang),
// the compiler frontend for RenderScript embeds an ARM specific triple in IR
// that is shipped in the app, after generating IR that has some assumptions
// that an ARM device is the target. As the IR is then compiled on a device of
// unknown (at time the IR was generated at least) architecture, when calling
// RenderScript API function as part of debugger expressions, we have to
// perform a fixup pass that removes those assumptions right before the module
// is sent to be generated by the llvm backend.

namespace {
bool registerRSDefaultTargetOpts(clang::TargetOptions &proto,
                                 const llvm::Triple::ArchType &arch) {
  switch (arch) {
  case llvm::Triple::ArchType::x86:
    proto.Triple = "i686--linux-android";
    proto.CPU = "atom";
    proto.Features.push_back("+long64");
    // Fallthrough for common x86 family features
    LLVM_FALLTHROUGH;
  case llvm::Triple::ArchType::x86_64:
    proto.Features.push_back("+mmx");
    proto.Features.push_back("+sse");
    proto.Features.push_back("+sse2");
    proto.Features.push_back("+sse3");
    proto.Features.push_back("+ssse3");
    proto.Features.push_back("+sse4.1");
    proto.Features.push_back("+sse4.2");
    break;
  case llvm::Triple::ArchType::mipsel:
    // pretend this is `arm' for the front-end
    proto.Triple = "armv7-none-linux-android";
    proto.CPU = "";
    proto.Features.push_back("+long64");
    break;
  case llvm::Triple::ArchType::mips64el:
    // pretend this is `aarch64' for the front-end
    proto.Triple = "aarch64-none-linux-android";
    proto.CPU = "";
    break;
  default:
    return false;
  }
  return true;
}
} // end anonymous namespace

bool RenderScriptRuntimeModulePass::runOnModule(llvm::Module &module) {
  bool changed_module = false;
  Log *log(
      GetLogIfAllCategoriesSet(LIBLLDB_LOG_LANGUAGE | LIBLLDB_LOG_EXPRESSIONS));

  std::string err;
  llvm::StringRef real_triple =
      m_process_ptr->GetTarget().GetArchitecture().GetTriple().getTriple();
  const llvm::Target *target_info =
      llvm::TargetRegistry::lookupTarget(real_triple, err);
  if (!target_info) {
    if (log)
      log->Warning("couldn't determine real target architecture: '%s'",
                   err.c_str());
    return false;
  }

  llvm::Optional<llvm::Reloc::Model> reloc_model = llvm::None;
  assert(m_process_ptr && "no available lldb process");
  switch (m_process_ptr->GetTarget().GetArchitecture().GetMachine()) {
  case llvm::Triple::ArchType::x86:
    changed_module |= fixupX86FunctionCalls(module);
    // For some reason this triple gets totally missed by the backend, and must
    // be set manually. There a reference in bcc/Main.cpp about auto feature-
    // detection being removed from LLVM3.5, but I can't see that discussion
    // anywhere public.
    real_triple = "i686--linux-android";
    break;
  case llvm::Triple::ArchType::x86_64:
    changed_module |= fixupX86_64FunctionCalls(module);
    break;
  case llvm::Triple::ArchType::mipsel:
  case llvm::Triple::ArchType::mips64el:
    // No actual IR fixup pass is needed on MIPS, but the datalayout and
    // targetmachine do need to be explicitly set.

    // bcc explicitly compiles MIPS code to use the static relocation model due
    // to an issue with relocations in mclinker. see
    // libbcc/support/CompilerConfig.cpp for details
    reloc_model = llvm::Reloc::Static;
    changed_module = true;
    break;
  case llvm::Triple::ArchType::arm:
  case llvm::Triple::ArchType::aarch64:
    // ARM subtargets need no fixup passes as they are the initial target as
    // generated by the
    // slang compiler frontend.
    break;
  default:
    if (log)
      log->Warning("Ignoring unknown renderscript target");
    return false;
  }

  if (changed_module) {
    llvm::TargetOptions options;
    llvm::TargetMachine *target_machine = target_info->createTargetMachine(
        real_triple, "", "", options, reloc_model);
    assert(target_machine &&
           "failed to identify RenderScriptRuntime target machine");
    // We've been using a triple and datalayout of some ARM variant all along,
    // so we need to let the backend know that this is no longer the case.
    if (log) {
      LLDB_LOGF(log, "%s - Changing RS target triple to '%s'", __FUNCTION__,
                real_triple.str().c_str());
      LLDB_LOGF(
          log, "%s - Changing RS datalayout to '%s'", __FUNCTION__,
          target_machine->createDataLayout().getStringRepresentation().c_str());
    }
    module.setTargetTriple(real_triple);
    module.setDataLayout(target_machine->createDataLayout());
  }
  return changed_module;
}

char RenderScriptRuntimeModulePass::ID = 0;

namespace lldb_private {

bool RenderScriptRuntime::GetOverrideExprOptions(clang::TargetOptions &proto) {
  auto *process = GetProcess();
  assert(process);
  return registerRSDefaultTargetOpts(
      proto, process->GetTarget().GetArchitecture().GetMachine());
}

bool RenderScriptRuntime::GetIRPasses(LLVMUserExpression::IRPasses &passes) {
  if (!m_ir_passes)
    m_ir_passes = new RSIRPasses(GetProcess());
  assert(m_ir_passes);

  passes.EarlyPasses = m_ir_passes->EarlyPasses;
  passes.LatePasses = m_ir_passes->LatePasses;

  return true;
}

namespace lldb_renderscript {

RSIRPasses::RSIRPasses(Process *process) {
  IRPasses();
  assert(process);

  EarlyPasses = std::make_shared<llvm::legacy::PassManager>();
  assert(EarlyPasses);
  EarlyPasses->add(new RenderScriptRuntimeModulePass(process));
}

RSIRPasses::~RSIRPasses() {}

} // namespace lldb_renderscript
} // namespace lldb_private
