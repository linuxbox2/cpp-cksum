// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#pragma once

#include <memory>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/ChecksumAlgorithm.h>

namespace s3_common {

  std::shared_ptr<Aws::S3::S3Client> get_http_client(bool ssl = false);

  void putObjectFromFile(Aws::S3::S3Client& s3, Aws::String in_file_path,
			 Aws::String out_key_name,
			 Aws::S3::Model::ChecksumAlgorithm algo =
			 Aws::S3::Model::ChecksumAlgorithm::NOT_SET);

  void putObjectFromString(Aws::S3::S3Client& s3, Aws::String body,
			   Aws::String out_key_name,
			   Aws::S3::Model::ChecksumAlgorithm algo =
			   Aws::S3::Model::ChecksumAlgorithm::NOT_SET);

  void tryTransferManager(std::shared_ptr<Aws::S3::S3Client> s3,
			  Aws::String in_file_path, Aws::String out_key_name,
			  Aws::S3::Model::ChecksumAlgorithm algo =
			  Aws::S3::Model::ChecksumAlgorithm::NOT_SET);

  void create_bucket(Aws::S3::S3Client& s3, Aws::String bucket_name);

  void list_objects(Aws::S3::S3Client& s3);

  void delete_object(Aws::S3::S3Client& s3, Aws::String bucket_name,
		     Aws::String object_name);

} // namespace s3_common
