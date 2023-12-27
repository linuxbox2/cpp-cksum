// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

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
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>

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

    std::string file_name = "file-200b";
    std::cout << "put " << file_name << " via http" << std::endl;
    putObjectFromFile(*http_client, file_name, "object_out");

    std::cout << "put " << file_name << " via http w/SHA256 checksum" << std::endl;
    putObjectFromFile(*http_client, file_name, "object_out", ChecksumAlgorithm::SHA256);
  } catch (...) {
  }

  Aws::ShutdownAPI(options);
  
  return 0;
}
