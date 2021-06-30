WebSocket
=========

WebSocket codec in C++. Implements *only* RFC6455.
Robust one-class parser implementation (handshake and frames). The library *does not* control sockets, it works with buffers.
This gives you freedom to implement your I/O whatever way you like.

WebSocket class overview
---------
- **parseHandshake()** parses initial HTTP-like handshake and populates object properties like - host, port, etc.
- **answerHandshake()** returns appropriate HTTP-upgrade response.
- **makeFrame()** makes TEXT,BINARY,PING,PONG frame from message-data given in buffer and writes it into output-buffer.
- **getFrame()** tries to parse new frame from input-buffer (populated by the recv() socket function or similar). Returns frame type (enum: TEXT_FRAME, BINARY_FRAME, ...), if the buffer holds incomplete frame INCOMPLETE_FRAME value is returned.

Limitations
---------
**makeFrame()** handles messages shorter than 2^32 bytes.

TODO
---------
- Demo application with sockets and buffers.
- Demo Javascript code as a HTML5 page.

License
---------
MIT, see LICENSE file.
