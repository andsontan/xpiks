function Component()
{
    // constructor
    component.loaded.connect(this, Component.prototype.loaded);
}

Component.prototype.isDefault = function()
{
    // select the component by default
    return true;
}

Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();
		
		if (installer.value("os") === "win") {
			component.addOperation("CreateShortcut", "@TargetDir@/xpiks-qt.exe", "@StartMenuDir@/Xpiks.lnk", "workingDirectory=@TargetDir@");
			component.addOperation("CreateShortcut", "@TargetDir@/xpiks-qt.exe", "@DesktopDir@/Xpiks.lnk", "workingDirectory=@TargetDir@");
		}
    } catch (e) {
        console.log(e);
    }
}

Component.prototype.loaded = function ()
{
}