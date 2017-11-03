// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: message.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "message.pb.h"

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
class MessageDefaultTypeInternal : public ::google::protobuf::internal::ExplicitlyConstructed<Message> {
} _Message_default_instance_;

namespace protobuf_message_2eproto {


namespace {

::google::protobuf::Metadata file_level_metadata[1];
const ::google::protobuf::EnumDescriptor* file_level_enum_descriptors[1];

}  // namespace

const ::google::protobuf::uint32 TableStruct::offsets[] = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, type_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, ping_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, shutdown_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, advertisement_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, hello_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, state_),
};

static const ::google::protobuf::internal::MigrationSchema schemas[] = {
  { 0, -1, sizeof(Message)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&_Message_default_instance_),
};

namespace {

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "message.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, file_level_enum_descriptors, NULL);
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
  _Message_default_instance_.Shutdown();
  delete file_level_metadata[0].reflection;
}

void TableStruct::InitDefaultsImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::internal::InitProtobufDefaults();
  ::protobuf_ping_2eproto::InitDefaults();
  ::protobuf_advertisement_2eproto::InitDefaults();
  ::protobuf_shutdown_2eproto::InitDefaults();
  ::protobuf_hello_2eproto::InitDefaults();
  ::protobuf_state_2eproto::InitDefaults();
  _Message_default_instance_.DefaultConstruct();
  _Message_default_instance_.get_mutable()->ping_ = const_cast< ::Ping*>(
      ::Ping::internal_default_instance());
  _Message_default_instance_.get_mutable()->shutdown_ = const_cast< ::DoShutdown*>(
      ::DoShutdown::internal_default_instance());
  _Message_default_instance_.get_mutable()->advertisement_ = const_cast< ::Advertisement*>(
      ::Advertisement::internal_default_instance());
  _Message_default_instance_.get_mutable()->hello_ = const_cast< ::Hello*>(
      ::Hello::internal_default_instance());
  _Message_default_instance_.get_mutable()->state_ = const_cast< ::State*>(
      ::State::internal_default_instance());
}

void InitDefaults() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &TableStruct::InitDefaultsImpl);
}
void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] = {
      "\n\rmessage.proto\032\nping.proto\032\023advertiseme"
      "nt.proto\032\016shutdown.proto\032\013hello.proto\032\013s"
      "tate.proto\"\370\001\n\007Message\022\033\n\004type\030\001 \001(\0162\r.M"
      "essage.Type\022\023\n\004ping\030d \001(\0132\005.Ping\022\035\n\010shut"
      "down\030e \001(\0132\013.DoShutdown\022%\n\radvertisement"
      "\030f \001(\0132\016.Advertisement\022\025\n\005hello\030g \001(\0132\006."
      "Hello\022\025\n\005state\030h \001(\0132\006.State\"G\n\004Type\022\010\n\004"
      "PING\020\000\022\014\n\010SHUTDOWN\020\001\022\021\n\rADVERTISEMENT\020\002\022"
      "\t\n\005HELLO\020\003\022\t\n\005STATE\020\004b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 349);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "message.proto", &protobuf_RegisterTypes);
  ::protobuf_ping_2eproto::AddDescriptors();
  ::protobuf_advertisement_2eproto::AddDescriptors();
  ::protobuf_shutdown_2eproto::AddDescriptors();
  ::protobuf_hello_2eproto::AddDescriptors();
  ::protobuf_state_2eproto::AddDescriptors();
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

}  // namespace protobuf_message_2eproto

const ::google::protobuf::EnumDescriptor* Message_Type_descriptor() {
  protobuf_message_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_message_2eproto::file_level_enum_descriptors[0];
}
bool Message_Type_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      return true;
    default:
      return false;
  }
}

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const Message_Type Message::PING;
const Message_Type Message::SHUTDOWN;
const Message_Type Message::ADVERTISEMENT;
const Message_Type Message::HELLO;
const Message_Type Message::STATE;
const Message_Type Message::Type_MIN;
const Message_Type Message::Type_MAX;
const int Message::Type_ARRAYSIZE;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int Message::kTypeFieldNumber;
const int Message::kPingFieldNumber;
const int Message::kShutdownFieldNumber;
const int Message::kAdvertisementFieldNumber;
const int Message::kHelloFieldNumber;
const int Message::kStateFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

Message::Message()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    protobuf_message_2eproto::InitDefaults();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:Message)
}
Message::Message(const Message& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  if (from.has_ping()) {
    ping_ = new ::Ping(*from.ping_);
  } else {
    ping_ = NULL;
  }
  if (from.has_shutdown()) {
    shutdown_ = new ::DoShutdown(*from.shutdown_);
  } else {
    shutdown_ = NULL;
  }
  if (from.has_advertisement()) {
    advertisement_ = new ::Advertisement(*from.advertisement_);
  } else {
    advertisement_ = NULL;
  }
  if (from.has_hello()) {
    hello_ = new ::Hello(*from.hello_);
  } else {
    hello_ = NULL;
  }
  if (from.has_state()) {
    state_ = new ::State(*from.state_);
  } else {
    state_ = NULL;
  }
  type_ = from.type_;
  // @@protoc_insertion_point(copy_constructor:Message)
}

void Message::SharedCtor() {
  ::memset(&ping_, 0, reinterpret_cast<char*>(&type_) -
    reinterpret_cast<char*>(&ping_) + sizeof(type_));
  _cached_size_ = 0;
}

Message::~Message() {
  // @@protoc_insertion_point(destructor:Message)
  SharedDtor();
}

void Message::SharedDtor() {
  if (this != internal_default_instance()) {
    delete ping_;
  }
  if (this != internal_default_instance()) {
    delete shutdown_;
  }
  if (this != internal_default_instance()) {
    delete advertisement_;
  }
  if (this != internal_default_instance()) {
    delete hello_;
  }
  if (this != internal_default_instance()) {
    delete state_;
  }
}

void Message::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Message::descriptor() {
  protobuf_message_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_message_2eproto::file_level_metadata[0].descriptor;
}

const Message& Message::default_instance() {
  protobuf_message_2eproto::InitDefaults();
  return *internal_default_instance();
}

Message* Message::New(::google::protobuf::Arena* arena) const {
  Message* n = new Message;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void Message::Clear() {
// @@protoc_insertion_point(message_clear_start:Message)
  if (GetArenaNoVirtual() == NULL && ping_ != NULL) {
    delete ping_;
  }
  ping_ = NULL;
  if (GetArenaNoVirtual() == NULL && shutdown_ != NULL) {
    delete shutdown_;
  }
  shutdown_ = NULL;
  if (GetArenaNoVirtual() == NULL && advertisement_ != NULL) {
    delete advertisement_;
  }
  advertisement_ = NULL;
  if (GetArenaNoVirtual() == NULL && hello_ != NULL) {
    delete hello_;
  }
  hello_ = NULL;
  if (GetArenaNoVirtual() == NULL && state_ != NULL) {
    delete state_;
  }
  state_ = NULL;
  type_ = 0;
}

bool Message::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:Message)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(16383u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // .Message.Type type = 1;
      case 1: {
        if (tag == 8u) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          set_type(static_cast< ::Message_Type >(value));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // .Ping ping = 100;
      case 100: {
        if (tag == 802u) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_ping()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // .DoShutdown shutdown = 101;
      case 101: {
        if (tag == 810u) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_shutdown()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // .Advertisement advertisement = 102;
      case 102: {
        if (tag == 818u) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_advertisement()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // .Hello hello = 103;
      case 103: {
        if (tag == 826u) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_hello()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // .State state = 104;
      case 104: {
        if (tag == 834u) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_state()));
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
  // @@protoc_insertion_point(parse_success:Message)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Message)
  return false;
#undef DO_
}

void Message::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Message)
  // .Message.Type type = 1;
  if (this->type() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->type(), output);
  }

  // .Ping ping = 100;
  if (this->has_ping()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      100, *this->ping_, output);
  }

  // .DoShutdown shutdown = 101;
  if (this->has_shutdown()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      101, *this->shutdown_, output);
  }

  // .Advertisement advertisement = 102;
  if (this->has_advertisement()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      102, *this->advertisement_, output);
  }

  // .Hello hello = 103;
  if (this->has_hello()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      103, *this->hello_, output);
  }

  // .State state = 104;
  if (this->has_state()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      104, *this->state_, output);
  }

  // @@protoc_insertion_point(serialize_end:Message)
}

::google::protobuf::uint8* Message::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic;  // Unused
  // @@protoc_insertion_point(serialize_to_array_start:Message)
  // .Message.Type type = 1;
  if (this->type() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->type(), target);
  }

  // .Ping ping = 100;
  if (this->has_ping()) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageNoVirtualToArray(
        100, *this->ping_, false, target);
  }

  // .DoShutdown shutdown = 101;
  if (this->has_shutdown()) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageNoVirtualToArray(
        101, *this->shutdown_, false, target);
  }

  // .Advertisement advertisement = 102;
  if (this->has_advertisement()) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageNoVirtualToArray(
        102, *this->advertisement_, false, target);
  }

  // .Hello hello = 103;
  if (this->has_hello()) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageNoVirtualToArray(
        103, *this->hello_, false, target);
  }

  // .State state = 104;
  if (this->has_state()) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageNoVirtualToArray(
        104, *this->state_, false, target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:Message)
  return target;
}

size_t Message::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Message)
  size_t total_size = 0;

  // .Ping ping = 100;
  if (this->has_ping()) {
    total_size += 2 +
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->ping_);
  }

  // .DoShutdown shutdown = 101;
  if (this->has_shutdown()) {
    total_size += 2 +
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->shutdown_);
  }

  // .Advertisement advertisement = 102;
  if (this->has_advertisement()) {
    total_size += 2 +
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->advertisement_);
  }

  // .Hello hello = 103;
  if (this->has_hello()) {
    total_size += 2 +
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->hello_);
  }

  // .State state = 104;
  if (this->has_state()) {
    total_size += 2 +
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->state_);
  }

  // .Message.Type type = 1;
  if (this->type() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::EnumSize(this->type());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Message::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Message)
  GOOGLE_DCHECK_NE(&from, this);
  const Message* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const Message>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Message)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Message)
    MergeFrom(*source);
  }
}

void Message::MergeFrom(const Message& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Message)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  if (from.has_ping()) {
    mutable_ping()->::Ping::MergeFrom(from.ping());
  }
  if (from.has_shutdown()) {
    mutable_shutdown()->::DoShutdown::MergeFrom(from.shutdown());
  }
  if (from.has_advertisement()) {
    mutable_advertisement()->::Advertisement::MergeFrom(from.advertisement());
  }
  if (from.has_hello()) {
    mutable_hello()->::Hello::MergeFrom(from.hello());
  }
  if (from.has_state()) {
    mutable_state()->::State::MergeFrom(from.state());
  }
  if (from.type() != 0) {
    set_type(from.type());
  }
}

void Message::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Message)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Message::CopyFrom(const Message& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Message)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Message::IsInitialized() const {
  return true;
}

void Message::Swap(Message* other) {
  if (other == this) return;
  InternalSwap(other);
}
void Message::InternalSwap(Message* other) {
  std::swap(ping_, other->ping_);
  std::swap(shutdown_, other->shutdown_);
  std::swap(advertisement_, other->advertisement_);
  std::swap(hello_, other->hello_);
  std::swap(state_, other->state_);
  std::swap(type_, other->type_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata Message::GetMetadata() const {
  protobuf_message_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_message_2eproto::file_level_metadata[0];
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// Message

// .Message.Type type = 1;
void Message::clear_type() {
  type_ = 0;
}
::Message_Type Message::type() const {
  // @@protoc_insertion_point(field_get:Message.type)
  return static_cast< ::Message_Type >(type_);
}
void Message::set_type(::Message_Type value) {
  
  type_ = value;
  // @@protoc_insertion_point(field_set:Message.type)
}

// .Ping ping = 100;
bool Message::has_ping() const {
  return this != internal_default_instance() && ping_ != NULL;
}
void Message::clear_ping() {
  if (GetArenaNoVirtual() == NULL && ping_ != NULL) delete ping_;
  ping_ = NULL;
}
const ::Ping& Message::ping() const {
  // @@protoc_insertion_point(field_get:Message.ping)
  return ping_ != NULL ? *ping_
                         : *::Ping::internal_default_instance();
}
::Ping* Message::mutable_ping() {
  
  if (ping_ == NULL) {
    ping_ = new ::Ping;
  }
  // @@protoc_insertion_point(field_mutable:Message.ping)
  return ping_;
}
::Ping* Message::release_ping() {
  // @@protoc_insertion_point(field_release:Message.ping)
  
  ::Ping* temp = ping_;
  ping_ = NULL;
  return temp;
}
void Message::set_allocated_ping(::Ping* ping) {
  delete ping_;
  ping_ = ping;
  if (ping) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:Message.ping)
}

// .DoShutdown shutdown = 101;
bool Message::has_shutdown() const {
  return this != internal_default_instance() && shutdown_ != NULL;
}
void Message::clear_shutdown() {
  if (GetArenaNoVirtual() == NULL && shutdown_ != NULL) delete shutdown_;
  shutdown_ = NULL;
}
const ::DoShutdown& Message::shutdown() const {
  // @@protoc_insertion_point(field_get:Message.shutdown)
  return shutdown_ != NULL ? *shutdown_
                         : *::DoShutdown::internal_default_instance();
}
::DoShutdown* Message::mutable_shutdown() {
  
  if (shutdown_ == NULL) {
    shutdown_ = new ::DoShutdown;
  }
  // @@protoc_insertion_point(field_mutable:Message.shutdown)
  return shutdown_;
}
::DoShutdown* Message::release_shutdown() {
  // @@protoc_insertion_point(field_release:Message.shutdown)
  
  ::DoShutdown* temp = shutdown_;
  shutdown_ = NULL;
  return temp;
}
void Message::set_allocated_shutdown(::DoShutdown* shutdown) {
  delete shutdown_;
  shutdown_ = shutdown;
  if (shutdown) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:Message.shutdown)
}

// .Advertisement advertisement = 102;
bool Message::has_advertisement() const {
  return this != internal_default_instance() && advertisement_ != NULL;
}
void Message::clear_advertisement() {
  if (GetArenaNoVirtual() == NULL && advertisement_ != NULL) delete advertisement_;
  advertisement_ = NULL;
}
const ::Advertisement& Message::advertisement() const {
  // @@protoc_insertion_point(field_get:Message.advertisement)
  return advertisement_ != NULL ? *advertisement_
                         : *::Advertisement::internal_default_instance();
}
::Advertisement* Message::mutable_advertisement() {
  
  if (advertisement_ == NULL) {
    advertisement_ = new ::Advertisement;
  }
  // @@protoc_insertion_point(field_mutable:Message.advertisement)
  return advertisement_;
}
::Advertisement* Message::release_advertisement() {
  // @@protoc_insertion_point(field_release:Message.advertisement)
  
  ::Advertisement* temp = advertisement_;
  advertisement_ = NULL;
  return temp;
}
void Message::set_allocated_advertisement(::Advertisement* advertisement) {
  delete advertisement_;
  advertisement_ = advertisement;
  if (advertisement) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:Message.advertisement)
}

// .Hello hello = 103;
bool Message::has_hello() const {
  return this != internal_default_instance() && hello_ != NULL;
}
void Message::clear_hello() {
  if (GetArenaNoVirtual() == NULL && hello_ != NULL) delete hello_;
  hello_ = NULL;
}
const ::Hello& Message::hello() const {
  // @@protoc_insertion_point(field_get:Message.hello)
  return hello_ != NULL ? *hello_
                         : *::Hello::internal_default_instance();
}
::Hello* Message::mutable_hello() {
  
  if (hello_ == NULL) {
    hello_ = new ::Hello;
  }
  // @@protoc_insertion_point(field_mutable:Message.hello)
  return hello_;
}
::Hello* Message::release_hello() {
  // @@protoc_insertion_point(field_release:Message.hello)
  
  ::Hello* temp = hello_;
  hello_ = NULL;
  return temp;
}
void Message::set_allocated_hello(::Hello* hello) {
  delete hello_;
  hello_ = hello;
  if (hello) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:Message.hello)
}

// .State state = 104;
bool Message::has_state() const {
  return this != internal_default_instance() && state_ != NULL;
}
void Message::clear_state() {
  if (GetArenaNoVirtual() == NULL && state_ != NULL) delete state_;
  state_ = NULL;
}
const ::State& Message::state() const {
  // @@protoc_insertion_point(field_get:Message.state)
  return state_ != NULL ? *state_
                         : *::State::internal_default_instance();
}
::State* Message::mutable_state() {
  
  if (state_ == NULL) {
    state_ = new ::State;
  }
  // @@protoc_insertion_point(field_mutable:Message.state)
  return state_;
}
::State* Message::release_state() {
  // @@protoc_insertion_point(field_release:Message.state)
  
  ::State* temp = state_;
  state_ = NULL;
  return temp;
}
void Message::set_allocated_state(::State* state) {
  delete state_;
  state_ = state;
  if (state) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:Message.state)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
