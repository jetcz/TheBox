var getUrlParameter = function getUrlParameter(sParam) { 
    var sPageURL = decodeURIComponent(window.location.search.substring(1)),
        sURLVariables = sPageURL.split('&'),
        sParameterName,
        i;

    for (i = 0; i < sURLVariables.length; i++) {
        sParameterName = sURLVariables[i].split('=');

        if (sParameterName[0] === sParam) {
            return sParameterName[1] === undefined ? true : sParameterName[1];
        }
    }
};

$(document).ready(function () {
    $("#a_index").attr("href", "index.htm?host=" + getUrlParameter('host'));
    $("#a_graphs1").attr("href", "graphs1.htm?host=" + getUrlParameter('host'));
    $("#a_graphs2").attr("href", "graphs2.htm?host=" + getUrlParameter('host'));
    $("#a_sched").attr("href", "sched.htm?host=" + getUrlParameter('host'));
    $("#a_system").attr("href", "system.htm?host=" + getUrlParameter('host'));
}
)