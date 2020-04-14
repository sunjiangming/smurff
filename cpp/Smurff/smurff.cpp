#include <SmurffCpp/Priors/ILatentPrior.h>
#include <SmurffCpp/Sessions/CmdSession.h>
#include <Utils/counters.h>

int main(int argc, char** argv)
{
   using namespace smurff;

   std::shared_ptr<ISession> trainSession = create_cmd_session(argc, argv);
   { 
      COUNTER("main"); 
      trainSession->run(); 
   }
   #ifdef PROFILING
   perf_data_print();
   #endif
   return 0;
}
