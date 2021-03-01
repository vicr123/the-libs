/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include <QCoreApplication>
#include <QTextStream>
#include <QCommandLineParser>
#include <QDir>

#include "desktopfile.h"
#include "jsonfile.h"

void genJsonFile(QString desktopFile, QDir outputDirectory) {
    QTextStream output(stdout);

    JsonFile file;
    DesktopFile desktop(desktopFile);
    while (desktop.nextLine()) {
        if (desktop.isLineTranslatable()) {
            //Export this line to the translation file
            file.insertString(desktop.key(), desktop.value(), desktop.comment());
        }
    }

    //Write the output to en.json
    QString outputFile = outputDirectory.absoluteFilePath("en.json");
    output << "Generating file " << outputFile << "...\n";

    QFile f(outputFile);
    f.open(QFile::WriteOnly);
    f.write(file.output());
    f.close();
}

void genDesktopFile(QString templateFile, QDir jsonDirectory, QString outputFile) {
    QTextStream output(stdout);
    QMap<QString, JsonFile> jsonFiles;
    for (const QFileInfo& file : jsonDirectory.entryInfoList({"*.json"}, QDir::Files)) {
        JsonFile jsonFile(file.absoluteFilePath());
        jsonFiles.insert(file.baseName(), jsonFile);
    }

    DesktopFile desktop(templateFile);
    while (desktop.nextLine()) {
        if (desktop.isLineTranslatable()) {
            //Insert the translations into the desktop file
            //First, insert the translation without a locale
            desktop.insert(desktop.key(), desktop.value());

            for (const QString& locale : jsonFiles.keys()) {
                QString value = jsonFiles.value(locale).message(desktop.key());
                desktop.insert(desktop.key(), value, locale);
            }
        }
    }

    //Output to the output file
    output << "Generating file " << outputFile << "...\n";

    QFile f(outputFile);
    f.open(QFile::WriteOnly);
    f.write(desktop.outputText().toUtf8());
    f.close();
}

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.addOption({{"j", "json-generate"}, "Generate JSON Translation files from .desktop files"});
    parser.addOption({{"k", "desktop-generate"}, "Generate .desktop files from JSON Translation files"});
    parser.addOption({{"d", "json-directory"}, "Directory of JSON Translation files", "json-directory"});
    parser.addOption({{"t", "desktop-template"}, ".desktop template file", "desktop template"});
    parser.addOption({{"o", "desktop-output"}, ".desktop output file", "desktop output"});
    QCommandLineOption helpOption = parser.addHelpOption();
    parser.parse(a.arguments());

    QTextStream output(stdout);
    QTextStream eoutput(stderr);
    if (parser.isSet(helpOption)) {
        output << parser.helpText() << "\n";
        return 0;
    }

    bool genJson = parser.isSet("json-generate");
    bool genDesktop = parser.isSet("desktop-generate");
    if (!genJson && !genDesktop) {
        eoutput << "error: At least one of --json-generate or --desktop-generate must be specified.\n";
        return 1;
    }

    if (!parser.isSet("desktop-template")) {
        eoutput << "error: A desktop template file must be specified with --desktop-template.\n";
        return 1;
    }

    if (!parser.isSet("json-directory")) {
        eoutput << "error: A JSON output directory must be specified with --json-directory.\n";
        return 1;
    }

    if (!parser.isSet("desktop-output") && genDesktop) {
        eoutput << "error: A .desktop output file must be specified with --desktop-output.\n";
        return 1;
    }

    QString desktopTemplate = parser.value("desktop-template");
    QDir outputDirectory(parser.value("json-directory"));
    if (!outputDirectory.exists()) outputDirectory.mkpath(".");

    if (genJson) {
        //Generate the en JSON file from a .desktop file
        genJsonFile(desktopTemplate, outputDirectory);
    }
    if (genDesktop) {
        //Generate the final .desktop file from JSON files
        genDesktopFile(desktopTemplate, outputDirectory, parser.value("desktop-output"));
    }

    return 0;
}
