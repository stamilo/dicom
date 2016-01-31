#include "sop_class.hpp"

namespace dicom
{

namespace network
{

namespace dimse
{

using namespace data::dataset;

SOP_class::SOP_class(std::string SOP_class_UID, std::map<DIMSE_SERVICE_GROUP, std::function<response(std::unique_ptr<iod>)> > handler):
   sop_uid {SOP_class_UID}, operations {handler}
{
}

response SOP_class::operator()(DIMSE_SERVICE_GROUP op, std::unique_ptr<iod> data) const
{
   return operations.at(op)(std::move(data));
}

const char* SOP_class::get_SOP_class_UID() const
{
   return sop_uid.c_str();
}

SOP_class_request::SOP_class_request(std::string SOP_class_UID,
                                     DIMSE_SERVICE_GROUP dsg,
                                     std::function<response(std::unique_ptr<iod>)> handler):
   sop_uid {SOP_class_UID}, dsg {dsg}, handler {handler}
{
}

response SOP_class_request::operator()() const
{
   return handler(nullptr);
}

const char*SOP_class_request::get_SOP_class_UID() const
{
   return sop_uid.c_str();
}

DIMSE_SERVICE_GROUP SOP_class_request::get_service_group() const
{
   return dsg;
}


}

}

}
