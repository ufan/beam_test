#include <boost/python.hpp>
#include "DmpAlgPsdToSimple.h"

BOOST_PYTHON_MODULE(libDmpAlgPsdToSimple){
  using namespace boost::python;

  class_<DmpAlgPsdToSimple,boost::noncopyable,bases<DmpVAlg> >("DmpAlgPsdToSimple",init<>());
}
