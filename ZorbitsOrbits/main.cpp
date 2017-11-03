// Copyright(c) 2017 Happy Rock Studios

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "ZorbitsOrbits.h"

int main()
{
    ZorbitsOrbits::Ptr zo = new ZorbitsOrbits();
    zo->run();

    return EXIT_SUCCESS;
}
