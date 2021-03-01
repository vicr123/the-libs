unix:!macx {
    desktopgen.output = ${QMAKE_FILE_BASE}.desktop
    desktopgen.commands = tltrdesktop --desktop-generate --desktop-template ${QMAKE_FILE_NAME} --json-directory $$_PRO_FILE_PWD_/translations/desktop/${QMAKE_FILE_NAME}/ --desktop-output ${QMAKE_FILE_OUT}
    desktopgen.input = DESKTOP_FILE
    desktopgen.CONFIG = no_link target_predeps
    desktopgen.variable_out = DESKTOP_FILE_GENERATED

    desktopgenbp.output = ${QMAKE_FILE_BASE}.desktop
    desktopgenbp.commands = tltrdesktop --desktop-generate --desktop-template ${QMAKE_FILE_NAME} --json-directory $$_PRO_FILE_PWD_/translations/desktop/${QMAKE_FILE_NAME}/ --desktop-output ${QMAKE_FILE_OUT}
    desktopgenbp.input = DESKTOP_FILE_BLUEPRINT
    desktopgenbp.CONFIG = no_link target_predeps
    desktopgenbp.variable_out = DESKTOP_FILE_BLUEPRINT_GENERATED

    QMAKE_EXTRA_COMPILERS = desktopgen desktopgenbp

    !isEmpty(DESKTOP_FILE):!blueprint {
        desktop.files = DESKTOP_FILE_GENERATED
        desktop.path = /usr/share/applications
        desktop.extra = $$QMAKE_COPY $${OUT_PWD}/$$DESKTOP_FILE $(INSTALL_ROOT)/usr/share/applications
        INSTALLS += desktop

        message(Will install $$DESKTOP_FILE as the desktop file)
    }

    !isEmpty(DESKTOP_FILE_BLUEPRINT):blueprint {
        desktop.files = DESKTOP_FILE_GENERATED_BLUEPRINT
        desktop.path = /usr/share/applications
        desktop.extra = $$QMAKE_COPY $${OUT_PWD}/$$DESKTOP_FILE_BLUEPRINT $(INSTALL_ROOT)/usr/share/applications
        INSTALLS += desktop

        message(Will install $$DESKTOP_FILE_BLUEPRINT as the desktop file)
    }
}
