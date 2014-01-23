#include "Server.hh"

int		main(int argc, char *argv[])
{
  Server	server((argc > 1) ? (LibC::atoi(argv[1])) : (DEFAULT_PORT));

  if (argc > 2)
    std::cerr << "Warning: You must enter only one arguments or none. (USAGE: " << argv[0] << " [PORT])" << std::endl;
  return (server.start());
}
