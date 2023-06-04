#pragma once

#include <cstdio>
#include <cstdint>
#include <cstddef>

void hex_dump(const uint8_t* data, size_t size)
{
    size_t i = 0;

    for (; i < size; ++i)
    {
        printf("%02x ", data[i]);

        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }

    if (i % 16 != 0) {
        printf("\n");
    }
}

const char* get_current_time_string()
{
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);

    static char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    return buffer;
}
