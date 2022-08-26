// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: raft_message.proto

#include "raft_message.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
namespace hit_consistency {
class RequestVoteRequestDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<RequestVoteRequest> _instance;
} _RequestVoteRequest_default_instance_;
class RequestVoteResponseDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<RequestVoteResponse> _instance;
} _RequestVoteResponse_default_instance_;
}  // namespace hit_consistency
static void InitDefaultsscc_info_RequestVoteRequest_raft_5fmessage_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::hit_consistency::_RequestVoteRequest_default_instance_;
    new (ptr) ::hit_consistency::RequestVoteRequest();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::hit_consistency::RequestVoteRequest::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_RequestVoteRequest_raft_5fmessage_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_RequestVoteRequest_raft_5fmessage_2eproto}, {}};

static void InitDefaultsscc_info_RequestVoteResponse_raft_5fmessage_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::hit_consistency::_RequestVoteResponse_default_instance_;
    new (ptr) ::hit_consistency::RequestVoteResponse();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::hit_consistency::RequestVoteResponse::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_RequestVoteResponse_raft_5fmessage_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_RequestVoteResponse_raft_5fmessage_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_raft_5fmessage_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_raft_5fmessage_2eproto = nullptr;
static const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* file_level_service_descriptors_raft_5fmessage_2eproto[1];

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_raft_5fmessage_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, server_id_),
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, peer_id_),
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, term_),
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, last_log_term_),
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, last_log_index_),
  0,
  1,
  2,
  3,
  4,
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteResponse, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteResponse, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteResponse, term_),
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteResponse, granted_),
  0,
  1,
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 10, sizeof(::hit_consistency::RequestVoteRequest)},
  { 15, 22, sizeof(::hit_consistency::RequestVoteResponse)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::hit_consistency::_RequestVoteRequest_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::hit_consistency::_RequestVoteResponse_default_instance_),
};

const char descriptor_table_protodef_raft_5fmessage_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\022raft_message.proto\022\017hit_consistency\"u\n"
  "\022RequestVoteRequest\022\021\n\tserver_id\030\001 \001(\t\022\017"
  "\n\007peer_id\030\002 \001(\t\022\014\n\004term\030\003 \001(\003\022\025\n\rlast_lo"
  "g_term\030\004 \001(\003\022\026\n\016last_log_index\030\005 \001(\003\"4\n\023"
  "RequestVoteResponse\022\014\n\004term\030\001 \001(\003\022\017\n\007gra"
  "nted\030\002 \001(\0102e\n\013RaftService\022V\n\007prevote\022#.h"
  "it_consistency.RequestVoteRequest\032$.hit_"
  "consistency.RequestVoteResponse\"\000B\003\200\001\001"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_raft_5fmessage_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_raft_5fmessage_2eproto_sccs[2] = {
  &scc_info_RequestVoteRequest_raft_5fmessage_2eproto.base,
  &scc_info_RequestVoteResponse_raft_5fmessage_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_raft_5fmessage_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_raft_5fmessage_2eproto = {
  false, false, descriptor_table_protodef_raft_5fmessage_2eproto, "raft_message.proto", 318,
  &descriptor_table_raft_5fmessage_2eproto_once, descriptor_table_raft_5fmessage_2eproto_sccs, descriptor_table_raft_5fmessage_2eproto_deps, 2, 0,
  schemas, file_default_instances, TableStruct_raft_5fmessage_2eproto::offsets,
  file_level_metadata_raft_5fmessage_2eproto, 2, file_level_enum_descriptors_raft_5fmessage_2eproto, file_level_service_descriptors_raft_5fmessage_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_raft_5fmessage_2eproto = (static_cast<void>(::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_raft_5fmessage_2eproto)), true);
namespace hit_consistency {

// ===================================================================

void RequestVoteRequest::InitAsDefaultInstance() {
}
class RequestVoteRequest::_Internal {
 public:
  using HasBits = decltype(std::declval<RequestVoteRequest>()._has_bits_);
  static void set_has_server_id(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_peer_id(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_term(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_last_log_term(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_last_log_index(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
};

RequestVoteRequest::RequestVoteRequest(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:hit_consistency.RequestVoteRequest)
}
RequestVoteRequest::RequestVoteRequest(const RequestVoteRequest& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  server_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_server_id()) {
    server_id_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from._internal_server_id(),
      GetArena());
  }
  peer_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_peer_id()) {
    peer_id_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from._internal_peer_id(),
      GetArena());
  }
  ::memcpy(&term_, &from.term_,
    static_cast<size_t>(reinterpret_cast<char*>(&last_log_index_) -
    reinterpret_cast<char*>(&term_)) + sizeof(last_log_index_));
  // @@protoc_insertion_point(copy_constructor:hit_consistency.RequestVoteRequest)
}

void RequestVoteRequest::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_RequestVoteRequest_raft_5fmessage_2eproto.base);
  server_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  peer_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&term_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&last_log_index_) -
      reinterpret_cast<char*>(&term_)) + sizeof(last_log_index_));
}

RequestVoteRequest::~RequestVoteRequest() {
  // @@protoc_insertion_point(destructor:hit_consistency.RequestVoteRequest)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void RequestVoteRequest::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  server_id_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  peer_id_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void RequestVoteRequest::ArenaDtor(void* object) {
  RequestVoteRequest* _this = reinterpret_cast< RequestVoteRequest* >(object);
  (void)_this;
}
void RequestVoteRequest::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void RequestVoteRequest::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const RequestVoteRequest& RequestVoteRequest::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_RequestVoteRequest_raft_5fmessage_2eproto.base);
  return *internal_default_instance();
}


void RequestVoteRequest::Clear() {
// @@protoc_insertion_point(message_clear_start:hit_consistency.RequestVoteRequest)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      server_id_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      peer_id_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x0000001cu) {
    ::memset(&term_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&last_log_index_) -
        reinterpret_cast<char*>(&term_)) + sizeof(last_log_index_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* RequestVoteRequest::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // optional string server_id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_server_id();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          #ifndef NDEBUG
          ::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "hit_consistency.RequestVoteRequest.server_id");
          #endif  // !NDEBUG
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // optional string peer_id = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_peer_id();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          #ifndef NDEBUG
          ::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "hit_consistency.RequestVoteRequest.peer_id");
          #endif  // !NDEBUG
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // optional int64 term = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_term(&has_bits);
          term_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // optional int64 last_log_term = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          _Internal::set_has_last_log_term(&has_bits);
          last_log_term_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // optional int64 last_log_index = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          _Internal::set_has_last_log_index(&has_bits);
          last_log_index_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* RequestVoteRequest::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:hit_consistency.RequestVoteRequest)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // optional string server_id = 1;
  if (cached_has_bits & 0x00000001u) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::VerifyUTF8StringNamedField(
      this->_internal_server_id().data(), static_cast<int>(this->_internal_server_id().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SERIALIZE,
      "hit_consistency.RequestVoteRequest.server_id");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_server_id(), target);
  }

  // optional string peer_id = 2;
  if (cached_has_bits & 0x00000002u) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::VerifyUTF8StringNamedField(
      this->_internal_peer_id().data(), static_cast<int>(this->_internal_peer_id().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SERIALIZE,
      "hit_consistency.RequestVoteRequest.peer_id");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_peer_id(), target);
  }

  // optional int64 term = 3;
  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt64ToArray(3, this->_internal_term(), target);
  }

  // optional int64 last_log_term = 4;
  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt64ToArray(4, this->_internal_last_log_term(), target);
  }

  // optional int64 last_log_index = 5;
  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt64ToArray(5, this->_internal_last_log_index(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:hit_consistency.RequestVoteRequest)
  return target;
}

size_t RequestVoteRequest::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:hit_consistency.RequestVoteRequest)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x0000001fu) {
    // optional string server_id = 1;
    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_server_id());
    }

    // optional string peer_id = 2;
    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_peer_id());
    }

    // optional int64 term = 3;
    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int64Size(
          this->_internal_term());
    }

    // optional int64 last_log_term = 4;
    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int64Size(
          this->_internal_last_log_term());
    }

    // optional int64 last_log_index = 5;
    if (cached_has_bits & 0x00000010u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int64Size(
          this->_internal_last_log_index());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void RequestVoteRequest::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:hit_consistency.RequestVoteRequest)
  GOOGLE_DCHECK_NE(&from, this);
  const RequestVoteRequest* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<RequestVoteRequest>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:hit_consistency.RequestVoteRequest)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:hit_consistency.RequestVoteRequest)
    MergeFrom(*source);
  }
}

void RequestVoteRequest::MergeFrom(const RequestVoteRequest& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:hit_consistency.RequestVoteRequest)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x0000001fu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_server_id(from._internal_server_id());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_peer_id(from._internal_peer_id());
    }
    if (cached_has_bits & 0x00000004u) {
      term_ = from.term_;
    }
    if (cached_has_bits & 0x00000008u) {
      last_log_term_ = from.last_log_term_;
    }
    if (cached_has_bits & 0x00000010u) {
      last_log_index_ = from.last_log_index_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void RequestVoteRequest::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:hit_consistency.RequestVoteRequest)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RequestVoteRequest::CopyFrom(const RequestVoteRequest& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:hit_consistency.RequestVoteRequest)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RequestVoteRequest::IsInitialized() const {
  return true;
}

void RequestVoteRequest::InternalSwap(RequestVoteRequest* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  server_id_.Swap(&other->server_id_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  peer_id_.Swap(&other->peer_id_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(RequestVoteRequest, last_log_index_)
      + sizeof(RequestVoteRequest::last_log_index_)
      - PROTOBUF_FIELD_OFFSET(RequestVoteRequest, term_)>(
          reinterpret_cast<char*>(&term_),
          reinterpret_cast<char*>(&other->term_));
}

::PROTOBUF_NAMESPACE_ID::Metadata RequestVoteRequest::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void RequestVoteResponse::InitAsDefaultInstance() {
}
class RequestVoteResponse::_Internal {
 public:
  using HasBits = decltype(std::declval<RequestVoteResponse>()._has_bits_);
  static void set_has_term(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_granted(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
};

RequestVoteResponse::RequestVoteResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:hit_consistency.RequestVoteResponse)
}
RequestVoteResponse::RequestVoteResponse(const RequestVoteResponse& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&term_, &from.term_,
    static_cast<size_t>(reinterpret_cast<char*>(&granted_) -
    reinterpret_cast<char*>(&term_)) + sizeof(granted_));
  // @@protoc_insertion_point(copy_constructor:hit_consistency.RequestVoteResponse)
}

void RequestVoteResponse::SharedCtor() {
  ::memset(&term_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&granted_) -
      reinterpret_cast<char*>(&term_)) + sizeof(granted_));
}

RequestVoteResponse::~RequestVoteResponse() {
  // @@protoc_insertion_point(destructor:hit_consistency.RequestVoteResponse)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void RequestVoteResponse::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void RequestVoteResponse::ArenaDtor(void* object) {
  RequestVoteResponse* _this = reinterpret_cast< RequestVoteResponse* >(object);
  (void)_this;
}
void RequestVoteResponse::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void RequestVoteResponse::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const RequestVoteResponse& RequestVoteResponse::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_RequestVoteResponse_raft_5fmessage_2eproto.base);
  return *internal_default_instance();
}


void RequestVoteResponse::Clear() {
// @@protoc_insertion_point(message_clear_start:hit_consistency.RequestVoteResponse)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    ::memset(&term_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&granted_) -
        reinterpret_cast<char*>(&term_)) + sizeof(granted_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* RequestVoteResponse::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  ::PROTOBUF_NAMESPACE_ID::Arena* arena = GetArena(); (void)arena;
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // optional int64 term = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_term(&has_bits);
          term_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // optional bool granted = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_granted(&has_bits);
          granted_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* RequestVoteResponse::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:hit_consistency.RequestVoteResponse)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // optional int64 term = 1;
  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt64ToArray(1, this->_internal_term(), target);
  }

  // optional bool granted = 2;
  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(2, this->_internal_granted(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:hit_consistency.RequestVoteResponse)
  return target;
}

size_t RequestVoteResponse::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:hit_consistency.RequestVoteResponse)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    // optional int64 term = 1;
    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int64Size(
          this->_internal_term());
    }

    // optional bool granted = 2;
    if (cached_has_bits & 0x00000002u) {
      total_size += 1 + 1;
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void RequestVoteResponse::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:hit_consistency.RequestVoteResponse)
  GOOGLE_DCHECK_NE(&from, this);
  const RequestVoteResponse* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<RequestVoteResponse>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:hit_consistency.RequestVoteResponse)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:hit_consistency.RequestVoteResponse)
    MergeFrom(*source);
  }
}

void RequestVoteResponse::MergeFrom(const RequestVoteResponse& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:hit_consistency.RequestVoteResponse)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      term_ = from.term_;
    }
    if (cached_has_bits & 0x00000002u) {
      granted_ = from.granted_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void RequestVoteResponse::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:hit_consistency.RequestVoteResponse)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RequestVoteResponse::CopyFrom(const RequestVoteResponse& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:hit_consistency.RequestVoteResponse)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RequestVoteResponse::IsInitialized() const {
  return true;
}

void RequestVoteResponse::InternalSwap(RequestVoteResponse* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(RequestVoteResponse, granted_)
      + sizeof(RequestVoteResponse::granted_)
      - PROTOBUF_FIELD_OFFSET(RequestVoteResponse, term_)>(
          reinterpret_cast<char*>(&term_),
          reinterpret_cast<char*>(&other->term_));
}

::PROTOBUF_NAMESPACE_ID::Metadata RequestVoteResponse::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

RaftService::~RaftService() {}

const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* RaftService::descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_raft_5fmessage_2eproto);
  return file_level_service_descriptors_raft_5fmessage_2eproto[0];
}

const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* RaftService::GetDescriptor() {
  return descriptor();
}

void RaftService::prevote(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                         const ::hit_consistency::RequestVoteRequest*,
                         ::hit_consistency::RequestVoteResponse*,
                         ::google::protobuf::Closure* done) {
  controller->SetFailed("Method prevote() not implemented.");
  done->Run();
}

void RaftService::CallMethod(const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method,
                             ::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                             const ::PROTOBUF_NAMESPACE_ID::Message* request,
                             ::PROTOBUF_NAMESPACE_ID::Message* response,
                             ::google::protobuf::Closure* done) {
  GOOGLE_DCHECK_EQ(method->service(), file_level_service_descriptors_raft_5fmessage_2eproto[0]);
  switch(method->index()) {
    case 0:
      prevote(controller,
             ::PROTOBUF_NAMESPACE_ID::internal::DownCast<const ::hit_consistency::RequestVoteRequest*>(
                 request),
             ::PROTOBUF_NAMESPACE_ID::internal::DownCast<::hit_consistency::RequestVoteResponse*>(
                 response),
             done);
      break;
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      break;
  }
}

const ::PROTOBUF_NAMESPACE_ID::Message& RaftService::GetRequestPrototype(
    const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::hit_consistency::RequestVoteRequest::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *::PROTOBUF_NAMESPACE_ID::MessageFactory::generated_factory()
          ->GetPrototype(method->input_type());
  }
}

const ::PROTOBUF_NAMESPACE_ID::Message& RaftService::GetResponsePrototype(
    const ::PROTOBUF_NAMESPACE_ID::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::hit_consistency::RequestVoteResponse::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *::PROTOBUF_NAMESPACE_ID::MessageFactory::generated_factory()
          ->GetPrototype(method->output_type());
  }
}

RaftService_Stub::RaftService_Stub(::PROTOBUF_NAMESPACE_ID::RpcChannel* channel)
  : channel_(channel), owns_channel_(false) {}
RaftService_Stub::RaftService_Stub(
    ::PROTOBUF_NAMESPACE_ID::RpcChannel* channel,
    ::PROTOBUF_NAMESPACE_ID::Service::ChannelOwnership ownership)
  : channel_(channel),
    owns_channel_(ownership == ::PROTOBUF_NAMESPACE_ID::Service::STUB_OWNS_CHANNEL) {}
RaftService_Stub::~RaftService_Stub() {
  if (owns_channel_) delete channel_;
}

void RaftService_Stub::prevote(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                              const ::hit_consistency::RequestVoteRequest* request,
                              ::hit_consistency::RequestVoteResponse* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(0),
                       controller, request, response, done);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace hit_consistency
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::hit_consistency::RequestVoteRequest* Arena::CreateMaybeMessage< ::hit_consistency::RequestVoteRequest >(Arena* arena) {
  return Arena::CreateMessageInternal< ::hit_consistency::RequestVoteRequest >(arena);
}
template<> PROTOBUF_NOINLINE ::hit_consistency::RequestVoteResponse* Arena::CreateMaybeMessage< ::hit_consistency::RequestVoteResponse >(Arena* arena) {
  return Arena::CreateMessageInternal< ::hit_consistency::RequestVoteResponse >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
