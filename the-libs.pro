TEMPLATE = subdirs

library.subdir = lib

unittest.subdir = test
unittest.depends = library

SUBDIRS += library

unix:!macx {
    SUBDIRS += tltrdesktop
}

#!android {
#    unittest
#}
