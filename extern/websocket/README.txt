WebSocket
---------

WebSocket codec in C++. Implements only RFC6455. Robust one-class parser
implementation (handshake and frames). The library does not control sockets, it
works with buffers. This gives you freedom to implement your I/O whatever way
you like.

Homepage: https://github.com/katzarsky/WebSocket

Copyright: Copyright (C) 2012-present by Boian Katzarsky

License: MIT
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  .
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  .
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

Version: 7ac51b89359667c07f44694b5c68aa5c33e40ea1

Patches:
  * 0001-extern-websocket-Add-missing-header.patch
  * 0002-extern-websocket-Add-CLOSING_OPCODE-WebSocketFrameTy.patch
