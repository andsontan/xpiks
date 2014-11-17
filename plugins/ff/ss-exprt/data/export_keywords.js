self.on("click", function (node) {
    var rows = document.querySelectorAll("div.shutterstock_submit_page > center > table > tbody > tr");

    var itemsLength = rows.length;

    var items = [];

    for (var i = 0; i < itemsLength; i++) {
        var row = rows[i];

        if (row.children.length < 4)
            continue;

        var subitems = row.children[3].querySelectorAll("table>tbody>tr>td>nobr");
        var subitemsArr = Array.prototype.slice.call(subitems);
        var categoriesArr = subitemsArr.map(function(e){return e.innerHTML.trim();});

        items.push({
            ss_id: row.children[0].innerHTML.trim(),
            image_title: row.children[1].querySelector("div > img").title,
            keywords: row.querySelector(".keywords-col").innerHTML.replace(/\s+|\s+/g, ''),
            categories: categoriesArr.join(",")
        });
    }

    self.postMessage(items);
});
