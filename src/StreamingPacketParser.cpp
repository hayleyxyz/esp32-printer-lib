#include <cstdio>
#include "PrinterCommand.h"
#include "Checksum8Bit.h"
#include "StreamingPacketParser.h"
#include <Arduino.h>
#include <esp_cpu.h>

void StreamingPacketParser::parse(uint8_t* data, size_t length) {
    size_t remainingDataLength = length;

    if (state == ParseState::Data) {
        packetData = data;
    }

    if (state == ParseState::Header) {
        if (length < sizeof(PacketHeader)) {
            memcpy(headerBuffer, data, length);
            headerIndex = length;
            return;
        }

        if (headerIndex > 0) {
            memcpy(headerBuffer + headerIndex, data, sizeof(PacketHeader) - headerIndex);
            header = reinterpret_cast<PacketHeader*>(headerBuffer);
            packetData = data + (sizeof(PacketHeader) - headerIndex);
            remainingDataLength -= sizeof(PacketHeader) - headerIndex;
            headerIndex = 0;

            if (header->magic != PrinterPacket::Magic)
            {
                printf("Invalid packet: magic mismatch\n");
                exit(EXIT_FAILURE);
                return;
            }
        }
        else {
            if (!PrinterPacket::dissectPacket(data, length, &header, &packetData, nullptr)) {
                printf("Invalid packet: dissectPacket() failed\n");
                exit(EXIT_FAILURE);
                return;
            }

            remainingDataLength -= sizeof(PacketHeader);
        }
        
        remainingPacketDataLength = header->length;
        remainingPacketFooterLength = sizeof(PacketFooter);

        checksum = 0;

        auto ms = pdTICKS_TO_MS(esp_cpu_get_ccount());

        Serial.print(ms, DEC);

        printf(" Command: (%x) %s | ", header->command, printerCommandToString(header->command));

        state = ParseState::Data;
    }

    if (state == ParseState::Data) {
        while (remainingDataLength > 0 && remainingPacketDataLength > 0) {
            printf("%02x ", *packetData);

            checksum = Checksum8Bit::calculate(packetData, 1, checksum);

            packetData++;
            remainingDataLength--;
            remainingPacketDataLength--;
        }

        if (remainingPacketDataLength == 0) {
            state = ParseState::Footer;
        }
    }

    if (state == ParseState::Footer) {
        while (remainingDataLength > 0 && remainingPacketFooterLength > 0) {
            uint8_t *footerData = data + (length - remainingDataLength);

            if (remainingPacketFooterLength == 1) {
                if (*footerData != PrinterPacket::End) {
                    printf("Unexpected: end byte not found\n");
                }
            }
            else if (remainingPacketFooterLength == 2) {
                if (*footerData != checksum) {
                    printf("Unexpected: checksum mismatch. Expected: %02x, actual: %02x\n", checksum,  *footerData);
                }
            }

            remainingPacketFooterLength--;
            remainingDataLength--;
        }

        if (remainingPacketFooterLength == 0) {
            state = ParseState::Header;

            if (remainingDataLength > 0) {
                putchar('\n');
                parse(data + (length - remainingDataLength), remainingDataLength);
                return;
            }
        }

        putchar('\n');
    }
}