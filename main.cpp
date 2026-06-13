#include "HardwareInfo.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("hardware-info"));
    QCoreApplication::setApplicationVersion(QStringLiteral(HARDWARE_INFO_VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Read stable hardware identifiers on Linux."));
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption jsonOption(
        QStringLiteral("json"),
        QStringLiteral("Print machine-readable JSON."));
    parser.addOption(jsonOption);
    parser.process(application);

    const QString cpuId = HardwareInfo::getCpuId();
    const QString diskId = HardwareInfo::getDiskId();
    const QString machineId = HardwareInfo::getMachineId();

    QTextStream output(stdout);
    if (parser.isSet(jsonOption)) {
        const QJsonObject result = {
            {QStringLiteral("cpuId"), cpuId},
            {QStringLiteral("diskId"), diskId},
            {QStringLiteral("machineId"), machineId},
        };
        output << QString::fromUtf8(QJsonDocument(result).toJson(QJsonDocument::Indented));
        return 0;
    }

    output << "CPU ID:     " << (cpuId.isEmpty() ? QStringLiteral("unavailable") : cpuId) << '\n'
           << "Disk ID:    " << (diskId.isEmpty() ? QStringLiteral("unavailable") : diskId) << '\n'
           << "Machine ID: " << (machineId.isEmpty() ? QStringLiteral("unavailable") : machineId)
           << '\n';
    return 0;
}
