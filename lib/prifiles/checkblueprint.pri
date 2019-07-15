message("$$_PRO_FILE_PWD_")

BLUEPRINT_CHECK_GIT_BRANCH = $$system("git -C $$_PRO_FILE_PWD_ rev-parse --abbrev-ref HEAD")
equals(BLUEPRINT_CHECK_GIT_BRANCH, "blueprint") {
    message("Building in Blueprint configuration")
    DEFINES += T_BLUEPRINT_BUILD
    CONFIG += blueprint
} else {
    message("Building in Stable configuration")
    DEFINES += T_STABLE_BUILD
}
