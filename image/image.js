//Image the project

const core = require('@actions/core');
const process = require('process');

let commands = null;

if (process.platform === 'linux') {
    let extraPlugins = core.getInput("extra-plugins-linux");
    if (extraPlugins !== "") {
        extraPlugins = `,${extraPlugins}`;
    }
    
    commands = [
        "source /opt/qt512/bin/qt512-env.sh",
        'wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"'
        "chmod a+x linuxdeployqt-continuous-x86_64.AppImage"
        `./linuxdeployqt-continuous-x86_64.AppImage ~/appdir/usr/share/applications/*.desktop -appimage -extra-plugins=iconengines/libqsvgicon.so,imageformats/libqsvg.so${extraPlugins}`
    ];
} else if (process.platform === 'darwin') {
//     commands = [
//         "qmake",
//         "make",
//         "make install INSTALL_ROOT=~/appdir",
//     ];
    
    //TODO: we need to figure out what to do with install_name_tool etc.
    core.setFailed("Not running on a supported platform.");
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
