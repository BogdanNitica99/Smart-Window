#include <pistache/endpoint.h>
#include <pistache/router.h>

using namespace std;
using namespace Pistache;
using namespace Pistache::Rest;


void testRoute(const Rest::Request& request, Http::ResponseWriter response) {
    cout << "Received a request from IP address " << request.address().host() << '\n';

    response.send(Http::Code::Ok, "Test Route Ok!\n");
} 


int main() {
    Router router;

    router.get("/test", Routes::bind(testRoute));

    Address addr(Ipv4::any(), Port(9080));

    auto opts = Http::Endpoint::options().threads(1);
    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(router.handler());
    server.serve(); 
}