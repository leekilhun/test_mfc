#include "ap/ap.hpp"


#ifdef USE_SEQUENCE_TERMINAL
bool TinyC::sequence_terminal::is_enabled = false;
#endif

void intialize()
{
  // consol out korean
  SetConsoleOutputCP(CP_UTF8);

  // create console window
#ifdef USE_SEQUENCE_TERMINAL
  auto &vt = TinyC::sequence_terminal::get_instance();
#endif


  apcr::init();
  apcr::main();
}

int main()
{
  intialize();


  while (1)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }


  apcr::exit();

#ifdef USE_SEQUENCE_TERMINAL
  TinyC::sequence_terminal::disable_sequence_terminal_processing();
#endif
  return 0;
}

