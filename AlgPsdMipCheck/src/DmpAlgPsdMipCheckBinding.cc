#include <boost/python.hpp>
#include "DmpAlgPsdMipCheck.h"

BOOST_PYTHON_MODULE(libDmpAlgPsdMipCheck){
  using namespace boost::python;

  class_<DmpAlgPsdMipCheck,boost::noncopyable,bases<DmpVAlg> >("DmpAlgPsdMipCheck",init<>());
}
