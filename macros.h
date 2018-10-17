#pragma once

#define max(x, y) ((x > y) ? x : y)
#define lengthof(x)  (sizeof(x) / sizeof((x)[0]))

#define STATIC_MAILBOX(name, len)  \
  msg_t  name ## _buffer [len];  \
  MAILBOX_DECL(name, name ## _buffer, len);
