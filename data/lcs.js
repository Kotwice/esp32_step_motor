var labels = new Array();
var sliders = new Array();
var switches = new Array();

labels[0] = document.getElementById('label-1');
sliders[0] = document.getElementById('slider-1');
switches[0] = document.getElementById('switch-1');

sliders[0].oninput = function () { 
    labels[0].innerHTML = 'Скважность: ' + this.value;
}

sliders[0].onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/lcs?cl_duty_cycle=' + this.value, true);
    xhttp.send();
}

switches[0].onchange = function () {
    var xhttp = new XMLHttpRequest();    
    if (switches[0].checked) {
        xhttp.open('GET', '/lcs?lcs_state=on', true);
    }
    else {
        xhttp.open('GET', '/lcs?lcs_state=off', true);
    }
    xhttp.send();
};


function lcs_initiate () {

    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function() {        

        if (this.readyState == 4 && this.status == 200) {

            var data = this.responseText;

            data = data.replace(/'/g, '"');            
            data = JSON.parse(data);          

            for (let i = 0; i < data.length; i++) {

                sliders[i].setAttribute('min', data[i].min);
                sliders[i].setAttribute('max', data[i].max);
                sliders[i].setAttribute('step', data[i].step);

                sliders[i].setAttribute('value', data[i].duty_cycle);

                labels[i].innerHTML = 'Скважность: ' + sliders[i].value;

                if (data[i].state == 'on') {

                    switches[0].checked = true;

                }
                else {

                    switches[0].checked = false;

                } 

            }          
        }
    };
    xhttp.open('GET', '/lcs_initiate', true);
    xhttp.send();
}

lcs_initiate();