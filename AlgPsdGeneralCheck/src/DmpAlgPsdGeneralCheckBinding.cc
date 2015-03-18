#include <boost/python.hpp>
#include "DmpAlgPsdGeneralCheck.h"

BOOST_PYTHON_MODULE(libDmpAlgPsdGeneralCheck){
  using namespace boost::python;

  class_<DmpAlgPsdGeneralCheck,boost::noncopyable,bases<DmpVAlg> >("DmpAlgPsdGeneralCheck",init<>());
}
