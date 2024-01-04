#include <iostream>
#include <cstdint>
#include <filesystem>

#include <CLI/CLI.hpp>
#include <StormLib.h>

#include "mpq.h"
#include "helpers.h"

namespace fs = std::filesystem;

int main(int argc, char **argv) {
    CLI::App app{"A command line tool to read, extract, search, create and verify MPQ file using the StormLib library"};

    std::string target = "default";
    std::string output = "default";
    bool patchExtractBin = false;

    // Subcommand: About
    CLI::App *about = app.add_subcommand("about", "Prints info about the program");

    // Subcommand: Info
    CLI::App *info = app.add_subcommand("info", "Prints info about the MPQ file");
    info->add_option("target", target, "Target MPQ file")
        ->required()
        ->check(CLI::ExistingFile);

    // Subcommand: Extract
    CLI::App *extract = app.add_subcommand("extract", "Extract files from the MPQ file");
    extract->add_option("target", target, "Target MPQ file")
        ->required()
        ->check(CLI::ExistingFile);
    extract->add_option("-o,--output", output, "Output directory");

    // Subcommand: Create
    CLI::App *create = app.add_subcommand("create", "Create MPQ file from target directory");
    create->add_option("target", target, "Target directory")
        ->required()
        ->check(CLI::ExistingDirectory);
    
    // Subcommand: List
    CLI::App *list = app.add_subcommand("list", "List files from the MPQ file");
    list->add_option("target", target, "Target MPQ file")
        ->required()
        ->check(CLI::ExistingFile);

    // Subcommand: Verify
    CLI::App *verify = app.add_subcommand("verify", "Verify the MPQ file");
    verify->add_option("target", target, "Target MPQ file")
        ->required()
        ->check(CLI::ExistingFile);

    // Subcommand: Patch
    CLI::App *patch = app.add_subcommand("patch", "Various MPQ patch helpers");
    patch->add_option("target", target, "Target file")
        ->required()
        ->check(CLI::ExistingFile);
    patch->add_flag("-b,--bin", patchExtractBin, "Extract BIN file from EXE patch (default false)");

    CLI11_PARSE(app, argc, argv);

    // Handle subcommand: About
    if (app.got_subcommand(about)){
        std::cout << "mpqcli" << std::endl;
        std::cout << "\nLibraries used:" << std::endl;
        std::cout << "StormLib - MIT (https://github.com/ladislav-zezula/StormLib)" << std::endl;
        std::cout << "CLI11 - BSD (https://github.com/CLIUtils/CLI11)" << std::endl;
    }

    // Handle subcommand: Info
    if (app.got_subcommand(info)){
        HANDLE hArchive;
        OpenMpqArchive(target, &hArchive);
        PrintMpqInfo(hArchive);
    }

    // Handle subcommand: Extract
    if (app.got_subcommand(extract)) {
        // If no output directory specified, use MPQ path without extension
        // If output directory specified, create it if it doesn't exist
        if (output == "default") {
            const fs::path outputPath = fs::canonical(target);
            output = outputPath.parent_path() / outputPath.stem();
        }
        fs::create_directory(output);

        HANDLE hArchive;
        OpenMpqArchive(target, &hArchive);
        ExtractFiles(hArchive, output);
    }

    // Handle subcommand: Create
    if (app.got_subcommand(create)) {
        std::cout << "[+] Not yet implemented... Exiting" << std::endl;
        return 0;
    }

    // Handle subcommand: Verify
    if (app.got_subcommand(verify)) {
        HANDLE hArchive;
        OpenMpqArchive(target, &hArchive);
        int signatureType = GetMpqArchiveSignatureType(hArchive);

        std::string signatureName;
        switch (signatureType) {
            case 1:
                signatureName = "weak";
                break;
            case 2:
                signatureName = "strong";
                break;
            default:
                signatureName = "none";
        }

        std::string signatureNameFormatted = " (" + signatureName + ")";
        std::cout << "[+] Signature type: " << signatureType << signatureNameFormatted << std::endl;

        PrintMpqSignature(hArchive, signatureType);
        return signatureType;
    }

    // Handle subcommand: List
    if (app.got_subcommand(list)) {
        HANDLE hArchive;
        OpenMpqArchive(target, &hArchive);
        ListFiles(hArchive);
        return 1;
    }

    // Handle subcommand: Patch
    if (app.got_subcommand(patch)) {
        HANDLE hArchive;
        OpenMpqArchive(target, &hArchive);
        ExtractMpqAndBinFromExe(hArchive, patchExtractBin);
    }

    return 0;
}
