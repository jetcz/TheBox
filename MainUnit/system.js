$(document).ready(function () {
    getStats();
    getNetwork();
});
function getStats() {
    $.ajax({
        type: "GET",
        url: "stats.xml",
        cache: false,
        dataType: "xml",
        success: xmlParseStats,
        complete: function () {
            setTimeout(getStats, 1000);
        }
    }
    );
}
function getNetwork() {
    $.ajax({
        type: "GET",
        url: "network.xml",
        cache: false,
        dataType: "xml",
        success: xmlParseNetwork,
        error: function () {
            $("header").append("<div class=\"wrapContent\"><div class=\"content\" style=\"color:red\"><b>FAILED TO GET NETWORK SETTINGS, DO NOT CONTINUE!</b></div></div>");
        }
    });
}
function xmlParseStats(xml) {
    var LocalTime = $(xml).find("Loc").text().trim();
    $("#LocalTime").text(LocalTime);
    var LastSync = $(xml).find("Sync").text().trim();
    $("#LastSync").text(LastSync);
    $(xml).find("Stats V").each(function () {
        var i = $(this).index();
        var value = $(this).text().trim();
        $(".Value").eq(i).text(value);

        //preset voltage background color
        if (i == 2 || i == 3) {
            if (parseInt(value) > 4500) {
                $('.tdVal').eq(i-2).css("background-color", "green");
            }
            if (parseInt(value) <= 4500 && parseInt(value) > 3500) {
                $('.tdVal').eq(i-2).css("background-color", "#e97900");
            }
            if (parseInt(value) <= 3500) {
                $('.tdVal').eq(i-2).css("background-color", "#c70000");
            }
        }
        //preset ds age  
        if (i == 4) {
            if (parseInt(value) < 12) {
                $('.tdVal').eq(i-2).css("background-color", "green");
            }

            if (parseInt(value) >= 12) {
                $('.tdVal').eq(i-2).css("background-color", "#c70000");
            }
        }

        if (i == 5) {
            if (parseInt(value) < 65) {
                $('.tdVal').eq(i-2).css("background-color", "green");
            }

            if (parseInt(value) >= 65 && parseInt(value) < 130) {
                $('.tdVal').eq(i-2).css("background-color", "#e97900");
            }
            if (parseInt(value) >= 130) {
                $('.tdVal').eq(i-2).css("background-color", "#c70000");
            }
        }
    })
};
function xmlParseNetwork(xml) {
    $(xml).find("Net V").each(function () {
        var i = $(this).index();
        var value = $(this).text().trim();
        $(".Net").eq(i).val(parseInt(value));
    })
}