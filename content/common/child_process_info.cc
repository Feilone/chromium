// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/child_process_info.h"

#include <limits>

#include "base/atomicops.h"
#include "base/i18n/rtl.h"
#include "base/logging.h"
#include "base/process_util.h"
#include "base/rand_util.h"
#include "base/stringprintf.h"
#include "base/utf_string_conversions.h"

ChildProcessInfo::ChildProcessInfo(content::ProcessType type, int id) :
    type_(type) {
  if (id == -1)
    id_ = GenerateChildProcessUniqueId();
  else
    id_ = id;
}

ChildProcessInfo::ChildProcessInfo(const ChildProcessInfo& original)
    : type_(original.type_),
      name_(original.name_),
      version_(original.version_),
      id_(original.id_),
      process_(original.process_) {
}

ChildProcessInfo::~ChildProcessInfo() {
}

ChildProcessInfo& ChildProcessInfo::operator=(
    const ChildProcessInfo& original) {
  if (&original != this) {
    type_ = original.type_;
    name_ = original.name_;
    version_ = original.version_;
    id_ = original.id_;
    process_ = original.process_;
  }
  return *this;
}

// static
std::string ChildProcessInfo::GetTypeNameInEnglish(
    content::ProcessType type) {
  switch (type) {
    case content::PROCESS_TYPE_BROWSER:
      return "Browser";
    case content::PROCESS_TYPE_RENDERER:
      return "Tab";
    case content::PROCESS_TYPE_PLUGIN:
      return "Plug-in";
    case content::PROCESS_TYPE_WORKER:
      return "Web Worker";
    case content::PROCESS_TYPE_UTILITY:
      return "Utility";
    case content::PROCESS_TYPE_PROFILE_IMPORT:
      return "Profile Import helper";
    case content::PROCESS_TYPE_ZYGOTE:
      return "Zygote";
    case content::PROCESS_TYPE_SANDBOX_HELPER:
      return "Sandbox helper";
    case content::PROCESS_TYPE_NACL_LOADER:
      return "Native Client module";
    case content::PROCESS_TYPE_NACL_BROKER:
      return "Native Client broker";
    case content::PROCESS_TYPE_GPU:
      return "GPU";
    case content::PROCESS_TYPE_PPAPI_PLUGIN:
      return "Pepper Plugin";
    case content::PROCESS_TYPE_PPAPI_BROKER:
      return "Pepper Plugin Broker";
    case content::PROCESS_TYPE_UNKNOWN:
    default:
      DCHECK(false) << "Unknown child process type!";
      return "Unknown";
  }
}

std::string ChildProcessInfo::GenerateRandomChannelID(void* instance) {
  // Note: the string must start with the current process id, this is how
  // child processes determine the pid of the parent.
  // Build the channel ID.  This is composed of a unique identifier for the
  // parent browser process, an identifier for the child instance, and a random
  // component. We use a random component so that a hacked child process can't
  // cause denial of service by causing future named pipe creation to fail.
  return base::StringPrintf("%d.%p.%d",
                            base::GetCurrentProcId(), instance,
                            base::RandInt(0, std::numeric_limits<int>::max()));
}

// static
int ChildProcessInfo::GenerateChildProcessUniqueId() {
  // This function must be threadsafe.
  static base::subtle::Atomic32 last_unique_child_id = 0;
  return base::subtle::NoBarrier_AtomicIncrement(&last_unique_child_id, 1);
}
