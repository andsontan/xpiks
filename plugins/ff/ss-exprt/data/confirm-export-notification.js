var div = document.getElementById("confirmation");
div.addEventListener("click", function(){self.port.emit("clicked");});
