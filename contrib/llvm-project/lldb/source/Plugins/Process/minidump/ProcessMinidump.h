//===-- ProcessMinidump.h ---------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef liblldb_ProcessMinidump_h_
#define liblldb_ProcessMinidump_h_

#include "MinidumpParser.h"
#include "MinidumpTypes.h"

#include "lldb/Target/Process.h"
#include "lldb/Target/StopInfo.h"
#include "lldb/Target/Target.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/Status.h"

#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"


namespace lldb_private {

namespace minidump {

class ProcessMinidump : public Process {
public:
  static lldb::ProcessSP CreateInstance(lldb::TargetSP target_sp,
                                        lldb::ListenerSP listener_sp,
                                        const FileSpec *crash_file_path);

  static void Initialize();

  static void Terminate();

  static ConstString GetPluginNameStatic();

  static const char *GetPluginDescriptionStatic();

  ProcessMinidump(lldb::TargetSP target_sp, lldb::ListenerSP listener_sp,
                  const FileSpec &core_file, lldb::DataBufferSP code_data);

  ~ProcessMinidump() override;

  bool CanDebug(lldb::TargetSP target_sp,
                bool plugin_specified_by_name) override;

  CommandObject *GetPluginCommandObject() override;

  Status DoLoadCore() override;

  DynamicLoader *GetDynamicLoader() override { return nullptr; }

  ConstString GetPluginName() override;

  uint32_t GetPluginVersion() override;

  SystemRuntime *GetSystemRuntime() override { return nullptr; }

  Status DoDestroy() override;

  void RefreshStateAfterStop() override;

  bool IsAlive() override;

  bool WarnBeforeDetach() const override;

  size_t ReadMemory(lldb::addr_t addr, void *buf, size_t size,
                    Status &error) override;

  size_t DoReadMemory(lldb::addr_t addr, void *buf, size_t size,
                      Status &error) override;

  ArchSpec GetArchitecture();

  Status GetMemoryRegionInfo(lldb::addr_t load_addr,
                             MemoryRegionInfo &range_info) override;

  Status GetMemoryRegions(
      lldb_private::MemoryRegionInfos &region_list) override;

  bool GetProcessInfo(ProcessInstanceInfo &info) override;

  Status WillResume() override {
    Status error;
    error.SetErrorStringWithFormat(
        "error: %s does not support resuming processes",
        GetPluginName().GetCString());
    return error;
  }

  llvm::Optional<MinidumpParser> m_minidump_parser;

protected:
  void Clear();

  bool UpdateThreadList(ThreadList &old_thread_list,
                        ThreadList &new_thread_list) override;

  void ReadModuleList();

  JITLoaderList &GetJITLoaders() override;

private:
  FileSpec m_core_file;
  lldb::DataBufferSP m_core_data;
  llvm::ArrayRef<minidump::Thread> m_thread_list;
  const minidump::ExceptionStream *m_active_exception;
  lldb::CommandObjectSP m_command_sp;
  bool m_is_wow64;
};

} // namespace minidump
} // namespace lldb_private

#endif // liblldb_ProcessMinidump_h_
