SimpleAmqpClient
==================

[SimpleAmqpClient](https://github.com/alanxz/SimpleAmqpClient) is an easy-to-use C++
wrapper around the [rabbitmq-c](https://github.com/rabbitmq/rabbitmq-c) C library.
It derives inspiration from the [puka](http://majek.github.com/puka/puka.html) AMQP library 
in that it abstracts away the underlying AMQP wire concept of channels and uses them
as an error/consumer scope. This should make writing simple single-threaded AMQP-enabled apps easy.

Installing
----------------

Known to work in the following environments:
- Windows 7 (MSVC 10, Win64, Win32). Likely to work in others, but has not been tested
- Linux (RHEL 6.0, GCC-4.4.5, 32 and 64 bit). Likely to work on other configurations, but has not been tested
- Mac OS X (10.7, 10.6, gcc-4.2, 32 and 64-bit). Likely to work on older version, but has not been tested

### Pre-requisites
+  [boost-1.47.0](http://www.boost.org/) or newer (uses chrono, system internally in addition to other header based libraries such as sharedptr and noncopyable)
+  [rabbitmq-c](http://github.com/alanxz/rabbitmq-c) you'll need version 0.2 or better.
+  [cmake 2.8+](http://www.cmake.org/) what is needed for the build system
+  [Doxygen](http://www.stack.nl/~dimitri/doxygen/) OPTIONAL only necessary to generate API documentation

### Build procedure
This is a typical cmake project, it should work like most typical cmake projects:

In a sibiling directory to where you extracted the source code:
    mkdir simpleamqpclient-build
    cd simpleamqpclient-build
    cmake ../SimpleAmqpClient

Then use your the appropriate build utility to build the library (make, msbuild)

Interesting targets
+  test - will build and run the tests
+  install - will install the library and headers to whatever CMAKE_INSTALL_PREFIX is defined to
+  doc - will generate API documentation if you have doxygen setup

Notes:
+ The test google-test based test suite can be enabled by passing ```-DENABLE_TESTING=ON``` to
  cmake

Using the library
-----------------

    #include <SimpleAmqpClient/SimpleAmqpClient.h>

Will include all the headers necessary to use the library.
The corresponding library is SimpleAmqpClient

The main interface to the library is the AmqpClient::Channel class.  It represents
a connection to an AMQP broker, the connection is established on contruction of an
instance of this class.

    AmqpClient::Channel::ptr_t connection = AmqpClient::Channel::Create("localhost");

All classes have a typedef ptr_t which is equivalent to boost::shared_ptr<> of the 
containing class.  All classes also have a Create() method does the job creating a new
ptr_t using boost::make_shared<>(). It is recommended that you use these methods
to construct objects in the library.

Commands dealing with declaring/binding/unbinding/deleting exchanges and queues are
all done with the above AmqpClient::Channel object. If one of these commands
fails to complete a AmqpClient::ChannelException will be thrown, which can be caught
and the AmqpClient::Channel object is still useable.  If a more severe error occurs
a AmqpClient::ConnectionException or AmqpClient::AmqpResponseLibraryException maybe
thrown, in which case the Channel object is no longer in a usable state and further
use will only generate more exceptions.

Consuming messages is done by setting up a consumer using the BasicConsume method.
This method returns a consumer tag that should be used with the BasicConsumeMessage
BasicQos, BasicRecover, and BasicCancel.

    std::string consumer_tag = channel->BasicConsume("my_queue", "");
    Envelope::ptr_t envelope = channel->BasicConsumeMessage(consumer_tag);
    envelope = channel->BasicConsumeMessage(consumer_tag);
    channel->BasicCancel(consumer_tag);


