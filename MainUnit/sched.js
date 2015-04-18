$(document).ready(function () {
    $.ajax({
        type: "GET",
        url: "sched.xml",
        cache: false,
        dataType: "xml",
        success: xmlParser,
        error: function () {
            $("header").append("<div class=\"wrapContent\"><div class=\"content\" style=\"color:red\"><b>FAILED TO GET SCHEDULER SETTINGS, DO NOT CONTINUE!</b></div></div>");
            for (var i = 0; i < 4; i++) {
                disableForm(i);
            }
        }
    });
});

function xmlParser(xml) {
    //for each relay
    $(xml).find("Relay").each(function () {
        var i = $(this).index();
        var variable = $(this).find("Variable").text().trim();
        $(".cmb").eq(i).val(parseInt(variable));
        if (variable == 0) disableForm(i); //disable form if variable is pir
        //for each interval
        $(this).find("Interval").each(function () {
            var j = $(this).index() - 1 + (i * 5); //j index starts for some reason on 1, 5 is the number of line indexes in one form
            //checkbox enabled
            var enabled = $(this).find("Enabled").text().trim();
            enabled = (enabled == "true");
            $(".cb").eq(j).prop('checked', enabled);
            if (enabled == false) disableInterval(j);
            //target time
            var H = $(this).find("H").text().trim();
            var M = $(this).find("M").text().trim();
            $(".H").eq(j).val(parseInt(H));
            $(".M").eq(j).val(parseInt(M));
            //target values
            var From = $(this).find("From").text().trim();
            var To = $(this).find("To").text().trim();
            $(".From").eq(j).val(parseFloat(From));
            $(".To").eq(j).val(parseFloat(To));
        })
    })
}

$(document).ready(function () {
    //handler for button
    $("button").click(function () {
        //placeholder for submit button
    });
    //handler for comboboxes
    $(".cmb").change(function () {
        var i = $('.cmb').index(this);
        disableForm(i);
    });
    //handler for checkboxes
    $(".cb").click(function () {
        var i = $('.cb').index(this);
        disableInterval(i);
    });

    $('.decimal').jStepper({ minValue: -100, maxValue: 100, normalStep: 0.1 });
    $('.time.H').jStepper({ minValue: 0, maxValue: 23 });
    $('.time.M').jStepper({ minValue: 0, maxValue: 59 });
})
//disable relay form if variable is set to pir
function disableForm(i) {
    var val = $('.cmb').eq(i).find('option:selected').val();
    if (val == 0) {
        $('.datagrid').eq(i).find('tr').each(function () {
            $(this).find('*').each(function () {
                $(this).attr('disabled', true);
                $(this).removeAttr('required');
            })
        })
    } else {
        $('.datagrid').eq(i).find('tr').each(function () {
            var j = $(this).index();
            $(this).find('*').each(function () {
                $(this).attr('disabled', false);
                $(this).prop('required', true);
            })
            disableInterval(j + (i * 5)); //disable lines, if we want to disable lines from second form, it means that indexes of these lines has values index+5
        })
    }
}
//disable line in form if checkbox of enabled is unchecked
function disableInterval(i) {
    var cb = $('.cb').eq(i).is(':checked');
    if (!cb) {
        $('.H').eq(i).prop('disabled', true);
        $('.M').eq(i).prop('disabled', true);
        $('.From').eq(i).prop('disabled', true);
        $('.To').eq(i).prop('disabled', true);
        $('.H').eq(i).removeAttr('required');
        $('.M').eq(i).removeAttr('required');
        $('.From').eq(i).removeAttr('required');
        $('.To').eq(i).removeAttr('required');
    } else {
        $('.H').eq(i).prop('disabled', false);
        $('.M').eq(i).prop('disabled', false);
        $('.From').eq(i).prop('disabled', false);
        $('.To').eq(i).prop('disabled', false);
        $('.H').eq(i).prop('required', true);
        $('.M').eq(i).prop('required', true);
        $('.From').eq(i).prop('required', true);
        $('.To').eq(i).prop('required', true);
    }
}

//validator for numeric values
$(document).on('keyup', '.decimal, .time', function (event) {
    var v = this.value;
    if ($.isNumeric(v) === false) {
        //chop off the last char entered
        this.value = this.value.slice(0, -1);
    }
});