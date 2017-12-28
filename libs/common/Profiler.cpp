#include "Profiler.hpp"

#include <gflags/gflags.h>

#include "Logging.hpp"
#include "Path.hpp"

extern "C" {
    #include <remotery/Remotery.c>
}

namespace komb {

static Remotery* s_remotery = nullptr;

const char* stringFromError(rmtError error)
{
    switch (error)
    {
    case RMT_ERROR_NONE:                            return "RMT_ERROR_NONE: No error.";                                                                                                                          // NOLINT
    case RMT_ERROR_MALLOC_FAIL:                     return "RMT_ERROR_MALLOC_FAIL: Malloc call within remotery failed";                                                                                          // NOLINT
    case RMT_ERROR_TLS_ALLOC_FAIL:                  return "RMT_ERROR_TLS_ALLOC_FAIL: Attempt to allocate thread local storage failed";                                                                          // NOLINT
    case RMT_ERROR_VIRTUAL_MEMORY_BUFFER_FAIL:      return "RMT_ERROR_VIRTUAL_MEMORY_BUFFER_FAIL: Failed to create a virtual memory mirror buffer";                                                              // NOLINT
    case RMT_ERROR_CREATE_THREAD_FAIL:              return "RMT_ERROR_CREATE_THREAD_FAIL: Failed to create a thread for the server";                                                                             // NOLINT
    case RMT_ERROR_SOCKET_INIT_NETWORK_FAIL:        return "RMT_ERROR_SOCKET_INIT_NETWORK_FAIL: Network initialisation failure (e.g. on Win32, WSAStartup fails)";                                               // NOLINT
    case RMT_ERROR_SOCKET_CREATE_FAIL:              return "RMT_ERROR_SOCKET_CREATE_FAIL: Can't create a socket for connection to the remote viewer";                                                            // NOLINT
    case RMT_ERROR_SOCKET_BIND_FAIL:                return "RMT_ERROR_SOCKET_BIND_FAIL: Can't bind a socket for the server";                                                                                     // NOLINT
    case RMT_ERROR_SOCKET_LISTEN_FAIL:              return "RMT_ERROR_SOCKET_LISTEN_FAIL: Created server socket failed to enter a listen state";                                                                 // NOLINT
    case RMT_ERROR_SOCKET_SET_NON_BLOCKING_FAIL:    return "RMT_ERROR_SOCKET_SET_NON_BLOCKING_FAIL: Created server socket failed to switch to a non-blocking state";                                             // NOLINT
    case RMT_ERROR_SOCKET_INVALID_POLL:             return "RMT_ERROR_SOCKET_INVALID_POLL: Poll attempt on an invalid socket";                                                                                   // NOLINT
    case RMT_ERROR_SOCKET_SELECT_FAIL:              return "RMT_ERROR_SOCKET_SELECT_FAIL: Server failed to call select on socket";                                                                               // NOLINT
    case RMT_ERROR_SOCKET_POLL_ERRORS:              return "RMT_ERROR_SOCKET_POLL_ERRORS: Poll notified that the socket has errors";                                                                             // NOLINT
    case RMT_ERROR_SOCKET_ACCEPT_FAIL:              return "RMT_ERROR_SOCKET_ACCEPT_FAIL: Server failed to accept connection from client";                                                                       // NOLINT
    case RMT_ERROR_SOCKET_SEND_TIMEOUT:             return "RMT_ERROR_SOCKET_SEND_TIMEOUT: Timed out trying to send data";                                                                                       // NOLINT
    case RMT_ERROR_SOCKET_SEND_FAIL:                return "RMT_ERROR_SOCKET_SEND_FAIL: Unrecoverable error occured while client/server tried to send data";                                                     // NOLINT
    case RMT_ERROR_SOCKET_RECV_NO_DATA:             return "RMT_ERROR_SOCKET_RECV_NO_DATA: No data available when attempting a receive";                                                                         // NOLINT
    case RMT_ERROR_SOCKET_RECV_TIMEOUT:             return "RMT_ERROR_SOCKET_RECV_TIMEOUT: Timed out trying to receive data";                                                                                    // NOLINT
    case RMT_ERROR_SOCKET_RECV_FAILED:              return "RMT_ERROR_SOCKET_RECV_FAILED: Unrecoverable error occured while client/server tried to receive data";                                                // NOLINT
    case RMT_ERROR_WEBSOCKET_HANDSHAKE_NOT_GET:     return "RMT_ERROR_WEBSOCKET_HANDSHAKE_NOT_GET: WebSocket server handshake failed, not HTTP GET";                                                             // NOLINT
    case RMT_ERROR_WEBSOCKET_HANDSHAKE_NO_VERSION:  return "RMT_ERROR_WEBSOCKET_HANDSHAKE_NO_VERSION: WebSocket server handshake failed, can't locate WebSocket version";                                        // NOLINT
    case RMT_ERROR_WEBSOCKET_HANDSHAKE_BAD_VERSION: return "RMT_ERROR_WEBSOCKET_HANDSHAKE_BAD_VERSION: WebSocket server handshake failed, unsupported WebSocket version";                                        // NOLINT
    case RMT_ERROR_WEBSOCKET_HANDSHAKE_NO_HOST:     return "RMT_ERROR_WEBSOCKET_HANDSHAKE_NO_HOST: WebSocket server handshake failed, can't locate host";                                                        // NOLINT
    case RMT_ERROR_WEBSOCKET_HANDSHAKE_BAD_HOST:    return "RMT_ERROR_WEBSOCKET_HANDSHAKE_BAD_HOST: WebSocket server handshake failed, host is not allowed to connect";                                          // NOLINT
    case RMT_ERROR_WEBSOCKET_HANDSHAKE_NO_KEY:      return "RMT_ERROR_WEBSOCKET_HANDSHAKE_NO_KEY: WebSocket server handshake failed, can't locate WebSocket key";                                                // NOLINT
    case RMT_ERROR_WEBSOCKET_HANDSHAKE_BAD_KEY:     return "RMT_ERROR_WEBSOCKET_HANDSHAKE_BAD_KEY: WebSocket server handshake failed, WebSocket key is ill-formed";                                              // NOLINT
    case RMT_ERROR_WEBSOCKET_HANDSHAKE_STRING_FAIL: return "RMT_ERROR_WEBSOCKET_HANDSHAKE_STRING_FAIL: WebSocket server handshake failed, internal error, bad string code";                                      // NOLINT
    case RMT_ERROR_WEBSOCKET_DISCONNECTED:          return "RMT_ERROR_WEBSOCKET_DISCONNECTED: WebSocket server received a disconnect request and closed the socket";                                             // NOLINT
    case RMT_ERROR_WEBSOCKET_BAD_FRAME_HEADER:      return "RMT_ERROR_WEBSOCKET_BAD_FRAME_HEADER: Couldn't parse WebSocket frame header";                                                                        // NOLINT
    case RMT_ERROR_WEBSOCKET_BAD_FRAME_HEADER_SIZE: return "RMT_ERROR_WEBSOCKET_BAD_FRAME_HEADER_SIZE: Partially received wide frame header size";                                                               // NOLINT
    case RMT_ERROR_WEBSOCKET_BAD_FRAME_HEADER_MASK: return "RMT_ERROR_WEBSOCKET_BAD_FRAME_HEADER_MASK: Partially received frame header data mask";                                                               // NOLINT
    case RMT_ERROR_WEBSOCKET_RECEIVE_TIMEOUT:       return "RMT_ERROR_WEBSOCKET_RECEIVE_TIMEOUT: Timeout receiving frame header";                                                                                // NOLINT
    case RMT_ERROR_REMOTERY_NOT_CREATED:            return "RMT_ERROR_REMOTERY_NOT_CREATED: Remotery object has not been created";                                                                               // NOLINT
    case RMT_ERROR_SEND_ON_INCOMPLETE_PROFILE:      return "RMT_ERROR_SEND_ON_INCOMPLETE_PROFILE: An attempt was made to send an incomplete profile tree to the client";                                         // NOLINT
    case RMT_ERROR_CUDA_DEINITIALIZED:              return "RMT_ERROR_CUDA_DEINITIALIZED: This indicates that the CUDA driver is in the process of shutting down";                                               // NOLINT
    case RMT_ERROR_CUDA_NOT_INITIALIZED:            return "RMT_ERROR_CUDA_NOT_INITIALIZED: This indicates that the CUDA driver has not been initialized with cuInit() or that initialization has failed";       // NOLINT
    case RMT_ERROR_CUDA_INVALID_CONTEXT:            return "RMT_ERROR_CUDA_INVALID_CONTEXT: This most frequently indicates that there is no context bound to the current thread";                                // NOLINT
    case RMT_ERROR_CUDA_INVALID_VALUE:              return "RMT_ERROR_CUDA_INVALID_VALUE: This indicates that one or more of the parameters passed to the API call is not within an acceptable range of values"; // NOLINT
    case RMT_ERROR_CUDA_INVALID_HANDLE:             return "RMT_ERROR_CUDA_INVALID_HANDLE: This indicates that a resource handle passed to the API call was not valid";                                          // NOLINT
    case RMT_ERROR_CUDA_OUT_OF_MEMORY:              return "RMT_ERROR_CUDA_OUT_OF_MEMORY: The API call failed because it was unable to allocate enough memory to perform the requested operation";               // NOLINT
    case RMT_ERROR_ERROR_NOT_READY:                 return "RMT_ERROR_ERROR_NOT_READY: This indicates that a resource handle passed to the API call was not valid";                                              // NOLINT
    case RMT_ERROR_D3D11_FAILED_TO_CREATE_QUERY:    return "RMT_ERROR_D3D11_FAILED_TO_CREATE_QUERY: Failed to create query for sample";                                                                          // NOLINT
    case RMT_ERROR_OPENGL_ERROR:                    return "RMT_ERROR_OPENGL_ERROR: Generic OpenGL error, no real need to expose more detail since app will probably have an OpenGL error callback registered";  // NOLINT
    case RMT_ERROR_CUDA_UNKNOWN:                    return "RMT_ERROR_CUDA_UNKNOWN: Unknown CUDA error";                                                                                                         // NOLINT
    default:                                        return "Unknown Remotery error.";
    }
}

void stopProfiling()
{
    if (s_remotery)
    {
        rmt_DestroyGlobalInstance(s_remotery);
        LOG_F(INFO, "Profiler server stopped.");
        s_remotery = nullptr;
    }
}

bool startProfilerServer()
{
#if RMT_ENABLED
    auto error = rmt_CreateGlobalInstance(&s_remotery);
    if (error != RMT_ERROR_NONE)
    {
        LOG_F(ERROR, "Failed to start remotery server: %s", stringFromError(error));
        return false;
    }

    atexit(stopProfiling);

    auto url = fs::path(loguru::current_dir()) / "komb/thirdparty/Remotery/vis/index.html";
    LOG_F(INFO, "Profiling enabled! Open the following link in your browser: %s", url.c_str());

    auto settings = rmt_Settings();
    CHECK_NOTNULL(settings);
    LOG_F(INFO, "Remotery port: %d", settings->port);
    return true;
#else
    LOG_F(ERROR, "Can't start profiler - you compiled with RMT_ENABLED=0 (Release build?");
    return true;
#endif
}

} // namespace komb
