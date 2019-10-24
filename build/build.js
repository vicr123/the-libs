//Build the project

const core = require('@actions/core');
const exec = require('@actions/exec');

(async () => {
    await exec.exec("qmake");
    await exec.exec("make");
    await exec.exec("make install INSTALL_ROOT=~/appdir");
})();
