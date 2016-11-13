//
// Created by Lectem on 12/11/2016.
//

#include <FileStream.h>
#include <MpqArchive.h>
#include <fmt/format.h>

using namespace WorldStone;
int main(int argc, char* argv[])
{
    if (argc >= 3) {
        const char* mpqFilename   = argv[1];
        const char* fileToExtract = argv[2];
        MpqArchive  mpqArchive(mpqFilename);
        if (!mpqArchive.good()) fmt::print("Could not open {}\n", mpqFilename);
        if (mpqArchive.exists(fileToExtract)) fmt::print("The file is in the MPQ !\n");
        else
            fmt::print("The file {} was not found in {}\n", fileToExtract, mpqFilename);
    }
    else
        fmt::print("MPQextract usage : MPQextract archive.mpq filetoextract\n");

    return 0;
}
