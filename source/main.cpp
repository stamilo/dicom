#include <iostream>
#include <chrono>

#include "dimse/dimse_pm.hpp"
#include "upperlayer/upperlayer.hpp"

#include "dictionary/dictionary_dyn.hpp"
#include "dictionary/datadictionary.hpp"
#include "attributedata/dataset_iterator.hpp"




int main()
{
   dictionary_dyn dic {"/media/STORAGE/_files/Studium/Sem 5/Studienprojekt/dicom/dicom/commanddictionary.txt"};
   commandset_data dat, dat2;
   commandset_processor cpr(dic);

   std::vector<unsigned char> echo_rsp {
      0x04, 0x00, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0x50, 0x01, 0x03,
      0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
      0x12, 0x00, 0x00, 0x00, 0x31, 0x2E, 0x32, 0x2E, 0x38, 0x34, 0x30, 0x2E, 0x31, 0x30, 0x30, 0x30,
      0x38, 0x2E, 0x31, 0x2E, 0x31, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x30, 0x80,
      0x00, 0x00, 0x20, 0x01, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x08, 0x02, 0x00,
      0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x09, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
   };

   for (const auto x : echo_rsp) {
      std::cout << x;
   }
   std::cout << "\n" << std::flush;

   dat.insert(make_elementfield<VR::UL>(0x0000, 0x0000, 4, 0x00f0));
   dat.insert(make_elementfield<VR::UI>(0x0000, 0x0002, 18, "1.2.840.10008.1.1"));
   dat.insert(make_elementfield<VR::US>(0x0000, 0x0120, 2, 2));
   dat.insert(make_elementfield<VR::US>(0x0000, 0x0800, 2, 0x0101));
   dat.insert(make_elementfield<VR::US>(0x0000, 0x0900, 2, 0));

   dat2.insert(make_elementfield<VR::UI>(0x0000, 0x0002, 18, "1.2.840.10008.1.1"));
   dat2.insert(make_elementfield<VR::US>(0x0000, 0x0120, 2, 65));
   dat2.insert(make_elementfield<VR::NN>(0xfffe, 0xe0dd));

   dat.insert(make_elementfield<VR::SQ>(0x0002, 0x0800, 0xffff, dat2));

   auto boog = cpr.deserialize(dat);
   for (const auto e : boog) {
      std::cout << e;
   }

   std::cout << "\n";

   commandset_data cs = cpr.serialize(boog);
   for (dataset_iterator it = cs.begin(); it != cs.end(); ++it) {
      std::cout << it->tag.element_id << " ";
   }

//   boog = cpr.deserialize(cs);
//   for (const auto e : boog) {
//      std::cout << e;
//   }
   std::cout << std::flush;

   try
   {
      upperlayer::scp sc(11112);
      dimse_pm dpm(sc);
      sc.run();
   } catch (std::exception& ec) {
      std::cout << ec.what();
   }
}
