
// bootstrap
Drupal.behaviors.jstimer = {
  attach: function(context) {
    Drupal.jstimer.countdown_auto_attach(
      new Array(
        new Drupal.jstimer.jst_clock()
      )
    );
  }
}

// Namespace for most of the javascript functions.
Drupal.jstimer = {};

// Array that holds all elements that need to be updated.
Drupal.jstimer.timer_stack = new Array();

// Attach all active widgets to their respective dom objects.
Drupal.jstimer.countdown_auto_attach = function (jstimer_active_widgets) {

  // Call .attach() on the active widget items.
  for (var i=0; i<jstimer_active_widgets.length; i++) {
    jstimer_active_widgets[i].attach();
  }

  // If you have any widget items, start the timing loop.
  if ( Drupal.jstimer.timer_stack.length > 0 ) {
    Drupal.jstimer.timer_loop();
  }

}

// The timing loop.
Drupal.jstimer.timer_loop = function() {
  // run backwards so we can remove items and not messup the loop data.
  for (var i = Drupal.jstimer.timer_stack.length - 1; i >= 0; i--) {
    if ( Drupal.jstimer.timer_stack[i].update() == false ) {
      Drupal.jstimer.timer_stack.splice(i, 1);
    }
  }

  // Stop the timer if there are not more timer items.
  if ( Drupal.jstimer.timer_stack.length > 0 ) {
    setTimeout('Drupal.jstimer.timer_loop()',999);
  }
}


/*
 * Clock widget
 */
Drupal.jstimer.jst_clock = function() {
  this.selector = ".jst_clock";
  this.attach = function() {
    jQuery(this.selector).each(
      function(i) {
        var t = new Drupal.jstimer.jst_clock_item(jQuery(this));
        Drupal.jstimer.timer_stack[Drupal.jstimer.timer_stack.length] = t;
      }
    );
  }
}

Drupal.jstimer.jst_clock_item = function(ele) {

  // class methods first so you can use them in the constructor.
  this.loadProps = function() {
    for (var prop in this.props) {
      var prop_selector = "span[class="+prop+"]";
      if ( this.element.children(prop_selector).length > 0 ) {
        this.props[prop] = this.element.children(prop_selector).html();
      }
    }

    // If browser doesn't support the html canvas element, default to 12-hour clock.
    if ( !supports_canvas() &&  this.props['clock_type'] == 2 ) {
      this.props['clock_type'] = 0;
    }
  }

  this.update = function() {
    var timenow = new Date();
    var hr = timenow.getHours();
    var min = timenow.getMinutes();
    var sec = timenow.getSeconds();
    if ( this.props['clock_type'] == 0 ) {
      var am_pm = ""
      if ( hr <= 12 ) {
        am_pm = "am";
      } else {
        am_pm = "pm";
        hr = hr - 12;
      }
      this.element.html(hr + ":" + LZ(min) + ":" + LZ(sec) + am_pm);
    } else if ( this.props['clock_type'] == 1 ) {
      this.element.html(hr + ":" + LZ(min) + ":" + LZ(sec));
    } else if ( this.props['clock_type'] == 2 ) {
      if ( this.canvas ) {
        var ctx = this.canvas[0].getContext("2d");

        // save original context
        ctx.save(); // 1

        // clear and set defaults.
        ctx.clearRect(0,0,this.props['size'],this.props['size']);
        ctx.translate((this.props['size']/2),(this.props['size']/2));
        ctx.scale(0.4,0.4);
        ctx.rotate(-Math.PI/2);
        ctx.strokeStyle = "black";
        ctx.fillStyle = "white";
        ctx.lineWidth = 8;
        ctx.lineCap = "round";
        ctx.save(); // 2

        // render hour tick marks
        for (var i=0;i<12;i++){
         ctx.beginPath();
         ctx.rotate(Math.PI/6);
         ctx.moveTo(this.props['size']-20,0);
         ctx.lineTo(this.props['size'],0);
         ctx.stroke();
        }
        ctx.restore();  // 1

        ctx.fillStyle = "black";

        // render hour hand
        ctx.save(); // 3
        var hr_hand_size = 0.4 * this.props['size'];
        ctx.rotate( hr*(Math.PI/6) + (Math.PI/360)*min + (Math.PI/21600)*sec )
        ctx.lineWidth = 14;
        ctx.beginPath();
        ctx.moveTo(-20,0);
        ctx.lineTo(hr_hand_size, 0);
        ctx.stroke();
        ctx.restore();   // 2

        // render minute hand
        var min_hand_size = 0.85 * this.props['size'];
        ctx.save(); // 4
        ctx.rotate( (Math.PI/30)*min + (Math.PI/1800)*sec )
        ctx.lineWidth = 10;
        ctx.beginPath();
        ctx.moveTo(-28,0);
        ctx.lineTo(min_hand_size, 0);
        ctx.stroke();
        ctx.restore();  // 3

        // render second hand
        ctx.save(); // 5
        var sec_hand_size = 0.85 * this.props['size'];
        ctx.rotate(sec * Math.PI/30);
        ctx.strokeStyle = "#D40000";
        ctx.fillStyle = "#D40000";
        ctx.lineWidth = 6;
        ctx.beginPath();
        ctx.moveTo(-30,0);
        ctx.lineTo(sec_hand_size, 0);
        ctx.stroke();
        ctx.beginPath();
        ctx.arc(0,0,10,0,Math.PI*2,true);
        ctx.fill();
        ctx.beginPath();
        ctx.arc(sec_hand_size, 0, 10, 0, Math.PI*2, true);
        ctx.stroke();
        ctx.fillStyle = "#555";
        ctx.arc(0,0,3,0,Math.PI*2,true);
        ctx.fill();
        ctx.restore();  // 4

        // render clock face circle
        ctx.save(); // 6
        ctx.beginPath();
        ctx.lineWidth = 14;
        ctx.strokeStyle = '#325FA2';
        ctx.arc(0,0,this.props['size'],0,Math.PI*2,true);
        ctx.stroke();
        ctx.restore(); // 5

        // Days box
        //ctx.save(); // 7
        //ctx.rotate(90 * Math.PI / 180);
        //ctx.fillStyle    = '#FFFFFF';
        //ctx.fillRect(0, this.props['size']/3, 100, 60);
        //ctx.strokeStyle = '#325FA2';
        //ctx.strokeRect(0, this.props['size']/3, 100, 60);
        //ctx.restore(); // 6

        // days text
        //ctx.rotate(90 * Math.PI / 180);
        //ctx.fillStyle    = '#0000FF';
        //ctx.font         = 'courier-new 30px';
        //ctx.fillText  ('Days', 110, (this.props['size']/3)+50);
        ctx.restore();  // 7
      }
    }

    // SVG Clock
    else if ( this.props['clock_type'] == 3 ) {
      if (this.clock && !this.clockSvgWin) {
        this.clockSvgWin = getSvgWindow(clock);
      }
      if (this.clock && this.clockSvgWin) {
        try{
          this.clockSvgWin.animate();
        } 
        catch(err) {
          //Some browsers will have a timing error and require one more iteration.
        }
      }
    }
    return true;
  }

  // begin constructor
  this.element = ele; // jquery ele
  this.props = {clock_type:'1', size:200};
  this.loadProps();

  // add canvas if analog.
  if ( this.props['clock_type'] == 2 ) {
    this.canvas = jQuery('<canvas class="ct_clock_canvas"><p>Your browser doesn\'t support canvas.</p></canvas>');
    this.canvas.attr("width",this.props['size']);
    this.canvas.attr("height",this.props['size']);
    this.element.append(this.canvas);
  }
  else if ( this.props['clock_type'] == 3 ) {
    var clock = jQuery('<object data="/sites/all/modules/jstimer/widgets/clocks/sleek1.svg" type="image/svg+xml"></object>');
    clock.attr('width',this.props['size']);
    clock.attr('height',this.props['size']);
    this.element.html('');
    this.element.append(clock);
    this.clock = clock;
  }


}

function supports_canvas() {
  return !!document.createElement('canvas').getContext;
}

function getSvgWindow(clock) {
  var svgDoc;
  var svgWin;
  var jsClock = clock.get(0);
  if (jsClock.contentDocument) {
    svgDoc = jsClock.contentDocument; //get the inner DOM of alpha.svg
  }
  else {
    try {
      svgDoc = jsClock.getSVGDocument();
    }
    catch(exception) {
      // Ignore errors, this exception will fire if the DOM is not loaded yet.
    }
  }

  if (svgDoc && svgDoc.defaultView) {
    svgWin = svgDoc.defaultView;
  }
  else if (jsClock.window) {
    svgWin = jsClock.window;
  }
  else try {
    svgWin = jsClock.getWindow();
  }
  catch(exception) {
    // Ignore errors, this exception will fire if the DOM is not loaded yet.
  }
  return svgWin;
}

// End of clock widget.





// Util functions
function LZ(x) {
  return (x >= 10 || x < 0 ? "" : "0") + x;
}

// iso8601 date parsing routines.  Extends the built-in javascript date object.
Date.prototype.jstimer_set_iso8601_date = function (string) {
  var iso8601_re = /^(?:(\d{4})(?:-(\d{2})(?:-(\d{2}))?)?)?(?:[T ](\d{2}):(\d{2})(?::(\d{2})(.\d+)?)?((?:[+-](\d{2}):(\d{2}))|Z)?)?$/;
  var date_bits = iso8601_re.exec(string);
  var date_obj = null;
  if ( date_bits ) {
    date_bits.shift();
    date_bits[1] && date_bits[1]--; // normalize month
    date_bits[6] && (date_bits[6] *= 1000); // convert mils
    date_obj = new Date(date_bits[0]||1970, date_bits[1]||0, date_bits[2]||0, date_bits[3]||0, date_bits[4]||0, date_bits[5]||0, date_bits[6]||0);

    if (date_bits[0] < 1000) {
      date_obj.setFullYear(date_bits[0]);
    }

    //timezone handling
    var zone_offset = 0;  // in minutes
    var zone_plus_minus = date_bits[7] && date_bits[7].charAt(0);
    // get offset from isostring time to Z time
    if ( zone_plus_minus != 'Z' ) {
      zone_offset = ((date_bits[8] || 0) * 60) + (Number(date_bits[9]) || 0);
      if ( zone_plus_minus != '-' ) {
        zone_offset *= -1;
      }
    }
    // convert offset to localtime offset, will include daylight savings
    if ( zone_plus_minus ) {
      zone_offset -= date_obj.getTimezoneOffset();
    }
    if ( zone_offset ) {
      date_obj.setTime(date_obj.getTime() + zone_offset * 60000);
    }
  }

  // set this object to current localtime representation
  try {
    this.setTime(date_obj.getTime());
  }
  catch(e) {
    throw new Object({name:"DatePatternFail",message:"jstimer: Date does not have proper format (ISO8601, see readme.txt)."});
  }
}
Date.prototype.jstimer_get_moy = function () {
  var myMonths=new Array("Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec");
  return myMonths[this.getMonth()];
}
Date.prototype.jstimer_get_dow = function () {
  var myDays=["Sun","Mon","Tue","Wed","Thu","Fri","Sat","Sun"];
  return myDays[this.getDay()];
}
