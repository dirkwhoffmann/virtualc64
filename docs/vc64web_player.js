/**
 * official vc64web player.
 * checks whether jquery is already there, if not lazy loads it when emulator is started
 */

var vc64web_player={
    listens: false,
    loadScript: function (url, callback){
        var script = document.createElement("script")
        script.type = "text/javascript";
        script.onload = callback;
        script.src = url;
        document.getElementsByTagName("head")[0].appendChild(script);
    },
    load: function(element, params, address) {
        if(this.listens == false)
        {
            window.addEventListener('message', event => {
                if(event.data.msg == "render_run_state")
                {
                    this.render_run_state(event.data.value);
                }
                else if(event.data.msg == "render_current_audio_state")
                {
                    this.render_current_audio_state(event.data.value);
                }
            });
            this.listens=true;
        }
        if(window.jQuery)
        {
            this.load_into(element,params, address);
        }
        else
        {
            this.loadScript("https://dirkwhoffmann.github.io/virtualc64web/js/jquery-3.5.0.min.js" , 
            function(){vc64web_player.load_into(element,params, address);});
        }
    },
    saved_pic_html: null,
    preview_pic_width: "100%",
    load_into: function (element, params, address) {
        var this_element=null;
        if(element == null)
        {
            if(this.saved_pic_html != null)
            {
                //that is also the case when another player is still active... because the preview pic is saved
                //then get the other running player
                this_element = $(document.querySelector('#player_container'));
            }
            else
            {
                alert("parameter element in the call vc64web_player.load(element, ...) is null.");
                return;
            }
        }
        else
        {
            var this_element = $(element);
        }

        var emu_container = this_element.parent();
        
        this.stop_emu_view();

        //save preview pic
        this.saved_pic_html = emu_container.html();
        this.preview_pic_width= emu_container.children(":first").width();

        var vc64web_url = "https://dirkwhoffmann.github.io/virtualc64web/";
        //turn picture into iframe
        var emuview_html = `
<div id="player_container" style="display:flex;flex-direction:column;">
<iframe id="vc64web" width="100%" height="100%" src="${vc64web_url}${params}#${address}"
>
</iframe>
<div style="display: flex"><svg  class="player_icon_btn" onclick="vc64web_player.stop_emu_view();return false;" xmlns="http://www.w3.org/2000/svg" width="2.0em" height="2.0em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
    <path d="M6.5 5A1.5 1.5 0 0 0 5 6.5v3A1.5 1.5 0 0 0 6.5 11h3A1.5 1.5 0 0 0 11 9.5v-3A1.5 1.5 0 0 0 9.5 5h-3z"/>
    <path d="M0 4a2 2 0 0 1 2-2h12a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H2a2 2 0 0 1-2-2V4zm15 0a1 1 0 0 0-1-1H2a1 1 0 0 0-1 1v8a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1V4z"/>
</svg>
<svg  id="toggle_icon" class="player_icon_btn" onclick="vc64web_player.toggle_run();return false;" xmlns="http://www.w3.org/2000/svg" width="2.0em" height="2.0em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
${this.pause_icon}
</svg>
<svg id="btn_unlock_audio" class="player_icon_btn" onclick="vc64web_player.toggle_audio();return false;" xmlns="http://www.w3.org/2000/svg" width="2.0em" height="2.0em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
${this.audio_locked_icon}
</svg>`;
if(address.toLowerCase().endsWith(".zip"))
{
    emuview_html += 
    `<svg id="btn_zip" class="player_icon_btn" style="margin-top:4px;margin-left:auto" onclick="vc64web_player.open_zip();return false;" xmlns="http://www.w3.org/2000/svg" width="1.5em" height="1.5em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
    ${this.folder_icon}
    </svg>`;
}
emuview_html += 
`<svg id="btn_overlay" class="player_icon_btn" style="margin-top:4px;margin-left:auto" onclick="vc64web_player.toggle_overlay();return false;" xmlns="http://www.w3.org/2000/svg" width="1.5em" height="1.5em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
${this.overlay_on_icon}
</svg>
<a id="btn_open_in_extra_tab" title="open fullwindow in new browser tab" style="border:none;width:1.5em;margin-top:4px" onclick="vc64web_player.stop_emu_view();" href="https://dirkwhoffmann.github.io/virtualc64web/${params}#${address}" target="blank">
    <svg class="player_icon_btn" xmlns="http://www.w3.org/2000/svg" width="1.5em" height="1.5em" fill="currentColor" class="bi bi-pause-btn" viewBox="0 0 16 16">
  <path fill-rule="evenodd" d="M15 2a1 1 0 0 0-1-1H2a1 1 0 0 0-1 1v12a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1V2zM0 2a2 2 0 0 1 2-2h12a2 2 0 0 1 2 2v12a2 2 0 0 1-2 2H2a2 2 0 0 1-2-2V2zm5.854 8.803a.5.5 0 1 1-.708-.707L9.243 6H6.475a.5.5 0 1 1 0-1h3.975a.5.5 0 0 1 .5.5v3.975a.5.5 0 1 1-1 0V6.707l-4.096 4.096z"/>
</svg>
</a>
</div>
</div>
`;
        emu_container.html(emuview_html); 

        $('#player_container').css("width",this.preview_pic_width);

        $vc64web = $('#vc64web');
        $vc64web.height($vc64web.width() * 200/320);
        $(window).bind('resize', function() { 
            if( vc64web_player.is_overlay)
            {
                vc64web_player.scale_overlay();
            }
            $vc64web.height($vc64web.width() * 200/320); 
        });

        document.addEventListener("click", this.grab_focus);
        document.getElementById("vc64web").onload = this.grab_focus;

        this.state_poller = setInterval(function(){ 
            let vc64web=document.getElementById("vc64web");            
            vc64web.contentWindow.postMessage("poll_state", "*");
        }, 900);
    },
    grab_focus: function(){            
        let vc64web=document.getElementById("vc64web");            
        if(vc64web != null)
        {
            document.activeElement.blur();
            vc64web.focus();
        }
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
    zip_icon:`<path d="M0 2a1 1 0 0 1 1-1h14a1 1 0 0 1 1 1v2a1 1 0 0 1-1 1v7.5a2.5 2.5 0 0 1-2.5 2.5h-9A2.5 2.5 0 0 1 1 12.5V5a1 1 0 0 1-1-1V2zm2 3v7.5A1.5 1.5 0 0 0 3.5 14h9a1.5 1.5 0 0 0 1.5-1.5V5H2zm13-3H1v2h14V2zM5 7.5a.5.5 0 0 1 .5-.5h5a.5.5 0 0 1 0 1h-5a.5.5 0 0 1-.5-.5z"/>`,
    folder_icon:`<path d="M1 3.5A1.5 1.5 0 0 1 2.5 2h2.764c.958 0 1.76.56 2.311 1.184C7.985 3.648 8.48 4 9 4h4.5A1.5 1.5 0 0 1 15 5.5v7a1.5 1.5 0 0 1-1.5 1.5h-11A1.5 1.5 0 0 1 1 12.5v-9zM2.5 3a.5.5 0 0 0-.5.5V6h12v-.5a.5.5 0 0 0-.5-.5H9c-.964 0-1.71-.629-2.174-1.154C6.374 3.334 5.82 3 5.264 3H2.5zM14 7H2v5.5a.5.5 0 0 0 .5.5h11a.5.5 0 0 0 .5-.5V7z"/>`,
    overlay_on_icon:`<path d="M1.5 1a.5.5 0 0 0-.5.5v4a.5.5 0 0 1-1 0v-4A1.5 1.5 0 0 1 1.5 0h4a.5.5 0 0 1 0 1h-4zM10 .5a.5.5 0 0 1 .5-.5h4A1.5 1.5 0 0 1 16 1.5v4a.5.5 0 0 1-1 0v-4a.5.5 0 0 0-.5-.5h-4a.5.5 0 0 1-.5-.5zM.5 10a.5.5 0 0 1 .5.5v4a.5.5 0 0 0 .5.5h4a.5.5 0 0 1 0 1h-4A1.5 1.5 0 0 1 0 14.5v-4a.5.5 0 0 1 .5-.5zm15 0a.5.5 0 0 1 .5.5v4a1.5 1.5 0 0 1-1.5 1.5h-4a.5.5 0 0 1 0-1h4a.5.5 0 0 0 .5-.5v-4a.5.5 0 0 1 .5-.5z"/>`,
    overlay_off_icon:`<path d="M5.5 0a.5.5 0 0 1 .5.5v4A1.5 1.5 0 0 1 4.5 6h-4a.5.5 0 0 1 0-1h4a.5.5 0 0 0 .5-.5v-4a.5.5 0 0 1 .5-.5zm5 0a.5.5 0 0 1 .5.5v4a.5.5 0 0 0 .5.5h4a.5.5 0 0 1 0 1h-4A1.5 1.5 0 0 1 10 4.5v-4a.5.5 0 0 1 .5-.5zM0 10.5a.5.5 0 0 1 .5-.5h4A1.5 1.5 0 0 1 6 11.5v4a.5.5 0 0 1-1 0v-4a.5.5 0 0 0-.5-.5h-4a.5.5 0 0 1-.5-.5zm10 1a1.5 1.5 0 0 1 1.5-1.5h4a.5.5 0 0 1 0 1h-4a.5.5 0 0 0-.5.5v4a.5.5 0 0 1-1 0v-4z"/>`,
    is_overlay: false,
    toggle_overlay: function () {
        var container = $('#player_container');
        if( container.css('position') == "fixed")
        {
            this.is_overlay=false;
            $('#btn_overlay').html(this.overlay_on_icon);
            $('#player_container').css({"position": "", "top": "", "left": "", "width": this.preview_pic_width, "z-index": ""});
        }
        else
        {
            $('#btn_overlay').html(this.overlay_off_icon);
            this.scale_overlay();
            this.is_overlay=true;
        }
        $vc64web.height($vc64web.width() * 200/320);
        
        let vc64web=document.getElementById("vc64web");
        //the blur and refocus is only needed for safari, when it goes into overlay
        document.activeElement.blur(); 
        vc64web.focus();
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
        $('#player_container').css({"position": "fixed", "top": `${margin_top}vh`, "left": `${margin_left}vw`, "width": `${width_percent}vw`, "z-index": 1000});
    },
    toggle_run: function () {
        var vc64web = document.getElementById("vc64web").contentWindow;
        //click emulators run toggle button
        vc64web.postMessage("button_run()", "*");
    },
    last_run_state:null,
    render_run_state: function (is_running)
    {
        if(this.last_run_state == is_running)
            return;

        $("#toggle_icon").html(
            is_running ? this.pause_icon : this.run_icon
        );
        this.last_run_state = is_running;
    },
    toggle_audio: function()
    {			
        var vc64web = document.getElementById("vc64web").contentWindow;
        vc64web.postMessage("toggle_audio()", "*");
    },
    last_audio_state:null,
    render_current_audio_state: function(state)
    {
        $('#btn_unlock_audio').html( state == 'running' ? this.audio_unlocked_icon : this.audio_locked_icon );
        this.last_audio_state=state;
    },
    open_zip: function () {
        var vc64web = document.getElementById("vc64web").contentWindow;
        //click emulators open_zip button
        vc64web.postMessage("open_zip()", "*");
    },
    stop_emu_view: function() {
        //close any other active emulator frame
        if (this.saved_pic_html != null) {
            //restore preview pic
            $('#player_container').parent().html(this.saved_pic_html); 
            this.saved_pic_html=null;
        }
        if(this.state_poller != null)
        {
            clearInterval(this.state_poller);
        }
        this.last_run_state=null; 
        this.last_audio_state=null;

        document.removeEventListener("click", this.grab_focus);
    }
}
