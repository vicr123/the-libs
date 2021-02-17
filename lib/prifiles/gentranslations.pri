# Generate translations

# Define all target languages
TRANSLATIONS += $$files($$_PRO_FILE_PWD_/translations/*.ts)

SKIP_GENTRANSLATION = $$(THE_LIBS_SKIP_GENTRANSLATION)
isEmpty(SKIP_GENTRANSLATION) {
    # Prepare lupdate and lrelease
    qtPrepareTool(LUPDATE, lupdate)
    qtPrepareTool(LRELEASE, lrelease)

    # Run lupdate before each build
    genlang.commands = "$$LUPDATE -no-obsolete -source-language en_US $$_PRO_FILE_"

    # Run lrelease before each build
    rellang.commands = "$$LRELEASE -removeidentical $$_PRO_FILE_"
    rellang.depends = genlang

    QMAKE_EXTRA_TARGETS = genlang rellang
    PRE_TARGETDEPS = genlang rellang
} else {
    message(Skipping translation generation)
}

DISTFILES += \
    $$PWD/checkblueprint.pri
