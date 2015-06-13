$(document).ready(function () {
    getInputsRepeat();
    var rb = document.getElementsByName('r1');
    for (i = 0; i < rb.length; i++) {
        rb[i].onclick = function () {
            sending = true;
            console.log(this.value);
            $.ajax({
                type: "POST",
                url: "relays.data",
                data: "1=" + this.value,
            }).done(getInputs());
        };
    }
    var rb = document.getElementsByName('r2');
    for (i = 0; i < rb.length; i++) {
        rb[i].onclick = function () {
            sending = true;
            console.log(this.value);
            $.ajax({
                type: "POST",
                url: "relays.data",
                data: "2=" + this.value,
            }).done(getInputs());
        };
    }
    var rb = document.getElementsByName('r3');
    for (i = 0; i < rb.length; i++) {
        rb[i].onclick = function () {
            sending = true;
            console.log(this.value);
            $.ajax({
                type: "POST",
                url: "relays.data",
                data: "3=" + this.value,
            }).done(getInputs());
        };
    }
    var rb = document.getElementsByName('r4');
    for (i = 0; i < rb.length; i++) {
        rb[i].onclick = function () {
            sending = true;
            console.log(this.value);
            $.ajax({
                type: "POST",
                url: "relays.data",
                data: "4=" + this.value,
            }).done(getInputs());
        };
    }
}); //end of on document ready
//get inputs once
var req2;
function getInputs() {
    if (req) req.abort();
    if (req2) req2.abort();
    req2 = $.ajax({
        type: "GET",
        url: "sensors.xml",
        cache: false,
        dataType: "xml",
        success: xmlParseInputs,
    });
}
//get inputs repeatedly
var req;
function getInputsRepeat() {
    req = $.ajax({
        type: "GET",
        url: "sensors.xml",
        cache: false,
        dataType: "xml",
        success: xmlParseInputs,
        complete: function () {
            setTimeout(getInputsRepeat, 3000);
        }
    })
}

var runned = false;
//parse inputs
function xmlParseInputs(xml) {
    //set panels
    if (!runned) {
        $(document).find(".wrapContent.message").hide();
        $(document).find(".wrapContent.data").removeAttr('style');
        runned = true;
    }

    //sensor values
    $(xml).find("Sen V").each(function () {
        var i = $(this).index();
        var value = $(this).text().trim();
        $(".sensor").eq(i).text(value);
    })
    //relay radio buttons
    $(xml).find("Mod M").each(function () {
        var i = $(this).index();
        var mode = $(this).text().trim();
        var name = "r" + String(i + 1);
        var elements = document.getElementsByName(name);
        switch (parseInt(mode)) {
            case 1:
                elements[0].checked = true;
                break;
            case 0:
                elements[1].checked = true;
                break;
            case 2:
                elements[2].checked = true;
                break;
            default:
                break;
        }
    })
    //color labels for relays
    var element = $(document).find('.state');
    $(xml).find("States S").each(function () {
        var i = $(this).index();
        var state = $(this).text().trim();
        switch (parseInt(state)) {
            case 1:
                element[i].style.background = "green";
                break;
            case 0:
                element[i].style.background = "#c70000";
                break;
            default:
                break;
        }
    })
    //grey out invalid remote values
    var elements = $(document).find('.remote');
    var valid = $(xml).find("OK").text().trim();
    for (i = 0; i < elements.length; i++) {
        if (parseInt(valid) == 0) elements[i].style.color = "#888";
        else elements[i].removeAttribute("style");
    }
};
