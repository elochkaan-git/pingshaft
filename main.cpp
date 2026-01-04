#include "ping.hpp"
#include "ping_dispatcher.hpp"

int
main()
{
  // Example of usage
  PingDispatcher pd(new JavaPing, new BedrockPing);
  Server t1{ "bee.mc-complex.com", 19132, Platform::Bedrock };
  Server t2{ "play.funnymc.net", 25565, Platform::Java };
  auto r1 = pd.ping(t1);
  auto r2 = pd.ping(t2);

  return 0;
}