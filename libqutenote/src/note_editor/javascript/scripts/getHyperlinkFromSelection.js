function getHyperlinkFromSelection() {
    console.log("getHyperlinkFromSelection");

    var element;
    var selection = window.getSelection();
    if (selection && selection.anchorNode && selection.anchorNode.parentNode) {
        element = selection.anchorNode.parentNode;
    }

    var foundLink = false;

    while(element) {
        if (Object.prototype.toString.call( element ) === '[object Array]') {
            console.log("Found array of elements");
            element = element[0];
            if (!element) {
                console.log("First element of the array is null");
                break;
            }
        }

        console.log("element.nodeType = " + element.nodeType);
        if (element.nodeType === 1) {
            console.log("Found element with nodeType == 1");

            if (element.nodeName === "A") {
                foundLink = true;
                console.log("Found hyperlink within the selection");
                break;
            }
        }

        element = element.parentNode;
        console.log("Checking the next parent");
    }

    if (!foundLink) {
        return;
    }

    return element.href;
}
