#include "cache_builder.h"
#include "node_native_module.h"
#include "util.h"

#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <cstdlib>

namespace node {
namespace native_module {

using v8::Context;
using v8::Function;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::ScriptCompiler;

static std::string GetDefName(const std::string& id) {
  char buf[64] = {0};
  size_t size = id.size();
  CHECK_LT(size, sizeof(buf));
  for (size_t i = 0; i < size; ++i) {
    char ch = id[i];
    buf[i] = (ch == '-' || ch == '/') ? '_' : ch;
  }
  return buf;
}

static std::string FormatSize(size_t size) {
  char buf[64] = {0};
  if (size < 1024) {
    snprintf(buf, sizeof(buf), "%.2fB", static_cast<double>(size));
  } else if (size < 1024 * 1024) {
    snprintf(buf, sizeof(buf), "%.2fKB", static_cast<double>(size / 1024));
  } else {
    snprintf(
        buf, sizeof(buf), "%.2fMB", static_cast<double>(size / 1024 / 1024));
  }
  return buf;
}

static std::string GetDefinition(const std::string& id,
                                 size_t size,
                                 const uint8_t* data) {
  std::stringstream ss;
  ss << "static const uint8_t " << GetDefName(id) << "[] = {\n";
  for (size_t i = 0; i < size; ++i) {
    uint8_t ch = data[i];
    ss << std::to_string(ch) << (i == size - 1 ? '\n' : ',');
  }
  ss << "};";
  return ss.str();
}

static void GetInitializer(const std::string& id, std::stringstream& ss) {
  std::string def_name = GetDefName(id);
  ss << "  code_cache.emplace(\n";
  ss << "    \"" << id << "\",\n";
  ss << "    std::make_unique<v8::ScriptCompiler::CachedData>(\n";
  ss << "      " << def_name << ",\n";
  ss << "      static_cast<int>(arraysize(" << def_name << ")), policy\n";
  ss << "    )\n";
  ss << "  );";
}

static std::string GenerateCodeCache(
    const std::map<std::string, ScriptCompiler::CachedData*>& data,
    bool log_progress) {
  std::stringstream ss;
  ss << R"(#include <cinttypes>
#include "node_native_module_env.h"

// This file is generated by tools/mkcodecache
// and is used when configure is run with \`--code-cache-path\`

namespace node {
namespace native_module {
)";

  size_t total = 0;
  for (const auto& x : data) {
    const std::string& id = x.first;
    ScriptCompiler::CachedData* cached_data = x.second;
    total += cached_data->length;
    std::string def = GetDefinition(id, cached_data->length, cached_data->data);
    ss << def << "\n\n";
    if (log_progress) {
      std::cout << "Generated cache for " << id
                << ", size = " << FormatSize(cached_data->length)
                << ", total = " << FormatSize(total) << "\n";
    }
  }

  ss << R"(void NativeModuleEnv::InitializeCodeCache() {
  NativeModuleCacheMap& code_cache =
      *NativeModuleLoader::GetInstance()->code_cache();
  CHECK(code_cache.empty());
  auto policy = v8::ScriptCompiler::CachedData::BufferPolicy::BufferNotOwned;
)";

  for (const auto& x : data) {
    GetInitializer(x.first, ss);
    ss << "\n\n";
  }

  ss << R"(
}

}  // namespace native_module
}  // namespace node
)";
  return ss.str();
}

std::string CodeCacheBuilder::Generate(Local<Context> context) {
  NativeModuleLoader* loader = NativeModuleLoader::GetInstance();
  std::vector<std::string> ids = loader->GetModuleIds();

  std::map<std::string, ScriptCompiler::CachedData*> data;

  for (const auto& id : ids) {
    // TODO(joyeecheung): we can only compile the modules that can be
    // required here because the parameters for other types of builtins
    // are still very flexible. We should look into auto-generating
    // the paramters from the source somehow.
    if (loader->CanBeRequired(id.c_str())) {
      NativeModuleLoader::Result result;
      USE(loader->CompileAsModule(context, id.c_str(), &result));
      ScriptCompiler::CachedData* cached_data =
          loader->GetCodeCache(id.c_str());
      if (cached_data == nullptr) {
        // TODO(joyeecheung): display syntax errors
        std::cerr << "Failed to complile " << id << "\n";
      } else {
        data.emplace(id, cached_data);
      }
    }
  }

  char env_buf[32];
  size_t env_size = sizeof(env_buf);
  int ret = uv_os_getenv("NODE_DEBUG", env_buf, &env_size);
  bool log_progress = false;
  if (ret == 0 && strcmp(env_buf, "mkcodecache") == 0) {
    log_progress = true;
  }
  return GenerateCodeCache(data, log_progress);
}

}  // namespace native_module
}  // namespace node
