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

uint64_t request_timeout_ms = 1000 * 60 * 60;

std::shared_ptr<S3Client> get_http_client(bool ssl = false)
{
  Aws::Client::ClientConfiguration config;

  config.verifySSL = false;
  config.enableEndpointDiscovery = false;

  config.httpRequestTimeoutMs = request_timeout_ms;
  config.region = "us-east-1";

  if (ssl) {
    config.scheme = Aws::Http::Scheme::HTTPS;
    config.endpointOverride = ssl_endpoint;
  } else {
    config.scheme = Aws::Http::Scheme::HTTP;
    config.endpointOverride = http_endpoint;
  }

  Aws::Auth::AWSCredentials credentials;
  credentials.SetAWSAccessKeyId(Aws::String(access_key));
  credentials.SetAWSSecretKey(Aws::String(secret_key));

  std::shared_ptr<S3EndpointProvider> endpointProvider =
      Aws::MakeShared<S3EndpointProvider>("s3 endpoint tag");

  bool sign_payloads = true;
  bool use_virtual_addressing = false;

  S3Client client(credentials, endpointProvider, config);

  std::shared_ptr<S3Client> s3 =
    Aws::MakeShared<S3Client>("s3 client tag", credentials, endpointProvider, config);

  return s3;
} /* get_http_client */

void putObjectFromFile(S3Client& s3, String in_file_path, String out_key_name,
		       ChecksumAlgorithm algo = ChecksumAlgorithm::NOT_SET) {
  Aws::S3::Model::PutObjectRequest req;
  req.SetBucket(bucket_name);
  req.SetKey(out_key_name);
  req.SetChecksumAlgorithm(algo);

  std::shared_ptr<Aws::IOStream> inputData =
    Aws::MakeShared<Aws::FStream>("SampleAllocationTag",
				  in_file_path.c_str(),
				  std::ios_base::in | std::ios_base::binary);

    if (!*inputData) {
        std::cerr << "Error unable to read file " << in_file_path << std::endl;
        return;
    }

    req.SetBody(inputData);

    Aws::S3::Model::PutObjectOutcome result = s3.PutObject(req);
    if (!result.IsSuccess()) {
        std::cerr << "Error: PutObject: " <<
                  result.GetError().GetMessage() << std::endl;
    } else {
      std::cout << "Added object " << out_key_name << " to bucket "
		<< bucket_name << std::endl;
    }
} /* putObjectFromFile */

void putObjectFromString(S3Client& s3, String body, String out_key_name,
			 ChecksumAlgorithm algo = ChecksumAlgorithm::NOT_SET)
{
  // Checksums in request body using aws-chunked trailer
  std::shared_ptr<Aws::IOStream> objectStream = Aws::MakeShared<Aws::StringStream>("s3 ostream");
  *objectStream << body;

  PutObjectRequest req;
  req.SetBucket(bucket_name);
  req.SetKey(out_key_name);
  req.SetBody(objectStream);
  req.SetChecksumAlgorithm(algo);
  auto result = s3.PutObject(req);
} /* putObjectfromString */

void tryTransferManager(std::shared_ptr<S3Client> s3, String in_file_path, String out_key_name,
		       ChecksumAlgorithm algo = ChecksumAlgorithm::NOT_SET)
{
  auto executor = Aws::MakeShared<Aws::Utils::Threading::PooledThreadExecutor>("executor", 25);
  Aws::Transfer::TransferManagerConfiguration transfer_config(executor.get());
  transfer_config.s3Client = s3;

  auto transfer_manager = Aws::Transfer::TransferManager::Create(transfer_config);
  auto uploadHandle =
    transfer_manager->UploadFile(in_file_path, bucket_name, out_key_name,
				 "text/plain", Aws::Map<Aws::String, Aws::String>());
  uploadHandle->WaitUntilFinished();
  bool success = uploadHandle->GetStatus() == Transfer::TransferStatus::COMPLETED;
  if (!success) {
    auto err = uploadHandle->GetLastError();
    std::cout << "File upload failed:  "<< err.GetMessage() << std::endl;
  } else
    {
      std::cout << "File upload finished." << std::endl;

      // Verify that the upload retrieved the expected amount of data.
      assert(uploadHandle->GetBytesTotalSize() == uploadHandle->GetBytesTransferred());
    }
} /* tryTransferManager */

void list_objects(S3Client& s3)
{
  Aws::S3::Model::ListObjectsRequest req;
  req.WithBucket(bucket_name);

  auto result = s3.ListObjects(req);

  if (!result.IsSuccess()) {
        std::cerr << "Error: ListObjects: " <<
	  result.GetError().GetMessage() << std::endl;
  }
  else {
    Aws::Vector<Aws::S3::Model::Object> objects
      = result.GetResult().GetContents();

    for (Aws::S3::Model::Object &object: objects) {
      std::cout << " name: " << object.GetKey()
		<< " size: " << object.GetSize()
		<< std::endl;
    }    
  }
} /* list_objects */

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
