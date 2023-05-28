#pragma once

#include <cstdint>

enum PrinterCommand : uint8_t
{
    SetPaperSize = 0xA1,
    PrintData = 0xA2,
    Status = 0xA3,
    SetHeat = 0xA4,
    PrintStartStop = 0xA6,
    GetDeviceInfo = 0xA8,
    SetEnergy = 0xAF,
    Draft = 0xBE,
    PaperFeedSpeed = 0xBD,
    SetPrintMode = 0xBE, // ???
    PrintDataCompressed = 0xBF,
};

const char* printerCommandToString(uint8_t command)
{
    switch (command)
    {
    case PrinterCommand::PrintData:
        return "PrintData";
    case PrinterCommand::Status:
        return "Status";
    case PrinterCommand::SetHeat:
        return "SetHeat";
    case PrinterCommand::PrintStartStop:
        return "PrintStartStop";
    case PrinterCommand::GetDeviceInfo:
        return "GetDeviceInfo";
    case PrinterCommand::SetEnergy:
        return "SetEnergy";
    case PrinterCommand::Draft:
        return "Draft";
    case PrinterCommand::PaperFeedSpeed:
        return "PaperFeedSpeed";
    case PrinterCommand::PrintDataCompressed:
        return "PrintDataCompressed";
    default:
        return "Unknown";
    }
}
