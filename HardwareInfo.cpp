#include "HardwareInfo.h"

#include "HardwareInfo_p.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QStringList>

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#include <cpuid.h>
#endif

namespace
{
QString readTextFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    return QString::fromUtf8(file.readAll()).trimmed();
}

QString cpuSerialFromProc()
{
    QFile cpuInfo(QStringLiteral("/proc/cpuinfo"));
    if (!cpuInfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    while (!cpuInfo.atEnd()) {
        const QString line = QString::fromUtf8(cpuInfo.readLine());
        const auto separator = line.indexOf(QLatin1Char(':'));
        if (separator < 0) {
            continue;
        }

        if (line.left(separator).trimmed().compare(QStringLiteral("Serial"), Qt::CaseInsensitive) == 0) {
            return HardwareInfoDetail::normalizeIdentifier(line.mid(separator + 1));
        }
    }

    return {};
}

QString x86ProcessorSignature()
{
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
    unsigned int eax = 0;
    unsigned int ebx = 0;
    unsigned int ecx = 0;
    unsigned int edx = 0;

    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx) != 0) {
        return QStringLiteral("%1%2")
            .arg(eax, 8, 16, QLatin1Char('0'))
            .arg(edx, 8, 16, QLatin1Char('0'))
            .toUpper();
    }
#endif

    return {};
}
} // namespace

namespace HardwareInfoDetail
{
QString normalizeIdentifier(const QString &value)
{
    const QString normalized = value.trimmed();
    static const QStringList invalidValues = {
        QStringLiteral("None"),
        QStringLiteral("Unknown"),
        QStringLiteral("Not Specified"),
    };

    for (const QString &invalidValue : invalidValues) {
        if (normalized.compare(invalidValue, Qt::CaseInsensitive) == 0) {
            return {};
        }
    }

    return normalized;
}

QString findDiskId(const QString &sysBlockPath)
{
    const QDir blockDevices(sysBlockPath);
    const QStringList deviceNames =
        blockDevices.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

    for (const QString &deviceName : deviceNames) {
        if (deviceName.startsWith(QStringLiteral("loop"))
            || deviceName.startsWith(QStringLiteral("ram"))
            || deviceName.startsWith(QStringLiteral("zram"))
            || deviceName.startsWith(QStringLiteral("fd"))
            || deviceName.startsWith(QStringLiteral("sr"))) {
            continue;
        }

        const QString devicePath = blockDevices.filePath(deviceName);
        if (QFile::exists(QDir(devicePath).filePath(QStringLiteral("partition")))) {
            continue;
        }

        const QStringList identifierPaths = {
            QDir(devicePath).filePath(QStringLiteral("device/serial")),
            QDir(devicePath).filePath(QStringLiteral("device/wwid")),
            QDir(devicePath).filePath(QStringLiteral("wwid")),
        };

        for (const QString &identifierPath : identifierPaths) {
            const QString identifier = normalizeIdentifier(readTextFile(identifierPath));
            if (!identifier.isEmpty()) {
                return identifier;
            }
        }
    }

    return {};
}

QString machineIdFromComponents(const QString &cpuId, const QString &diskId)
{
    QStringList components;
    if (!cpuId.isEmpty()) {
        components.append(QStringLiteral("cpu:") + cpuId);
    }
    if (!diskId.isEmpty()) {
        components.append(QStringLiteral("disk:") + diskId);
    }
    if (components.isEmpty()) {
        return {};
    }

    const QByteArray input = components.join(QLatin1Char('\n')).toUtf8();
    return QString::fromLatin1(QCryptographicHash::hash(input, QCryptographicHash::Sha256).toHex());
}
} // namespace HardwareInfoDetail

QString HardwareInfo::getCpuId()
{
    const QString serial = cpuSerialFromProc();
    return serial.isEmpty() ? x86ProcessorSignature() : serial;
}

QString HardwareInfo::getDiskId()
{
    return HardwareInfoDetail::findDiskId(QStringLiteral("/sys/class/block"));
}

QString HardwareInfo::getMachineId()
{
    return HardwareInfoDetail::machineIdFromComponents(getCpuId(), getDiskId());
}
