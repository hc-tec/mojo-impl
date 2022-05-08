//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_TYPES_H
#define MOJO_IMPL_TYPES_H

typedef uint64_t MojoHandle;


typedef uint32_t MojoResult;

const MojoResult MOJO_RESULT_OK = 0;
const MojoResult MOJO_RESULT_CANCELLED = 1;
const MojoResult MOJO_RESULT_UNKNOWN = 2;
const MojoResult MOJO_RESULT_INVALID_ARGUMENT = 3;
const MojoResult MOJO_RESULT_DEADLINE_EXCEEDED = 4;
const MojoResult MOJO_RESULT_NOT_FOUND = 5;
const MojoResult MOJO_RESULT_ALREADY_EXISTS = 6;
const MojoResult MOJO_RESULT_PERMISSION_DENIED = 7;
const MojoResult MOJO_RESULT_RESOURCE_EXHAUSTED = 8;
const MojoResult MOJO_RESULT_FAILED_PRECONDITION = 9;
const MojoResult MOJO_RESULT_ABORTED = 10;
const MojoResult MOJO_RESULT_OUT_OF_RANGE = 11;
const MojoResult MOJO_RESULT_UNIMPLEMENTED = 12;
const MojoResult MOJO_RESULT_INTERNAL = 13;
const MojoResult MOJO_RESULT_UNAVAILABLE = 14;
const MojoResult MOJO_RESULT_DATA_LOSS = 15;
const MojoResult MOJO_RESULT_BUSY = 16;
const MojoResult MOJO_RESULT_SHOULD_WAIT = 17;


typedef uint32_t MojoHandleSignals;

const MojoHandleSignals MOJO_HANDLE_SIGNAL_NONE = 0;
const MojoHandleSignals MOJO_HANDLE_SIGNAL_READABLE = 1 << 0;
const MojoHandleSignals MOJO_HANDLE_SIGNAL_WRITABLE = 1 << 1;
const MojoHandleSignals MOJO_HANDLE_SIGNAL_PEER_CLOSED = 1 << 2;
const MojoHandleSignals MOJO_HANDLE_SIGNAL_NEW_DATA_READABLE = 1 << 3;
const MojoHandleSignals MOJO_HANDLE_SIGNAL_PEER_REMOTE = 1 << 4;
const MojoHandleSignals MOJO_HANDLE_SIGNAL_QUOTA_EXCEEDED = 1 << 5;
#endif  // MOJO_IMPL_TYPES_H
