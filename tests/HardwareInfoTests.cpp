#include "HardwareInfo_p.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include <cstdlib>
#include <iostream>

namespace
{
void require(bool condition, const char *message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void writeFile(const QString &path, const QByteArray &contents)
{
    QFile file(path);
    require(file.open(QIODevice::WriteOnly | QIODevice::Text), "test fixture could not be written");
    require(file.write(contents) == contents.size(), "test fixture was only partially written");
}
} // namespace

int main()
{
    require(
        HardwareInfoDetail::normalizeIdentifier(QStringLiteral("  SERIAL  \n"))
            == QStringLiteral("SERIAL"),
        "identifiers should be trimmed");
    require(
        HardwareInfoDetail::normalizeIdentifier(QStringLiteral("unknown")).isEmpty(),
        "placeholder identifiers should be ignored");

    QTemporaryDir sysBlock;
    require(sysBlock.isValid(), "temporary sysfs directory should be available");
    QDir root(sysBlock.path());
    require(root.mkpath(QStringLiteral("loop0/device")), "loop device fixture should be created");
    require(root.mkpath(QStringLiteral("sda/device")), "disk fixture should be created");
    require(root.mkpath(QStringLiteral("sda1/device")), "partition fixture should be created");
    writeFile(root.filePath(QStringLiteral("loop0/device/serial")), QByteArray("LOOP-ID\n"));
    writeFile(root.filePath(QStringLiteral("sda/device/serial")), QByteArray("DISK-123\n"));
    writeFile(root.filePath(QStringLiteral("sda1/partition")), QByteArray("1\n"));
    writeFile(root.filePath(QStringLiteral("sda1/device/serial")), QByteArray("PARTITION-ID\n"));

    require(
        HardwareInfoDetail::findDiskId(sysBlock.path()) == QStringLiteral("DISK-123"),
        "physical disk serial should be selected");

    const QString firstFingerprint = HardwareInfoDetail::machineIdFromComponents(
        QStringLiteral("CPU-1"), QStringLiteral("DISK-1"));
    const QString secondFingerprint = HardwareInfoDetail::machineIdFromComponents(
        QStringLiteral("CPU-1"), QStringLiteral("DISK-1"));
    require(firstFingerprint == secondFingerprint, "machine fingerprint should be deterministic");
    require(firstFingerprint.size() == 64, "machine fingerprint should be a SHA-256 hex digest");
    require(
        HardwareInfoDetail::machineIdFromComponents({}, {}).isEmpty(),
        "machine fingerprint should be empty without identifiers");

    std::cout << "All hardware-info tests passed.\n";
    return EXIT_SUCCESS;
}
