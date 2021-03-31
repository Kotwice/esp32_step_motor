$(document).ready(function () {            
    $('.sidebarBth').click(function () {
        document.documentElement.webkitRequestFullscreen();
        $('.sidebar').toggleClass('sidebar-open');
        $('.sidebar').toggleClass('sidebar-close', false); 
        $('.contentbar').toggleClass('contentbar-close');
        $('.contentbar').toggleClass('contentbar-open', false);           
        $('.sidebarBth').toggleClass('toggle');                              
    });
    $('.item-1').click(function () {
        $('.sidebar').toggleClass('sidebar-close');
        $('.sidebar').toggleClass('sidebar-open', false);               
        $('.contentbar').toggleClass('contentbar-open');
        $('.contentbar').toggleClass('contentbar-close', false); 
        $('.sidebarBth').toggleClass('toggle');
        $('.contentbar').load('sm.html');                      
    });
    $('.item-2').click(function () {
        $('.sidebar').toggleClass('sidebar-close');
        $('.sidebar').toggleClass('sidebar-open', false);               
        $('.contentbar').toggleClass('contentbar-open');
        $('.contentbar').toggleClass('contentbar-close', false); 
        $('.sidebarBth').toggleClass('toggle');
        $('.contentbar').load('hs.html');                      
    });
    $('.item-2').click(function () {
        $('.sidebar').toggleClass('sidebar-close');
        $('.sidebar').toggleClass('sidebar-open', false);               
        $('.contentbar').toggleClass('contentbar-open');
        $('.contentbar').toggleClass('contentbar-close', false); 
        $('.sidebarBth').toggleClass('toggle');
        $('.contentbar').load('lcs.html');                      
    });
})