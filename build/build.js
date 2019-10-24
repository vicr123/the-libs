//Build the project

const core = require('@actions/core');
const process = require('process');

let commands = null;

if (process.platform === 'linux') {
    commands = [
        "source /opt/qt512/bin/qt512-env.sh",
        "qmake",
        "make",
        "make install INSTALL_ROOT=~/appdir",
    ];
} else if (process.platform === 'darwin') {
    commands = [
        "qmake",
        "make",
        "make install INSTALL_ROOT=~/appdir",
    ];
} else if (process.platform === 'win32') {
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
