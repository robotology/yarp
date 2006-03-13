
#include <yarp/BufferedConnectionWriter.h>

#include <yarp/StreamConnectionReader.h>

#include <yarp/NetType.h>

#include <yarp/BufferedConnectionWriter.h>

using namespace yarp;

yarp::os::ConnectionWriter *StreamConnectionReader::getWriter() {
  if (str==NULL) {
    return NULL;
  }
  if (writer==NULL) {
    writer = new BufferedConnectionWriter;
    YARP_ASSERT(writer!=NULL);
    writer->reset(isTextMode());
  }
  writer->clear();
  return writer;
}


void StreamConnectionReader::flushWriter() {
  if (writer!=NULL) {
    if (str!=NULL) {
      writer->write(str->getOutputStream());
      writer->clear();
    }
  }
}


StreamConnectionReader::~StreamConnectionReader() {
  if (writer!=NULL) {
    delete writer;
    writer = NULL;
  }
}


