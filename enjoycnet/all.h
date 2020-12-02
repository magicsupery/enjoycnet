#ifdef _WIN32
#define NOMINMAX // make windows maco min max useless
#define WIN32_LEAN_AND_MEAN // make use winsock2
#define GOOGLE_GLOG_DLL_DECL // make use static lig glog
#endif


#include "core/syscall.h"
#include "core/io_context.h"
#include "core/co_event.h"
#include "core/socket.h"
#include "core/proto_tcp.h"
#include "core/acceptor.h"
#include "core/connection.h"
#include "core/codec.h"
#include "core/connector.h"

