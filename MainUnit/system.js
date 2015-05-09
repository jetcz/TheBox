$(document).ready(function () {
    getStats();
    getSettings();


    //handler for dhcp radio
    $("input[name=DHCP]:radio").change(function () {
        if ($(this).val() == 1) {
            $(document).find('.Net').each(function () {
                $(this).attr('disabled', true);
            })
        } else {
            $(document).find('.Net').each(function () {
                $(this).removeAttr("disabled");
            })
        }
    });
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
function getSettings() {
    $.ajax({
        type: "GET",
        url: "settings.xml",
        cache: false,
        dataType: "xml",
        success: xmlParseSettings,
        error: function () {
            $("header").append("<div class=\"wrapContent\"><div class=\"content\" style=\"color:red\"><b>FAILED TO GET SETTINGS, DO NOT CONTINUE!</b></div></div>");
        }
    });
}
var runned = false;
function xmlParseStats(xml) {
    if (!runned) {
        $(document).find(".wrapContent.message").hide();
        $(document).find(".wrapContent.data").removeAttr('style');
        runned = true;
    }
    var LocalTime = $(xml).find("Loc").text().trim();
    $("#LocalTime").text(LocalTime);
    var LastSync = $(xml).find("Sync").text().trim();
    $("#LastSync").text(LastSync);
    $(xml).find("Stats V").each(function (i) {
        var value = $(this).text().trim();
        $(".Value").eq(i).text(value);

        //color td starting offset
        var offset = 3
        //preset voltage background color
        if (i == offset || i == offset + 1) {
            if (parseInt(value) > 4800) {
                $('.tdVal').eq(i - offset).css("background-color", "green");
            }
            if (parseInt(value) <= 4800 && parseInt(value) > 4000) {
                $('.tdVal').eq(i - offset).css("background-color", "#e97900");
            }
            if (parseInt(value) <= 4000) {
                $('.tdVal').eq(i - offset).css("background-color", "#c70000");
            }
        }
        //preset ds age  
        if (i == offset + 2) {
            if (parseInt(value) < 12) {
                $('.tdVal').eq(i - offset).css("background-color", "green");
            }

            if (parseInt(value) >= 12) {
                $('.tdVal').eq(i - offset).css("background-color", "#c70000");
            }
        }

        if (i == offset + 3) {
            if (parseInt(value) < 65) {
                $('.tdVal').eq(i - offset).css("background-color", "green");
            }

            if (parseInt(value) >= 65 && parseInt(value) < 180) {
                $('.tdVal').eq(i - offset).css("background-color", "#e97900");
            }
            if (parseInt(value) >= 180) {
                $('.tdVal').eq(i - offset).css("background-color", "#c70000");
            }
        }
    })
};
function xmlParseSettings(xml) {
    //settings
    $("#timeout").val(parseInt($(xml).find("RemoteDSTimeout").text().trim()));
    var InvalidDSAction = parseInt($(xml).find("InvalidDSAction").text().trim());
    var TSEnabled = parseInt($(xml).find("TSEnabled").text().trim());

    var radios = $("input[name=action]:radio");
    if (InvalidDSAction == 0) radios[0].checked = true;
    if (InvalidDSAction == 1) radios[1].checked = true;

    radios = $("input[name=thingspeak]:radio");
    if (TSEnabled == 0) radios[0].checked = true;
    if (TSEnabled == 1) radios[1].checked = true;

    $("#tsaddr").val($(xml).find("TSAddr").text().trim());
    $("#ntpaddr").val($(xml).find("NTPAddr").text().trim());

    //offsets
    $("#SysTempOffset").val(parseFloat($(xml).find("SysTempOffset").text().trim()));
    $("#PressureOffset").val(parseFloat($(xml).find("PressureOffset").text().trim()));
    $("#MainTempOffset").val(parseFloat($(xml).find("MainTempOffset").text().trim()));
    $("#RemoteTempOffset").val(parseFloat($(xml).find("RemoteTempOffset").text().trim()));
    $("#SoilTempOffset").val(parseFloat($(xml).find("SoilTempOffset").text().trim()));

    //network
    $(xml).find("Net V").each(function (i) {
        var value = $(this).text().trim();
        $(".Net").eq(i).val(parseInt(value));
    })

    var dhcp = parseInt($(xml).find("DHCP").text().trim());
    var radios = $("input[name=DHCP]:radio");
    if (dhcp == 1) {
        radios[0].checked = true;
        $(document).find('.Net').each(function () {
            $(this).attr('disabled', true);
        })
    }
    if (dhcp == 0) {
        radios[1].checked = true;
    }
}