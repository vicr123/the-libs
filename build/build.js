//Build the project

const core = require('@actions/core');
const exec = require('@actions/exec');
const process = require("process");

(async () => {
    await exec.exec("mkdir build");
    
    let options = {
        cwd: `${process.cwd()}/build`
    };
    await exec.exec("qmake ..", options);
    await exec.exec("make", options);
    await exec.exec("make install INSTALL_ROOT=~/appdir", options);
})();
