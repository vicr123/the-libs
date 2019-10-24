//Install Qt 5.12 and the-libs

const core = require('@actions/core');
const process = require('process');

let commands = null;

if (process.platform == 'linux') {
    commands = [
        "sudo add-apt-repository -y ppa:beineri/opt-qt-5.12.3-xenial",
        "sudo apt-get update -qq",
        `sudo apt-get install libgl1-mesa-dev qt512-meta-minimal qt512x11extras qt512tools qt512translations qt512svg qt512multimedia ${core.getInput("extra-deps-linux")}`,
        "source /opt/qt512/bin/qt512-env.sh",
        "git clone https://github.com/vicr123/the-libs.git",
        "cd the-libs",
        "git checkout blueprint",
        "qmake",
        "make",
        "sudo make install INSTALL_ROOT=/"
    ];
} else if (process.platform == 'darwin') {
    commands = [
        `brew install qt5 ${core.getInput("extra-deps-mac")}`,
        "git clone https://github.com/vicr123/the-libs.git",
        "cd the-libs",
        "git checkout blueprint",
        "qmake",
        "make",
        "sudo make install INSTALL_ROOT=/"
    ];
    
    core.addPath("/usr/local/opt/qt/bin");
} else if (process.platform == 'win32') {
    //TODO
    core.setFailed("Not running on a supported platform.");
} else {
    //Fail
    core.setFailed("Not running on a supported platform.");
}

if (commands) {
    const child_process = require('child_process');
    let retval = child_process.spawnSync(commands.join("; "), [], {
        shell: (process.platform === 'win32' ? true : "/bin/bash")
    });
    if (retval.status != 0) process.exit(1);
}
