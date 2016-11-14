#include "main.h"
#include "engine.h"
#include "eventserver.h"

int main(int argc, char * argv[]) {

    mainInit(argc, argv);
    engineInit();
    serverRun(argc, argv);
    engineRun();

    serverStop();

    return 0;
}

void mainInit(int argc, char *argv[]) {
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp (argv[i], "--help") == 0 || strcmp (argv[i], "-h") == 0) {
            printUsage();
        }
    }
}

void printUsage() {
    printf(
        "--- GPRS by MotionBI ---\n"
        "Usage:\n"
        "    gprs [--port|-p <port number>]\n"
        "    gprs [--help|-h]\n"
        "\n"
        "Options:\n"
        "    --port|-p    set event server's port to \"port number\"\n"
        "    --help|-h    show this usage message\n"
        "\n"
        "Bye! Have a beautiful time!\n"
        );
    exit(0);
}
