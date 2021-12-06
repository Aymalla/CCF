// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the Apache 2.0 License.

#pragma once

#include "common/enclave_interface_types.h"
#include "consensus/consensus_types.h"
#include "crypto/curve.h"
#include "ds/logger.h"
#include "ds/oversized.h"
#include "ds/unit_strings.h"
#include "enclave/consensus_type.h"
#include "enclave/reconfiguration_type.h"
#include "node/config.h"
#include "node/members.h"
#include "node/node_info_network.h"

#include <optional>
#include <string>
#include <vector>

namespace logger
{
#ifdef VERBOSE_LOGGING
  DECLARE_JSON_ENUM(
    Level,
    {{Level::TRACE, "trace"},
     {Level::DEBUG, "debug"},
     {Level::INFO, "info"},
     {Level::FAIL, "fail"},
     {Level::FATAL, "fatal"}});
#else
  DECLARE_JSON_ENUM(
    Level,
    {{Level::INFO, "info"}, {Level::FAIL, "fail"}, {Level::FATAL, "fatal"}});
#endif
}

DECLARE_JSON_ENUM(
  StartType,
  {{StartType::Start, "start"},
   {StartType::Join, "join"},
   {StartType::Recover, "recover"}});

struct EnclaveConfig
{
  uint8_t* to_enclave_buffer_start;
  size_t to_enclave_buffer_size;
  ringbuffer::Offsets* to_enclave_buffer_offsets;

  uint8_t* from_enclave_buffer_start;
  size_t from_enclave_buffer_size;
  ringbuffer::Offsets* from_enclave_buffer_offsets;

  oversized::WriterConfig writer_config = {};
};

struct CCFConfig
{
  size_t worker_threads = 0;
  consensus::Configuration consensus = {};
  ccf::NodeInfoNetwork network = {};

  struct NodeCertificateInfo
  {
    std::string subject_name = "CN=CCF Node";
    std::vector<std::string> subject_alt_names = {};
    crypto::CurveID curve_id = crypto::CurveID::SECP384R1;
    size_t initial_validity_days = 1;

    bool operator==(const NodeCertificateInfo&) const = default;
  };
  NodeCertificateInfo node_certificate = {};

  struct Intervals
  {
    size_t signature_interval_size = 5000;
    size_t signature_interval_duration_ms = 1000;

    bool operator==(const Intervals&) const = default;
  };
  Intervals intervals = {};

  struct JWT
  {
    size_t key_refresh_interval_s = 1800;

    bool operator==(const JWT&) const = default;
  };
  JWT jwt = {};
};

DECLARE_JSON_TYPE_WITH_OPTIONAL_FIELDS(CCFConfig::NodeCertificateInfo);
DECLARE_JSON_REQUIRED_FIELDS(CCFConfig::NodeCertificateInfo)
DECLARE_JSON_OPTIONAL_FIELDS(
  CCFConfig::NodeCertificateInfo,
  subject_name,
  subject_alt_names,
  curve_id,
  initial_validity_days);

DECLARE_JSON_TYPE_WITH_OPTIONAL_FIELDS(CCFConfig::Intervals);
DECLARE_JSON_REQUIRED_FIELDS(CCFConfig::Intervals);
DECLARE_JSON_OPTIONAL_FIELDS(
  CCFConfig::Intervals,
  signature_interval_size,
  signature_interval_duration_ms);

DECLARE_JSON_TYPE_WITH_OPTIONAL_FIELDS(CCFConfig::JWT);
DECLARE_JSON_REQUIRED_FIELDS(CCFConfig::JWT);
DECLARE_JSON_OPTIONAL_FIELDS(CCFConfig::JWT, key_refresh_interval_s);

DECLARE_JSON_TYPE_WITH_OPTIONAL_FIELDS(CCFConfig);
DECLARE_JSON_REQUIRED_FIELDS(CCFConfig, network);
DECLARE_JSON_OPTIONAL_FIELDS(
  CCFConfig, worker_threads, node_certificate, consensus, intervals, jwt);

struct StartupConfig : CCFConfig
{
  // Only if joining or recovering
  std::vector<uint8_t> startup_snapshot = {};

  std::optional<size_t> startup_snapshot_evidence_seqno_for_1_x = std::nullopt;

  std::string startup_host_time;
  size_t snapshot_tx_interval = 10'000;

  struct Start
  {
    std::vector<ccf::NewMember> members;
    std::string constitution;
    ccf::ServiceConfiguration service_configuration;

    bool operator==(const Start& other) const = default;
  };
  Start start = {};

  struct Join
  {
    ccf::NodeInfoNetwork_v2::NetAddress target_rpc_address;
    size_t timer_ms = 1000;
    std::vector<uint8_t> network_cert = {};
  };
  Join join = {};
};

DECLARE_JSON_TYPE(StartupConfig::Start);
DECLARE_JSON_REQUIRED_FIELDS(
  StartupConfig::Start, members, constitution, service_configuration);

DECLARE_JSON_TYPE(StartupConfig::Join);
DECLARE_JSON_REQUIRED_FIELDS(
  StartupConfig::Join, target_rpc_address, timer_ms, network_cert);

DECLARE_JSON_TYPE_WITH_BASE_AND_OPTIONAL_FIELDS(StartupConfig, CCFConfig);
DECLARE_JSON_REQUIRED_FIELDS(
  StartupConfig,
  startup_snapshot,
  startup_host_time,
  snapshot_tx_interval,
  start,
  join);
DECLARE_JSON_OPTIONAL_FIELDS(
  StartupConfig, startup_snapshot_evidence_seqno_for_1_x);
