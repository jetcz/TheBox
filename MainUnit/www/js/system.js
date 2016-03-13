$(document).ready(function () {
    getUrls();
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

function getUrls() {
    $("#rebootarduino").attr("action", "http://" + getUrlParameter('host') + "/reboot");
    $("#rebootwifi").attr("action", "http://" + getUrlParameter('host') + "/rebootwifi");
    $("#settingsForm").attr("action", "http://" + getUrlParameter('host') + "/settings.data");
    $("#settingsDefaultForm").attr("action", "http://" + getUrlParameter('host') + "/settings.default");
    $("#offsetsDefaultForm").attr("action", "http://" + getUrlParameter('host') + "/offsets.default");
    $("#networkForm").attr("action", "http://" + getUrlParameter('host') + "/network.data");
}


function getStats() {
    $.ajax({
        type: "GET",
        url: "http://" + getUrlParameter('host') + "/stats.xml",
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
        url: "http://" + getUrlParameter('host') + "/settings.xml",
        cache: false,
        dataType: "xml",
        success: xmlParseSettings,
        error: function () {
            $("header").append("<div class=\"wrapContent\"><div class=\"content\" style=\"color:red\"><b>FAILED TO GET SETTINGS, DO NOT CONTINUE!</b></div></div>");
        }
    });
}
var runned = false;
var interval = 22;
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

        //calculate percentage of free ram
        if (i == 2) {
            var perc = parseFloat(value) / 8192 * 100;
            $("#mainRamPerc").text(perc.toFixed(1));
        }
        //calculate percentage of free ram
        if (i == 3) {
            var perc = parseFloat(value) / 2048 * 100;
            $("#remoteRamPerc").text(perc.toFixed(1));
        }
        //color td starting offset
        var offset = 5;
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
            if (parseInt(value) <= interval) {
                $('.tdVal').eq(i - offset).css("background-color", "green");
            }

            if (parseInt(value) > interval && parseInt(value) <= timeout) {
                $('.tdVal').eq(i - offset).css("background-color", "#e97900");
            }
            if (parseInt(value) > timeout) {
                $('.tdVal').eq(i - offset).css("background-color", "#c70000");
            }
        }
    })
};
var timeout = 180;
function xmlParseSettings(xml) {
    //settings
    timeout = parseInt($(xml).find("RemoteDSTimeout").text().trim());
    interval = parseInt($(xml).find("RadioMsgInterval").text().trim());
    $("#timeout").val(timeout);
    var InvalidDSAction = parseInt($(xml).find("InvalidDSAction").text().trim());
    var TSEnabled = parseInt($(xml).find("TSEnabled").text().trim());
    var TSMethod = parseInt($(xml).find("TSMethod").text().trim());

    var radios = $("input[name=action]:radio");
    if (InvalidDSAction == 0) radios[0].checked = true;
    if (InvalidDSAction == 1) radios[1].checked = true;

    radios = $("input[name=thingspeak]:radio");
    if (TSEnabled == 0) radios[0].checked = true;
    if (TSEnabled == 1) radios[1].checked = true;

    radios = $("input[name=tsmethod]:radio");
    if (TSMethod == 0) radios[0].checked = true;
    if (TSMethod == 1) radios[1].checked = true;

    $("#tsaddr").val($(xml).find("TSAddr").text().trim());
    $("#ntpaddr").val($(xml).find("NTPAddr").text().trim());
    $("#nrfch").val($(xml).find("NRFCh").text().trim());

    $("#mainapikey").val($(xml).find("MainAPIKey").text().trim());
    $("#remoteapikey").val($(xml).find("RemoteAPIKey").text().trim());
    $("#systemapikey").val($(xml).find("SystemAPIKey").text().trim());
    $("#tscnntimeout").val($(xml).find("TSCnnTimeout").text().trim());

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