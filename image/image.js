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
        core.setOutput("asset-name", core.getInput("image-name-linux"));
        core.setOutput("asset-content-type", "application/x-appimage");
    } else if (process.platform === 'darwin') {        
        let bundlePath = core.getInput("app-bundle-mac");
        if (bundlePath === "") {
            core.setFailed("Not running on a supported platform.");
            return;
        }
        
        bundlePath = `${process.cwd()}/build/${bundlePath}`;
        let executableName = bundlePath.remove(".app");
        if (executableName.includes("/")) executableName = executableName.substr(executableName.lastIndexOf("/"));
        
        //Embed libraries
        let embedLibs = core.getInput("embed-libraries-mac").split(" ");
        embedLibs.push("the-libs");
        
        let libDir = `${bundlePath}/Contents/Libraries`;
        await io.mkdirP(libDir);
        
        for (let lib of embedLibs) {
            if (lib == "") continue;
            
            await io.cp(`/usr/local/lib/lib${lib}.1.dylib`, libDir);
            await exec.exec("install_name_tool", ["-change", `lib${lib}.1.dylib`, `@executable_path/../Libraries/lib${lib}.1.dylib`, `${bundlePath}/Contents/MacOS/${executableName}`])
        }
        
        await exec.exec("macdeployqt", [bundlePath]);
    } else if (process.platform === 'win32') {
        //TODO
        core.setFailed("Not running on a supported platform.");
        return;
    } else {
        //Fail
        core.setFailed("Not running on a supported platform.");
        return;
    }
})().catch(function(error) {
    console.log(error);
    core.setFailed("Catastrophic Failure");
});
