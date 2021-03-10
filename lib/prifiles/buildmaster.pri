# Set common variables
include(varset.pri)

# Generate translations
include(gentranslations.pri)

# Install translations
include(installtranslations.pri)

# Check if this is a Blueprint build
include(checkblueprint.pri)

# Generate .desktop translations
include(processdesktoptranslations.pri)
