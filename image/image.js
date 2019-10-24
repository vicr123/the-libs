//Image the project

const core = require('@actions/core');
const exec = require('@actions/exec');
const io = require('@actions/io');
const tc = require('@actions/tool-cache');
const process = require('process');
const fs = require('fs');

(async () => {
    if (process.platform === 'linux') {
        let extraPlugins = core.getInput("extra-plugins-linux");
        if (extraPlugins !== "") {
            extraPlugins = `,${extraPlugins}`;
        }
        
        const ldepqt = await tc.downloadTool("https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage");
        await exec.exec(`chmod a+x ${ldepqt}`);
        
        let ldepqtArgs = [];
        
        let applications = fs.readdirSync(`${process.env["HOME"]}/appdir/usr/share/applications/`);
        for (let application of applications) {
            ldepqtArgs.push(`${process.env["HOME"]}/appdir/usr/share/applications/${application}`);
        }
        
        ldepqtArgs.push("-appimage");
        ldepqtArgs.push(`-extra-plugins=iconengines/libqsvgicon.so,imageformats/libqsvg.so${extraPlugins}`);
        if (await exec.exec(ldepqt, ldepqtArgs) != 0) {
            core.setFailed("linuxdeployqt failed.");
            return;
        }
        
        let appimages = fs.readdirSync(process.cwd());
        for (let appimage of appimages) {
            if (appimage.endsWith(".AppImage")) {
                await io.cp(`${process.cwd()}/${appimage}`, `${process.env["HOME"]}/${core.getInput("image-name-linux")}`);
            }
        }
        
        core.setOutput("image-path", `${process.env["HOME"]}/${core.getInput("image-name-linux")}`);
    } else if (process.platform === 'darwin') {
        //TODO: we need to figure out what to do with install_name_tool etc.
        core.setFailed("Not running on a supported platform.");
        return;
    } else if (process.platform === 'win32') {
        //TODO
        core.setFailed("Not running on a supported platform.");
        return;
    } else {
        //Fail
        core.setFailed("Not running on a supported platform.");
        return;
    }
})();
