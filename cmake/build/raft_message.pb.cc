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

PROTOBUF_PRAGMA_INIT_SEG
namespace hit_consistency {
constexpr RequestVoteRequest::RequestVoteRequest(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : server_id_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , peer_id_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , term_(int64_t{0})
  , last_log_term_(int64_t{0})
  , last_log_index_(int64_t{0}){}
struct RequestVoteRequestDefaultTypeInternal {
  constexpr RequestVoteRequestDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~RequestVoteRequestDefaultTypeInternal() {}
  union {
    RequestVoteRequest _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT RequestVoteRequestDefaultTypeInternal _RequestVoteRequest_default_instance_;
constexpr RequestVoteResponse::RequestVoteResponse(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : term_(int64_t{0})
  , granted_(false){}
struct RequestVoteResponseDefaultTypeInternal {
  constexpr RequestVoteResponseDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~RequestVoteResponseDefaultTypeInternal() {}
  union {
    RequestVoteResponse _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT RequestVoteResponseDefaultTypeInternal _RequestVoteResponse_default_instance_;
}  // namespace hit_consistency
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_raft_5fmessage_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_raft_5fmessage_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_raft_5fmessage_2eproto = nullptr;

const uint32_t TableStruct_raft_5fmessage_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, server_id_),
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, peer_id_),
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, term_),
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, last_log_term_),
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteRequest, last_log_index_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteResponse, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteResponse, term_),
  PROTOBUF_FIELD_OFFSET(::hit_consistency::RequestVoteResponse, granted_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::hit_consistency::RequestVoteRequest)},
  { 11, -1, -1, sizeof(::hit_consistency::RequestVoteResponse)},
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
  "consistency.RequestVoteResponse\"\000b\006proto"
  "3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_raft_5fmessage_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_raft_5fmessage_2eproto = {
  false, false, 321, descriptor_table_protodef_raft_5fmessage_2eproto, "raft_message.proto", 
  &descriptor_table_raft_5fmessage_2eproto_once, nullptr, 0, 2,
  schemas, file_default_instances, TableStruct_raft_5fmessage_2eproto::offsets,
  file_level_metadata_raft_5fmessage_2eproto, file_level_enum_descriptors_raft_5fmessage_2eproto, file_level_service_descriptors_raft_5fmessage_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable* descriptor_table_raft_5fmessage_2eproto_getter() {
  return &descriptor_table_raft_5fmessage_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_raft_5fmessage_2eproto(&descriptor_table_raft_5fmessage_2eproto);
namespace hit_consistency {

// ===================================================================

class RequestVoteRequest::_Internal {
 public:
};

RequestVoteRequest::RequestVoteRequest(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor();
  if (!is_message_owned) {
    RegisterArenaDtor(arena);
  }
  // @@protoc_insertion_point(arena_constructor:hit_consistency.RequestVoteRequest)
}
RequestVoteRequest::RequestVoteRequest(const RequestVoteRequest& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  server_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    server_id_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), "", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_server_id().empty()) {
    server_id_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_server_id(), 
      GetArenaForAllocation());
  }
  peer_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    peer_id_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), "", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_peer_id().empty()) {
    peer_id_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_peer_id(), 
      GetArenaForAllocation());
  }
  ::memcpy(&term_, &from.term_,
    static_cast<size_t>(reinterpret_cast<char*>(&last_log_index_) -
    reinterpret_cast<char*>(&term_)) + sizeof(last_log_index_));
  // @@protoc_insertion_point(copy_constructor:hit_consistency.RequestVoteRequest)
}

inline void RequestVoteRequest::SharedCtor() {
server_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  server_id_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), "", GetArenaForAllocation());
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
peer_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  peer_id_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), "", GetArenaForAllocation());
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&term_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&last_log_index_) -
    reinterpret_cast<char*>(&term_)) + sizeof(last_log_index_));
}

RequestVoteRequest::~RequestVoteRequest() {
  // @@protoc_insertion_point(destructor:hit_consistency.RequestVoteRequest)
  if (GetArenaForAllocation() != nullptr) return;
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

inline void RequestVoteRequest::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
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

void RequestVoteRequest::Clear() {
// @@protoc_insertion_point(message_clear_start:hit_consistency.RequestVoteRequest)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  server_id_.ClearToEmpty();
  peer_id_.ClearToEmpty();
  ::memset(&term_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&last_log_index_) -
      reinterpret_cast<char*>(&term_)) + sizeof(last_log_index_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* RequestVoteRequest::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // string server_id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          auto str = _internal_mutable_server_id();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "hit_consistency.RequestVoteRequest.server_id"));
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // string peer_id = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 18)) {
          auto str = _internal_mutable_peer_id();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "hit_consistency.RequestVoteRequest.peer_id"));
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int64 term = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 24)) {
          term_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int64 last_log_term = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 32)) {
          last_log_term_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // int64 last_log_index = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 40)) {
          last_log_index_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* RequestVoteRequest::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:hit_consistency.RequestVoteRequest)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // string server_id = 1;
  if (!this->_internal_server_id().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_server_id().data(), static_cast<int>(this->_internal_server_id().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "hit_consistency.RequestVoteRequest.server_id");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_server_id(), target);
  }

  // string peer_id = 2;
  if (!this->_internal_peer_id().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_peer_id().data(), static_cast<int>(this->_internal_peer_id().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "hit_consistency.RequestVoteRequest.peer_id");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_peer_id(), target);
  }

  // int64 term = 3;
  if (this->_internal_term() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt64ToArray(3, this->_internal_term(), target);
  }

  // int64 last_log_term = 4;
  if (this->_internal_last_log_term() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt64ToArray(4, this->_internal_last_log_term(), target);
  }

  // int64 last_log_index = 5;
  if (this->_internal_last_log_index() != 0) {
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

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string server_id = 1;
  if (!this->_internal_server_id().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_server_id());
  }

  // string peer_id = 2;
  if (!this->_internal_peer_id().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_peer_id());
  }

  // int64 term = 3;
  if (this->_internal_term() != 0) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int64SizePlusOne(this->_internal_term());
  }

  // int64 last_log_term = 4;
  if (this->_internal_last_log_term() != 0) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int64SizePlusOne(this->_internal_last_log_term());
  }

  // int64 last_log_index = 5;
  if (this->_internal_last_log_index() != 0) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int64SizePlusOne(this->_internal_last_log_index());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData RequestVoteRequest::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSizeCheck,
    RequestVoteRequest::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*RequestVoteRequest::GetClassData() const { return &_class_data_; }

void RequestVoteRequest::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to,
                      const ::PROTOBUF_NAMESPACE_ID::Message& from) {
  static_cast<RequestVoteRequest *>(to)->MergeFrom(
      static_cast<const RequestVoteRequest &>(from));
}


void RequestVoteRequest::MergeFrom(const RequestVoteRequest& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:hit_consistency.RequestVoteRequest)
  GOOGLE_DCHECK_NE(&from, this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_server_id().empty()) {
    _internal_set_server_id(from._internal_server_id());
  }
  if (!from._internal_peer_id().empty()) {
    _internal_set_peer_id(from._internal_peer_id());
  }
  if (from._internal_term() != 0) {
    _internal_set_term(from._internal_term());
  }
  if (from._internal_last_log_term() != 0) {
    _internal_set_last_log_term(from._internal_last_log_term());
  }
  if (from._internal_last_log_index() != 0) {
    _internal_set_last_log_index(from._internal_last_log_index());
  }
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
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
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      &server_id_, lhs_arena,
      &other->server_id_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      &peer_id_, lhs_arena,
      &other->peer_id_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(RequestVoteRequest, last_log_index_)
      + sizeof(RequestVoteRequest::last_log_index_)
      - PROTOBUF_FIELD_OFFSET(RequestVoteRequest, term_)>(
          reinterpret_cast<char*>(&term_),
          reinterpret_cast<char*>(&other->term_));
}

::PROTOBUF_NAMESPACE_ID::Metadata RequestVoteRequest::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_raft_5fmessage_2eproto_getter, &descriptor_table_raft_5fmessage_2eproto_once,
      file_level_metadata_raft_5fmessage_2eproto[0]);
}

// ===================================================================

class RequestVoteResponse::_Internal {
 public:
};

RequestVoteResponse::RequestVoteResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor();
  if (!is_message_owned) {
    RegisterArenaDtor(arena);
  }
  // @@protoc_insertion_point(arena_constructor:hit_consistency.RequestVoteResponse)
}
RequestVoteResponse::RequestVoteResponse(const RequestVoteResponse& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&term_, &from.term_,
    static_cast<size_t>(reinterpret_cast<char*>(&granted_) -
    reinterpret_cast<char*>(&term_)) + sizeof(granted_));
  // @@protoc_insertion_point(copy_constructor:hit_consistency.RequestVoteResponse)
}

inline void RequestVoteResponse::SharedCtor() {
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&term_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&granted_) -
    reinterpret_cast<char*>(&term_)) + sizeof(granted_));
}

RequestVoteResponse::~RequestVoteResponse() {
  // @@protoc_insertion_point(destructor:hit_consistency.RequestVoteResponse)
  if (GetArenaForAllocation() != nullptr) return;
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

inline void RequestVoteResponse::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
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

void RequestVoteResponse::Clear() {
// @@protoc_insertion_point(message_clear_start:hit_consistency.RequestVoteResponse)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&term_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&granted_) -
      reinterpret_cast<char*>(&term_)) + sizeof(granted_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* RequestVoteResponse::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // int64 term = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          term_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // bool granted = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          granted_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* RequestVoteResponse::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:hit_consistency.RequestVoteResponse)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // int64 term = 1;
  if (this->_internal_term() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt64ToArray(1, this->_internal_term(), target);
  }

  // bool granted = 2;
  if (this->_internal_granted() != 0) {
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

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // int64 term = 1;
  if (this->_internal_term() != 0) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int64SizePlusOne(this->_internal_term());
  }

  // bool granted = 2;
  if (this->_internal_granted() != 0) {
    total_size += 1 + 1;
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData RequestVoteResponse::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSizeCheck,
    RequestVoteResponse::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*RequestVoteResponse::GetClassData() const { return &_class_data_; }

void RequestVoteResponse::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to,
                      const ::PROTOBUF_NAMESPACE_ID::Message& from) {
  static_cast<RequestVoteResponse *>(to)->MergeFrom(
      static_cast<const RequestVoteResponse &>(from));
}


void RequestVoteResponse::MergeFrom(const RequestVoteResponse& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:hit_consistency.RequestVoteResponse)
  GOOGLE_DCHECK_NE(&from, this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_term() != 0) {
    _internal_set_term(from._internal_term());
  }
  if (from._internal_granted() != 0) {
    _internal_set_granted(from._internal_granted());
  }
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
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
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(RequestVoteResponse, granted_)
      + sizeof(RequestVoteResponse::granted_)
      - PROTOBUF_FIELD_OFFSET(RequestVoteResponse, term_)>(
          reinterpret_cast<char*>(&term_),
          reinterpret_cast<char*>(&other->term_));
}

::PROTOBUF_NAMESPACE_ID::Metadata RequestVoteResponse::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_raft_5fmessage_2eproto_getter, &descriptor_table_raft_5fmessage_2eproto_once,
      file_level_metadata_raft_5fmessage_2eproto[1]);
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
