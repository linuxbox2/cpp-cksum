// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include <aws/common/assert.h>
#include <aws/core/http/Scheme.h>
#include <aws/core/utils/memory/stl/AWSAllocator.h>
#include <aws/s3/S3ServiceClientModel.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/ChecksumAlgorithm.h>
#include <cstdint>
#include <memory>
#include <iostream>
#include <fstream>
#include <format>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>

/* for TransferManager */
#include <aws/core/utils/threading/Executor.h>
#include <aws/transfer/TransferManager.h>
#include <aws/transfer/TransferHandle.h>

#include "s3_common.h"

using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Http;
using namespace Aws::S3;
using namespace Aws::S3::Model;

static Aws::String bucket_name = "sheik";
static Aws::String object_name = "jerbuti";
static Aws::String access_key = "0555b35654ad1656d804";
static Aws::String secret_key = "h7GhxuBLTrlhVUyxSPUKUV8r/2EI4ngqJxD7iBdBYLhwluN30JaT3Q==";

static Aws::String http_endpoint = "http://192.168.111.1:8000";
static Aws::String ssl_endpoint = "https://192.168.111.1:8443";

static constexpr uint64_t request_timeout_ms = 1000 * 60 * 60;

using namespace s3_common;

int main(int argc, char* argv[])
{
  Aws::SDKOptions options;
  options.loggingOptions.logLevel = Utils::Logging::LogLevel::Debug;
  Aws::InitAPI(options); // Should only be called once.

  std::cout << "s3_cksum" << std::endl;

  try {
    auto http_client = get_http_client(false);
    std::cout << "http result:" << std::endl;
    list_objects(*http_client);

    auto ssl_client = get_http_client(true /* ssl */);
    std::cout << "ssl result:" << std::endl;

    create_bucket(*ssl_client, bucket_name);

    list_objects(*ssl_client);

#if 0
    std::string file_name = "file-200b";
    std::cout << "put " << file_name << " via http" << std::endl;
    putObjectFromFile2(*http_client, file_name, "object_out");

    std::cout << "put " << file_name << " via http w/SHA256 checksum" << std::endl;
    putObjectFromFile2(*http_client, file_name, "object_out", ChecksumAlgorithm::SHA256);

    file_name = "file-200b";
    tryTransferManager(http_client, file_name, "object_out");
#endif
    
    std::string dolor =
      R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.)";

    for (auto algo : {ChecksumAlgorithm::CRC32, ChecksumAlgorithm::CRC32C, ChecksumAlgorithm::SHA1,
						   ChecksumAlgorithm::SHA256}) {
      std::cout << "put text="
		<< std::format("\"{}\"", dolor)
                << " via http checksum=" << int(algo)
                << std::endl;
      putObjectFromString(*http_client, dolor, "object_out", algo);
    }
  } catch (...) {
  }

  Aws::ShutdownAPI(options);
  
  return 0;
}
