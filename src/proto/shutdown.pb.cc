// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: shutdown.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "shutdown.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
class DoShutdownDefaultTypeInternal : public ::google::protobuf::internal::ExplicitlyConstructed<DoShutdown> {
} _DoShutdown_default_instance_;

namespace protobuf_shutdown_2eproto {


namespace {

::google::protobuf::Metadata file_level_metadata[1];

}  // namespace

const ::google::protobuf::uint32 TableStruct::offsets[] = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DoShutdown, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(DoShutdown, force_),
};

static const ::google::protobuf::internal::MigrationSchema schemas[] = {
  { 0, -1, sizeof(DoShutdown)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&_DoShutdown_default_instance_),
};

namespace {

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "shutdown.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

}  // namespace

void TableStruct::Shutdown() {
  _DoShutdown_default_instance_.Shutdown();
  delete file_level_metadata[0].reflection;
}

void TableStruct::InitDefaultsImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::internal::InitProtobufDefaults();
  _DoShutdown_default_instance_.DefaultConstruct();
}

void InitDefaults() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &TableStruct::InitDefaultsImpl);
}
void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] = {
      "\n\016shutdown.proto\"\033\n\nDoShutdown\022\r\n\005force\030"
      "\001 \001(\010b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 53);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "shutdown.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::OnShutdown(&TableStruct::Shutdown);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;

}  // namespace protobuf_shutdown_2eproto


// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int DoShutdown::kForceFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

DoShutdown::DoShutdown()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    protobuf_shutdown_2eproto::InitDefaults();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:DoShutdown)
}
DoShutdown::DoShutdown(const DoShutdown& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  force_ = from.force_;
  // @@protoc_insertion_point(copy_constructor:DoShutdown)
}

void DoShutdown::SharedCtor() {
  force_ = false;
  _cached_size_ = 0;
}

DoShutdown::~DoShutdown() {
  // @@protoc_insertion_point(destructor:DoShutdown)
  SharedDtor();
}

void DoShutdown::SharedDtor() {
}

void DoShutdown::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* DoShutdown::descriptor() {
  protobuf_shutdown_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_shutdown_2eproto::file_level_metadata[0].descriptor;
}

const DoShutdown& DoShutdown::default_instance() {
  protobuf_shutdown_2eproto::InitDefaults();
  return *internal_default_instance();
}

DoShutdown* DoShutdown::New(::google::protobuf::Arena* arena) const {
  DoShutdown* n = new DoShutdown;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void DoShutdown::Clear() {
// @@protoc_insertion_point(message_clear_start:DoShutdown)
  force_ = false;
}

bool DoShutdown::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:DoShutdown)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // bool force = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(8u)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &force_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:DoShutdown)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:DoShutdown)
  return false;
#undef DO_
}

void DoShutdown::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:DoShutdown)
  // bool force = 1;
  if (this->force() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(1, this->force(), output);
  }

  // @@protoc_insertion_point(serialize_end:DoShutdown)
}

::google::protobuf::uint8* DoShutdown::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic;  // Unused
  // @@protoc_insertion_point(serialize_to_array_start:DoShutdown)
  // bool force = 1;
  if (this->force() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(1, this->force(), target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:DoShutdown)
  return target;
}

size_t DoShutdown::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:DoShutdown)
  size_t total_size = 0;

  // bool force = 1;
  if (this->force() != 0) {
    total_size += 1 + 1;
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void DoShutdown::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:DoShutdown)
  GOOGLE_DCHECK_NE(&from, this);
  const DoShutdown* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const DoShutdown>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:DoShutdown)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:DoShutdown)
    MergeFrom(*source);
  }
}

void DoShutdown::MergeFrom(const DoShutdown& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:DoShutdown)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  if (from.force() != 0) {
    set_force(from.force());
  }
}

void DoShutdown::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:DoShutdown)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void DoShutdown::CopyFrom(const DoShutdown& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:DoShutdown)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool DoShutdown::IsInitialized() const {
  return true;
}

void DoShutdown::Swap(DoShutdown* other) {
  if (other == this) return;
  InternalSwap(other);
}
void DoShutdown::InternalSwap(DoShutdown* other) {
  std::swap(force_, other->force_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata DoShutdown::GetMetadata() const {
  protobuf_shutdown_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_shutdown_2eproto::file_level_metadata[0];
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// DoShutdown

// bool force = 1;
void DoShutdown::clear_force() {
  force_ = false;
}
bool DoShutdown::force() const {
  // @@protoc_insertion_point(field_get:DoShutdown.force)
  return force_;
}
void DoShutdown::set_force(bool value) {
  
  force_ = value;
  // @@protoc_insertion_point(field_set:DoShutdown.force)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
