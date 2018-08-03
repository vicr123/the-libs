TEMPLATE = subdirs

library.subdir = lib

unittest.subdir = test
unittest.depends = library

SUBDIRS += library unittest
