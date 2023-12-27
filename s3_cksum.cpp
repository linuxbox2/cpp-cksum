// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include <iostream>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>

using namespace Aws;
using namespace Aws::Auth;

int main(int argc, char* argv[])
{
    Aws::SDKOptions options;
    options.loggingOptions.logLevel = Utils::Logging::LogLevel::Debug;
    Aws::InitAPI(options); // Should only be called once.
  
  using namespace std;

  cout << "s3_cksum" << endl;

  return 0;
}
