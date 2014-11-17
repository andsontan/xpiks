var data = require("sdk/self").data;
var contextMenu = require("sdk/context-menu");

const {Cu} = require("chrome");
const {TextEncoder, OS} = Cu.import("resource://gre/modules/osfile.jsm", {});

function writeTextToFile(text, filename) {
  var fileIO = require("sdk/io/file");
  var TextWriter = fileIO.open(filename, "w");
  if (!TextWriter.closed) {
    TextWriter.write(text);
    TextWriter.close();
  }
}

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
    }
});
