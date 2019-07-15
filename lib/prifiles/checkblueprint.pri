#Check to see if we should configure for Blueprint
#Ask Git for the current branch
BLUEPRINT_CHECK_GIT_BRANCH = $$system("git -C $$_PRO_FILE_PWD_ rev-parse --abbrev-ref HEAD")
equals(BLUEPRINT_CHECK_GIT_BRANCH, "blueprint") {
    CONFIG += blueprint
}

#Check to see if we're on Travis
BLUEPRINT_CHECK_GIT_BRANCH = $$(TRAVIS_BRANCH)
equals(BLUEPRINT_CHECK_GIT_BRANCH, "blueprint") {
    CONFIG += blueprint
}

#Check to see if we're on AppVeyor
BLUEPRINT_CHECK_GIT_BRANCH = $$(APPVEYOR_REPO_BRANCH)
equals(BLUEPRINT_CHECK_GIT_BRANCH, "blueprint") {
    CONFIG += blueprint
}

blueprint {
    message("Building in Blueprint configuration")
    DEFINES += T_BLUEPRINT_BUILD
} else {
    message("Building in Stable configuration")
    DEFINES += T_STABLE_BUILD
}
