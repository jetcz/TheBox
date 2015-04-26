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
    $(xml).find("Relay").each(function (i) {
        var variable = $(this).find("Variable").text().trim();
        $(".cmb").eq(i).val(parseInt(variable));

        //for each interval
        $(this).find("Interval").each(function (ii) {
            var j = ii + (i * 5);
            //checkbox enabled
            var enabled = $(this).find("Enabled").text().trim();
            enabled = (enabled == "true");
            $(".cb").eq(j).prop('checked', enabled);
            if (!enabled) disableInterval(j);
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
    for (var i = 0; i < 4; i++) {
        disableCbs(i);
        disableForm(i);
    }
    syncHiddenFields();
}


//disable relay form if variable is set to pir
function disableForm(i) {
    //disable
    var val = $('.cmb').eq(i).find('option:selected').val();
    if (val == 0) {
        $('.datagrid').eq(i).find('tr').each(function () {
            $(this).find('*').each(function () {
                $(this).attr('disabled', true);
                $(this).removeAttr('required');
            })
        })
    } else {
        //enable
        $('.datagrid').eq(i).find('tr').each(function (j) {
            disableInterval(j + (i * 5)); //disable lines, if we want to disable lines from second form, it means that indexes of these lines has values index+5
        })
        disableCbs(i);
    }
}

//after loading xml, disable/enable appropriate checkboxes
//TODO
function disableCbs(i) {
    $('.datagrid').eq(i).find(".cb").each(function () {
        var s = "." + this.classList[0] + "." + this.classList[1]; //this creates string like .cb.r1
        var j = 0;
        $(document).find(s).each(function (i) {
            if ($(s).eq(i).prop('checked')) {
                j++;
            }
        })
        $(document).find(s).each(function (i) {
            if (i == j || i + 1 == j)
                $(s).eq(i).attr('disabled', false);
            else $(s).eq(i).attr('disabled', true);
        })

    })
}

function syncHiddenFields() {
    $(document).find(".cb").each(function (i) {
        setHiddenField(i);
    })
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

function setHiddenField(i) {
    var cb = $(".cb").eq(i);
    if (cb.prop('checked')) cb.next().val(1);
    else cb.next().val(0);
}


function validateForm() {
    var valid = true;
    $(document).find(".datagrid").each(function (i) {
        for (var j = 0; j < 10; j += 2) {
            if (j > 1) {
                if (!$(this).find(".time").eq(j).is(":disabled") || !$(this).find(".time").eq(j + 1).is(":disabled")) {
                    var secs = $(this).find(".time").eq(j).val() * 60 + $(this).find(".time").eq(j + 1).val();
                    var prevsecs = $(this).find(".time").eq(j - 2).val() * 60 + $(this).find(".time").eq(j - 1).val();
                    if (secs <= prevsecs) {
                        valid = false;
                        break;
                    }
                }
            }
        }
    })
    if (!valid) alert("Times must be sorted from earliest to latest.");
    return valid;
}

$(document).ready(function() {

    //handler for comboboxes
    $(".cmb").change(function() {
        var i = $('.cmb').index(this);
        disableForm(i);
    });
    //disabling lines according to interval
    $(".cb").click(function() {
        var i = $('.cb').index(this);
        disableInterval(i);
        setHiddenField(i);
    });

    //disabling checkboxes according to other checkboxes
    $(".cb").click(function() {
        var s = "." + this.classList[0]+ "." + this.classList[1]; //this creates string like .cb.r1
        var i = $(s).index(this);
        //firs disable all checkboxes for given class
        $(s).each(function() {
            $(this).attr('disabled', true);
        });
        //then enable the right checkboxes
        if ($(this).is(':checked')) {
            $(s).eq(i).attr('disabled', false);
            if (!(i + 1 > 5))
                $(s).eq(i + 1).attr('disabled', false);
        } else {
            if (!(i - 1 < 0))
                $(s).eq(i - 1).attr('disabled', false);
            $(s).eq(i).attr('disabled', false);
        }
    });

    $('.decimal').jStepper({ minValue: -100, maxValue: 100, normalStep: 0.1 });
    $('.time.H').jStepper({ minValue: 0, maxValue: 23 });
    $('.time.M').jStepper({ minValue: 0, maxValue: 59 });



    //validator for numeric values
    $(document).on('keyup', '.decimal, .time', function (event) {
        var v = this.value;
        if ($.isNumeric(v) === false) {
            //chop off the last char entered
            this.value = this.value.slice(0, -1);
        }
    });

})