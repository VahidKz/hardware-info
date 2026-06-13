#pragma once

#include <QString>

class HardwareInfo final
{
public:
    HardwareInfo() = delete;

    [[nodiscard]] static QString getCpuId();
    [[nodiscard]] static QString getDiskId();
    [[nodiscard]] static QString getMachineId();
};
