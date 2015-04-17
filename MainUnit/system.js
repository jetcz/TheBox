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
    })
};
function xmlParseNetwork(xml) {
    $(xml).find("Net V").each(function () {
        var i = $(this).index();
        var value = $(this).text().trim();
        $(".Net").eq(i).val(parseInt(value));
    })
}