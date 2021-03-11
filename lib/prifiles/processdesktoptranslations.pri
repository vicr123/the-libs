unix:!macx {
    desktopgen.output = ${QMAKE_FILE_BASE}.desktop
    desktopgen.commands = tltrdesktop --json-generate --desktop-generate --desktop-template ${QMAKE_FILE_NAME} --json-directory $$_PRO_FILE_PWD_/translations/desktop/${QMAKE_FILE_BASE}/ --desktop-output ${QMAKE_FILE_OUT}
    desktopgen.input = DESKTOP_FILE
    desktopgen.depend_command = ls $$_PRO_FILE_PWD_/translations/desktop/${QMAKE_FILE_BASE}/*.json ${QMAKE_FILE_NAME}
    desktopgen.CONFIG = no_link target_predeps explicit_dependencies
    desktopgen.variable_out = DESKTOP_FILE_GENERATED

    desktopgenbp.output = ${QMAKE_FILE_BASE}.desktop
    desktopgenbp.commands = tltrdesktop --json-generate --desktop-generate --desktop-template ${QMAKE_FILE_NAME} --json-directory $$_PRO_FILE_PWD_/translations/desktop/${QMAKE_FILE_BASE}/ --desktop-output ${QMAKE_FILE_OUT}
    desktopgenbp.input = DESKTOP_FILE_BLUEPRINT
    desktopgenbp.depend_command = ls $$_PRO_FILE_PWD_/translations/desktop/${QMAKE_FILE_BASE}/*.json ${QMAKE_FILE_NAME}
    desktopgenbp.CONFIG = no_link target_predeps explicit_dependencies
    desktopgenbp.variable_out = DESKTOP_FILE_BLUEPRINT_GENERATED

    QMAKE_EXTRA_COMPILERS = desktopgen desktopgenbp

    !isEmpty(DESKTOP_FILE):!blueprint {
        desktop.files = DESKTOP_FILE_GENERATED
        desktop.path = $$[QT_INSTALL_PREFIX]/share/applications
        desktop.extra = $$QMAKE_COPY $${OUT_PWD}/$$DESKTOP_FILE $(INSTALL_ROOT)$$THELIBS_INSTALL_PREFIX/share/applications/
        INSTALLS += desktop

        message(Will install $$DESKTOP_FILE as the desktop file)
    }

    !isEmpty(DESKTOP_FILE_BLUEPRINT):blueprint {
        desktopbp.files = DESKTOP_FILE_GENERATED_BLUEPRINT
        desktopbp.path = $$[QT_INSTALL_PREFIX]/share/applications
        desktopbp.extra = $$QMAKE_COPY $${OUT_PWD}/$$DESKTOP_FILE_BLUEPRINT $(INSTALL_ROOT)$$THELIBS_INSTALL_PREFIX/share/applications/
        INSTALLS += desktopbp

        message(Will install $$DESKTOP_FILE_BLUEPRINT as the desktop file)
    }
}
