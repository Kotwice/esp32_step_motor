var labels = new Array();
var switches = new Array();
var sliders = new Array();

labels[0] = document.getElementById('label-1');
switches[0] = document.getElementById('switch-1');
sliders[0] = document.getElementById('slider-1');

sliders[0].oninput = function () { 
    labels[0].innerHTML = 'Частота ' + this.value + ' [Гц]';
}

sliders[0].onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/sm?sm_freq_1=' + this.value, true);
    xhttp.send();
}

switches[0].onchange = function () {
    var xhttp = new XMLHttpRequest();    
    if (switches[0].checked) {
        xhttp.open('GET', '/sm?sm_state_1=ON', true);
    }
    else {
        xhttp.open('GET', '/sm?sm_state_1=OFF', true);
    }
    xhttp.send();
};

/*INI-SM*/

function sm_initiate () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {        
        if (this.readyState == 4 && this.status == 200) {
            var data = this.responseText;
            data = data.replace(/'/g, '"');            
            data = JSON.parse(data);            
            for (let i = 0; i < data.length; i++) {

                sliders[i].value = data[i].sm_freq;

                //sliders[i].setAttribute('value', data[i].sm_freq);
                sliders[i].setAttribute('min', data[i].min);
                sliders[i].setAttribute('max', data[i].max);
                sliders[i].setAttribute('step', data[i].step);

                if (data[i].sm_state == 'ON') {
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