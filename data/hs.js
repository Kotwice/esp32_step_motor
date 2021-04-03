
var inputs = new Array();

inputs[0] = document.getElementById('input-1-i');
inputs[1] = document.getElementById('input-1-p');
inputs[2] = document.getElementById('input-1-d');
inputs[3] = document.getElementById('input-1-destination');

inputs[4] = document.getElementById('input-2-i');
inputs[5] = document.getElementById('input-2-p');
inputs[6] = document.getElementById('input-2-d');
inputs[7] = document.getElementById('input-2-destination');

inputs[0].onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/hs?pid_i_1=' + inputs[0].value, true);
    xhttp.send();
}

inputs[1].onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/hs?pid_p_1=' + this.value, true);
    xhttp.send();
}

inputs[2].onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/hs?pid_d_1=' + this.value, true);
    xhttp.send();
}

inputs[3] .onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/hs?pid_destination_1=' + this.value, true);
    xhttp.send();
}

inputs[4].onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/hs?pid_i_2=' + this.value, true);
    xhttp.send();
}

inputs[5].onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/hs?pid_p_2=' + this.value, true);
    xhttp.send();
}

inputs[6].onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/hs?pid_d_2=' + this.value, true);
    xhttp.send();
}

inputs[7].onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/hs?pid_destination_2=' + this.value, true);
    xhttp.send();
}

var switches = new Array();

switches[0] = document.getElementById('switch-1');
switches[1] = document.getElementById('switch-2');

switches[0].onchange = function () {
    var xhttp = new XMLHttpRequest();    
    if (switches[0].checked) {
        xhttp.open('GET', '/hs?pid_state_1=on', true);
    }
    else {
        xhttp.open('GET', '/hs?pid_state_1=off', true);
    }
    xhttp.send();
};

switches[1].onchange = function () {
    var xhttp = new XMLHttpRequest();    
    if (switches[1].checked) {
        xhttp.open('GET', '/hs?pid_state_2=on', true);
    }
    else {
        xhttp.open('GET', '/hs?pid_state_2=off', true);
    }
    xhttp.send();
};

function hs_initiate () {

    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function() {

        if (this.readyState == 4 && this.status == 200) {

            var data = this.responseText;

            data = data.replace(/'/g, '"');            
            data = JSON.parse(data);

            inputs[0].value = data[0].i;
            inputs[1].value = data[0].p;
            inputs[2].value = data[0].d;
            inputs[3].value = data[0].destination;

            inputs[4].value = data[1].i;
            inputs[5].value = data[1].p;
            inputs[6].value = data[1].d;
            inputs[7].value = data[1].destination;

            if (data[0].state == 'on') {

                switches[0].checked = true;

            }
            else {

                switches[0].checked = false;

            } 

            if (data[1].state == 'on') {

                switches[1].checked = true;

            }
            else {

                switches[1].checked = false;

            }     
       
        }
    };

    xhttp.open('GET', '/hs_initiate', true);
    xhttp.send();

}

var Time_Step = 2000;

var mesumets = new Array();

mesumets[0] = document.getElementById('temperature-1');
mesumets[1] = document.getElementById('temperature-2');

var controls = new Array();

controls[0] = document.getElementById('duty-cycle-1');
controls[1] = document.getElementById('duty-cycle-2');

function get_temperatures () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var data = this.responseText;
            data = data.replace(/'/g, '"');            
            data = JSON.parse(data);            
            mesumets[0].innerHTML = 'T1 = ' + data[0].mesument + ' [degC]';
            mesumets[1].innerHTML = 'T2 = ' + data[1].mesument  + ' [degC]';
        }
    };
    xhttp.open('GET', '/temperatures', true);
    xhttp.send();
}

function get_controls () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var data = this.responseText;
            data = data.replace(/'/g, '"');            
            data = JSON.parse(data);            
            controls[0].innerHTML = 'Скважность: ' + data[0].control;
            controls[1].innerHTML = 'Скважность: ' + data[1].control;
        }
    };
    xhttp.open('GET', '/controls', true);
    xhttp.send();
}

setTimeout(() => {
    get_temperatures();
    get_controls();
});

setInterval(() => {
    get_temperatures();    
    get_controls();
}, Time_Step);

hs_initiate();