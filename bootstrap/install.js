//Install Qt 5.12 and the-libs

const core = require('@actions/core');
const exec = require('@actions/exec');
const process = require('process');

(async () => {
    if (process.platform == 'linux') {
        await exec.exec("sudo add-apt-repository -y ppa:beineri/opt-qt-5.12.3-xenial");
        await exec.exec("sudo apt-get update -qq");
        await exec.exec(`sudo apt-get install libgl1-mesa-dev qt512-meta-minimal qt512x11extras qt512tools qt512translations qt512svg qt512multimedia ${core.getInput("extra-deps-linux")}`);
        
        //Set the path to Qt
        let qtBase = "/opt/qt512"
        core.exportVariable("QTDIR", qtBase);
        core.addPath(`${qtBase}/bin`);
        core.exportVariable("LD_LIBRARY_PATH", `${qtBase}/lib/x86_64-linux-gnu:${qtBase}/lib:${process.env["LD_LIBRARY_PATH"]}`);
        core.exportVariable("PKG_CONFIG_PATH", `${qtBase}/lib/pkgconfig:${process.env["PKG_CONFIG_PATH"]}`);
    } else if (process.platform == 'darwin') {
        await exec.exec(`brew install qt5 ${core.getInput("extra-deps-mac")}`)
        core.addPath("/usr/local/opt/qt/bin");
    } else if (process.platform == 'win32') {
        //TODO
        core.setFailed("Not running on a supported platform.");
        return;
    } else {
        //Fail
        core.setFailed("Not running on a supported platform.");
        return;
    }
    
    //Install the-libs
    let options = {
        cwd: `${process.cwd()}/the-libs`
    };
    
    await exec.exec("git clone https://github.com/vicr123/the-libs.git");
    await exec.exec(`git checkout ${core.getInput("the-libs-branch")}`, [], options);
    await exec.exec(`qmake`, [], options);
    await exec.exec(`make`, [], options);
    await exec.exec(`sudo make install INSTALL_ROOT=/`, [], options);
})();
