var labels = new Array();
var switches = new Array();
var sliders = new Array();

labels[0] = document.getElementById('label-1');
switches[0] = document.getElementById('switch-1');
sliders[0] = document.getElementById('slider-1');

labels[1] = document.getElementById('label-2');
switches[1] = document.getElementById('switch-2');
sliders[1] = document.getElementById('slider-2');

sliders[0].oninput = function () { 
    labels[0].innerHTML = 'Частота ' + this.value + ' [Гц]';
}

sliders[0].onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/sm?sm_frequency_1=' + this.value, true);
    xhttp.send();
}

switches[0].onchange = function () {
    var xhttp = new XMLHttpRequest();    
    if (switches[0].checked) {
        xhttp.open('GET', '/sm?sm_state_1=on', true);
    }
    else {
        xhttp.open('GET', '/sm?sm_state_1=off', true);
    }
    xhttp.send();
};

sliders[1].oninput = function () { 
    labels[1].innerHTML = 'Частота ' + this.value + ' [Гц]';
}

sliders[1].onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/sm?sm_frequency_2=' + this.value, true);
    xhttp.send();
}

switches[1].onchange = function () {
    var xhttp = new XMLHttpRequest();    
    if (switches[1].checked) {
        xhttp.open('GET', '/sm?sm_state_2=on', true);
    }
    else {
        xhttp.open('GET', '/sm?sm_state_2=off', true);
    }
    xhttp.send();
};

function sm_initiate () {
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
                sliders[i].setAttribute('value', data[i].frequency);

                if (data[i].state == 'on') {
                    switches[i].checked = true;
                }
                else {
                    switches[i].checked = false;
                }                
                labels[i].innerHTML = 'Частота ' + sliders[i].value + '[Гц]';
            }          
        }
    };
    xhttp.open('GET', '/sm_initiate', true);
    xhttp.send();
}

sm_initiate();