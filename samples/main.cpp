#include <iostream>
#include <chrono>
#include <tuple>

//#include "network/dimse/dimse_pm.hpp"
//#include "network/upperlayer/upperlayer.hpp"

//#include "data/attribute/attribute.hpp"
//#include "data/dataset/dataset_iterator.hpp"
//#include "data/dictionary/dictionary_dyn.hpp"
//#include "data/dictionary/datadictionary.hpp"
//#include "data/dictionary/dictionary.hpp"
//#include "data/attribute/constants.hpp"

//#include "filesystem/dicomfile.hpp"

//#include "serviceclass/storage_scu.hpp"
//#include "serviceclass/storage_scp.hpp"
//#include "serviceclass/queryretrieve_scp.hpp"

//#include "util/channel_sev_logger.hpp"

#include "libdicompp/all.hpp"

#include <boost/variant.hpp>


int main()
{
   dicom::util::log::init_log();

   using namespace dicom::data;
   using namespace dicom::data::attribute;
   using namespace dicom::data::dictionary;
   using namespace dicom::network;
   using namespace dicom::serviceclass;

   dicom::data::dictionary::dictionaries& dict = get_default_dictionaries();

   dataset::iod dat;
   std::string a = "Hello";
   value<VR::FD> coord {0.3, 0.3, 0.3};
   dat[{0x0010, 0x0010}] = value<VR::PN> {"Marius^Herzog"};
   dat[{0x0010, 0x0010}] = value<VR::FD> {1.0, 0.0, -1.0};
   dat[{0x007f, 0x0010}] = value<VR::ST> {a};

//   value_ref<VR::FD> val = dat[{0x0010, 0x0010}];
//   *(val.value().begin()) = 2.3;

   value<VR::FD> val2 = dat[{0x0010, 0x0010}];
   value<VR::ST> val3 = dat[{0x007f, 0x0010}];
   auto v = *(val2.get().begin());
   dat[{0x0010, 0x0010}] = coord;
   val2 = dat[{0x0010, 0x0010}];
   v = *(val2.get().begin());
   std::string c = dat[{0x007f, 0x0010}].value<VR::ST>();

//   {
//      {
//         dataset::iod dicm;
//         dicm[{0x0008, 0x0016}] = make_elementfield<VR::CS>("1.2.840.10008.5.1.4.1.1.7");
//         dicm[{0x0008, 0x0018}] = make_elementfield<VR::CS>("1.2.840.10008.25.25.25.1");
//         dicm[{0x0010, 0x0010}] = make_elementfield<VR::PN>("test^test");
//         dicom::filesystem::dicomfile file(dicm, dict);
//         std::fstream outfile("outfile.dcm", std::ios::out | std::ios::binary);
//         outfile << file;
//         outfile.flush();
//      }
//      {
//         dataset::iod dicm;
//         dicom::filesystem::dicomfile file(dicm, dict);

//         std::fstream outfile("../XA-MONO2-8-12x-catheter10kb.dcm", std::ios::in | std::ios::binary);
//         outfile >> file;
////         std::cout << file.dataset() << std::flush;

//         auto& set = file.dataset();
//         set[{0x0014, 0x0010}] = value<VR::FD> {1.0, 0.0, -1.0};
//         std::cout << set[{0x0010, 0x0010}].value<VR::PN>() << std::endl;

////         set[{0x0080, 0x0080}] = make_elementfield<VR::OB>({1, 9, 2, 65});

//         std::fstream outfile2("outfile.dcm", std::ios::out | std::ios::binary);
//         outfile2 << file;
//         outfile2.flush();
//      }
//   }

   dimse::SOP_class echo {"1.2.840.10008.1.1",
   { { dataset::DIMSE_SERVICE_GROUP::C_ECHO_RSP,
      [](dimse::dimse_pm* pm, dataset::commandset_data command, std::unique_ptr<dataset::iod> data) {
         assert(data == nullptr);
         std::cout << "Received C_ECHO_RSP\n";
         std::cout << command;
//         pm->send_response({dataset::DIMSE_SERVICE_GROUP::C_ECHO_RQ, command});
         pm->release_association();
      }}}
   };

   dimse::SOP_class findrsp {"1.2.840.10008.5.1.4.31",
   { { dataset::DIMSE_SERVICE_GROUP::C_FIND_RQ,
      [](dimse::dimse_pm* pm, dataset::commandset_data command, std::unique_ptr<dataset::iod> data) {
         assert(data != nullptr);
         std::cout << "Received C_ECHO_RSP\n";
//         std::cout << *data;

         (*data)[{0x0008,0x4000}] = dicom::data::attribute::make_elementfield<VR::LT>("h\x12qej\x13");
         (*data)[{0x0018,0x1150}] = dicom::data::attribute::make_elementfield<VR::IS>("292w9292");

         pm->send_response({dataset::DIMSE_SERVICE_GROUP::C_FIND_RSP, command, *data, 0xff00});
         pm->send_response({dataset::DIMSE_SERVICE_GROUP::C_FIND_RSP, command, boost::none, 0x0000});
//         pm->release_association();
      }}}
   };

   dimse::SOP_class findrsp2 {"1.2.840.10008.5.1.4.31",
   { { dataset::DIMSE_SERVICE_GROUP::C_FIND_RSP,
      [](dimse::dimse_pm* pm, dataset::commandset_data command, std::unique_ptr<dataset::iod> data) {
         //assert(data != nullptr);
         std::cout << "Received C_FIND_RSP\n";
         if (data) {
            std::cout << *data;
         } else {
            std::cout << "No dataset present\n";
         }
         pm->release_association();
      }}}
   };

   dimse::SOP_class echorsp {"1.2.840.10008.1.1",
   { { dataset::DIMSE_SERVICE_GROUP::C_ECHO_RQ,
      [](dimse::dimse_pm* pm, dataset::commandset_data command, std::unique_ptr<dataset::iod> data) {
         assert(data == nullptr);
         std::cout << "Received C_ECHO_RQ\n";
         pm->send_response({dataset::DIMSE_SERVICE_GROUP::C_ECHO_RSP, command});
      }}}
   };

   dimse::SOP_class findrq {"1.2.840.10008.5.1.4.1.1.2",
   { { dataset::DIMSE_SERVICE_GROUP::C_FIND_RQ,
      [](dimse::dimse_pm* pm, dataset::commandset_data command, std::unique_ptr<dataset::iod> data) {
         assert(data == nullptr);
         std::cout << "Send C_FIND_RQ\n";
         dataset::dataset_type dat, dat2, dat3;
         dataset::iod seq;
//         dat[dicom::data::attribute::Item] = dicom::data::attribute::make_elementfield<VR::NI>(0xffffffff);
//         dat[{0x0008, 0x0104}] = dicom::data::attribute::make_elementfield<VR::LO>(4, "meo");
//         dat[{0xfffe, 0xe00d}] = dicom::data::attribute::make_elementfield<VR::NI>();
//         dat2[dicom::data::attribute::Item] = dicom::data::attribute::make_elementfield<VR::NI>(0xffffffff);
//         dat2[{0x0008, 0x0104}] = dicom::data::attribute::make_elementfield<VR::LO>(4, "mwo");
//         dat2[{0xfffe, 0xe00d}] = dicom::data::attribute::make_elementfield<VR::NI>();
//         dat3[dicom::data::attribute::SequenceDelimitationItem] = dicom::data::attribute::make_elementfield<VR::NI>();
//         seq[{0x0032, 0x1064}] = dicom::data::attribute::make_elementfield<VR::SQ>(0xffffffff, {dat, dat2, dat3});
         std::vector<unsigned short> largedat(25000000, 0xff);
         seq[{0x7fe0,0x0010}] = dicom::data::attribute::make_elementfield<VR::OW>(25000000, largedat);
         pm->send_response({dataset::DIMSE_SERVICE_GROUP::C_FIND_RQ, command, seq});
      }}}
   };

   dimse::SOP_class echorq {"1.2.840.10008.1.1",
   { { dataset::DIMSE_SERVICE_GROUP::C_ECHO_RQ,
      [](dimse::dimse_pm* pm, dataset::commandset_data command, std::unique_ptr<dataset::iod> data) {
         assert(data == nullptr);
         std::cout << "Send C_ECHO_RQ\n";
         pm->send_response({dataset::DIMSE_SERVICE_GROUP::C_ECHO_RQ, command});
      }}}
   };

   dimse::association_definition ascdef {"STORESCP", "OFFIS",
      {
//          {echorq, {"1.2.840.10008.1.2"}, dimse::association_definition::DIMSE_MSG_TYPE::INITIATOR},
          {findrq, {"1.2.840.10008.1.2"}, dimse::association_definition::DIMSE_MSG_TYPE::INITIATOR},
          {echo, {"1.2.840.10008.1.2"}, dimse::association_definition::DIMSE_MSG_TYPE::RESPONSE},
          {echorsp, {"1.2.840.10008.1.2"}, dimse::association_definition::DIMSE_MSG_TYPE::RESPONSE},
          {findrsp, {"1.2.840.10008.1.2.1"}, dimse::association_definition::DIMSE_MSG_TYPE::RESPONSE},
          {findrsp2, {"1.2.840.10008.1.2.1"}, dimse::association_definition::DIMSE_MSG_TYPE::RESPONSE}
      },
      4096
   };


   try
   {

//      int n = 0;
//      queryretrieve_scp qr({"QRSCP", "QRSCU", "", 1113}, dict,
//                           [&n](queryretrieve_scp* st, dicom::data::dataset::commandset_data cmd, std::shared_ptr<dicom::data::dataset::iod> data) {
//         dataset::iod seq;
//         seq[{0x0010,0x0010}] = dicom::data::attribute::make_elementfield<VR::PN>("test");
//         ++n;
//         if (n < 15)
//            st->send_image(*data);
//         else
//            st->send_image(boost::none);

//      });
//      qr.set_move_destination("MOVESCU", {"QRSCP", "QRSCU", "localhost", 1114});
//      qr.run();

//      storage_scp store({"STORAGESCU", "STORAGESCP", "", 11112}, dict, [&dict](storage_scp* st, dicom::data::dataset::commandset_data cmd, std::unique_ptr<dicom::data::dataset::iod> data, std::string ts)
//      {
////         std::ofstream out("out", std::ios::binary);
////    std::cout << *data;
////         std::vector<unsigned short> imdata;
////         auto value_field = (*data)[{0x7fe0,0x0010}];
////         get_value_field<VR::OW>(value_field, imdata);
////         out.write((char*)imdata.data(), imdata.size()*sizeof(unsigned short));
////         out.flush();
//         std::string sop_uid;
//         get_value_field<VR::UI>(cmd[{0x0000, 0x1000}], sop_uid);

//         std::cout << "Transfer Syntax: " << ts << std::endl;
//         dicom::filesystem::dicomfile file(*data, dict);
//         file.set_transfer_syntax(ts);
//         std::fstream outfile(sop_uid + ".dcm", std::ios::out | std::ios::binary);
//         outfile << file;
//         outfile.flush();
//      });
//      store.run();

      find_scu find({"STORAGESCU", "STORAGESCP", "88.202.185.144", 11112}, dict, [&dict](find_scu* st, dicom::data::dataset::commandset_data cmd, std::unique_ptr<dicom::data::dataset::iod> data)
      {
         if (data != nullptr) {
            std::cout << *data << "\n";
         }
         std::cout << "State: " << cmd[Status].value<VR::US>() << "\n";
         std::cout << std::flush;
      });
      dataset::iod find_request;
      find_request[PatientName] = value<VR::PN> {"test*"};
      find_request[QueryRetrieveLevel] = value<VR::CS> {"STUDY"};
      find_request[ModalitiesInStudy] = value<VR::CS> {};
      find_request[SOPClassesInStudy] = value<VR::CS> {};
      find_request[StudyInstanceUID] = value<VR::UI> {};
      find_request[PatientID] = value<VR::LO> {};
      find_request[StudyDescription] = value<VR::LO> {};
      find.set_request(find_request);
      find.run();

//      int n = 0;
//      storage_scu store({"STORAGESCP", "STORAGESCU", "46.5.0.221", 11112}, dict,
//                        [&dict, &n](storage_scu* sc, dicom::data::dataset::commandset_data cmd, std::unique_ptr<dicom::data::dataset::iod> data)
//      {
//            if (n == 0) {
//               ++n;
//            }
//            else
//            sc->release();
//      });
//      store.set_store_data(find_request);
//      store.run();
   } catch (std::exception& ec) {
      std::cout << ec.what();
   }
}
