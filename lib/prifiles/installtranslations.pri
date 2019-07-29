# Install translations

unix:!macx {
    # UNIX: Put translations in /usr/share/appname/translations
    isEmpty(SHARE_APP_NAME) {
        error(Please define SHARE_APP_NAME with the name of the folder to store data in /usr/share.)
    }

    translations.files = translations/*.qm

    translations.path = /usr/share/$$SHARE_APP_NAME/translations
    INSTALLS += translations
}

macx {
    #macOS: Put translations inside the application bundle
    translations.files = translations/
    translations.path = Contents/translations

    QMAKE_BUNDLE_DATA += translations
}

win {
    # Not our job to install translations on Windows.
    # theInstaller will deal with putting files in the right place
}
