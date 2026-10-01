//===--- SystemZ.cpp - SystemZ Helpers for Tools ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SystemZ.h"
#include "clang/Config/config.h"
#include "clang/Options/Options.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Option/ArgList.h"
#include "llvm/TargetParser/Host.h"

using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang;
using namespace llvm::opt;

systemz::FloatABI systemz::getSystemZFloatABI(const Driver &D,
                                              const ArgList &Args) {
  // Hard float is the default.
  systemz::FloatABI ABI = systemz::FloatABI::Hard;
  if (Args.hasArg(options::OPT_mfloat_abi_EQ))
    D.Diag(diag::err_drv_unsupported_opt)
      << Args.getLastArg(options::OPT_mfloat_abi_EQ)->getAsString(Args);

  if (Arg *A =
          Args.getLastArg(options::OPT_msoft_float, options::OPT_mhard_float))
    if (A->getOption().matches(options::OPT_msoft_float))
      ABI = systemz::FloatABI::Soft;

  return ABI;
}

std::string systemz::getSystemZTargetCPU(const ArgList &Args,
                                         const llvm::Triple &T) {
  if (const Arg *A = Args.getLastArg(options::OPT_march_EQ)) {
    llvm::StringRef CPUName = A->getValue();

    if (CPUName == "native") {
      std::string CPU = std::string(llvm::sys::getHostCPUName());
      if (!CPU.empty() && CPU != "generic")
        return CPU;
      else
        return "";
    }

    return std::string(CPUName);
  }
  if (T.isOSzOS())
    return "zEC12";
  return CLANG_SYSTEMZ_DEFAULT_ARCH;
}

namespace {
struct KernelABITweak {
  llvm::StringLiteral Name;
  // A ','-separated list of prerequisites, each of which is a '|'-separated
  // list of alternatives.
  llvm::StringLiteral Requires;
};
} // namespace

static const KernelABITweak KernelABITweaks[] = {
    {"struct-ret", ""},
    {"struct-arg", ""},
    {"int128", ""},
};

static void
getSystemZKernelABIFeatures(const Driver &D, const llvm::Triple &Triple,
                            const ArgList &Args, systemz::FloatABI FloatABI,
                            std::vector<llvm::StringRef> &Features) {
  const Arg *A = Args.getLastArg(options::OPT_mexperimental_kernel_abi_EQ);
  if (!A)
    return;
  if (Triple.isOSzOS()) {
    D.Diag(diag::err_drv_unsupported_opt_for_target)
        << A->getSpelling() << Triple.str();
    return;
  }
  if (FloatABI != systemz::FloatABI::Soft) {
    D.Diag(diag::err_drv_argument_only_allowed_with)
        << A->getAsString(Args) << "-msoft-float";
    return;
  }

  llvm::SmallSet<StringRef, 8> Enabled;
  for (StringRef Value : A->getValues()) {
    if (llvm::none_of(KernelABITweaks, [&](const KernelABITweak &T) {
          return T.Name == Value;
        })) {
      D.Diag(diag::err_drv_unsupported_option_argument)
          << A->getSpelling() << Value;
      return;
    }
    Enabled.insert(Value);
  }

  for (const KernelABITweak &T : KernelABITweaks) {
    if (!Enabled.contains(T.Name))
      continue;
    SmallVector<StringRef, 2> Requires;
    T.Requires.split(Requires, ',', /*MaxSplit=*/-1, /*KeepEmpty=*/false);
    for (StringRef Req : Requires) {
      SmallVector<StringRef, 2> Alternatives;
      Req.split(Alternatives, '|');
      if (llvm::none_of(Alternatives,
                        [&](StringRef Alt) { return Enabled.contains(Alt); }))
        D.Diag(diag::err_drv_argument_only_allowed_with)
            << T.Name << llvm::join(Alternatives, "' or '");
    }
    Features.push_back(
        Args.MakeArgString("+experimental-kernel-abi-" + T.Name));
  }
}

void systemz::getSystemZTargetFeatures(const Driver &D,
                                       const llvm::Triple &Triple,
                                       const ArgList &Args,
                                       std::vector<llvm::StringRef> &Features) {
  // -m(no-)htm overrides use of the transactional-execution facility.
  if (Arg *A = Args.getLastArg(options::OPT_mhtm, options::OPT_mno_htm)) {
    if (A->getOption().matches(options::OPT_mhtm))
      Features.push_back("+transactional-execution");
    else
      Features.push_back("-transactional-execution");
  }
  // -m(no-)vx overrides use of the vector facility.
  if (Arg *A = Args.getLastArg(options::OPT_mvx, options::OPT_mno_vx)) {
    if (A->getOption().matches(options::OPT_mvx))
      Features.push_back("+vector");
    else
      Features.push_back("-vector");
  }

  systemz::FloatABI FloatABI = systemz::getSystemZFloatABI(D, Args);
  if (FloatABI == systemz::FloatABI::Soft)
    Features.push_back("+soft-float");

  if (const Arg *A = Args.getLastArg(options::OPT_munaligned_symbols,
                                     options::OPT_mno_unaligned_symbols)) {
    if (A->getOption().matches(options::OPT_munaligned_symbols))
      Features.push_back("+unaligned-symbols");
    else
      Features.push_back("-unaligned-symbols");
  }

  getSystemZKernelABIFeatures(D, Triple, Args, FloatABI, Features);
}
