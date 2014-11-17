var data = require("sdk/self").data;
var contextMenu = require("sdk/context-menu");

const {Cu} = require("chrome");
const {TextEncoder, OS} = Cu.import("resource://gre/modules/osfile.jsm", {});

var confirmation = require("sdk/panel").Panel({
    width:350,
    height:220,
    contentURL: data.url("confirmation-dialog.html"),
    contentScriptFile: data.url("confirm-export-notification.js")
});

confirmation.port.on("clicked", function() {confirmation.hide();});

var menuItem = contextMenu.Item({
    label: "Export images keywords",
    context: contextMenu.URLContext("file://*"),
    contentScriptFile: data.url("export_keywords.js"),

    onMessage: function (items) {
        var jsonStr = JSON.stringify(items);
        var keywordsFilePath = OS.Path.join(OS.Constants.Path.desktopDir, 'keywords.txt');

        let encoder = new TextEncoder();
        let array = encoder.encode(jsonStr);
        let promise = OS.File.writeAtomic(keywordsFilePath, array, {tmpPath: "keywords.txt.tmp"});

        confirmation.show();
    }
});
