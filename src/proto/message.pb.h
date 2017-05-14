// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: message.proto

#ifndef PROTOBUF_message_2eproto__INCLUDED
#define PROTOBUF_message_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3002000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3002000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
#include "ping.pb.h"
#include "advertisement.pb.h"
#include "shutdown.pb.h"
#include "hello.pb.h"
// @@protoc_insertion_point(includes)
class Advertisement;
class AdvertisementDefaultTypeInternal;
extern AdvertisementDefaultTypeInternal _Advertisement_default_instance_;
class DoShutdown;
class DoShutdownDefaultTypeInternal;
extern DoShutdownDefaultTypeInternal _DoShutdown_default_instance_;
class Hello;
class HelloDefaultTypeInternal;
extern HelloDefaultTypeInternal _Hello_default_instance_;
class Message;
class MessageDefaultTypeInternal;
extern MessageDefaultTypeInternal _Message_default_instance_;
class Ping;
class PingDefaultTypeInternal;
extern PingDefaultTypeInternal _Ping_default_instance_;

namespace protobuf_message_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::uint32 offsets[];
  static void InitDefaultsImpl();
  static void Shutdown();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_message_2eproto

enum Message_Type {
  Message_Type_PING = 0,
  Message_Type_SHUTDOWN = 1,
  Message_Type_ADVERTISEMENT = 2,
  Message_Type_HELLO = 3,
  Message_Type_Message_Type_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  Message_Type_Message_Type_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool Message_Type_IsValid(int value);
const Message_Type Message_Type_Type_MIN = Message_Type_PING;
const Message_Type Message_Type_Type_MAX = Message_Type_HELLO;
const int Message_Type_Type_ARRAYSIZE = Message_Type_Type_MAX + 1;

const ::google::protobuf::EnumDescriptor* Message_Type_descriptor();
inline const ::std::string& Message_Type_Name(Message_Type value) {
  return ::google::protobuf::internal::NameOfEnum(
    Message_Type_descriptor(), value);
}
inline bool Message_Type_Parse(
    const ::std::string& name, Message_Type* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Message_Type>(
    Message_Type_descriptor(), name, value);
}
// ===================================================================

class Message : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:Message) */ {
 public:
  Message();
  virtual ~Message();

  Message(const Message& from);

  inline Message& operator=(const Message& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Message& default_instance();

  static inline const Message* internal_default_instance() {
    return reinterpret_cast<const Message*>(
               &_Message_default_instance_);
  }

  void Swap(Message* other);

  // implements Message ----------------------------------------------

  inline Message* New() const PROTOBUF_FINAL { return New(NULL); }

  Message* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const Message& from);
  void MergeFrom(const Message& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output)
      const PROTOBUF_FINAL {
    return InternalSerializeWithCachedSizesToArray(
        ::google::protobuf::io::CodedOutputStream::IsDefaultSerializationDeterministic(), output);
  }
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(Message* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  typedef Message_Type Type;
  static const Type PING =
    Message_Type_PING;
  static const Type SHUTDOWN =
    Message_Type_SHUTDOWN;
  static const Type ADVERTISEMENT =
    Message_Type_ADVERTISEMENT;
  static const Type HELLO =
    Message_Type_HELLO;
  static inline bool Type_IsValid(int value) {
    return Message_Type_IsValid(value);
  }
  static const Type Type_MIN =
    Message_Type_Type_MIN;
  static const Type Type_MAX =
    Message_Type_Type_MAX;
  static const int Type_ARRAYSIZE =
    Message_Type_Type_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Type_descriptor() {
    return Message_Type_descriptor();
  }
  static inline const ::std::string& Type_Name(Type value) {
    return Message_Type_Name(value);
  }
  static inline bool Type_Parse(const ::std::string& name,
      Type* value) {
    return Message_Type_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // .Ping ping = 100;
  bool has_ping() const;
  void clear_ping();
  static const int kPingFieldNumber = 100;
  const ::Ping& ping() const;
  ::Ping* mutable_ping();
  ::Ping* release_ping();
  void set_allocated_ping(::Ping* ping);

  // .DoShutdown shutdown = 101;
  bool has_shutdown() const;
  void clear_shutdown();
  static const int kShutdownFieldNumber = 101;
  const ::DoShutdown& shutdown() const;
  ::DoShutdown* mutable_shutdown();
  ::DoShutdown* release_shutdown();
  void set_allocated_shutdown(::DoShutdown* shutdown);

  // .Advertisement advertisement = 102;
  bool has_advertisement() const;
  void clear_advertisement();
  static const int kAdvertisementFieldNumber = 102;
  const ::Advertisement& advertisement() const;
  ::Advertisement* mutable_advertisement();
  ::Advertisement* release_advertisement();
  void set_allocated_advertisement(::Advertisement* advertisement);

  // .Hello hello = 103;
  bool has_hello() const;
  void clear_hello();
  static const int kHelloFieldNumber = 103;
  const ::Hello& hello() const;
  ::Hello* mutable_hello();
  ::Hello* release_hello();
  void set_allocated_hello(::Hello* hello);

  // .Message.Type type = 1;
  void clear_type();
  static const int kTypeFieldNumber = 1;
  ::Message_Type type() const;
  void set_type(::Message_Type value);

  // @@protoc_insertion_point(class_scope:Message)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::Ping* ping_;
  ::DoShutdown* shutdown_;
  ::Advertisement* advertisement_;
  ::Hello* hello_;
  int type_;
  mutable int _cached_size_;
  friend struct protobuf_message_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// Message

// .Message.Type type = 1;
inline void Message::clear_type() {
  type_ = 0;
}
inline ::Message_Type Message::type() const {
  // @@protoc_insertion_point(field_get:Message.type)
  return static_cast< ::Message_Type >(type_);
}
inline void Message::set_type(::Message_Type value) {
  
  type_ = value;
  // @@protoc_insertion_point(field_set:Message.type)
}

// .Ping ping = 100;
inline bool Message::has_ping() const {
  return this != internal_default_instance() && ping_ != NULL;
}
inline void Message::clear_ping() {
  if (GetArenaNoVirtual() == NULL && ping_ != NULL) delete ping_;
  ping_ = NULL;
}
inline const ::Ping& Message::ping() const {
  // @@protoc_insertion_point(field_get:Message.ping)
  return ping_ != NULL ? *ping_
                         : *::Ping::internal_default_instance();
}
inline ::Ping* Message::mutable_ping() {
  
  if (ping_ == NULL) {
    ping_ = new ::Ping;
  }
  // @@protoc_insertion_point(field_mutable:Message.ping)
  return ping_;
}
inline ::Ping* Message::release_ping() {
  // @@protoc_insertion_point(field_release:Message.ping)
  
  ::Ping* temp = ping_;
  ping_ = NULL;
  return temp;
}
inline void Message::set_allocated_ping(::Ping* ping) {
  delete ping_;
  ping_ = ping;
  if (ping) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:Message.ping)
}

// .DoShutdown shutdown = 101;
inline bool Message::has_shutdown() const {
  return this != internal_default_instance() && shutdown_ != NULL;
}
inline void Message::clear_shutdown() {
  if (GetArenaNoVirtual() == NULL && shutdown_ != NULL) delete shutdown_;
  shutdown_ = NULL;
}
inline const ::DoShutdown& Message::shutdown() const {
  // @@protoc_insertion_point(field_get:Message.shutdown)
  return shutdown_ != NULL ? *shutdown_
                         : *::DoShutdown::internal_default_instance();
}
inline ::DoShutdown* Message::mutable_shutdown() {
  
  if (shutdown_ == NULL) {
    shutdown_ = new ::DoShutdown;
  }
  // @@protoc_insertion_point(field_mutable:Message.shutdown)
  return shutdown_;
}
inline ::DoShutdown* Message::release_shutdown() {
  // @@protoc_insertion_point(field_release:Message.shutdown)
  
  ::DoShutdown* temp = shutdown_;
  shutdown_ = NULL;
  return temp;
}
inline void Message::set_allocated_shutdown(::DoShutdown* shutdown) {
  delete shutdown_;
  shutdown_ = shutdown;
  if (shutdown) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:Message.shutdown)
}

// .Advertisement advertisement = 102;
inline bool Message::has_advertisement() const {
  return this != internal_default_instance() && advertisement_ != NULL;
}
inline void Message::clear_advertisement() {
  if (GetArenaNoVirtual() == NULL && advertisement_ != NULL) delete advertisement_;
  advertisement_ = NULL;
}
inline const ::Advertisement& Message::advertisement() const {
  // @@protoc_insertion_point(field_get:Message.advertisement)
  return advertisement_ != NULL ? *advertisement_
                         : *::Advertisement::internal_default_instance();
}
inline ::Advertisement* Message::mutable_advertisement() {
  
  if (advertisement_ == NULL) {
    advertisement_ = new ::Advertisement;
  }
  // @@protoc_insertion_point(field_mutable:Message.advertisement)
  return advertisement_;
}
inline ::Advertisement* Message::release_advertisement() {
  // @@protoc_insertion_point(field_release:Message.advertisement)
  
  ::Advertisement* temp = advertisement_;
  advertisement_ = NULL;
  return temp;
}
inline void Message::set_allocated_advertisement(::Advertisement* advertisement) {
  delete advertisement_;
  advertisement_ = advertisement;
  if (advertisement) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:Message.advertisement)
}

// .Hello hello = 103;
inline bool Message::has_hello() const {
  return this != internal_default_instance() && hello_ != NULL;
}
inline void Message::clear_hello() {
  if (GetArenaNoVirtual() == NULL && hello_ != NULL) delete hello_;
  hello_ = NULL;
}
inline const ::Hello& Message::hello() const {
  // @@protoc_insertion_point(field_get:Message.hello)
  return hello_ != NULL ? *hello_
                         : *::Hello::internal_default_instance();
}
inline ::Hello* Message::mutable_hello() {
  
  if (hello_ == NULL) {
    hello_ = new ::Hello;
  }
  // @@protoc_insertion_point(field_mutable:Message.hello)
  return hello_;
}
inline ::Hello* Message::release_hello() {
  // @@protoc_insertion_point(field_release:Message.hello)
  
  ::Hello* temp = hello_;
  hello_ = NULL;
  return temp;
}
inline void Message::set_allocated_hello(::Hello* hello) {
  delete hello_;
  hello_ = hello;
  if (hello) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:Message.hello)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)


#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::Message_Type> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Message_Type>() {
  return ::Message_Type_descriptor();
}

}  // namespace protobuf
}  // namespace google
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_message_2eproto__INCLUDED
