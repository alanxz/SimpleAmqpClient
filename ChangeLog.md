Changes since v2.3 (v2.4):
- Add support for consumer cancellation notification (5d35698)
- Improvements in extra-frame bookkeeping reducing memory useage under certain
  conditions (e9de652, f4980bc)
- API support for waiting for multiple consumers at a time (e7e701a)
- Add version header (0fc6cab)
- Add pkg-config on install (a13c99a)
- Add DeclareQueueWithCounts API (Kai Blaschke 7fbcd96)
- Support for C++11 (Alexandre Jacquin 57a8d85)
- Add BasicReject API (Luca Marturana 0c9478e)
- Upgrade gtest to v1.7.0 (8fe82fd)

Changes since v2.2 (v2.3):
NOTE: this release uses new rabbitmq-c interfaces introduced in v0.4.0, thus
requires rabbitmq-c v0.4.0 or later.
- Add support for SSL (Ashok Anand 44b8b4e)
- Use new rabbitmq-c socket interface (Nikita Vasiliev 9f6cdac)
- Use new rabbitmq-c error-string interface (a26da26)
- Code formatting and license header updates (752ae75, 5a2f64c, c3dec10, 2b82942)
- Use new rabbitmq-c timeout interface when reading frames (d4a9f31)
- Use new rabbitmq-c interface to release memory on a per-channel basis (49b8ba8)

Changes since v2.1 (v2.2):
NOTE: this is the last version targeting rabbitmq-c v0.3, newer versions will
      target rabbitmq-c v0.4
- Disable building test suite by default (4f6af4e)
- Default to building Release build when none is specified (c60d0e9)
- Add -Wall -Wextra to default C++ flags (bf813e5)
- Improve documentation (f967758, 23151d3)
- BUG: throw std::bad_alloc when a 0-length table is received (6d17950, d694d4b)
- Improve Channel::BasicGet documentation (ead3936)
- Disable tests that exercise the immediate flag in basic.publish (48636b1)
- Add Channel::BasicAck() overload allowing basic.ack without keeping the whole Envelope obj (0dea3b8, fcd094a)
- Add method to create Channel from an AMQP URI (c8cae56, 8dd62b5)
- Updated examples (fcc1176, a9d4eec, 03bb42d)

Changes since v2.0-beta (v2.1)
 - Add wrapping of amqp_table_t for passing table arguments to various
   AMQP RPC methods (bae7b97)
 - Fix for bug in BasicConsumeMessage default timeout (6412fcf3)
 - Enable travis-ci continuous integration (44089d65)
 - Ship google-test framework with library (8d86d2e4)
 - Implement SOVERSION-ing (b44f3b7b)
 - Missing include in AmqpException.cpp (20ccca9)
 - Fix for memory leak in BasicPublish when exception is thrown (56e20b2)
 - Fix for memory leak in BasicMessage when new body assigned (e5bf1157)
 - Missing string.h include in AmqpException.h (ecee2104)
 - Compile changes to compile cleanly under -Wall -Wextra (2b5a1a23)
 - Fix for crash when AmqpException thrown without a class or method id (6a4fac62)
 - Fix for incorrect timeout units when BasicConsumeMessage (3cdf94d9)
 - Relicensed library under MIT license (a069444b)
 - Fix sending unitialized data to broker (080bd9e9)
 - Fix free strings returned by amqp_error_string (c7b0cfcc)
 - Fix destroy amqp_connection_state object if an exception is thrown in Channel constructor (af936d0)
 - Add ability to build as static library (50b6afd)
 - Fix for macro redefinition (548084)
 - Correct usage of stdint.h on VS2008 and earlier (795c0fea)

