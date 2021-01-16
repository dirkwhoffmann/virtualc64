/**
 * official vc64web player.
 */

var vc64web_player={
    saved_pic_html: null,
    current_active_emulator: null,
    load: function (element, params, address) {
        this.stop_emu_view();
        //save preview pic
        var emu_container = $(element).parent();
        this.saved_pic_html = emu_container.html();
        this.current_active_emulator = emu_container.attr('id');

        //turn picture into iframe
        var emuview_html = `
<div id="player_container" style="display:flex;flex-direction:column;">
<iframe id="vc64web" width="100%" height="100%" onclick="event.preventDefault();$vc64web.focus();return false;"
    src="https://dirkwhoffmann.github.io/virtualc64web/${params}#${address}">
</iframe>
<div style="display: flex"><svg  class="icon_btn" onclick="vc64web_player.stop_emu_view();" xmlns="http://www.w3.org/2000/svg" width="2.0em" height="2.0em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
    <path d="M6.5 5A1.5 1.5 0 0 0 5 6.5v3A1.5 1.5 0 0 0 6.5 11h3A1.5 1.5 0 0 0 11 9.5v-3A1.5 1.5 0 0 0 9.5 5h-3z"/>
    <path d="M0 4a2 2 0 0 1 2-2h12a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H2a2 2 0 0 1-2-2V4zm15 0a1 1 0 0 0-1-1H2a1 1 0 0 0-1 1v8a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1V4z"/>
</svg>
<svg  id="toggle_icon" class="icon_btn" onclick="vc64web_player.toggle_run();" xmlns="http://www.w3.org/2000/svg" width="2.0em" height="2.0em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
${this.pause_icon}
</svg>
<svg id="btn_unlock_audio" class="icon_btn" onclick="vc64web_player.toggle_audio();" xmlns="http://www.w3.org/2000/svg" width="2.0em" height="2.0em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
${this.audio_locked_icon}
</svg>

<svg id="btn_overlay" class="icon_btn" style="margin-top:4px;margin-left:auto" onclick="vc64web_player.toggle_overlay();" xmlns="http://www.w3.org/2000/svg" width="1.5em" height="1.5em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
${this.overlay_on_icon}
</svg>
<a title="open fullwindow in new browser tab" style="border:none;width:1.5em;margin-top:4px" onclick="vc64web_player.stop_emu_view();" href="https://dirkwhoffmann.github.io/virtualc64web/${params}#${address}" target="blank">
    <svg class="icon_btn" xmlns="http://www.w3.org/2000/svg" width="1.5em" height="1.5em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
  <path fill-rule="evenodd" d="M15 2a1 1 0 0 0-1-1H2a1 1 0 0 0-1 1v12a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1V2zM0 2a2 2 0 0 1 2-2h12a2 2 0 0 1 2 2v12a2 2 0 0 1-2 2H2a2 2 0 0 1-2-2V2zm5.854 8.803a.5.5 0 1 1-.708-.707L9.243 6H6.475a.5.5 0 1 1 0-1h3.975a.5.5 0 0 1 .5.5v3.975a.5.5 0 1 1-1 0V6.707l-4.096 4.096z"/>
</svg>
</a>
</div>
</div>
`;
        emu_container.html(emuview_html); 

        $vc64web = $('#vc64web');
        $vc64web.height($vc64web.width() * 200/320);
        $(window).bind('resize', function() { 
            if( vc64web_player.is_overlay)
            {
                vc64web_player.scale_overlay();
            }
            $vc64web.height($vc64web.width() * 200/320); 
        });

        this.state_poller = setInterval(function(){ 
            if(document.activeElement.nodeName.toLowerCase() == 'body')
            { 
                $vc64web.focus();
            }
            vc64web_player.render_run_state();
            vc64web_player.render_current_audio_state();
        }, 900);

    },

    state_poller: null,
    audio_locked_icon:`<path d="M6.717 3.55A.5.5 0 0 1 7 4v8a.5.5 0 0 1-.812.39L3.825 10.5H1.5A.5.5 0 0 1 1 10V6a.5.5 0 0 1 .5-.5h2.325l2.363-1.89a.5.5 0 0 1 .529-.06zm7.137 2.096a.5.5 0 0 1 0 .708L12.207 8l1.647 1.646a.5.5 0 0 1-.708.708L11.5 8.707l-1.646 1.647a.5.5 0 0 1-.708-.708L10.793 8 9.146 6.354a.5.5 0 1 1 .708-.708L11.5 7.293l1.646-1.647a.5.5 0 0 1 .708 0z"/>`,
    audio_unlocked_icon:`<path d="M11.536 14.01A8.473 8.473 0 0 0 14.026 8a8.473 8.473 0 0 0-2.49-6.01l-.708.707A7.476 7.476 0 0 1 13.025 8c0 2.071-.84 3.946-2.197 5.303l.708.707z"/>
<path d="M10.121 12.596A6.48 6.48 0 0 0 12.025 8a6.48 6.48 0 0 0-1.904-4.596l-.707.707A5.483 5.483 0 0 1 11.025 8a5.483 5.483 0 0 1-1.61 3.89l.706.706z"/>
<path d="M8.707 11.182A4.486 4.486 0 0 0 10.025 8a4.486 4.486 0 0 0-1.318-3.182L8 5.525A3.489 3.489 0 0 1 9.025 8 3.49 3.49 0 0 1 8 10.475l.707.707zM6.717 3.55A.5.5 0 0 1 7 4v8a.5.5 0 0 1-.812.39L3.825 10.5H1.5A.5.5 0 0 1 1 10V6a.5.5 0 0 1 .5-.5h2.325l2.363-1.89a.5.5 0 0 1 .529-.06z"/>`,
    run_icon: `<path d="M6.79 5.093A.5.5 0 0 0 6 5.5v5a.5.5 0 0 0 .79.407l3.5-2.5a.5.5 0 0 0 0-.814l-3.5-2.5z"/>
        <path d="M0 4a2 2 0 0 1 2-2h12a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H2a2 2 0 0 1-2-2V4zm15 0a1 1 0 0 0-1-1H2a1 1 0 0 0-1 1v8a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1V4z"/>`,
    pause_icon:`<path d="M6.25 5C5.56 5 5 5.56 5 6.25v3.5a1.25 1.25 0 1 0 2.5 0v-3.5C7.5 5.56 6.94 5 6.25 5zm3.5 0c-.69 0-1.25.56-1.25 1.25v3.5a1.25 1.25 0 1 0 2.5 0v-3.5C11 5.56 10.44 5 9.75 5z"/>
        <path d="M0 4a2 2 0 0 1 2-2h12a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H2a2 2 0 0 1-2-2V4zm15 0a1 1 0 0 0-1-1H2a1 1 0 0 0-1 1v8a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1V4z"/>
        `,
    overlay_on_icon:`<path d="M1.5 1a.5.5 0 0 0-.5.5v4a.5.5 0 0 1-1 0v-4A1.5 1.5 0 0 1 1.5 0h4a.5.5 0 0 1 0 1h-4zM10 .5a.5.5 0 0 1 .5-.5h4A1.5 1.5 0 0 1 16 1.5v4a.5.5 0 0 1-1 0v-4a.5.5 0 0 0-.5-.5h-4a.5.5 0 0 1-.5-.5zM.5 10a.5.5 0 0 1 .5.5v4a.5.5 0 0 0 .5.5h4a.5.5 0 0 1 0 1h-4A1.5 1.5 0 0 1 0 14.5v-4a.5.5 0 0 1 .5-.5zm15 0a.5.5 0 0 1 .5.5v4a1.5 1.5 0 0 1-1.5 1.5h-4a.5.5 0 0 1 0-1h4a.5.5 0 0 0 .5-.5v-4a.5.5 0 0 1 .5-.5z"/>`,
    overlay_off_icon:`<path d="M5.5 0a.5.5 0 0 1 .5.5v4A1.5 1.5 0 0 1 4.5 6h-4a.5.5 0 0 1 0-1h4a.5.5 0 0 0 .5-.5v-4a.5.5 0 0 1 .5-.5zm5 0a.5.5 0 0 1 .5.5v4a.5.5 0 0 0 .5.5h4a.5.5 0 0 1 0 1h-4A1.5 1.5 0 0 1 10 4.5v-4a.5.5 0 0 1 .5-.5zM0 10.5a.5.5 0 0 1 .5-.5h4A1.5 1.5 0 0 1 6 11.5v4a.5.5 0 0 1-1 0v-4a.5.5 0 0 0-.5-.5h-4a.5.5 0 0 1-.5-.5zm10 1a1.5 1.5 0 0 1 1.5-1.5h4a.5.5 0 0 1 0 1h-4a.5.5 0 0 0-.5.5v4a.5.5 0 0 1-1 0v-4z"/>`,
    is_overlay: false,
    toggle_overlay: function () {
        var container = $('#player_container');
        if( container.css('position') == "fixed")
        {
            this.is_overlay=false;
            $('#btn_overlay').html(this.overlay_on_icon);
            $('#player_container').css({"position": "", "top": "", "left": "", "width": "", "background-color": ""});
        }
        else
        {
            $('#btn_overlay').html(this.overlay_off_icon);
            this.scale_overlay();
            this.is_overlay=true;
        }
        $vc64web.height($vc64web.width() * 200/320);
    },
    scale_overlay: function(){
        var ratio=1.6;
        var w1=window.innerWidth/window.innerWidth;
        var w2=window.innerHeight * ratio /window.innerWidth;
        var width_percent = Math.min(w1,w2)*100;
        var margin_left = Math.round((100-width_percent)/2);
        var calc_pixel_height = window.innerWidth*width_percent/100 / ratio;
        var height_percent = calc_pixel_height/window.innerHeight *100;
        var margin_top  = Math.round((100 -  height_percent )/2);
        width_percent = Math.round(width_percent);
        if(margin_top<5)
        {//give some extra room for height of player bottom bar controls 
            width_percent -= 6; 
            margin_left += 3;
        }
        $('#player_container').css({"position": "fixed", "top": `${margin_top}vh`, "left": `${margin_left}vw`, "width": `${width_percent}vw`,  "background-color": "white"});
    },
    toggle_run: function () {
        var vc64web = document.getElementById("vc64web").contentWindow;
        if(vc64web.required_roms_loaded)// that means emu already runs
        {
            //click emulators run toggle button
            $vc64web.contents().find('#button_run').click();

            render_run_state();
        }
    },

    last_run_state:null,
    render_run_state: function ()
    {
        var vc64web = document.getElementById("vc64web").contentWindow;
        if(vc64web.required_roms_loaded)// that means emu already runs
        {
            var is_running = vc64web.is_running();
            if(this.last_run_state == is_running)
                return;

            $("#toggle_icon").html(
                is_running ? this.pause_icon : this.run_icon
            );
            this.last_run_state = is_running;
        }
    },
    get_audio_context: function ()
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
    },
    toggle_audio: function()
    {			
        var context = this.get_audio_context();			
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
        this.render_current_audio_state();
    },
    last_audio_state:null,
    render_current_audio_state: function()
    {
        var context = this.get_audio_context();
        if(context == null && this.last_audio_state == null)
            return;
        if(context != null && this.last_audio_state == context.state)
            return;

        if(context == null)
        {
            $('#btn_unlock_audio').empty();	
            this.last_audio_state=null;			
        }
        else
        {
            $('#btn_unlock_audio').html( context.state == 'running' ? this.audio_unlocked_icon : this.audio_locked_icon );
            this.last_audio_state=context.state;
        }
    },
    stop_emu_view: function() {
        //close any other active emulator frame
        if (this.saved_pic_html != null) {
            //restore preview pic
            $('#' + this.current_active_emulator).html(this.saved_pic_html);
        }
        if(this.state_poller != null)
        {
            clearInterval(this.state_poller);
        }
        this.last_run_state=null; this.last_audio_state=null;
    }
}
