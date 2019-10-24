//Build the project

const core = require('@actions/core');
const exec = require('@actions/exec');
const process = require("process");
const fs = require('fs');

(async () => {
    await exec.exec("mkdir build");
    
    let options = {
        cwd: `${process.cwd()}/build`
    };
    
    let qmakeFile;
    let files = fs.readdirSync(process.cwd());
    for (let file of files) {
        if (file.endsWith(".pro")) qmakeFile = `${process.cwd()}/file`;
    }
    
    await exec.exec(`qmake ${qmakeFile}`, [], options);
    if (await exec.exec("make", [], options) !== 0) {
        core.setFailed("make failed.");
        return;
    }
    await exec.exec("make install INSTALL_ROOT=~/appdir", [], options);
})();
