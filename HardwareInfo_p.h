#pragma once

#include <QString>

namespace HardwareInfoDetail
{
[[nodiscard]] QString normalizeIdentifier(const QString &value);
[[nodiscard]] QString findDiskId(const QString &sysBlockPath);
[[nodiscard]] QString machineIdFromComponents(const QString &cpuId, const QString &diskId);
} // namespace HardwareInfoDetail
