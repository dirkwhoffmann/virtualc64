
let saved_pic_html = null;
let current_active_emulator = null;
function load_title(element, params, address) {
    stop_emu_view();
    //save preview pic
    var emu_container = $(element).parent();
    saved_pic_html = emu_container.html();
    current_active_emulator = emu_container.attr('id');

    //turn picture into iframe
    var emuview_html = `
<iframe id="vc64web" width="100%" height="100%"
    src="https://dirkwhoffmann.github.io/virtualc64web/${params}#${address}">
</iframe>
<svg  class="icon_btn" onclick="stop_emu_view();" xmlns="http://www.w3.org/2000/svg" width="2.0em" height="2.0em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
    <path d="M6.5 5A1.5 1.5 0 0 0 5 6.5v3A1.5 1.5 0 0 0 6.5 11h3A1.5 1.5 0 0 0 11 9.5v-3A1.5 1.5 0 0 0 9.5 5h-3z"/>
    <path d="M0 4a2 2 0 0 1 2-2h12a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H2a2 2 0 0 1-2-2V4zm15 0a1 1 0 0 0-1-1H2a1 1 0 0 0-1 1v8a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1V4z"/>
</svg>
<svg  id="toggle_icon" class="icon_btn" onclick="toggle_run();" xmlns="http://www.w3.org/2000/svg" width="2.0em" height="2.0em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
</svg>
<svg id="btn_unlock_audio" class="icon_btn" onclick="toggle_audio();" xmlns="http://www.w3.org/2000/svg" width="2.0em" height="2.0em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
</svg>
`;
    emu_container.html(emuview_html); 

    $div = emu_container;
    $div.height($div.width() * 200/320);
    $(window).bind('resize', function() { $div.height($div.width() * 200/320); });

    $('#vc64web').focus();

    state_poller = setInterval(function(){ 
        render_run_state();
        render_current_audio_state();
    }, 1000);

}

var state_poller=null;
const audio_locked_icon=`<path d="M6.717 3.55A.5.5 0 0 1 7 4v8a.5.5 0 0 1-.812.39L3.825 10.5H1.5A.5.5 0 0 1 1 10V6a.5.5 0 0 1 .5-.5h2.325l2.363-1.89a.5.5 0 0 1 .529-.06zm7.137 2.096a.5.5 0 0 1 0 .708L12.207 8l1.647 1.646a.5.5 0 0 1-.708.708L11.5 8.707l-1.646 1.647a.5.5 0 0 1-.708-.708L10.793 8 9.146 6.354a.5.5 0 1 1 .708-.708L11.5 7.293l1.646-1.647a.5.5 0 0 1 .708 0z"/>`;
const audio_unlocked_icon=`<path d="M11.536 14.01A8.473 8.473 0 0 0 14.026 8a8.473 8.473 0 0 0-2.49-6.01l-.708.707A7.476 7.476 0 0 1 13.025 8c0 2.071-.84 3.946-2.197 5.303l.708.707z"/>
<path d="M10.121 12.596A6.48 6.48 0 0 0 12.025 8a6.48 6.48 0 0 0-1.904-4.596l-.707.707A5.483 5.483 0 0 1 11.025 8a5.483 5.483 0 0 1-1.61 3.89l.706.706z"/>
<path d="M8.707 11.182A4.486 4.486 0 0 0 10.025 8a4.486 4.486 0 0 0-1.318-3.182L8 5.525A3.489 3.489 0 0 1 9.025 8 3.49 3.49 0 0 1 8 10.475l.707.707zM6.717 3.55A.5.5 0 0 1 7 4v8a.5.5 0 0 1-.812.39L3.825 10.5H1.5A.5.5 0 0 1 1 10V6a.5.5 0 0 1 .5-.5h2.325l2.363-1.89a.5.5 0 0 1 .529-.06z"/>`;
const run_icon = `<path d="M6.79 5.093A.5.5 0 0 0 6 5.5v5a.5.5 0 0 0 .79.407l3.5-2.5a.5.5 0 0 0 0-.814l-3.5-2.5z"/>
        <path d="M0 4a2 2 0 0 1 2-2h12a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H2a2 2 0 0 1-2-2V4zm15 0a1 1 0 0 0-1-1H2a1 1 0 0 0-1 1v8a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1V4z"/>`; 
const pause_icon =`<path d="M6.25 5C5.56 5 5 5.56 5 6.25v3.5a1.25 1.25 0 1 0 2.5 0v-3.5C7.5 5.56 6.94 5 6.25 5zm3.5 0c-.69 0-1.25.56-1.25 1.25v3.5a1.25 1.25 0 1 0 2.5 0v-3.5C11 5.56 10.44 5 9.75 5z"/>
        <path d="M0 4a2 2 0 0 1 2-2h12a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H2a2 2 0 0 1-2-2V4zm15 0a1 1 0 0 0-1-1H2a1 1 0 0 0-1 1v8a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1V4z"/>
        `;

function toggle_run() {
    var vc64web = document.getElementById("vc64web").contentWindow;
    if(vc64web.required_roms_loaded)// that means emu already runs
    {
        //click emulators run toggle button
        $('#vc64web').contents().find('#button_run').click();

        render_run_state();
    }
}

var last_run_state=null;
function render_run_state()
{
    var vc64web = document.getElementById("vc64web").contentWindow;
    if(vc64web.required_roms_loaded)// that means emu already runs
    {
        var is_running = vc64web.is_running();
        if(last_run_state == is_running)
            return;

        $("#toggle_icon").html(
            is_running ? pause_icon : run_icon
        );
        last_run_state = is_running;
    }
}

function get_audio_context()
{
    var vc64web = document.getElementById("vc64web").contentWindow;
    if (typeof vc64web.Module === 'undefined'
        || typeof vc64web.Module.SDL2 == 'undefined'
        || typeof vc64web.Module.SDL2.audioContext == 'undefined')
    {
        return null;
    }
    else
    {
        return vc64web.Module.SDL2.audioContext;
    }
}

function toggle_audio()
{			
    var context = get_audio_context();			
    if (context !=null)
    {
        if(context.state == 'suspended') {
            context.resume();
        }
        else if (context.state == 'running')
        {
            context.suspend();
        }
    }
    render_current_audio_state();
}

var last_audio_state=null;
function render_current_audio_state()
{
    var context = get_audio_context();
    if(context == null && last_audio_state == null)
        return;
    if(context != null && last_audio_state == context.state)
        return;

    if(context == null)
    {
        $('#btn_unlock_audio').empty();	
        last_audio_state=null;			
    }
    else
    {
        $('#btn_unlock_audio').html( context.state == 'running' ? audio_unlocked_icon : audio_locked_icon );
        last_audio_state=context.state;
    }
}



function stop_emu_view() {
    //close any other active emulator frame
    if (saved_pic_html != null) {
        //restore preview pic
        $('#' + current_active_emulator).html(saved_pic_html);
    }
    if(state_poller != null)
    {
        clearInterval(state_poller);
    }
    last_run_state=null; last_audio_state=null;
}

